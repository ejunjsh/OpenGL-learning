#include "header/glmaterials.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QtMath>

GLMaterials::GLMaterials(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLMaterials");
    setupMenu();
}

void GLMaterials::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    // 场景选择
    QRadioButton *scene1Btn = new QRadioButton("Light Color", menu);
    QRadioButton *scene2Btn = new QRadioButton("Materials (Exercise 1)", menu);
    scene1Btn->setChecked(true);
    scene1Btn->setStyleSheet("color: white;");
    scene2Btn->setStyleSheet("color: white;");

    menuLayout->addWidget(scene1Btn);
    menuLayout->addWidget(scene2Btn);

    // 材质下拉框（仅场景2可见）
    m_materialCombo = new QComboBox(menu);
    m_materialCombo->setStyleSheet("color: white;");
    menuLayout->addWidget(m_materialCombo);

    // Author Enhanced 标签（仅场景2可见）
    m_enhancedLabel = new QLabel("(Author Enhanced)", menu);
    m_enhancedLabel->setStyleSheet("color: gray; font-size: 8pt;");
    menuLayout->addWidget(m_enhancedLabel);

    // 初始隐藏
    m_materialCombo->hide();
    m_enhancedLabel->hide();

    menuLayout->addStretch();

    // 光源动画开关（底部靠右）
    QCheckBox *animateChk = new QCheckBox("Animate Light(Author Enhanced)", menu);
    animateChk->setStyleSheet("color: white; font-size: 8pt;");

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    bottomLayout->addWidget(animateChk);
    menuLayout->addLayout(bottomLayout);

    m_materials.clear();
    predefinedMaterials(m_materials);
    for (const auto &mat : m_materials) {
        m_materialCombo->addItem(mat.name);
    }

    connect(scene1Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 0;
            m_materialCombo->hide();
            m_enhancedLabel->hide();
        }
    });
    connect(scene2Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 1;
            m_materialCombo->show();
            m_enhancedLabel->show();
        }
    });

    connect(m_materialCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
        if (index >= 0) {
            m_materialIndex = index;
        }
    });

    connect(animateChk, &QCheckBox::toggled, this, [this](bool checked) {
        m_animateLight = checked;
    });
}

void GLMaterials::initializeGL()
{
    GLBase::initializeGL();

    // 编译 materials 着色器
    if (!m_materialProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/materials/materials.vert") ||
        !m_materialProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/materials/materials.frag"))
    {
        qFatal("Failed to compile materials shader");
    }
    if (!m_materialProgram.link())
    {
        qFatal("Failed to link materials shader program");
    }

    // 编译光源立方体着色器（复用 color/light_cube）
    if (!m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/color/light_cube.vert") ||
        !m_lightProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/color/light_cube.frag"))
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

    // 配置立方体 VAO（位置 + 法线）
    glGenVertexArrays(1, &m_cubeVAO);
    glBindVertexArray(m_cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 配置光源立方体 VAO（仅位置）
    glGenVertexArrays(1, &m_lightVAO);
    glBindVertexArray(m_lightVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void GLMaterials::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();
    const QVector3D viewPos = m_camera->getPosition();

    // ---- 设置 light 和 material 属性 ----
    m_materialProgram.bind();

    const float time = elapsedTime();
    if (m_animateLight) {
        m_lightPos.setX(1.0f + qSin(time) * 2.0f);
        m_lightPos.setY(qSin(time / 2.0f) * 1.0f);
        m_lightPos.setZ(m_lightPos.z());
    }

    if (m_sceneIndex == 0)
    {
        // 场景0: 光源颜色随时间变化
        QVector3D lightColor;
        lightColor.setX(qSin(time * 2.0f));
        lightColor.setY(qSin(time * 0.7f));
        lightColor.setZ(qSin(time * 1.3f));

        const QVector3D diffuseColor = lightColor * 0.5f;
        const QVector3D ambientColor = diffuseColor * 0.2f;

        m_materialProgram.setUniformValue("light.ambient",  ambientColor);
        m_materialProgram.setUniformValue("light.diffuse",  diffuseColor);
        m_materialProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

        m_materialProgram.setUniformValue("material.ambient",  1.0f, 0.5f, 0.31f);
        m_materialProgram.setUniformValue("material.diffuse",  1.0f, 0.5f, 0.31f);
        m_materialProgram.setUniformValue("material.specular", 0.5f, 0.5f, 0.5f);
        m_materialProgram.setUniformValue("material.shininess", 32.0f);
    }
    else
    {
        // 场景2: 全白光 + 下拉框选中的材质
        m_materialProgram.setUniformValue("light.ambient",  1.0f, 1.0f, 1.0f);
        m_materialProgram.setUniformValue("light.diffuse",  1.0f, 1.0f, 1.0f);
        m_materialProgram.setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);

        const MaterialProperties &mat = m_materials.at(m_materialIndex);
        m_materialProgram.setUniformValue("material.ambient",  mat.ambient);
        m_materialProgram.setUniformValue("material.diffuse",  mat.diffuse);
        m_materialProgram.setUniformValue("material.specular", mat.specular);
        m_materialProgram.setUniformValue("material.shininess", mat.shininess);
    }

    m_materialProgram.setUniformValue("light.position", m_lightPos);
    // common uniforms
    m_materialProgram.setUniformValue("viewPos", viewPos);
    m_materialProgram.setUniformValue("projection", projection);
    m_materialProgram.setUniformValue("view", view);
    m_materialProgram.setUniformValue("model", QMatrix4x4());

    glBindVertexArray(m_cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    m_materialProgram.release();

    // ---- 绘制光源立方体 ----
    drawLightCube(projection, view);
}

void GLMaterials::drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view)
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

    m_lightProgram.release();
}
