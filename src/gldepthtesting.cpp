#include "header/gldepthtesting.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

GLDepthTesting::GLDepthTesting(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("Depth Testing");
    setupMenu();
}

void GLDepthTesting::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    m_depthCheck = new QCheckBox(menu);
    m_depthCheck->setChecked(true);
    m_depthCheck->setText("Enable Depth Test");

    menuLayout->addWidget(m_depthCheck);
    menuLayout->addStretch();

    connect(m_depthCheck, &QCheckBox::toggled, this, &GLDepthTesting::onDepthToggled);
}

void GLDepthTesting::onDepthToggled(bool checked)
{
    m_depthTest = checked;
}

void GLDepthTesting::initializeGL()
{
    GLBase::initializeGL();

    // 编译 shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/depth_testing/depth_testing.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/depth_testing/depth_testing.frag"))
    {
        qFatal("Failed to compile depth testing shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link depth testing shader program");
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

void GLDepthTesting::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // GL_LESS: 正常深度测试（近的挡住远的）
    // GL_ALWAYS: 始终通过深度测试（后绘制的盖住先绘制的）
    glDepthFunc(m_depthTest ? GL_LESS : GL_ALWAYS);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    m_program.bind();
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);


    // ---- 绘制两个立方体 ----
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    glBindVertexArray(m_cubeVAO);

    QMatrix4x4 model;
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    m_program.setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    m_program.setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

        // ---- 绘制地板 ----
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    glBindVertexArray(m_planeVAO);
    m_program.setUniformValue("model", QMatrix4x4());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    m_program.release();
}
