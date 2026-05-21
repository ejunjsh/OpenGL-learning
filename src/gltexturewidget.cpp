#include "header/gltexturewidget.h"
#include <QRadioButton>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>

GLTextureWidget::GLTextureWidget(QWidget *parent)
    : GLWidget(parent)
    , m_sceneIndex(0)
{
    setName("GLTextureWidget");

    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *singleBtn = new QRadioButton("Single", menu);
    QRadioButton *colorMixBtn = new QRadioButton("ColorMix", menu);
    QRadioButton *mixBtn = new QRadioButton("Mix", menu);
    QRadioButton *exercise1Btn = new QRadioButton("Exercise1", menu);
    QRadioButton *exercise2Btn = new QRadioButton("Exercise2", menu);
    QRadioButton *exercise3Btn = new QRadioButton("Exercise3", menu);
    QRadioButton *exercise4Btn = new QRadioButton("Exercise4", menu);
    singleBtn->setChecked(true);
    singleBtn->setStyleSheet("color: white;");
    colorMixBtn->setStyleSheet("color: white;");
    mixBtn->setStyleSheet("color: white;");
    exercise1Btn->setStyleSheet("color: white;");
    exercise2Btn->setStyleSheet("color: white;");
    exercise3Btn->setStyleSheet("color: white;");
    exercise4Btn->setStyleSheet("color: white;");

    menuLayout->addWidget(singleBtn);
    menuLayout->addWidget(colorMixBtn);
    menuLayout->addWidget(mixBtn);
    menuLayout->addWidget(exercise1Btn);
    menuLayout->addWidget(exercise2Btn);
    menuLayout->addWidget(exercise3Btn);
    menuLayout->addWidget(exercise4Btn);

    // Slider for Exercise4 mixValue control
    QSlider *mixSlider = new QSlider(Qt::Horizontal, menu);
    mixSlider->setRange(0, 100);
    mixSlider->setValue(20);
    mixSlider->setStyleSheet("QSlider::groove:horizontal { height: 6px; background: #555; border-radius: 3px; }"
                             "QSlider::handle:horizontal { width: 14px; margin: -5px 0; background: #4a9eff; border-radius: 7px; }");
    QLabel *mixLabel = new QLabel("Mix: 0.20", menu);
    mixLabel->setStyleSheet("color: white;");
    menuLayout->addWidget(mixLabel);
    menuLayout->addWidget(mixSlider);
    mixLabel->hide();
    mixSlider->hide();
    menuLayout->addStretch();

    connect(singleBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 0;
        }
    });
    connect(colorMixBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 1;
        }
    });
    connect(mixBtn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 2;
        }
    });
    connect(exercise1Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 3;
        }
    });
    connect(exercise2Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 4;
        }
    });
    connect(exercise3Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            m_sceneIndex = 5;
        }
    });
    connect(exercise4Btn, &QRadioButton::toggled, this, [this, mixSlider, mixLabel](bool checked) {
        if (checked) {
            m_sceneIndex = 6;
        }
        mixSlider->setVisible(checked);
        mixLabel->setVisible(checked);
    });
    connect(mixSlider, &QSlider::valueChanged, this, [this, mixLabel](int value) {
        m_mixValue = value / 100.0f;
        mixLabel->setText(QString("Mix: %1").arg(m_mixValue, 0, 'f', 2));
    });
}

