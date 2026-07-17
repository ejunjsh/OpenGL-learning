#include "header/glfaceculling.h"
#include <QMatrix4x4>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDebug>

GLFaceCulling::GLFaceCulling(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("Face Culling");
    setupMenu();

    m_camera->setPosition(QVector3D(0.0f, 0.5f, 5.0f));
    m_camera->setPitch(-12.0f);
    m_camera->setYaw(-90.0f);
    m_camera->updateCameraVectors();
}

GLFaceCulling::~GLFaceCulling()
{
    makeCurrent();
    doneCurrent();
}

void GLFaceCulling::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    m_enableCullingCheck = new QCheckBox(menu);
    m_enableCullingCheck->setChecked(true);
    m_enableCullingCheck->setText("Enable Culling");

    m_cullFrontCheck = new QCheckBox(menu);
    m_cullFrontCheck->setChecked(false);
    m_cullFrontCheck->setText("Cull Front");

    m_frontFaceCWCheck = new QCheckBox(menu);
    m_frontFaceCWCheck->setChecked(false);
    m_frontFaceCWCheck->setText("CW is Front");

    menuLayout->addWidget(m_enableCullingCheck);
    menuLayout->addWidget(m_cullFrontCheck);
    menuLayout->addWidget(m_frontFaceCWCheck);
    menuLayout->addStretch();

    connect(m_enableCullingCheck, &QCheckBox::toggled,
            this, &GLFaceCulling::onCullingToggled);
    connect(m_cullFrontCheck, &QCheckBox::toggled,
            this, &GLFaceCulling::onCullFaceToggled);
    connect(m_frontFaceCWCheck, &QCheckBox::toggled,
            this, &GLFaceCulling::onFrontFaceToggled);
}

void GLFaceCulling::onCullingToggled(bool checked)
{
    m_cullingEnabled = checked;
}

void GLFaceCulling::onCullFaceToggled(bool checked)
{
    m_cullFront = checked;
}

void GLFaceCulling::onFrontFaceToggled(bool checked)
{
    m_frontFaceCW = checked;
}

void GLFaceCulling::initializeGL()
{
    GLBase::initializeGL();

    // shader program
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/face_culling/face_culling.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/face_culling/face_culling.frag"))
    {
        qFatal("Failed to compile face_culling shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link face_culling shader program");
    }

    // 纹理
    m_cubeTexture  = loadTexture("textures/marble.jpg");
    m_floorTexture = loadTexture("textures/metal.png");

    // ---- 立方体顶点（CCW 绕序） ----
    float cubeVertices[] = {
            // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
        // Right face
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
    };

    // ---- 地板顶点（CW 绕序，从上方看） ----
    float planeVertices[] = {
        // Triangle 1
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,  // front-right
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,  // back-left
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,  // front-left
        // Triangle 2
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,  // front-right
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f,  // back-right
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,  // back-left
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

    glBindVertexArray(0);

    m_program.bind();
    m_program.setUniformValue("uTexture", 0);
    m_program.release();
}

void GLFaceCulling::drawCube(const QVector3D &pos)
{
    QMatrix4x4 model;
    model.translate(pos);
    m_program.setUniformValue("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLFaceCulling::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    // ---- 配置面剔除 ----
    if (m_cullingEnabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(m_cullFront ? GL_FRONT : GL_BACK);
        glFrontFace(m_frontFaceCW ? GL_CW : GL_CCW);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    m_program.bind();

    // 地板
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    glBindVertexArray(m_planeVAO);
    m_program.setUniformValue("model", QMatrix4x4());
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 三个立方体
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    glBindVertexArray(m_cubeVAO);
    drawCube(QVector3D(-1.0f, 0.0f, -1.0f));
    drawCube(QVector3D( 2.0f, 0.0f,  0.0f));

    m_program.release();

    glDisable(GL_CULL_FACE);
    glBindVertexArray(0);
}
