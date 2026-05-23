#include "header/glcamerabase.h"

GLCameraBase::GLCameraBase(QWidget *parent)
    : GLBase(parent)
    , m_camera(std::make_unique<Camera>())
{
    initCameraMembers();
}

void GLCameraBase::initCameraMembers()
{
    // 相机相关初始化已在 GLBase 中完成
}

void GLCameraBase::updateCamera(float dt)
{
    // Shift 键加速
    const float speedMultiplier = m_keys.contains(Qt::Key_Shift) ? 3.0f : 1.0f;
    m_camera->setMovementSpeed(2.5f * speedMultiplier);

    // 使用 Camera 类处理键盘输入
    m_camera->processKeyboard(m_keys, dt);
}

QString GLCameraBase::getHelpText() const
{
    return "Mouse drag: Rotate view\n"
           "Scroll: Zoom\n"
           "W/A/S/D: Move\n"
           "Space: Move up\n"
           "Ctrl: Move down\n"
           "Shift: Speed up\n"
           "ESC: Exit";
}

void GLCameraBase::mousePressEvent(QMouseEvent *event)
{
    m_mousePressed = true;
    m_firstMouse = true;
    setCursor(Qt::ClosedHandCursor);
    QOpenGLWidget::mousePressEvent(event);
}

void GLCameraBase::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePressed = false;
    setCursor(Qt::OpenHandCursor);
    QOpenGLWidget::mouseReleaseEvent(event);
}

void GLCameraBase::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::OpenHandCursor);
}

void GLCameraBase::leaveEvent(QEvent *event)
{
    m_firstMouse = true;
    QOpenGLWidget::leaveEvent(event);
}

void GLCameraBase::wheelEvent(QWheelEvent *event)
{
    const float delta = event->angleDelta().y() / 120.0f;
    m_camera->processMouseScroll(delta);
    QOpenGLWidget::wheelEvent(event);
}

void GLCameraBase::mouseMoveEvent(QMouseEvent *event)
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

void GLCameraBase::keyPressEvent(QKeyEvent *event)
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

void GLCameraBase::keyReleaseEvent(QKeyEvent *event)
{
    m_keys.remove(event->key());

    QOpenGLWidget::keyReleaseEvent(event);
}
