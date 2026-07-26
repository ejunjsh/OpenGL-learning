#include "header/glframebuffers.h"
#include <QMatrix4x4>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDebug>

// ============================================================================
// 构造 / 析构
// ============================================================================

GLFramebuffers::GLFramebuffers(QWidget *parent)
    : GLCameraBase(parent),
      m_sceneProgram(new QOpenGLShaderProgram(this)),
      m_screenProgram(new QOpenGLShaderProgram(this))
{
    setName("Framebuffers");
    setupMenu();
}

GLFramebuffers::~GLFramebuffers()
{
    makeCurrent();
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteRenderbuffers(1, &m_rbo);
    glDeleteTextures(1, &m_texColorBuffer);
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
    m_sceneProgram->removeAllShaders();
    m_screenProgram->removeAllShaders();
    doneCurrent();
}

// ============================================================================
// 菜单（效果互斥，类似 radio button）
// ============================================================================

void GLFramebuffers::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    m_effectNormal       = new QCheckBox("Normal",           menu);
    m_effectInvert       = new QCheckBox("Inversion",        menu);
    m_effectSimpleGray   = new QCheckBox("Simple Grayscale", menu);
    m_effectWeightedGray = new QCheckBox("Weighted Grayscale", menu);
    m_effectSharpen      = new QCheckBox("Sharpen",          menu);
    m_effectBlur         = new QCheckBox("Blur",             menu);
    m_effectEdge         = new QCheckBox("Edge Detection",   menu);

    m_effectNormal->setChecked(true);

    menuLayout->addWidget(m_effectNormal);
    menuLayout->addWidget(m_effectInvert);
    menuLayout->addWidget(m_effectSimpleGray);
    menuLayout->addWidget(m_effectWeightedGray);
    menuLayout->addWidget(m_effectSharpen);
    menuLayout->addWidget(m_effectBlur);
    menuLayout->addWidget(m_effectEdge);
    menuLayout->addStretch();

    QList<QCheckBox*> boxes = {m_effectNormal, m_effectInvert, m_effectSimpleGray,
                               m_effectWeightedGray, m_effectSharpen,
                               m_effectBlur, m_effectEdge};

    for (int i = 0; i < boxes.size(); ++i)
    {
        connect(boxes[i], &QCheckBox::clicked, this, [this, i, boxes](bool checked) {
            if (!checked) { boxes[i]->setChecked(true); return; }
            for (int j = 0; j < boxes.size(); ++j)
                if (j != i) boxes[j]->setChecked(false);
            m_currentEffect = i;
        });
    }
}

// ============================================================================
// Framebuffer 创建 / 重建（窗口大小变化时调用）
//   - 颜色附件：纹理（可被 shader 采样）
//   - 深度/模板附件：RBO（只写，性能更优）
// ============================================================================

void GLFramebuffers::setupFramebuffer(int w, int h)
{
    qDebug() << "setupFramebuffer:" << w << "x" << h;

    // 清理旧附件
    if (m_texColorBuffer) glDeleteTextures(1, &m_texColorBuffer);
    if (m_rbo) glDeleteRenderbuffers(1, &m_rbo);

    m_fboWidth = w;
    m_fboHeight = h;

    // ---- 颜色纹理附件 ----
    glGenTextures(1, &m_texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_texColorBuffer, 0);

    // ---- 深度+模板 RBO ----
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, m_rbo);

    // 检查完整性
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qWarning() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
}

// ============================================================================
// initializeGL
// ============================================================================

