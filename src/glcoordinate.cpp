#include "header/glcoordinate.h"
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <climits>
#include "header/mesh.h"

GLCoordinate::GLCoordinate(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLCoordinate");

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
    getMenuPanel()->setLayout(layout);

    connect(btn, &QPushButton::clicked, this, [this]() {
        makeCurrent();
        setupScene(m_cubeCountSpinBox->value());
        doneCurrent();
    });
}

void GLCoordinate::initializeGL()
{
    GLBase::initializeGL();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag"))
    {
        qFatal("Failed to compile texture shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link texture shader program");
    }

    setupScene(m_cubeCountSpinBox->value());
}

void GLCoordinate::paintGL()
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

    if (m_rootObject)
    {
        // 每个立方体随时间独立旋转（相同转速，固定方向）
        const auto &children = m_rootObject->getChildren();
        const float speed = 30.0f;  // 统一转速
        for (size_t i = 0; i < children.size(); ++i) {
            auto cube = children[i];
            const QVector3D &dir = m_rotationDirections[i];
            cube->setRotation(QVector3D(
                elapsedTime() * speed * dir.x(),
                elapsedTime() * speed * dir.y(),
                elapsedTime() * speed * dir.z()
            ));
        }
        m_rootObject->draw(m_program);
    }

    m_program.release();
}

void GLCoordinate::setupScene(int cubeCount)
{
    // 立方体顶点数据（位置 + 纹理坐标）
    std::vector<Vertex> meshVertices = {
        // 背面
        Vertex(QVector3D(-0.5f, -0.5f, -0.5f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D( 0.5f, -0.5f, -0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D( 0.5f,  0.5f, -0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D( 0.5f,  0.5f, -0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D(-0.5f,  0.5f, -0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D(-0.5f, -0.5f, -0.5f), QVector2D(0.0f, 0.0f)),

        // 正面
        Vertex(QVector3D(-0.5f, -0.5f,  0.5f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D( 0.5f, -0.5f,  0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D( 0.5f,  0.5f,  0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D( 0.5f,  0.5f,  0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D(-0.5f,  0.5f,  0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D(-0.5f, -0.5f,  0.5f), QVector2D(0.0f, 0.0f)),

        // 左面
        Vertex(QVector3D(-0.5f,  0.5f,  0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D(-0.5f,  0.5f, -0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D(-0.5f, -0.5f, -0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D(-0.5f, -0.5f, -0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D(-0.5f, -0.5f,  0.5f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D(-0.5f,  0.5f,  0.5f), QVector2D(1.0f, 0.0f)),

        // 右面
        Vertex(QVector3D( 0.5f,  0.5f,  0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D( 0.5f,  0.5f, -0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D( 0.5f, -0.5f, -0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D( 0.5f, -0.5f, -0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D( 0.5f, -0.5f,  0.5f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D( 0.5f,  0.5f,  0.5f), QVector2D(1.0f, 0.0f)),

        // 下面
        Vertex(QVector3D(-0.5f, -0.5f, -0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D( 0.5f, -0.5f, -0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D( 0.5f, -0.5f,  0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D( 0.5f, -0.5f,  0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D(-0.5f, -0.5f,  0.5f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D(-0.5f, -0.5f, -0.5f), QVector2D(0.0f, 1.0f)),

        // 上面
        Vertex(QVector3D(-0.5f,  0.5f, -0.5f), QVector2D(0.0f, 1.0f)),
        Vertex(QVector3D( 0.5f,  0.5f, -0.5f), QVector2D(1.0f, 1.0f)),
        Vertex(QVector3D( 0.5f,  0.5f,  0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D( 0.5f,  0.5f,  0.5f), QVector2D(1.0f, 0.0f)),
        Vertex(QVector3D(-0.5f,  0.5f,  0.5f), QVector2D(0.0f, 0.0f)),
        Vertex(QVector3D(-0.5f,  0.5f, -0.5f), QVector2D(0.0f, 1.0f))
    };

    // 立方体不需要索引
    std::vector<unsigned int> indices;

    // 加载纹理
    Texture containerTex;
    containerTex.id = loadTexture(":/textures/container.jpg");
    containerTex.name = "container";

    Texture faceTex;
    faceTex.id = loadTexture(":/textures/awesomeface.png", true);
    faceTex.name = "face";

    std::vector<Texture> textures = {containerTex, faceTex};

    auto mesh = std::make_shared<Mesh>(meshVertices, indices, textures);

    m_rootObject = std::make_shared<Object3D>("Root");
    m_rotationDirections.clear();

    // 创建立方体，随机位置分布在摄像头前方
    for (int i = 0; i < cubeCount; i++) {
        auto cube = std::make_shared<Object3D>(QString("Cube%1").arg(i));
        cube->addMesh(mesh);

        // 随机位置：在摄像头前方 (z: -2 到 -15), x: -20 到 20, y: -15 到 15
        int x = QRandomGenerator::global()->bounded(-20, 20);
        int y = QRandomGenerator::global()->bounded(-15, 15);
        int z = QRandomGenerator::global()->bounded(-15, -2);
        cube->setPosition(QVector3D(x, y, z));

        // 每个立方体有独立的随机初始旋转
        float rotX = QRandomGenerator::global()->bounded(360);
        float rotY = QRandomGenerator::global()->bounded(360);
        float rotZ = QRandomGenerator::global()->bounded(360);
        cube->setRotation(QVector3D(rotX, rotY, rotZ));

        // 生成固定旋转方向
        float dirX = (QRandomGenerator::global()->bounded(0, 201) - 100) / 100.0f;
        float dirY = (QRandomGenerator::global()->bounded(0, 201) - 100) / 100.0f;
        float dirZ = (QRandomGenerator::global()->bounded(0, 201) - 100) / 100.0f;
        m_rotationDirections.append(QVector3D(dirX, dirY, dirZ));

        m_rootObject->addChild(cube);
    }
}
