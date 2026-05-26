#include "header/gllightingmaps.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QtMath>

GLLightingMaps::GLLightingMaps(QWidget *parent)
    : GLCameraBase(parent)
    , m_sceneIndex(0)
{
    setName("GLLightingMaps");
    setupMenu();
}

void GLLightingMaps::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *diffuseBtn = new QRadioButton("Diffuse", menu);
    QRadioButton *specBtn = new QRadioButton("Diff+Spec", menu);
    diffuseBtn->setChecked(true);
    diffuseBtn->setStyleSheet("color: white;");
    specBtn->setStyleSheet("color: white;");

    menuLayout->addWidget(diffuseBtn);
    menuLayout->addWidget(specBtn);
    menuLayout->addStretch();

    connect(diffuseBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(specBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
}

void GLLightingMaps::initializeGL()
{
    GLBase::initializeGL();

    // Compile lighting maps shader (diffuse + specular)
    if (!m_lightingProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lighting_maps/lighting_maps.vert") ||
        !m_lightingProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lighting_maps/lighting_maps_specular.frag"))
    {
        qFatal("Failed to compile lighting maps specular shader");
    }
    if (!m_lightingProgram.link())
    {
        qFatal("Failed to link lighting maps specular shader program");
    }

    // Compile lighting maps shader (diffuse only)
    if (!m_diffuseProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lighting_maps/lighting_maps.vert") ||
        !m_diffuseProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lighting_maps/lighting_maps_diffuse.frag"))
    {
        qFatal("Failed to compile lighting maps diffuse shader");
    }
    if (!m_diffuseProgram.link())
    {
        qFatal("Failed to link lighting maps diffuse shader program");
    }

    // Compile light cube shader
    if (!m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/color/light_cube.vert") ||
        !m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/color/light_cube.frag"))
    {
        qFatal("Failed to compile light cube shader");
    }
    if (!m_lightProgram.link())
    {
        qFatal("Failed to link light cube shader program");
    }

    // Set texture samplers for specular program
    m_lightingProgram.bind();
    m_lightingProgram.setUniformValue("material.diffuse", 0);
    m_lightingProgram.setUniformValue("material.specular", 1);

    // Set texture sampler for diffuse-only program
    m_diffuseProgram.bind();
    m_diffuseProgram.setUniformValue("material.diffuse", 0);

    // Vertex data: position(3) + normal(3) + texCoord(2) = 8 floats
    float vertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        // front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        // left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        // top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_cubeVAO);
    glGenVertexArrays(1, &m_lightVAO);
    glGenBuffers(1, &m_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Cube VAO: position(0), normal(1), texCoord(2) — 8 floats stride
    glBindVertexArray(m_cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Light VAO: position only (reuses same VBO with 8-float stride)
    glBindVertexArray(m_lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Load diffuse map
    m_diffuseMap = loadTexture(":/textures/container2.png");
    // Load specular map
    m_specularMap = loadTexture(":/textures/container2_specular.png");
}

void GLLightingMaps::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();
    const QVector3D viewPos = m_camera->getPosition();

    // ---- Draw main cube with lighting maps ----
    if (m_sceneIndex == 1) {
        // Scene 1: diffuse + specular
        m_lightingProgram.bind();

        m_lightingProgram.setUniformValue("light.position", m_lightPos);
        m_lightingProgram.setUniformValue("viewPos", viewPos);

        m_lightingProgram.setUniformValue("light.ambient",  0.2f, 0.2f, 0.2f);
        m_lightingProgram.setUniformValue("light.diffuse",  0.5f, 0.5f, 0.5f);
        m_lightingProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

        m_lightingProgram.setUniformValue("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        m_lightingProgram.setUniformValue("projection", projection);
        m_lightingProgram.setUniformValue("view", view);
        m_lightingProgram.setUniformValue("model", QMatrix4x4());

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_lightingProgram.release();
    } else {
        // Scene 0: diffuse only
        m_diffuseProgram.bind();

        m_diffuseProgram.setUniformValue("light.position", m_lightPos);
        m_diffuseProgram.setUniformValue("viewPos", viewPos);

        m_diffuseProgram.setUniformValue("light.ambient",  0.2f, 0.2f, 0.2f);
        m_diffuseProgram.setUniformValue("light.diffuse",  0.5f, 0.5f, 0.5f);
        m_diffuseProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

         // Material properties
        m_diffuseProgram.setUniformValue("material.specular",  0.5f, 0.5f, 0.5f);
        m_diffuseProgram.setUniformValue("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);

        m_diffuseProgram.setUniformValue("projection", projection);
        m_diffuseProgram.setUniformValue("view", view);
        m_diffuseProgram.setUniformValue("model", QMatrix4x4());

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_diffuseProgram.release();
    }

    // ---- Draw light cube ----
    drawLightCube(projection, view);
}

void GLLightingMaps::drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view)
{
    m_lightProgram.bind();
    m_lightProgram.setUniformValue("projection", projection);
    m_lightProgram.setUniformValue("view", view);

    QMatrix4x4 lightModel;
    lightModel.translate(m_lightPos);
    lightModel.scale(0.2f);
    m_lightProgram.setUniformValue("model", lightModel);

    glBindVertexArray(m_lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
