#pragma once

#include <QVector3D>
#include <QMatrix4x4>
#include <QSet>
#include <QtMath>

class Camera
{
public:
    Camera();

    // 获取视图矩阵
    QMatrix4x4 getViewMatrix() const;
    // 获取投影矩阵
    QMatrix4x4 getProjectionMatrix(float aspect) const;

    // 鼠标控制
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

    // 键盘控制
    void processKeyboard(const QSet<int> &keys, float deltaTime);

    // 更新相机向量
    void updateCameraVectors();

    // 相机参数
    QVector3D getPosition() const { return m_position; }
    void setPosition(const QVector3D &pos) { m_position = pos; }

    float getYaw() const { return m_yaw; }
    void setYaw(float yaw) { m_yaw = yaw; updateCameraVectors(); }

    float getPitch() const { return m_pitch; }
    void setPitch(float pitch) { m_pitch = pitch; updateCameraVectors(); }

    float getMovementSpeed() const { return m_movementSpeed; }
    void setMovementSpeed(float speed) { m_movementSpeed = speed; }

    float getMouseSensitivity() const { return m_mouseSensitivity; }
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }

    float getZoom() const { return m_zoom; }
    void setZoom(float zoom) { m_zoom = qBound(1.0f, zoom, 45.0f); }

private:
    // 相机属性
    QVector3D m_position;
    QVector3D m_front;
    QVector3D m_up;
    QVector3D m_right;
    QVector3D m_worldUp;

    // 欧拉角
    float m_yaw;
    float m_pitch;

    // 相机选项
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;

    // 投影参数
    float m_fov;
    float m_nearPlane;
    float m_farPlane;
};
