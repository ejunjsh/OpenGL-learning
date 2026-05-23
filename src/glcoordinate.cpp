#include "header/glcoordinate.h"
#include <QMatrix4x4>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QtMath>

GLCoordinate::GLCoordinate(QWidget *parent)
    : GLBase(parent)
    , m_sceneIndex(0)
{
    setName("Coordinate Systems");

    // 场景切换按钮
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *quadBtn = new QRadioButton("Quad", menu);
    QRadioButton *depthBtn = new QRadioButton("Depth", menu);
    QRadioButton *multiBtn = new QRadioButton("Multiple", menu);
    QRadioButton *exerBtn = new QRadioButton("Exercise 1", menu);
    quadBtn->setChecked(true);
    quadBtn->setStyleSheet("color: white;");
    depthBtn->setStyleSheet("color: white;");
    multiBtn->setStyleSheet("color: white;");
    exerBtn->setStyleSheet("color: white;");

    menuLayout->addWidget(quadBtn);
    menuLayout->addWidget(depthBtn);
    menuLayout->addWidget(multiBtn);
    menuLayout->addWidget(exerBtn);
    menuLayout->addStretch();

    connect(quadBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(depthBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
    connect(multiBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 2;
    });
    connect(exerBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 3;
    });
}

void GLCoordinate::initializeGL()
{
    GLBase::initializeGL();

    // 编译链接着色器程序
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/coordinate/coordinate.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/coordinate/coordinate.frag"))
    {
        qFatal("Failed to compile coordinate shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link coordinate shader program");
    }

    // ---- Scene 0: 2D Quad (indexed) ----
    {
        float vertices[] = {
            // positions          // texture coords
             0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
             0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
            -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
        };
        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        glGenVertexArrays(1, &m_VAO_QUAD);
        glGenBuffers(1, &m_VBO_QUAD);
        glGenBuffers(1, &m_EBO_QUAD);

        glBindVertexArray(m_VAO_QUAD);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO_QUAD);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_QUAD);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    // ---- Scene 1: 3D Cube (depth) ----
    {
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        glGenVertexArrays(1, &m_VAO_CUBE);
        glGenBuffers(1, &m_VBO_CUBE);

        glBindVertexArray(m_VAO_CUBE);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO_CUBE);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    // 加载纹理
    m_texture1 = loadTexture(":/textures/container.jpg", true);
    m_texture2 = loadTexture(":/textures/awesomeface.png", true);

    // 设置采样器对应的纹理单元
    m_program.bind();
    m_program.setUniformValue("texture1", 0);
    m_program.setUniformValue("texture2", 1);
    m_program.release();
}

void GLCoordinate::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    if (m_sceneIndex == 0) {
        glClear(GL_COLOR_BUFFER_BIT);
        drawQuad();
    } else if (m_sceneIndex == 1) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawDepthCube();
    } else if (m_sceneIndex == 2) {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawMultipleCubes();
    } else {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawExercise1();
    }
}

void GLCoordinate::drawQuad()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    QMatrix4x4 model;
    model.rotate(-55.0f, 1.0f, 0.0f, 0.0f);

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -3.0f);

    const float aspect = static_cast<float>(width()) / height();
    QMatrix4x4 projection;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    m_program.setUniformValue("model", model);
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);

    glBindVertexArray(m_VAO_QUAD);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_program.release();
}

void GLCoordinate::drawDepthCube()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    const float aspect = static_cast<float>(width()) / height();

    QMatrix4x4 model;
    model.rotate(elapsedTime() * 50.0f, 0.5f, 1.0f, 0.0f);

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -3.0f);

    QMatrix4x4 projection;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    m_program.setUniformValue("model", model);
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);

    glBindVertexArray(m_VAO_CUBE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    m_program.release();
}

void GLCoordinate::drawMultipleCubes()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    const float aspect = static_cast<float>(width()) / height();

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -3.0f);

    QMatrix4x4 projection;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);

    // World-space positions of 10 cubes
    QVector3D cubePositions[] = {
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

    glBindVertexArray(m_VAO_CUBE);
    for (int i = 0; i < 10; i++) {
        QMatrix4x4 model;
        model.translate(cubePositions[i]);
        model.rotate(static_cast<float>(i) * 20.0f + elapsedTime() * 25.0f,
                     1.0f, 0.3f, 0.5f);
        m_program.setUniformValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

    m_program.release();
}

void GLCoordinate::drawExercise1()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    const float aspect = static_cast<float>(width()) / height();

    QMatrix4x4 view;
    view.translate(0.0f, 0.0f, -3.0f);

    QMatrix4x4 projection;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);

    // World-space positions of 10 cubes
    QVector3D cubePositions[] = {
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

    glBindVertexArray(m_VAO_CUBE);
    for (int i = 0; i < 10; i++) {
        QMatrix4x4 model;
        model.translate(cubePositions[i]);

        // every 3rd iteration (including the 0th) rotates with time,
        // the rest use fixed angles
        float angle = static_cast<float>(i) * 20.0f;
        if (i % 3 == 0)
            angle = elapsedTime() * 25.0f;
        model.rotate(angle, 1.0f, 0.3f, 0.5f);

        m_program.setUniformValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(0);

    m_program.release();
}
