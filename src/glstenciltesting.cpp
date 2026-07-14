#include "header/glstenciltesting.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDebug>

GLStencilTesting::GLStencilTesting(QWidget *parent)
    : GLCameraBase(parent),
      m_outlineProgram(new QOpenGLShaderProgram(this))
{
    setName("Stencil Testing");
    setupMenu();

    // 初始相机位置略靠后，便于观察场景
    m_camera->setPosition(QVector3D(0.0f, 0.5f, 4.0f));
    m_camera->setPitch(-10.0f);
    m_camera->updateCameraVectors();
}

GLStencilTesting::~GLStencilTesting()
{
    makeCurrent();
    m_outlineProgram->removeAllShaders();
    doneCurrent();
}

void GLStencilTesting::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    m_outlineCheck = new QCheckBox(menu);
    m_outlineCheck->setChecked(true);
    m_outlineCheck->setText("Show Outline");

    menuLayout->addWidget(m_outlineCheck);
    menuLayout->addStretch();

    connect(m_outlineCheck, &QCheckBox::toggled,
            this, &GLStencilTesting::onOutlineToggled);
}

void GLStencilTesting::onOutlineToggled(bool checked)
{
    m_showOutline = checked;
}

void GLStencilTesting::initializeGL()
{
    GLBase::initializeGL();

    // ---- 正常纹理渲染 program（父类 m_program） ----
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/stencil_testing/stencil_testing.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/stencil_testing/stencil_testing.frag"))
    {
        qFatal("Failed to compile stencil testing shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link stencil testing shader program");
    }

    // ---- 描边纯色 program ----
    if (!m_outlineProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/stencil_testing/stencil_testing.vert") ||
        !m_outlineProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/stencil_testing/stencil_single_color.frag"))
    {
        qFatal("Failed to compile stencil outline shader");
    }
    if (!m_outlineProgram->link())
    {
        qFatal("Failed to link stencil outline shader program");
    }

    // 加载纹理
    m_cubeTexture  = loadTexture("textures/marble.jpg");
    m_floorTexture = loadTexture("textures/metal.png");

    // 立方体顶点：position(3) + texCoord(2) = 5 floats
    float cubeVertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        // front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    };

    float planeVertices[] = {
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
    };

    // Cube VAO + VBO
    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);
    glBindVertexArray(m_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    // position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord (location 2)
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

    glBindVertexArray(0);

    // 设置 texture sampler uniform
    m_program.bind();
    m_program.setUniformValue("uTexture", 0);
    m_program.release();
}

void GLStencilTesting::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    // ================================================================
    // 第一遍：绘制地板（不写模板缓冲）
    // ================================================================
    glStencilMask(0x00);

    m_program.bind();
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    glBindVertexArray(m_planeVAO);
    m_program.setUniformValue("model", QMatrix4x4());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_program.release();

    // ================================================================
    // 第二遍：绘制立方体，并将模板值写入 1
    // ================================================================
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    m_program.bind();
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    glBindVertexArray(m_cubeVAO);

    // 立方体 1
    QMatrix4x4 model;
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    m_program.setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 立方体 2
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    m_program.setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_program.release();

    // ================================================================
    // 第三遍：绘制描边（仅在模板值 ≠ 1 处绘制放大的立方体）
    // ================================================================
    if (m_showOutline)
    {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        m_outlineProgram->bind();
        m_outlineProgram->setUniformValue("projection", projection);
        m_outlineProgram->setUniformValue("view", view);

        float scale = 1.1f;

        // 立方体 1 描边
        QMatrix4x4 outlineModel;
        outlineModel.translate(QVector3D(-1.0f, 0.0f, -1.0f));
        outlineModel.scale(scale);
        m_outlineProgram->setUniformValue("model", outlineModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 立方体 2 描边
        outlineModel.setToIdentity();
        outlineModel.translate(QVector3D(2.0f, 0.0f, 0.0f));
        outlineModel.scale(scale);
        m_outlineProgram->setUniformValue("model", outlineModel);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        m_outlineProgram->release();

        glEnable(GL_DEPTH_TEST);
    }

    // 恢复默认状态
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);

    glBindVertexArray(0);
}
