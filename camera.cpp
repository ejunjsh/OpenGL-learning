#include "camera.h"
#include <algorithm>

Camera::Camera()
    : m_position(QVector3D(0.0f, 0.0f, 3.0f))
    , m_front(QVector3D(0.0f, 0.0f, -1.0f))
    , m_worldUp(QVector3D(0.0f, 1.0f, 0.0f))
    , m_yaw(-90.0f)
    , m_pitch(0.0f)
    , m_movementSpeed(2.5f)
    , m_mouseSensitivity(0.12f)
    , m_zoom(45.0f)
    , m_fov(45.0f)
    , m_nearPlane(0.1f)
    , m_farPlane(100.0f)
{
    updateCameraVectors();
}

QMatrix4x4 Camera::getViewMatrix() const
{
    QMatrix4x4 view;
    view.lookAt(m_position, m_position + m_front, m_up);
    return view;
}

QMatrix4x4 Camera::getProjectionMatrix(float aspect) const
{
    QMatrix4x4 projection;
    projection.perspective(m_zoom, aspect, m_nearPlane, m_farPlane);
    return projection;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch)
    {
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    m_zoom -= yoffset;
    m_zoom = qBound(1.0f, m_zoom, m_fov);
}

void Camera::processKeyboard(const QSet<int> &keys, float deltaTime)
{
    float speed = m_movementSpeed * deltaTime;

    // 水平前进方向（忽略Y轴）
    QVector3D horizontalFront = m_front;
    horizontalFront.setY(0.0f);
    if (!horizontalFront.isNull())
        horizontalFront.normalize();

    // 右方向
    QVector3D right = QVector3D::crossProduct(m_front, m_worldUp).normalized();

    if (keys.contains(Qt::Key_W))
        m_position += horizontalFront * speed;
    if (keys.contains(Qt::Key_S))
        m_position -= horizontalFront * speed;
    if (keys.contains(Qt::Key_A))
        m_position -= right * speed;
    if (keys.contains(Qt::Key_D))
        m_position += right * speed;
    if (keys.contains(Qt::Key_Space))
        m_position += m_worldUp * speed;
    if (keys.contains(Qt::Key_Control))
        m_position -= m_worldUp * speed;
}

void Camera::updateCameraVectors()
{
    // 计算新的前向量
    QVector3D front;
    front.setX(cos(qDegreesToRadians(m_yaw)) * cos(qDegreesToRadians(m_pitch)));
    front.setY(sin(qDegreesToRadians(m_pitch)));
    front.setZ(sin(qDegreesToRadians(m_yaw)) * cos(qDegreesToRadians(m_pitch)));
    m_front = front.normalized();

    // 重新计算右向量和上向量
    m_right = QVector3D::crossProduct(m_front, m_worldUp).normalized();
    m_up = QVector3D::crossProduct(m_right, m_front).normalized();
}
