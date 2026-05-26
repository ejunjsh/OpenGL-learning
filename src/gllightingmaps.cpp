#include "header/gllightingmaps.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QtMath>

GLLightingMaps::GLLightingMaps(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLLightingMaps");
    setupMenu();
}

void GLLightingMaps::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);
    menuLayout->addStretch();
}

void GLLightingMaps::initializeGL()
{
    GLBase::initializeGL();

    // Compile lighting maps shader
    if (!m_lightingProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lighting_maps/lighting_maps.vert") ||
        !m_lightingProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lighting_maps/lighting_maps.frag"))
    {
        qFatal("Failed to compile lighting maps shader");
    }
    if (!m_lightingProgram.link())
    {
        qFatal("Failed to link lighting maps shader program");
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

    // Set texture samplers
    m_lightingProgram.bind();
    m_lightingProgram.setUniformValue("material.diffuse", 0);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    m_lightingProgram.bind();

    m_lightingProgram.setUniformValue("light.position", m_lightPos);
    m_lightingProgram.setUniformValue("viewPos", viewPos);

    // Light properties
    m_lightingProgram.setUniformValue("light.ambient",  0.2f, 0.2f, 0.2f);
    m_lightingProgram.setUniformValue("light.diffuse",  0.5f, 0.5f, 0.5f);
    m_lightingProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

    // Material properties
    m_lightingProgram.setUniformValue("material.specular",  0.5f, 0.5f, 0.5f);
    m_lightingProgram.setUniformValue("material.shininess", 64.0f);

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_diffuseMap);

    // Matrices
    m_lightingProgram.setUniformValue("projection", projection);
    m_lightingProgram.setUniformValue("view", view);
    m_lightingProgram.setUniformValue("model", QMatrix4x4());

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    m_lightingProgram.release();

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
