#include "header/gllightcasters.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QtMath>

GLLightCasters::GLLightCasters(QWidget *parent)
    : GLCameraBase(parent)
    , m_sceneIndex(0)
{
    setName("GLLightCasters");
    setupMenu();
}

void GLLightCasters::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *dirBtn = new QRadioButton("Directional", menu);
    QRadioButton *pointBtn = new QRadioButton("Point", menu);
    QRadioButton *spotBtn = new QRadioButton("Spot", menu);
    QRadioButton *spotSoftBtn = new QRadioButton("Spot Soft", menu);
    dirBtn->setChecked(true);
    dirBtn->setStyleSheet("color: white;");
    pointBtn->setStyleSheet("color: white;");
    spotBtn->setStyleSheet("color: white;");
    spotSoftBtn->setStyleSheet("color: white;");

    menuLayout->addWidget(dirBtn);
    menuLayout->addWidget(pointBtn);
    menuLayout->addWidget(spotBtn);
    menuLayout->addWidget(spotSoftBtn);
    menuLayout->addStretch();

    connect(dirBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(pointBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
    connect(spotBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 2;
    });
    connect(spotSoftBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 3;
    });
}

void GLLightCasters::initializeGL()
{
    GLBase::initializeGL();

    // Compile directional light shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/light_casters/light_casters_directional.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/light_casters/light_casters_directional.frag"))
    {
        qFatal("Failed to compile directional light shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link directional light shader program");
    }

    // Compile point light shader
    if (!m_pointProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/light_casters/light_casters_point.vert") ||
        !m_pointProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/light_casters/lights_casters_point.frag"))
    {
        qFatal("Failed to compile point light shader");
    }
    if (!m_pointProgram.link())
    {
        qFatal("Failed to link point light shader program");
    }

    // Compile light cube shader (reuse existing)
    if (!m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/color/light_cube.vert") ||
        !m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/color/light_cube.frag"))
    {
        qFatal("Failed to compile light cube shader");
    }
    if (!m_lightProgram.link())
    {
        qFatal("Failed to link light cube shader program");
    }

    // Compile spot light shader (reuse point vert + spot frag)
    if (!m_spotProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/light_casters/light_casters_point.vert") ||
        !m_spotProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/light_casters/light_casters_spot.frag"))
    {
        qFatal("Failed to compile spot light shader");
    }
    if (!m_spotProgram.link())
    {
        qFatal("Failed to link spot light shader program");
    }

    // Compile spot soft light shader (reuse point vert + spot soft frag)
    if (!m_spotSoftProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/light_casters/light_casters_point.vert") ||
        !m_spotSoftProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/light_casters/light_casters_spot_soft.frag"))
    {
        qFatal("Failed to compile spot soft light shader");
    }
    if (!m_spotSoftProgram.link())
    {
        qFatal("Failed to link spot soft light shader program");
    }

    // Set texture samplers for both programs
    m_program.bind();
    m_program.setUniformValue("material.diffuse", 0);
    m_program.setUniformValue("material.specular", 1);
    m_pointProgram.bind();
    m_pointProgram.setUniformValue("material.diffuse", 0);
    m_pointProgram.setUniformValue("material.specular", 1);
    m_spotProgram.bind();
    m_spotProgram.setUniformValue("material.diffuse", 0);
    m_spotProgram.setUniformValue("material.specular", 1);
    m_spotSoftProgram.bind();
    m_spotSoftProgram.setUniformValue("material.diffuse", 0);
    m_spotSoftProgram.setUniformValue("material.specular", 1);

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

    // Load textures
    m_diffuseMap = loadTexture("textures/container2.png");
    m_specularMap = loadTexture("textures/container2_specular.png");
}

void GLLightCasters::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();
    const QVector3D viewPos = m_camera->getPosition();

    // Positions for 10 containers
    const QVector3D cubePositions[] = {
        QVector3D( 0.0f,  0.0f,  0.0f),
        QVector3D( 2.0f,  5.0f, -15.0f),
        QVector3D(-1.5f, -2.2f, -2.5f),
        QVector3D(-3.8f, -2.0f, -12.3f),
        QVector3D( 2.4f, -0.4f, -3.5f),
        QVector3D(-1.7f,  3.0f, -7.5f),
        QVector3D( 1.3f, -2.0f, -2.5f),
        QVector3D( 1.5f,  2.0f, -2.5f),
        QVector3D( 1.5f,  0.2f, -1.5f),
        QVector3D(-1.3f,  1.0f, -1.5f)
    };

    // ---- Draw main cubes ----
    if (m_sceneIndex == 0) {
        // Scene 0: directional light
        m_program.bind();

        m_program.setUniformValue("light.direction", -0.2f, -1.0f, -0.3f);
        m_program.setUniformValue("light.ambient",   0.2f, 0.2f, 0.2f);
        m_program.setUniformValue("light.diffuse",   0.5f, 0.5f, 0.5f);
        m_program.setUniformValue("light.specular",  1.0f, 1.0f, 1.0f);

        m_program.setUniformValue("material.shininess", 32.0f);

        m_program.setUniformValue("viewPos", viewPos);
        m_program.setUniformValue("projection", projection);
        m_program.setUniformValue("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        glBindVertexArray(m_cubeVAO);
        for (int i = 0; i < 10; i++)
        {
            QMatrix4x4 model;
            model.translate(cubePositions[i]);
            model.rotate(20.0f * i, QVector3D(1.0f, 0.3f, 0.5f));
            m_program.setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        m_program.release();
    } else if (m_sceneIndex == 1) {
        // Scene 1: point light with attenuation
        m_pointProgram.bind();

        m_pointProgram.setUniformValue("light.position", m_lightPos);
        m_pointProgram.setUniformValue("viewPos", viewPos);

        m_pointProgram.setUniformValue("light.ambient",   0.2f, 0.2f, 0.2f);
        m_pointProgram.setUniformValue("light.diffuse",   0.5f, 0.5f, 0.5f);
        m_pointProgram.setUniformValue("light.specular",  1.0f, 1.0f, 1.0f);

        m_pointProgram.setUniformValue("light.constant",  1.0f);
        m_pointProgram.setUniformValue("light.linear",    0.09f);
        m_pointProgram.setUniformValue("light.quadratic", 0.032f);

        m_pointProgram.setUniformValue("material.shininess", 32.0f);

        m_pointProgram.setUniformValue("projection", projection);
        m_pointProgram.setUniformValue("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        glBindVertexArray(m_cubeVAO);
        for (int i = 0; i < 10; i++)
        {
            QMatrix4x4 model;
            model.translate(cubePositions[i]);
            model.rotate(20.0f * i, QVector3D(1.0f, 0.3f, 0.5f));
            m_pointProgram.setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        m_pointProgram.release();

        // Draw light cube
        drawLightCube(projection, view);
    } else if (m_sceneIndex == 2) {
        // Scene 2: spot light (hard edge)
        m_spotProgram.bind();

        m_spotProgram.setUniformValue("light.position", viewPos);
        m_spotProgram.setUniformValue("light.direction", m_camera->getFront());
        m_spotProgram.setUniformValue("light.cutOff", qCos(qDegreesToRadians(12.5f)));
        m_spotProgram.setUniformValue("light.outerCutOff", qCos(qDegreesToRadians(17.5f)));
        m_spotProgram.setUniformValue("viewPos", viewPos);

        m_spotProgram.setUniformValue("light.ambient",   0.1f, 0.1f, 0.1f);
        m_spotProgram.setUniformValue("light.diffuse",   0.8f, 0.8f, 0.8f);
        m_spotProgram.setUniformValue("light.specular",  1.0f, 1.0f, 1.0f);

        m_spotProgram.setUniformValue("light.constant",  1.0f);
        m_spotProgram.setUniformValue("light.linear",    0.09f);
        m_spotProgram.setUniformValue("light.quadratic", 0.032f);

        m_spotProgram.setUniformValue("material.shininess", 32.0f);

        m_spotProgram.setUniformValue("projection", projection);
        m_spotProgram.setUniformValue("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        glBindVertexArray(m_cubeVAO);
        for (int i = 0; i < 10; i++)
        {
            QMatrix4x4 model;
            model.translate(cubePositions[i]);
            model.rotate(20.0f * i, QVector3D(1.0f, 0.3f, 0.5f));
            m_spotProgram.setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        m_spotProgram.release();
    } else {
        // Scene 3: spot light (soft edge)
        m_spotSoftProgram.bind();

        m_spotSoftProgram.setUniformValue("light.position", viewPos);
        m_spotSoftProgram.setUniformValue("light.direction", m_camera->getFront());
        m_spotSoftProgram.setUniformValue("light.cutOff", qCos(qDegreesToRadians(12.5f)));
        m_spotSoftProgram.setUniformValue("light.outerCutOff", qCos(qDegreesToRadians(17.5f)));
        m_spotSoftProgram.setUniformValue("viewPos", viewPos);

        m_spotSoftProgram.setUniformValue("light.ambient",   0.1f, 0.1f, 0.1f);
        m_spotSoftProgram.setUniformValue("light.diffuse",   0.8f, 0.8f, 0.8f);
        m_spotSoftProgram.setUniformValue("light.specular",  1.0f, 1.0f, 1.0f);

        m_spotSoftProgram.setUniformValue("light.constant",  1.0f);
        m_spotSoftProgram.setUniformValue("light.linear",    0.09f);
        m_spotSoftProgram.setUniformValue("light.quadratic", 0.032f);

        m_spotSoftProgram.setUniformValue("material.shininess", 32.0f);

        m_spotSoftProgram.setUniformValue("projection", projection);
        m_spotSoftProgram.setUniformValue("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        glBindVertexArray(m_cubeVAO);
        for (int i = 0; i < 10; i++)
        {
            QMatrix4x4 model;
            model.translate(cubePositions[i]);
            model.rotate(20.0f * i, QVector3D(1.0f, 0.3f, 0.5f));
            m_spotSoftProgram.setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        m_spotSoftProgram.release();
    }
}

void GLLightCasters::drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view)
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
