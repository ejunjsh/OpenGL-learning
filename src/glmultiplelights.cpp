#include "header/glmultiplelights.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QRadiobutton>
#include <QVBoxLayout>
#include <QtMath>

GLMultipleLights::GLMultipleLights(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLMultipleLights");
    setupMenu();
}

void GLMultipleLights::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *defaultBtn = new QRadioButton("Default", menu);
    QRadioButton *desertBtn = new QRadioButton("Desert", menu);
    QRadioButton *factoryBtn = new QRadioButton("Factory", menu);
    QRadioButton *horrorBtn = new QRadioButton("Horror", menu);
    QRadioButton *bioBtn = new QRadioButton("Biochemical Lab", menu);
    defaultBtn->setChecked(true);
    defaultBtn->setStyleSheet("color: white;");
    desertBtn->setStyleSheet("color: white;");
    factoryBtn->setStyleSheet("color: white;");
    horrorBtn->setStyleSheet("color: white;");
    bioBtn->setStyleSheet("color: white;");

    menuLayout->addWidget(defaultBtn);
    menuLayout->addWidget(desertBtn);
    menuLayout->addWidget(factoryBtn);
    menuLayout->addWidget(horrorBtn);
    menuLayout->addWidget(bioBtn);
    menuLayout->addStretch();

    connect(defaultBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(desertBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
    connect(factoryBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 2;
    });
    connect(horrorBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 3;
    });
    connect(bioBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 4;
    });
}