void GLFramebuffers::initializeGL()
{
    GLBase::initializeGL();

    // ---- 场景着色器 ----
    if (!m_sceneProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                        "shaders/framebuffers/framebuffers_scene.vert") ||
        !m_sceneProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                        "shaders/framebuffers/framebuffers_scene.frag"))
    {
        qFatal("Failed to compile framebuffers scene shader");
    }
    if (!m_sceneProgram->link())
        qFatal("Failed to link framebuffers scene shader");

    // ---- 屏幕四边形着色器 ----
    if (!m_screenProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                        "shaders/framebuffers/framebuffers_screen.vert") ||
        !m_screenProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                        "shaders/framebuffers/framebuffers_screen.frag"))
    {
        qFatal("Failed to compile framebuffers screen shader");
    }
    if (!m_screenProgram->link())
        qFatal("Failed to link framebuffers screen shader");

    // ---- 纹理 ----
    m_cubeTexture  = loadTexture("textures/marble.jpg");
    m_floorTexture = loadTexture("textures/metal.png");

    // ---- 立方体顶点 ----
    float cubeVertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    };

    // ---- 地板顶点 ----
    float planeVertices[] = {
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
    };

    // ---- 屏幕四边形（NDC，覆盖全屏） ----
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
        -1.0f, -1.0f,  0.0f, 0.0f,  // bottom-left
         1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right
        -1.0f,  1.0f,  0.0f, 1.0f,  // top-left
         1.0f, -1.0f,  1.0f, 0.0f,  // bottom-right
         1.0f,  1.0f,  1.0f, 1.0f,  // top-right
    };

    // Cube VAO + VBO
    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);
    glBindVertexArray(m_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Plane VAO + VBO
    glGenVertexArrays(1, &m_planeVAO);
    glGenBuffers(1, &m_planeVBO);
    glBindVertexArray(m_planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Screen quad VAO + VBO
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // ---- 设置纹理 uniform ----
    m_sceneProgram->bind();
    m_sceneProgram->setUniformValue("uTexture", 0);
    m_sceneProgram->release();

    m_screenProgram->bind();
    m_screenProgram->setUniformValue("screenTexture", 0);
    m_screenProgram->release();

    // ---- 创建 FBO（附件在 resizeGL 中创建） ----
    glGenFramebuffers(1, &m_fbo);
}

// ============================================================================
// resizeGL — 窗口大小变化时重建附件
// ============================================================================

void GLFramebuffers::resizeGL(int w, int h)
{
    GLBase::resizeGL(w, h);
    setupFramebuffer(w, h);
}

// ============================================================================
// 辅助函数
// ============================================================================

void GLFramebuffers::drawCube(const QVector3D &pos)
{
    QMatrix4x4 model;
    model.translate(pos);
    m_sceneProgram->setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// ============================================================================
// paintGL — 双通道渲染
//   第一通道：渲染场景到自定义 FBO
//   第二通道：将 FBO 颜色纹理绘制到屏幕四边形（后处理效果）
// ============================================================================

void GLFramebuffers::paintGL()
{
    // 保存 Qt 默认帧缓冲的 viewport（同时获取其实际像素尺寸）
    GLint defaultViewport[4];
    glGetIntegerv(GL_VIEWPORT, defaultViewport);
    const int vpW = defaultViewport[2];
    const int vpH = defaultViewport[3];

    // FBO 分辨率必须与默认帧缓冲一致，否则上/下采样导致模糊
    if (m_texColorBuffer == 0 || m_fboWidth != vpW || m_fboHeight != vpH)
        setupFramebuffer(vpW, vpH);

    const float aspect = static_cast<float>(vpW) / vpH;
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    // ================================================================
    // 第一通道：渲染到自定义 FBO
    // ================================================================
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, vpW, vpH);  // viewport 与 FBO 尺寸一致
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_sceneProgram->bind();

    // 地板
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    glBindVertexArray(m_planeVAO);
    m_sceneProgram->setUniformValue("model", QMatrix4x4());
    m_sceneProgram->setUniformValue("projection", projection);
    m_sceneProgram->setUniformValue("view", view);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 立方体
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    glBindVertexArray(m_cubeVAO);
    drawCube(QVector3D(-1.0f, 0.0f, -1.0f));
    drawCube(QVector3D( 2.0f, 0.0f,  0.0f));

    m_sceneProgram->release();

    // ================================================================
    // 第二通道：屏幕四边形 → 默认帧缓冲（应用后处理效果）
    // ================================================================
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());  // QOpenGLWidget 的默认帧缓冲
    glViewport(defaultViewport[0], defaultViewport[1],
               defaultViewport[2], defaultViewport[3]);  // 恢复 Qt 的原始 viewport
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);  // 确保四边形渲染在最前面

    m_screenProgram->bind();
    m_screenProgram->setUniformValue("screenTexture", 0);
    m_screenProgram->setUniformValue("uEffect", m_currentEffect);
    m_screenProgram->setUniformValue("uKernelOffset", 1.0f / 300.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texColorBuffer);
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_screenProgram->release();
    glBindVertexArray(0);
}
