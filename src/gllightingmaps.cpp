#include "header/gllightingmaps.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QRadioButton>
#include <QVBoxLayout>
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
    QRadioButton *specInvBtn = new QRadioButton("Diff+Spec Inv (Ex. 2)", menu);
    QRadioButton *specColorBtn = new QRadioButton("Colored Specular (Ex. 3)", menu);
    QRadioButton *emissionBtn = new QRadioButton("Emission (Ex. 4)", menu);
    diffuseBtn->setChecked(true);
    diffuseBtn->setStyleSheet("color: white;");
    specBtn->setStyleSheet("color: white;");
    specInvBtn->setStyleSheet("color: white;");
    specColorBtn->setStyleSheet("color: white;");
    emissionBtn->setStyleSheet("color: white;");

    menuLayout->addWidget(diffuseBtn);
    menuLayout->addWidget(specBtn);
    menuLayout->addWidget(specInvBtn);
    menuLayout->addWidget(specColorBtn);
    menuLayout->addWidget(emissionBtn);
    menuLayout->addStretch();

    // 光源动画开关（底部靠右）
    QCheckBox *animateChk = new QCheckBox("Animate Light(Author Enhanced)", menu);
    animateChk->setStyleSheet("color: white; font-size: 8pt;");

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(animateChk);
    menuLayout->addLayout(bottomLayout);

    connect(diffuseBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(specBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
    connect(specInvBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 2;
    });
    connect(specColorBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 3;
    });
    connect(emissionBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 4;
    });
    connect(animateChk, &QCheckBox::toggled, this, [this](bool checked) {
        m_animateLight = checked;
    });
}

