#include "header/gldepthtesting.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

GLDepthTesting::GLDepthTesting(QWidget *parent)
    : GLCameraBase(parent),
      m_depthViewProgram(new QOpenGLShaderProgram(this)),
      m_depthView2Program(new QOpenGLShaderProgram(this))
{
    setName("Depth Testing");
    setupMenu();
}

GLDepthTesting::~GLDepthTesting()
{
    makeCurrent();
    m_depthViewProgram->removeAllShaders();
    m_depthView2Program->removeAllShaders();
    doneCurrent();
}

void GLDepthTesting::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    m_depthCheck = new QCheckBox(menu);
    m_depthCheck->setChecked(true);
    m_depthCheck->setText("Enable Depth Test");

    m_viewCombo = new QComboBox(menu);
    m_viewCombo->setStyleSheet(
        "QComboBox { color: white; background: #3a3a3a; border: 1px solid #555; "
        "padding: 4px 8px; border-radius: 4px; }"
        "QComboBox QAbstractItemView { color: white; background: #2b2b2b; "
        "selection-background-color: #555; }");
    m_viewCombo->addItem("Normal");
    m_viewCombo->addItem("Depth View (Non-linear)");
    m_viewCombo->addItem("Depth View (Linear)");

    menuLayout->addWidget(m_depthCheck);
    menuLayout->addWidget(m_viewCombo);
    menuLayout->addStretch();

    connect(m_depthCheck, &QCheckBox::toggled, this, &GLDepthTesting::onDepthToggled);
    connect(m_viewCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GLDepthTesting::onViewModeChanged);
}

void GLDepthTesting::onDepthToggled(bool checked)
{
    m_depthTest = checked;
}

void GLDepthTesting::onViewModeChanged(int index)
{
    m_viewMode = static_cast<ViewMode>(index);
}

void GLDepthTesting::initializeGL()
{
    GLBase::initializeGL();

    // ---- 正常纹理渲染 program（用父类 m_program） ----
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/depth_testing/depth_testing.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/depth_testing/depth_testing.frag"))
    {
        qFatal("Failed to compile depth testing normal shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link depth testing normal shader program");
    }

    // ---- 深度可视化 program (非线性) ----
    if (!m_depthViewProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/depth_testing/depth_testing.vert") ||
        !m_depthViewProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/depth_testing/depth_testing_view.frag"))
    {
        qFatal("Failed to compile depth view shader");
    }
    if (!m_depthViewProgram->link())
    {
        qFatal("Failed to link depth view shader program");
    }

    // ---- 深度可视化 program (线性) ----
    if (!m_depthView2Program->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/depth_testing/depth_testing.vert") ||
        !m_depthView2Program->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/depth_testing/depth_testing_view2.frag"))
    {
        qFatal("Failed to compile depth view2 shader");
    }
    if (!m_depthView2Program->link())
    {
        qFatal("Failed to link depth view2 shader program");
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

    QOpenGLShaderProgram *program = nullptr;
    switch (m_viewMode) {
    case DepthLinear:     program = m_depthView2Program; break;
    case DepthNonLinear:  program = m_depthViewProgram;  break;
    default:              program = &m_program;          break;
    }
    program->bind();
    program->setUniformValue("projection", projection);
    program->setUniformValue("view", view);

    // ---- 绘制两个立方体 ----
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    glBindVertexArray(m_cubeVAO);

    QMatrix4x4 model;
    model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
    program->setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model.setToIdentity();
    model.translate(QVector3D(2.0f, 0.0f, 0.0f));
    program->setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ---- 绘制地板 ----
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    glBindVertexArray(m_planeVAO);
    program->setUniformValue("model", QMatrix4x4());
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    program->release();
}
