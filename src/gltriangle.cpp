#include "header/gltriangle.h"
#include <QRadioButton>
#include <QVBoxLayout>

GLTriangle::GLTriangle(QWidget *parent)
    : GLBase(parent)
    , m_sceneIndex(0)
{
    setName("GLTriangle");

    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *triBtn = new QRadioButton("Triangle", menu);
    QRadioButton *indexedBtn = new QRadioButton("Indexed", menu);
    QRadioButton *exercise1Btn = new QRadioButton("Exercise1", menu);
    QRadioButton *exercise2Btn = new QRadioButton("Exercise2", menu);
    QRadioButton *exercise3Btn = new QRadioButton("Exercise3", menu);
    triBtn->setChecked(true);
    triBtn->setStyleSheet("color: white;");
    indexedBtn->setStyleSheet("color: white;");
    exercise1Btn->setStyleSheet("color: white;");
    exercise2Btn->setStyleSheet("color: white;");
    exercise3Btn->setStyleSheet("color: white;");

    menuLayout->addWidget(triBtn);
    menuLayout->addWidget(indexedBtn);
    menuLayout->addWidget(exercise1Btn);
    menuLayout->addWidget(exercise2Btn);
    menuLayout->addWidget(exercise3Btn);
    menuLayout->addStretch();

    connect(triBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 0;
        }
    });
    connect(indexedBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 1;
        }
    });
    connect(exercise1Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 2;
        }
    });
    connect(exercise2Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 3;
        }
    });
    connect(exercise3Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 4;
        }
    });
}

void GLTriangle::initializeGL()
{
    GLBase::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/triangle.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/triangle.frag"))
    {
        qFatal("Failed to compile shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link shader program");
    }

    if (!m_program1.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/triangle.vert") ||
        !m_program1.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/triangle1.frag"))
    {
        qFatal("Failed to compile shader 1");
    }

    if (!m_program1.link())
    {
        qFatal("Failed to link shader program 1");
    }

    setupTriangle();
    setupIndexed();
    setupExercise1();
    setupExercise2();
    setupExercise3();
}

void GLTriangle::setupTriangle()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO_TRI);
    glGenBuffers(1, &VBO_TRI);

    glBindVertexArray(VAO_TRI);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_TRI);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLTriangle::setupIndexed()
{
    float vertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO_INDEXED);
    glGenBuffers(1, &VBO_INDEXED);
    glGenBuffers(1, &EBO_INDEXED);

    glBindVertexArray(VAO_INDEXED);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_INDEXED);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_INDEXED);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLTriangle::setupExercise1()
{
    float vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,
        -0.0f, -0.5f, 0.0f,
        -0.45f, 0.5f, 0.0f,
        // second triangle
         0.0f, -0.5f, 0.0f,
         0.9f, -0.5f, 0.0f,
         0.45f, 0.5f, 0.0f
    };

    glGenVertexArrays(1, &VAO_EX1);
    glGenBuffers(1, &VBO_EX1);

    glBindVertexArray(VAO_EX1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_EX1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLTriangle::drawTriangle()
{
    m_program.bind();
    glBindVertexArray(VAO_TRI);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    m_program.release();
}

void GLTriangle::drawIndexed()
{
    m_program.bind();
    glBindVertexArray(VAO_INDEXED);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    m_program.release();
}

void GLTriangle::drawExercise1()
{
    m_program.bind();
    glBindVertexArray(VAO_EX1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    m_program.release();
}

void GLTriangle::setupExercise2()
{
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,
        -0.0f, -0.5f, 0.0f,
        -0.45f, 0.5f, 0.0f,
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,
        0.9f, -0.5f, 0.0f,
        0.45f, 0.5f, 0.0f
    };

    glGenVertexArrays(2, VAO_EX2);
    glGenBuffers(2, VBO_EX2);

    glBindVertexArray(VAO_EX2[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_EX2[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_EX2[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_EX2[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void GLTriangle::drawExercise2()
{
    m_program.bind();
    glBindVertexArray(VAO_EX2[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(VAO_EX2[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    m_program.release();
}

void GLTriangle::setupExercise3()
{
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,
        -0.0f, -0.5f, 0.0f,
        -0.45f, 0.5f, 0.0f,
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,
        0.9f, -0.5f, 0.0f,
        0.45f, 0.5f, 0.0f
    };

    glGenVertexArrays(2, VAO_EX3);
    glGenBuffers(2, VBO_EX3);

    glBindVertexArray(VAO_EX3[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_EX3[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO_EX3[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_EX3[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void GLTriangle::drawExercise3()
{
    m_program.bind();
    glBindVertexArray(VAO_EX3[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    m_program.release();

    m_program1.bind();
    glBindVertexArray(VAO_EX3[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    m_program1.release();
}

void GLTriangle::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_sceneIndex == 0) {
        drawTriangle();
    } else if (m_sceneIndex == 1) {
        drawIndexed();
    } else if (m_sceneIndex == 2) {
        drawExercise1();
    } else if (m_sceneIndex == 3) {
        drawExercise2();
    } else {
        drawExercise3();
    }
}

void GLTriangle::setSceneIndex(int index)
{
    m_sceneIndex = index;
}
