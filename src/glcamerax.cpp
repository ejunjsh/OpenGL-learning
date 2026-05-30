#include "header/glcamerax.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <climits>

GLCameraX::GLCameraX(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLCameraX");

    // 添加立方体数量输入框到菜单面板
    QVBoxLayout *layout = new QVBoxLayout(getMenuPanel());
    layout->setContentsMargins(10, 10, 10, 10);

    QLabel *label = new QLabel("立方体数量:", getMenuPanel());
    label->setStyleSheet("color: white;");
    layout->addWidget(label);

    m_cubeCountSpinBox = new QSpinBox(getMenuPanel());
    m_cubeCountSpinBox->setMaximum(INT_MAX);
    m_cubeCountSpinBox->setValue(100);
    m_cubeCountSpinBox->setStyleSheet("color: white; background: rgba(50,50,50,200);");
    layout->addWidget(m_cubeCountSpinBox);

    QPushButton *btn = new QPushButton("生成", getMenuPanel());
    btn->setStyleSheet("color: white; background: rgba(80,80,80,200);");
    layout->addWidget(btn);

    layout->addStretch();

    connect(btn, &QPushButton::clicked, this, [this]() {
        setupScene(m_cubeCountSpinBox->value());
    });
}

void GLCameraX::initializeGL()
{
    GLBase::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, "shaders/texture.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, "shaders/texture.frag"))
    {
        qFatal("Failed to compile texture shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link texture shader program");
    }

    // 立方体顶点数据：position(3) + texCoord(2) = 5 floats
    static const float vertices[] = {
        // 背面
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        // 正面
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // 左面
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // 右面
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // 下面
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // 上面
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);

    glBindVertexArray(m_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position (location 0): 3 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord (location 2): 2 floats (matches texture.vert layout)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // 加载纹理
    m_containerTex = loadTexture("textures/container.jpg");
    m_faceTex = loadTexture("textures/awesomeface.png", true);

    // 设置 sampler uniforms
    m_program.bind();
    m_program.setUniformValue("container", 0);
    m_program.setUniformValue("face", 1);

    setupScene(m_cubeCountSpinBox->value());
}

void GLCameraX::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();

    m_program.bind();
    m_program.setUniformValue("view", view);
    m_program.setUniformValue("projection", projection);

    // 绑定纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_containerTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_faceTex);

    // 绘制所有立方体
    glBindVertexArray(m_cubeVAO);
    const float speed = 30.0f;
    const float t = elapsedTime();

    for (const auto &cube : m_cubes)
    {
        QMatrix4x4 model;
        model.translate(cube.position);
        model.rotate(t * speed * cube.rotationDir.x(), QVector3D(1.0f, 0.0f, 0.0f));
        model.rotate(t * speed * cube.rotationDir.y(), QVector3D(0.0f, 1.0f, 0.0f));
        model.rotate(t * speed * cube.rotationDir.z(), QVector3D(0.0f, 0.0f, 1.0f));
        m_program.setUniformValue("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
    m_program.release();
}

void GLCameraX::setupScene(int cubeCount)
{
    m_cubes.clear();
    m_cubes.reserve(cubeCount);

    for (int i = 0; i < cubeCount; i++)
    {
        CubeData cube;

        // 随机位置：在摄像头前方 (z: -2 到 -15), x: -20 到 20, y: -15 到 15
        cube.position = QVector3D(
            QRandomGenerator::global()->bounded(-20, 20),
            QRandomGenerator::global()->bounded(-15, 15),
            QRandomGenerator::global()->bounded(-15, -2)
        );

        // 初始旋转
        cube.rotation = QVector3D(
            QRandomGenerator::global()->bounded(360),
            QRandomGenerator::global()->bounded(360),
            QRandomGenerator::global()->bounded(360)
        );

        // 固定旋转方向
        cube.rotationDir = QVector3D(
            (QRandomGenerator::global()->bounded(0, 201) - 100) / 100.0f,
            (QRandomGenerator::global()->bounded(0, 201) - 100) / 100.0f,
            (QRandomGenerator::global()->bounded(0, 201) - 100) / 100.0f
        );

        m_cubes.append(cube);
    }
}
