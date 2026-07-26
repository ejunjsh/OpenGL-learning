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
    glDeleteFramebuffers(1, &m_mirrorFbo);
    glDeleteRenderbuffers(1, &m_mirrorRbo);
    glDeleteTextures(1, &m_mirrorTexColor);
    glDeleteVertexArrays(1, &m_cubeVAO);
    glDeleteBuffers(1, &m_cubeVBO);
    glDeleteVertexArrays(1, &m_planeVAO);
    glDeleteBuffers(1, &m_planeVBO);
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteBuffers(1, &m_quadVBO);
    glDeleteVertexArrays(1, &m_mirrorQuadVAO);
    glDeleteBuffers(1, &m_mirrorQuadVBO);
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

    m_mirrorToggle = new QCheckBox("Rear-view Mirror (Ex.)", menu);

    m_effectNormal->setChecked(true);

    menuLayout->addWidget(m_effectNormal);
    menuLayout->addWidget(m_effectInvert);
    menuLayout->addWidget(m_effectSimpleGray);
    menuLayout->addWidget(m_effectWeightedGray);
    menuLayout->addWidget(m_effectSharpen);
    menuLayout->addWidget(m_effectBlur);
    menuLayout->addWidget(m_effectEdge);
    menuLayout->addWidget(m_mirrorToggle);
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

    connect(m_mirrorToggle, &QCheckBox::toggled, this, [this](bool checked) {
        m_showMirror = checked;
    });
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

    // ---- 后视镜四边形（NDC，屏幕顶部中央，纹理水平翻转模拟真实镜子）----
    float mirrorQuadVertices[] = {
        // positions     // texCoords (u 左右翻转)
        -0.35f,  1.0f,   1.0f, 1.0f,  // top-left
        -0.35f,  0.65f,  1.0f, 0.0f,  // bottom-left
         0.35f,  0.65f,  0.0f, 0.0f,  // bottom-right
        -0.35f,  1.0f,   1.0f, 1.0f,  // top-left
         0.35f,  0.65f,  0.0f, 0.0f,  // bottom-right
         0.35f,  1.0f,   0.0f, 1.0f,  // top-right
    };

    glGenVertexArrays(1, &m_mirrorQuadVAO);
    glGenBuffers(1, &m_mirrorQuadVBO);
    glBindVertexArray(m_mirrorQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_mirrorQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mirrorQuadVertices), mirrorQuadVertices,
                 GL_STATIC_DRAW);
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
    glGenFramebuffers(1, &m_mirrorFbo);
}

// ============================================================================
// resizeGL — 窗口大小变化时重建附件
// ============================================================================

void GLFramebuffers::resizeGL(int w, int h)
{
    GLBase::resizeGL(w, h);
    setupFramebuffer(w, h);
    // 镜像 FBO 用一半分辨率（长宽比保持一致），节省带宽
    setupMirrorFramebuffer(w / 2, h / 2);
}

// ============================================================================
// 辅助函数
// ============================================================================

void GLFramebuffers::setupMirrorFramebuffer(int w, int h)
{
    // 清理旧附件
    if (m_mirrorTexColor) glDeleteTextures(1, &m_mirrorTexColor);
    if (m_mirrorRbo) glDeleteRenderbuffers(1, &m_mirrorRbo);

    m_mirrorWidth = w;
    m_mirrorHeight = h;

    // ---- 颜色纹理附件 ----
    glGenTextures(1, &m_mirrorTexColor);
    glBindTexture(GL_TEXTURE_2D, m_mirrorTexColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_mirrorFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_mirrorTexColor, 0);

    // ---- 深度+模板 RBO ----
    glGenRenderbuffers(1, &m_mirrorRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_mirrorRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, m_mirrorRbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        qWarning() << "ERROR::MIRROR_FRAMEBUFFER:: Framebuffer is not complete!";

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
}

// ============================================================================
// 绘制场景（地板 + 立方体），用于主通道和镜像通道复用
// ============================================================================

void GLFramebuffers::drawScene(const QMatrix4x4 &projection, const QMatrix4x4 &view)
{
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
}

void GLFramebuffers::drawCube(const QVector3D &pos)
{
    QMatrix4x4 model;
    model.translate(pos);
    m_sceneProgram->setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// ============================================================================
// paintGL — 三通道渲染
//   通道 0：渲染场景到镜像 FBO（相机旋转 180 度，后视视角）
//   通道 1：渲染场景到主 FBO（正常视角）
//   通道 2：将主 FBO 纹理 + 镜像纹理合成到屏幕
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

    // 镜像 FBO 使用一半分辨率（节省带宽，长宽比保持一致）
    const int mirrorW = vpW / 2;
    const int mirrorH = vpH / 2;
    if (m_mirrorTexColor == 0 || m_mirrorWidth != mirrorW || m_mirrorHeight != mirrorH)
        setupMirrorFramebuffer(mirrorW, mirrorH);

    // ---- 投影 / 视图矩阵 ----
    const float aspect = static_cast<float>(vpW) / vpH;
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    // 后视镜视图：后方平视（投影到水平面，模拟真实固定后视镜）
    QVector3D rearDir = -m_camera->getFront();
    rearDir.setY(0.0f);
    rearDir.normalize();
    QMatrix4x4 mirrorView;
    mirrorView.lookAt(m_camera->getPosition(),
                      m_camera->getPosition() + rearDir,
                      QVector3D(0.0f, 1.0f, 0.0f));

    // ================================================================
    // 通道 0：渲染到镜像 FBO（后视视角）
    // ================================================================
    if (m_showMirror) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_mirrorFbo);
        glViewport(0, 0, mirrorW, mirrorH);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawScene(projection, mirrorView);
    }

    // ================================================================
    // 通道 1：渲染到主 FBO（正常视角，供后处理使用）
    // ================================================================
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, vpW, vpH);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene(projection, view);

    // ================================================================
    // 通道 2：合成到默认帧缓冲
    //   (a) 全屏四边形 — 主场景纹理（带后处理效果）
    //   (b) 后视镜四边形 — 镜像纹理（无效果，覆盖在顶部）
    // ================================================================
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(defaultViewport[0], defaultViewport[1],
               defaultViewport[2], defaultViewport[3]);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    m_screenProgram->bind();
    m_screenProgram->setUniformValue("screenTexture", 0);
    m_screenProgram->setUniformValue("uKernelOffset", 1.0f / 300.0f);

    // (a) 全屏四边形 — 主场景
    m_screenProgram->setUniformValue("uEffect", m_currentEffect);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texColorBuffer);
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // (b) 后视镜四边形 — 镜像纹理（叠加在顶部）
    if (m_showMirror) {
        m_screenProgram->setUniformValue("uEffect", m_currentEffect);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_mirrorTexColor);
        glBindVertexArray(m_mirrorQuadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    m_screenProgram->release();
    glBindVertexArray(0);
}
