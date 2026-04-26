#include "glwidget.h"
#include <QMatrix4x4>
#include <QtMath>
#include <QDebug>
#include <algorithm>
#include <cmath>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_vbo(QOpenGLBuffer::VertexBuffer)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::OpenHandCursor);

    m_fpsLabel = new QLabel(this);
    m_fpsLabel->setStyleSheet("color: white; background: rgba(0,0,0,100); padding: 4px; min-width: 80px;");
    m_fpsLabel->setText("FPS: 0");
    m_fpsLabel->move(10, 0);

    m_timer.start();
    m_fpsTimer.start();
    connect(&m_frameTimer, &QTimer::timeout, this, [this]() {
        const qint64 ms = m_timer.restart();
        const float dt = static_cast<float>(ms) / 1000.0f;
        updateCameraByKeyboard(dt);

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
    m_vao.destroy();
    m_vbo.destroy();
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

    // 三角形顶点
    const float vertices[] = {
        0.0f, 0.6f, 0.0f,
        -0.6f, -0.6f, 0.0f,
        0.6f, -0.6f, 0.0f
    };

    m_vao.create();
    m_vao.bind();
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));
    m_program.bind();
    m_program.setAttributeBuffer("aPos", GL_FLOAT, 0, 3);
    m_program.enableAttributeArray("aPos");
    m_vao.release();
    m_vbo.release();
    m_program.release();
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

    QMatrix4x4 model;
    model.setToIdentity();

    QMatrix4x4 view;
    view.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    QMatrix4x4 projection;
    const float aspect = static_cast<float>(width()) / height();
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);

    const QMatrix4x4 mvp = projection * view * model;

    // 绘制三角形（橙色）
    m_program.bind();
    m_program.setUniformValue("uMVP", mvp);
    m_program.setUniformValue("uColor", QVector3D(0.95f, 0.45f, 0.2f));
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_vao.release();
    m_program.release();
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

    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

    QVector3D front;
    front.setX(std::cos(qDegreesToRadians(m_yaw)) * std::cos(qDegreesToRadians(m_pitch)));
    front.setY(std::sin(qDegreesToRadians(m_pitch)));
    front.setZ(std::sin(qDegreesToRadians(m_yaw)) * std::cos(qDegreesToRadians(m_pitch)));
    m_cameraFront = front.normalized();

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
    if (event->key() == Qt::Key_Shift)
    {
        m_fastMode = true;
    }

    QOpenGLWidget::keyPressEvent(event);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    m_keys.remove(event->key());
    if (event->key() == Qt::Key_Shift)
    {
        m_fastMode = false;
    }

    QOpenGLWidget::keyReleaseEvent(event);
}

void GLWidget::updateCameraByKeyboard(float dt)
{
    const float speed = m_moveSpeed * (m_fastMode ? 3.0f : 1.0f) * dt;

    const QVector3D worldUp(0.0f, 1.0f, 0.0f);
    const QVector3D front = m_cameraFront.normalized();
    const QVector3D right = QVector3D::crossProduct(front, worldUp).normalized();

    // 水平行走：只保留前向向量的水平分量
    QVector3D horizontalFront = front;
    horizontalFront.setY(0.0f);
    horizontalFront.normalize();

    if (m_keys.contains(Qt::Key_W))
        m_cameraPos += horizontalFront * speed;
    if (m_keys.contains(Qt::Key_S))
        m_cameraPos -= horizontalFront * speed;
    if (m_keys.contains(Qt::Key_A))
        m_cameraPos -= right * speed;
    if (m_keys.contains(Qt::Key_D))
        m_cameraPos += right * speed;

    if (m_keys.contains(Qt::Key_Space))
        m_cameraPos += worldUp * speed;
    if (m_keys.contains(Qt::Key_Control))
        m_cameraPos -= worldUp * speed;
}
