#include "glwidget.h"
#include <QMatrix4x4>
#include <QtMath>
#include <QDebug>
#include <algorithm>
#include <cmath>
#include "mesh.h"

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_camera(std::make_unique<Camera>())
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::OpenHandCursor);

    m_fpsLabel = new QLabel(this);
    m_fpsLabel->setStyleSheet("color: yellow; background: rgba(0,0,0,100); padding: 4px; min-width: 80px; font-weight: bold;");
    m_fpsLabel->setText("FPS: 0");
    m_fpsLabel->move(10, 0);

    m_timer.start();
    m_fpsTimer.start();
    connect(&m_frameTimer, &QTimer::timeout, this, [this]() {
        const qint64 ms = m_timer.restart();
        const float dt = static_cast<float>(ms) / 1000.0f;
        updateCamera(dt);

        m_frameCount++;
        const qint64 elapsed = m_fpsTimer.elapsed();
        if (elapsed >= 1000) {
            const float fps = m_frameCount * 1000.0 / elapsed;
            m_fpsLabel->setText(QString("FPS: %1").arg(fps, 0, 'f', 1));
            m_frameCount = 0;
            m_fpsTimer.restart();
        }

        update();
    });

    m_frameTimer.start(16);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    m_rootObject.reset();
    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/triangle.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/triangle.frag"))
    {
        qFatal("Failed to compile shader");
    }

    if (!m_program.link())
    {
        qFatal("Failed to link shader program");
    }

    // 创建场景
    setupScene();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
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
    m_program.setUniformValue("uColor", QVector3D(0.95f, 0.45f, 0.2f));  // 橙色

    // 绘制场景
    if (m_rootObject)
    {
        m_rootObject->draw(m_program);
    }

    m_program.release();
}

void GLWidget::setupScene()
{
    // 创建三角形顶点
    std::vector<Vertex> vertices = {
        Vertex(QVector3D(0.0f, 0.6f, 0.0f)),
        Vertex(QVector3D(-0.6f, -0.6f, 0.0f)),
        Vertex(QVector3D(0.6f, -0.6f, 0.0f))
    };

    std::vector<unsigned int> indices = {0, 1, 2};

    auto mesh = std::make_shared<Mesh>(vertices, indices);
    auto triangle = std::make_shared<Object3D>("Triangle");
    triangle->addMesh(mesh);
    triangle->setPosition(QVector3D(0.0f, 0.0f, 0.0f));

    m_rootObject = std::make_shared<Object3D>("Root");
    m_rootObject->addChild(triangle);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePressed = true;
    m_firstMouse = true;
    setCursor(Qt::ClosedHandCursor);
    QOpenGLWidget::mousePressEvent(event);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePressed = false;
    setCursor(Qt::OpenHandCursor);
    QOpenGLWidget::mouseReleaseEvent(event);
}

void GLWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::OpenHandCursor);
}

void GLWidget::leaveEvent(QEvent *event)
{
    m_firstMouse = true;
    QOpenGLWidget::leaveEvent(event);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_mousePressed)
    {
        return;
    }

    const float xpos = static_cast<float>(event->position().x());
    const float ypos = static_cast<float>(event->position().y());

    if (m_firstMouse)
    {
        m_lastX = xpos;
        m_lastY = ypos;
        m_firstMouse = false;
        return;
    }

    float xoffset = xpos - m_lastX;
    float yoffset = m_lastY - ypos;
    m_lastX = xpos;
    m_lastY = ypos;

    // 使用 Camera 类处理鼠标移动
    m_camera->processMouseMovement(xoffset, yoffset);

    QOpenGLWidget::mouseMoveEvent(event);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        QWidget *mainWindow = parentWidget();
        mainWindow->close();
        return;
    }

    m_keys.insert(event->key());

    QOpenGLWidget::keyPressEvent(event);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    m_keys.remove(event->key());

    QOpenGLWidget::keyReleaseEvent(event);
}

void GLWidget::updateCamera(float dt)
{
    // Shift 键加速
    const float speedMultiplier = m_keys.contains(Qt::Key_Shift) ? 3.0f : 1.0f;
    m_camera->setMovementSpeed(2.5f * speedMultiplier);

    // 使用 Camera 类处理键盘输入
    m_camera->processKeyboard(m_keys, dt);
}
