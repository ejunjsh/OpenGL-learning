#include "header/gltransform.h"
#include <QMatrix4x4>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QtMath>

GLTransform::GLTransform(QWidget *parent)
    : GLBase(parent)
    , m_sceneIndex(0)
{
    setName("GLTransform");

    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *transformBtn = new QRadioButton("Transform", menu);
    QRadioButton *exercise1Btn = new QRadioButton("Exercise1", menu);
    QRadioButton *exercise2Btn = new QRadioButton("Exercise2", menu);
    transformBtn->setChecked(true);
    transformBtn->setStyleSheet("color: white;");
    exercise1Btn->setStyleSheet("color: white;");
    exercise2Btn->setStyleSheet("color: white;");

    menuLayout->addWidget(transformBtn);
    menuLayout->addWidget(exercise1Btn);
    menuLayout->addWidget(exercise2Btn);
    menuLayout->addStretch();

    connect(transformBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(exercise1Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
    connect(exercise2Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 2;
    });
}

void GLTransform::initializeGL()
{
    GLBase::initializeGL();

    // 编译着色器
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/transform/transform.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/transform/transform.frag"))
    {
        qFatal("Failed to compile transform shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link transform shader program");
    }

    // 顶点数据：位置 + 纹理坐标
    float vertices[] = {
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 加载纹理
    m_texture1 = loadTexture(":/textures/container.jpg");
    m_texture2 = loadTexture(":/textures/awesomeface.png", true);

    // 设置每个采样器对应的纹理单元
    m_program.bind();
    m_program.setUniformValue("texture1", 0);
    m_program.setUniformValue("texture2", 1);
    m_program.release();
}

void GLTransform::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_sceneIndex == 0)
        drawTransform();
    else if (m_sceneIndex == 1)
        drawExercise1();
    else
        drawExercise2();
}

void GLTransform::drawTransform()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    // 先平移，再旋转（绕自身中心旋转）
    QMatrix4x4 transform;
    transform.translate(0.5f, -0.5f, 0.0f);
    transform.rotate(elapsedTime() * 57.29578f, 0.0f, 0.0f, 1.0f);

    m_program.bind();
    m_program.setUniformValue("transform", transform);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_program.release();
}

void GLTransform::drawExercise1()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    // 先旋转，再平移（绕原点公转）
    QMatrix4x4 transform;
    transform.rotate(elapsedTime() * 57.29578f, 0.0f, 0.0f, 1.0f);
    transform.translate(0.5f, -0.5f, 0.0f);

    m_program.bind();
    m_program.setUniformValue("transform", transform);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_program.release();
}

void GLTransform::drawExercise2()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    // 第一个容器：平移 + 旋转
    QMatrix4x4 transform;
    transform.translate(0.5f, -0.5f, 0.0f);
    transform.rotate(elapsedTime() * 57.29578f, 0.0f, 0.0f, 1.0f);
    m_program.setUniformValue("transform", transform);
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // 第二个容器：平移 + 缩放（随时间脉冲）
    transform.setToIdentity();
    transform.translate(-0.5f, 0.5f, 0.0f);
    float scaleAmount = static_cast<float>(qSin(elapsedTime()));
    transform.scale(scaleAmount, scaleAmount, scaleAmount);
    m_program.setUniformValue("transform", transform);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    m_program.release();
}
