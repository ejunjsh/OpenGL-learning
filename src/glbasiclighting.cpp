#include "header/glbasiclighting.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QtMath>

GLBasicLighting::GLBasicLighting(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLBasicLighting");
    setupMenu();
}

void GLBasicLighting::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *scene1Btn = new QRadioButton("Diffuse", menu);
    QRadioButton *scene2Btn = new QRadioButton("Specular", menu);
    QRadioButton *scene3Btn = new QRadioButton("Moving(Exercise 1)", menu);
    QRadioButton *scene4Btn = new QRadioButton("View Space(Exercise 2)", menu);
    QRadioButton *scene5Btn = new QRadioButton("Gouraud(Exercise 3)", menu);
    scene1Btn->setChecked(true);
    scene1Btn->setStyleSheet("color: white;");
    scene2Btn->setStyleSheet("color: white;");
    scene3Btn->setStyleSheet("color: white;");
    scene4Btn->setStyleSheet("color: white;");
    scene5Btn->setStyleSheet("color: white;");

    menuLayout->addWidget(scene1Btn);
    menuLayout->addWidget(scene2Btn);
    menuLayout->addWidget(scene3Btn);
    menuLayout->addWidget(scene4Btn);
    menuLayout->addWidget(scene5Btn);
    menuLayout->addStretch();

    connect(scene1Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(scene2Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
    connect(scene3Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 2;
    });
    connect(scene4Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 3;
    });
    connect(scene5Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 4;
    });
}