void GLMultipleLights::initializeGL()
{
    GLBase::initializeGL();

    // Compile main shader
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/light_casters/light_casters_point.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/light_casters/multiple_lights.frag"))
    {
        qFatal("Failed to compile multiple lights shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link multiple lights shader program");
    }

    // Compile light cube shader
    if (!m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/color/light_cube.vert") ||
        !m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/color/light_cube_colored.frag"))
    {
        qFatal("Failed to compile light cube shader");
    }
    if (!m_lightProgram.link())
    {
        qFatal("Failed to link light cube shader program");
    }

    // Set texture samplers
    m_program.bind();
    m_program.setUniformValue("material.diffuse", 0);
    m_program.setUniformValue("material.specular", 1);

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

    // Light VAO: position only
    glBindVertexArray(m_lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Load textures
    m_diffuseMap = loadTexture("textures/container2.png");
    m_specularMap = loadTexture("textures/container2_specular.png");
}

void GLMultipleLights::paintGL()
{
    // ---- Scene-dependent global parameters ----
    QVector3D clearColor;
    QVector3D pointLightColors[4];
    QVector3D dirAmbient, dirDiffuse, dirSpecular;
    float spotCutOff, spotOuterCutOff;
    QVector3D spotAmbient, spotDiffuse, spotSpecular;
    float spotConstant, spotLinear, spotQuadratic;

    switch (m_sceneIndex)
    {
    default:
    case 0:  // DEFAULT
        clearColor = QVector3D(0.1f, 0.1f, 0.1f);
        pointLightColors[0] = QVector3D(1.0f, 1.0f, 1.0f);
        pointLightColors[1] = QVector3D(1.0f, 1.0f, 1.0f);
        pointLightColors[2] = QVector3D(1.0f, 1.0f, 1.0f);
        pointLightColors[3] = QVector3D(1.0f, 1.0f, 1.0f);
        dirAmbient  = QVector3D(0.05f, 0.05f, 0.05f);
        dirDiffuse  = QVector3D(0.4f, 0.4f, 0.4f);
        dirSpecular = QVector3D(0.5f, 0.5f, 0.5f);
        spotCutOff      = 12.5f;
        spotOuterCutOff = 15.0f;
        spotAmbient  = QVector3D(0.0f, 0.0f, 0.0f);
        spotDiffuse  = QVector3D(1.0f, 1.0f, 1.0f);
        spotSpecular = QVector3D(1.0f, 1.0f, 1.0f);
        spotConstant  = 1.0f;
        spotLinear    = 0.09f;
        spotQuadratic = 0.032f;
        break;

    case 1:  // DESERT
        clearColor = QVector3D(0.75f, 0.52f, 0.3f);
        pointLightColors[0] = QVector3D(1.0f, 0.6f, 0.0f);
        pointLightColors[1] = QVector3D(1.0f, 0.0f, 0.0f);
        pointLightColors[2] = QVector3D(1.0f, 1.0f, 0.0f);
        pointLightColors[3] = QVector3D(0.2f, 0.2f, 1.0f);
        dirAmbient  = QVector3D(0.3f, 0.24f, 0.14f);
        dirDiffuse  = QVector3D(0.7f, 0.42f, 0.26f);
        dirSpecular = QVector3D(0.5f, 0.5f, 0.5f);
        spotCutOff      = 12.5f;
        spotOuterCutOff = 13.0f;
        spotAmbient  = QVector3D(0.0f, 0.0f, 0.0f);
        spotDiffuse  = QVector3D(0.8f, 0.8f, 0.0f);
        spotSpecular = QVector3D(0.8f, 0.8f, 0.0f);
        spotConstant  = 1.0f;
        spotLinear    = 0.09f;
        spotQuadratic = 0.032f;
        break;

    case 2:  // FACTORY
        clearColor = QVector3D(0.1f, 0.1f, 0.1f);
        pointLightColors[0] = QVector3D(0.2f, 0.2f, 0.6f);
        pointLightColors[1] = QVector3D(0.3f, 0.3f, 0.7f);
        pointLightColors[2] = QVector3D(0.0f, 0.0f, 0.3f);
        pointLightColors[3] = QVector3D(0.4f, 0.4f, 0.4f);
        dirAmbient  = QVector3D(0.05f, 0.05f, 0.1f);
        dirDiffuse  = QVector3D(0.2f, 0.2f, 0.7f);
        dirSpecular = QVector3D(0.7f, 0.7f, 0.7f);
        spotCutOff      = 10.0f;
        spotOuterCutOff = 12.5f;
        spotAmbient  = QVector3D(0.0f, 0.0f, 0.0f);
        spotDiffuse  = QVector3D(1.0f, 1.0f, 1.0f);
        spotSpecular = QVector3D(1.0f, 1.0f, 1.0f);
        spotConstant  = 1.0f;
        spotLinear    = 0.009f;
        spotQuadratic = 0.0032f;
        break;

    case 3:  // HORROR
        clearColor = QVector3D(0.0f, 0.0f, 0.0f);
        pointLightColors[0] = QVector3D(0.1f, 0.1f, 0.1f);
        pointLightColors[1] = QVector3D(0.1f, 0.1f, 0.1f);
        pointLightColors[2] = QVector3D(0.1f, 0.1f, 0.1f);
        pointLightColors[3] = QVector3D(0.3f, 0.1f, 0.1f);
        dirAmbient  = QVector3D(0.0f, 0.0f, 0.0f);
        dirDiffuse  = QVector3D(0.05f, 0.05f, 0.05f);
        dirSpecular = QVector3D(0.2f, 0.2f, 0.2f);
        spotCutOff      = 10.0f;
        spotOuterCutOff = 15.0f;
        spotAmbient  = QVector3D(0.0f, 0.0f, 0.0f);
        spotDiffuse  = QVector3D(1.0f, 1.0f, 1.0f);
        spotSpecular = QVector3D(1.0f, 1.0f, 1.0f);
        spotConstant  = 1.0f;
        spotLinear    = 0.09f;
        spotQuadratic = 0.032f;
        break;

    case 4:  // BIOCHEMICAL LAB
        clearColor = QVector3D(0.9f, 0.9f, 0.9f);
        pointLightColors[0] = QVector3D(0.4f, 0.7f, 0.1f);
        pointLightColors[1] = QVector3D(0.4f, 0.7f, 0.1f);
        pointLightColors[2] = QVector3D(0.4f, 0.7f, 0.1f);
        pointLightColors[3] = QVector3D(0.4f, 0.7f, 0.1f);
        dirAmbient  = QVector3D(0.5f, 0.5f, 0.5f);
        dirDiffuse  = QVector3D(1.0f, 1.0f, 1.0f);
        dirSpecular = QVector3D(1.0f, 1.0f, 1.0f);
        spotCutOff      = 7.0f;
        spotOuterCutOff = 10.0f;
        spotAmbient  = QVector3D(0.0f, 0.0f, 0.0f);
        spotDiffuse  = QVector3D(0.0f, 1.0f, 0.0f);
        spotSpecular = QVector3D(0.0f, 1.0f, 0.0f);
        spotConstant  = 1.0f;
        spotLinear    = 0.07f;
        spotQuadratic = 0.017f;
        break;
    }

    glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), 1.0f);
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

    // 4 point light positions (static)
    const QVector3D pointLightPositions[] = {
        QVector3D( 0.7f,  0.2f,  2.0f),
        QVector3D( 2.3f, -3.3f, -4.0f),
        QVector3D(-4.0f,  2.0f, -12.0f),
        QVector3D( 0.0f,  0.0f, -3.0f)
    };

    // ---- Draw main cubes ----
    m_program.bind();

    // Directional light
    m_program.setUniformValue("dirLight.direction", -0.2f, -1.0f, -0.3f);
    m_program.setUniformValue("dirLight.ambient",  dirAmbient);
    m_program.setUniformValue("dirLight.diffuse",  dirDiffuse);
    m_program.setUniformValue("dirLight.specular", dirSpecular);

    // Point lights (4)
    static const float pointAttenConstant  = 1.0f;
    static const float pointAttenLinear    = 0.09f;
    static const float pointAttenQuadratic = 0.032f;

    for (int i = 0; i < 4; i++)
    {
        const QString prefix = QString("pointLights[%1].").arg(i);
        m_program.setUniformValue((prefix + "position").toUtf8(), pointLightPositions[i]);
        m_program.setUniformValue((prefix + "ambient").toUtf8(),
                                  pointLightColors[i] * 0.1f);
        m_program.setUniformValue((prefix + "diffuse").toUtf8(),  pointLightColors[i]);
        m_program.setUniformValue((prefix + "specular").toUtf8(), pointLightColors[i]);
        m_program.setUniformValue((prefix + "constant").toUtf8(),  pointAttenConstant);
        m_program.setUniformValue((prefix + "linear").toUtf8(),    pointAttenLinear);
        m_program.setUniformValue((prefix + "quadratic").toUtf8(), pointAttenQuadratic);
    }

    // Spot light (flashlight at camera)
    m_program.setUniformValue("spotLight.position", viewPos);
    m_program.setUniformValue("spotLight.direction", m_camera->getFront());
    m_program.setUniformValue("spotLight.cutOff",      qCos(qDegreesToRadians(spotCutOff)));
    m_program.setUniformValue("spotLight.outerCutOff", qCos(qDegreesToRadians(spotOuterCutOff)));
    m_program.setUniformValue("spotLight.ambient",  spotAmbient);
    m_program.setUniformValue("spotLight.diffuse",  spotDiffuse);
    m_program.setUniformValue("spotLight.specular", spotSpecular);
    m_program.setUniformValue("spotLight.constant",  spotConstant);
    m_program.setUniformValue("spotLight.linear",    spotLinear);
    m_program.setUniformValue("spotLight.quadratic", spotQuadratic);

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

    // ---- Draw light cubes for point lights ----
    for (int i = 0; i < 4; i++)
    {
        drawLightCube(projection, view, pointLightPositions[i], pointLightColors[i]);
    }
}

void GLMultipleLights::drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view,
                                     const QVector3D &position, const QVector3D &color)
{
    m_lightProgram.bind();
    m_lightProgram.setUniformValue("projection", projection);
    m_lightProgram.setUniformValue("view", view);
    m_lightProgram.setUniformValue("lightColor", color);

    QMatrix4x4 lightModel;
    lightModel.translate(position);
    lightModel.scale(0.2f);
    m_lightProgram.setUniformValue("model", lightModel);

    glBindVertexArray(m_lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