void GLLightingMaps::initializeGL()
{
    GLBase::initializeGL();

    // Compile lighting maps shader (diffuse + specular)
    if (!m_specularProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/lighting_maps/lighting_maps.vert") ||
        !m_specularProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/lighting_maps/lighting_maps_specular.frag"))
    {
        qFatal("Failed to compile lighting maps specular shader");
    }
    if (!m_specularProgram.link())
    {
        qFatal("Failed to link lighting maps specular shader program");
    }

    // Compile lighting maps shader (diffuse + inverted specular)
    if (!m_specInvProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/lighting_maps/lighting_maps.vert") ||
        !m_specInvProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/lighting_maps/lighting_maps_specular_inv.frag"))
    {
        qFatal("Failed to compile lighting maps inverted specular shader");
    }
    if (!m_specInvProgram.link())
    {
        qFatal("Failed to link lighting maps inverted specular shader program");
    }

    // Compile lighting maps shader (diffuse + specular + emission)
    if (!m_emissionProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/lighting_maps/lighting_maps.vert") ||
        !m_emissionProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/lighting_maps/lighting_maps_emission.frag"))
    {
        qFatal("Failed to compile lighting maps emission shader");
    }
    if (!m_emissionProgram.link())
    {
        qFatal("Failed to link lighting maps emission shader program");
    }

    // Compile lighting maps shader (diffuse only)
    if (!m_diffuseProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/lighting_maps/lighting_maps.vert") ||
        !m_diffuseProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/lighting_maps/lighting_maps_diffuse.frag"))
    {
        qFatal("Failed to compile lighting maps diffuse shader");
    }
    if (!m_diffuseProgram.link())
    {
        qFatal("Failed to link lighting maps diffuse shader program");
    }

    // Compile light cube shader
    if (!m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/color/light_cube.vert") ||
        !m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/color/light_cube.frag"))
    {
        qFatal("Failed to compile light cube shader");
    }
    if (!m_lightProgram.link())
    {
        qFatal("Failed to link light cube shader program");
    }

    // Set texture samplers for specular program
    m_specularProgram.bind();
    m_specularProgram.setUniformValue("material.diffuse", 0);
    m_specularProgram.setUniformValue("material.specular", 1);

    // Set texture samplers for inverted specular program
    m_specInvProgram.bind();
    m_specInvProgram.setUniformValue("material.diffuse", 0);
    m_specInvProgram.setUniformValue("material.specular", 1);

    // Set texture sampler for diffuse-only program
    m_diffuseProgram.bind();
    m_diffuseProgram.setUniformValue("material.diffuse", 0);

    // Set texture samplers for emission program
    m_emissionProgram.bind();
    m_emissionProgram.setUniformValue("material.diffuse", 0);
    m_emissionProgram.setUniformValue("material.specular", 1);
    m_emissionProgram.setUniformValue("material.emission", 2);

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
    m_diffuseMap = loadTexture("textures/container2.png");
    // Load specular map
    m_specularMap = loadTexture("textures/container2_specular.png");
    // Load colored specular map (Exercise 3)
    m_specColoredMap = loadTexture("textures/container2_specular_colored.png");
    // Load emission map
    m_emissionMap = loadTexture("textures/matrix.jpg");
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

    // Animate light position
    const float time = elapsedTime();
    if (m_animateLight) {
        m_lightPos.setX(1.0f + qSin(time) * 2.0f);
        m_lightPos.setY(qSin(time / 2.0f) * 1.0f);
        m_lightPos.setZ(m_lightPos.z());
    }

    // ---- Draw main cube with lighting maps ----
    if (m_sceneIndex == 1) {
        // Scene 1: diffuse + specular
        m_specularProgram.bind();

        m_specularProgram.setUniformValue("light.position", m_lightPos);
        m_specularProgram.setUniformValue("viewPos", viewPos);

        m_specularProgram.setUniformValue("light.ambient",  0.2f, 0.2f, 0.2f);
        m_specularProgram.setUniformValue("light.diffuse",  0.5f, 0.5f, 0.5f);
        m_specularProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

        m_specularProgram.setUniformValue("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        m_specularProgram.setUniformValue("projection", projection);
        m_specularProgram.setUniformValue("view", view);
        m_specularProgram.setUniformValue("model", QMatrix4x4());

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_specularProgram.release();
    } else if (m_sceneIndex == 2) {
        // Scene 2: diffuse + inverted specular
        m_specInvProgram.bind();

        m_specInvProgram.setUniformValue("light.position", m_lightPos);
        m_specInvProgram.setUniformValue("viewPos", viewPos);

        m_specInvProgram.setUniformValue("light.ambient",  0.2f, 0.2f, 0.2f);
        m_specInvProgram.setUniformValue("light.diffuse",  0.5f, 0.5f, 0.5f);
        m_specInvProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

        m_specInvProgram.setUniformValue("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);

        m_specInvProgram.setUniformValue("projection", projection);
        m_specInvProgram.setUniformValue("view", view);
        m_specInvProgram.setUniformValue("model", QMatrix4x4());

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_specInvProgram.release();
    } else if (m_sceneIndex == 3) {
        // Scene 3 (Exercise 3): diffuse + colored specular map
        m_specularProgram.bind();

        m_specularProgram.setUniformValue("light.position", m_lightPos);
        m_specularProgram.setUniformValue("viewPos", viewPos);

        m_specularProgram.setUniformValue("light.ambient",  0.2f, 0.2f, 0.2f);
        m_specularProgram.setUniformValue("light.diffuse",  0.5f, 0.5f, 0.5f);
        m_specularProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

        m_specularProgram.setUniformValue("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specColoredMap);

        m_specularProgram.setUniformValue("projection", projection);
        m_specularProgram.setUniformValue("view", view);
        m_specularProgram.setUniformValue("model", QMatrix4x4());

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_specularProgram.release();
    } else if (m_sceneIndex == 4) {
        // Scene 4: diffuse + specular + emission
        m_emissionProgram.bind();

        m_emissionProgram.setUniformValue("light.position", m_lightPos);
        m_emissionProgram.setUniformValue("viewPos", viewPos);

        m_emissionProgram.setUniformValue("light.ambient",  0.2f, 0.2f, 0.2f);
        m_emissionProgram.setUniformValue("light.diffuse",  0.5f, 0.5f, 0.5f);
        m_emissionProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

        m_emissionProgram.setUniformValue("material.shininess", 64.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_specularMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_emissionMap);

        m_emissionProgram.setUniformValue("projection", projection);
        m_emissionProgram.setUniformValue("view", view);
        m_emissionProgram.setUniformValue("model", QMatrix4x4());

        glBindVertexArray(m_cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_emissionProgram.release();
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