void GLBasicLighting::initializeGL()
{
    GLBase::initializeGL();

    // 编译 diffuse 光照着色器
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic_lighting/basic_lighting.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_lighting/basic_lighting_diffuse.frag"))
    {
        qFatal("Failed to compile diffuse lighting shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link diffuse lighting shader program");
    }

    // 编译 specular 光照着色器（共用顶点着色器）
    if (!m_specularProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic_lighting/basic_lighting.vert") ||
        !m_specularProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_lighting/basic_lighting_specular.frag"))
    {
        qFatal("Failed to compile specular lighting shader");
    }
    if (!m_specularProgram.link())
    {
        qFatal("Failed to link specular lighting shader program");
    }

    // 编译 view-space 光照着色器
    if (!m_viewSpaceProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic_lighting/basic_lighting_viewspace.vert") ||
        !m_viewSpaceProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_lighting/basic_lighting_viewspace.frag"))
    {
        qFatal("Failed to compile view-space lighting shader");
    }
    if (!m_viewSpaceProgram.link())
    {
        qFatal("Failed to link view-space lighting shader program");
    }

    // 编译 Gouraud 着色器（逐顶点光照）
    if (!m_gouraudProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basic_lighting/basic_lighting_gouraud.vert") ||
        !m_gouraudProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basic_lighting/basic_lighting_gouraud.frag"))
    {
        qFatal("Failed to compile Gouraud lighting shader");
    }
    if (!m_gouraudProgram.link())
    {
        qFatal("Failed to link Gouraud lighting shader program");
    }

    // 编译光源立方体着色器（复用 color/light_cube）
    if (!m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/color/light_cube.vert") ||
        !m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/color/light_cube.frag"))
    {
        qFatal("Failed to compile light cube shader");
    }
    if (!m_lightProgram.link())
    {
        qFatal("Failed to link light cube shader program");
    }

    // 立方体顶点数据（位置 + 法线）
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    };

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 配置立方体 VAO（含位置 + 法线）
    glGenVertexArrays(1, &m_cubeVAO);
    glBindVertexArray(m_cubeVAO);
    // 位置属性 location=0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 法线属性 location=1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 配置光源立方体 VAO（仅位置）
    glGenVertexArrays(1, &m_lightVAO);
    glBindVertexArray(m_lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void GLBasicLighting::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();
    const QVector3D viewPos = m_camera->getPosition();

    if (m_sceneIndex == 0)
    {
        drawDiffuse(projection, view);
        drawLightCube(projection, view, m_lightPos);
    }
    else if (m_sceneIndex == 1)
    {
        drawSpecular(projection, view, viewPos);
        drawLightCube(projection, view, m_lightPos);
    }
    else if (m_sceneIndex == 2)
    {
        // 光源位置随时间变化
        const float time = elapsedTime();
        QVector3D dynamicLightPos;
        dynamicLightPos.setX(1.0f + qSin(time) * 2.0f);
        dynamicLightPos.setY(qSin(time / 2.0f) * 1.0f);
        dynamicLightPos.setZ(m_lightPos.z());

        drawMoving(projection, view, viewPos, dynamicLightPos);
        drawLightCube(projection, view, dynamicLightPos);
    }
    else if (m_sceneIndex == 3)
    {
        const float time = elapsedTime();
        QVector3D dynamicLightPos;
        dynamicLightPos.setX(1.0f + qSin(time) * 2.0f);
        dynamicLightPos.setY(qSin(time / 2.0f) * 1.0f);
        dynamicLightPos.setZ(m_lightPos.z());

        drawViewSpace(projection, view, dynamicLightPos);
        drawLightCube(projection, view, dynamicLightPos);
    }
    else
    {
        const float time = elapsedTime();
        QVector3D dynamicLightPos;
        dynamicLightPos.setX(1.0f + qSin(time) * 2.0f);
        dynamicLightPos.setY(qSin(time / 2.0f) * 1.0f);
        dynamicLightPos.setZ(m_lightPos.z());

        drawGouraud(projection, view, viewPos, dynamicLightPos);
        drawLightCube(projection, view, dynamicLightPos);
    }
}

void GLBasicLighting::drawDiffuse(const QMatrix4x4 &projection, const QMatrix4x4 &view)
{
    m_program.bind();
    m_program.setUniformValue("objectColor", 1.0f, 0.5f, 0.31f);
    m_program.setUniformValue("lightColor", 1.0f, 1.0f, 1.0f);
    m_program.setUniformValue("lightPos", m_lightPos);
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);

    QMatrix4x4 model;
    m_program.setUniformValue("model", model);

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLBasicLighting::drawSpecular(const QMatrix4x4 &projection, const QMatrix4x4 &view, const QVector3D &viewPos)
{
    m_specularProgram.bind();
    m_specularProgram.setUniformValue("objectColor", 1.0f, 0.5f, 0.31f);
    m_specularProgram.setUniformValue("lightColor", 1.0f, 1.0f, 1.0f);
    m_specularProgram.setUniformValue("lightPos", m_lightPos);
    m_specularProgram.setUniformValue("viewPos", viewPos);
    m_specularProgram.setUniformValue("projection", projection);
    m_specularProgram.setUniformValue("view", view);

    QMatrix4x4 model;
    m_specularProgram.setUniformValue("model", model);

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLBasicLighting::drawMoving(const QMatrix4x4 &projection, const QMatrix4x4 &view, const QVector3D &viewPos, const QVector3D &lightPos)
{
    m_specularProgram.bind();
    m_specularProgram.setUniformValue("objectColor", 1.0f, 0.5f, 0.31f);
    m_specularProgram.setUniformValue("lightColor", 1.0f, 1.0f, 1.0f);
    m_specularProgram.setUniformValue("lightPos", lightPos);
    m_specularProgram.setUniformValue("viewPos", viewPos);
    m_specularProgram.setUniformValue("projection", projection);
    m_specularProgram.setUniformValue("view", view);

    QMatrix4x4 model;
    m_specularProgram.setUniformValue("model", model);

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLBasicLighting::drawViewSpace(const QMatrix4x4 &projection, const QMatrix4x4 &view, const QVector3D &lightPos)
{
    // view-space: lightPos 传入 VS，VS 将其变换到观察空间再传给 FS
    // viewDir = -FragPos（观察空间中相机在原点），不需要 viewPos uniform
    m_viewSpaceProgram.bind();
    m_viewSpaceProgram.setUniformValue("objectColor", 1.0f, 0.5f, 0.31f);
    m_viewSpaceProgram.setUniformValue("lightColor", 1.0f, 1.0f, 1.0f);
    m_viewSpaceProgram.setUniformValue("lightPos", lightPos); // world-space, VS transforms to view-space
    m_viewSpaceProgram.setUniformValue("projection", projection);
    m_viewSpaceProgram.setUniformValue("view", view);

    QMatrix4x4 model;
    m_viewSpaceProgram.setUniformValue("model", model);

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLBasicLighting::drawGouraud(const QMatrix4x4 &projection, const QMatrix4x4 &view, const QVector3D &viewPos, const QVector3D &lightPos)
{
    // Gouraud shading: 光照计算在 VS 中逐顶点完成，FS 只做插值后的颜色乘法
    // specular strength=1.0 以更明显展示 Gouraud 的视觉缺陷
    m_gouraudProgram.bind();
    m_gouraudProgram.setUniformValue("objectColor", 1.0f, 0.5f, 0.31f);
    m_gouraudProgram.setUniformValue("lightColor", 1.0f, 1.0f, 1.0f);
    m_gouraudProgram.setUniformValue("lightPos", lightPos);
    m_gouraudProgram.setUniformValue("viewPos", viewPos);
    m_gouraudProgram.setUniformValue("projection", projection);
    m_gouraudProgram.setUniformValue("view", view);

    QMatrix4x4 model;
    m_gouraudProgram.setUniformValue("model", model);

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLBasicLighting::drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view, const QVector3D &lightPos)
{
    m_lightProgram.bind();
    m_lightProgram.setUniformValue("projection", projection);
    m_lightProgram.setUniformValue("view", view);

    QMatrix4x4 lightModel;
    lightModel.translate(lightPos);
    lightModel.scale(0.2f);
    m_lightProgram.setUniformValue("model", lightModel);

    glBindVertexArray(m_lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_lightProgram.release();
}