void GLTextureWidget::initializeGL()
{
    GLWidget::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture/texture_simple.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture/texture_simple.frag"))
    {
        qFatal("Failed to compile texture shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link texture shader program");
    }

    if (!m_programColorMix.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture/texture_simple.vert") ||
        !m_programColorMix.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture/texture_color_mix.frag"))
    {
        qFatal("Failed to compile color mix shader");
    }

    if (!m_programColorMix.link())
    {
        qFatal("Failed to link color mix shader program");
    }

    if (!m_programMix.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture/texture_simple.vert") ||
        !m_programMix.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture/texture_mix.frag"))
    {
        qFatal("Failed to compile mix texture shader");
    }

    if (!m_programMix.link())
    {
        qFatal("Failed to link mix texture shader program");
    }

    if (!m_programEx1.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture/texture_simple.vert") ||
        !m_programEx1.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture/texture_exercise1.frag"))
    {
        qFatal("Failed to compile exercise1 shader");
    }

    if (!m_programEx1.link())
    {
        qFatal("Failed to link exercise1 shader program");
    }

    if (!m_programEx4.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture/texture_simple.vert") ||
        !m_programEx4.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture/texture_exercise4.frag"))
    {
        qFatal("Failed to compile exercise4 shader");
    }

    if (!m_programEx4.link())
    {
        qFatal("Failed to link exercise4 shader program");
    }

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Exercise2 VAO with 2.0f texture coords
    float verticesEx2[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f
    };
    unsigned int indicesEx2[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO_EX2);
    glGenBuffers(1, &VBO_EX2);
    glGenBuffers(1, &EBO_EX2);

    glBindVertexArray(VAO_EX2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_EX2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesEx2), verticesEx2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_EX2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesEx2), indicesEx2, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Exercise3 VAO with zoomed-in texture coords (0.45-0.55) to see individual pixels
    float verticesEx3[] = {
        // positions          // colors           // texture coords (zoomed in on center)
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.55f, 0.45f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.45f, 0.45f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.45f, 0.55f  // top left
    };
    unsigned int indicesEx3[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO_EX3);
    glGenBuffers(1, &VBO_EX3);
    glGenBuffers(1, &EBO_EX3);

    glBindVertexArray(VAO_EX3);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_EX3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesEx3), verticesEx3, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_EX3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesEx3), indicesEx3, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // load texture1
    m_texture1 = loadTexture(":/textures/container.jpg");
    // load texture2
    m_texture2 = loadTexture(":/textures/awesomeface.png", true);

    // load texture1 with GL_CLAMP_TO_EDGE for exercise2
    {
        QImage image(":/textures/container.jpg");
        image = image.convertToFormat(QImage::Format_RGBA8888);

        glGenTextures(1, &m_textureClamp);
        glBindTexture(GL_TEXTURE_2D, m_textureClamp);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // load textures with GL_NEAREST filtering for exercise3 (to see individual pixels)
    // texture1 (container) with GL_CLAMP_TO_EDGE + GL_NEAREST
    {
        QImage image(":/textures/container.jpg");
        image = image.convertToFormat(QImage::Format_RGBA8888);

        glGenTextures(1, &m_textureEx3_1);
        glBindTexture(GL_TEXTURE_2D, m_textureEx3_1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // texture2 (awesomeface) with GL_REPEAT + GL_NEAREST
    {
        QImage image(":/textures/awesomeface.png");
        image = image.convertToFormat(QImage::Format_RGBA8888);

        glGenTextures(1, &m_textureEx3_2);
        glBindTexture(GL_TEXTURE_2D, m_textureEx3_2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(),
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void GLTextureWidget::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_sceneIndex == 0) {
        drawSingle();
    } else if (m_sceneIndex == 1) {
        drawColorMix();
    } else if (m_sceneIndex == 2) {
        drawMix();
    } else if (m_sceneIndex == 3) {
        drawExercise1();
    } else if (m_sceneIndex == 4) {
        drawExercise2();
    } else if (m_sceneIndex == 5) {
        drawExercise3();
    } else {
        drawExercise4();
    }
}

void GLTextureWidget::drawSingle()
{
    m_program.bind();

    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_program.release();
}

void GLTextureWidget::drawColorMix()
{
    m_programColorMix.bind();

    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_programColorMix.release();
}

void GLTextureWidget::drawMix()
{
    m_programMix.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_programMix.setUniformValue("texture1", 0);
    m_programMix.setUniformValue("texture2", 1);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_programMix.release();
}

void GLTextureWidget::drawExercise1()
{
    m_programEx1.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_programEx1.setUniformValue("ourTexture1", 0);
    m_programEx1.setUniformValue("ourTexture2", 1);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_programEx1.release();
}

void GLTextureWidget::drawExercise2()
{
    m_programMix.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureClamp);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_programMix.setUniformValue("texture1", 0);
    m_programMix.setUniformValue("texture2", 1);

    glBindVertexArray(VAO_EX2);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_programMix.release();
}

void GLTextureWidget::drawExercise3()
{
    m_programMix.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureEx3_1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureEx3_2);

    m_programMix.setUniformValue("texture1", 0);
    m_programMix.setUniformValue("texture2", 1);

    glBindVertexArray(VAO_EX3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_programMix.release();
}

void GLTextureWidget::drawExercise4()
{
    m_programEx4.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_programEx4.setUniformValue("texture1", 0);
    m_programEx4.setUniformValue("texture2", 1);
    m_programEx4.setUniformValue("mixValue", m_mixValue);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    m_programEx4.release();
}
