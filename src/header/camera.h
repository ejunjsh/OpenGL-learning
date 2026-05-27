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
    QVector3D getFront() const { return m_front; }

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
    // ===== 相机位置与方向 =====
    QVector3D m_position;   // 相机在世界坐标系中的位置
    QVector3D m_front;      // 相机的前向向量（指向观察目标）
    QVector3D m_up;         // 相机的上向量（已正交化）
    QVector3D m_right;      // 相机的右向量（由 front x worldUp 计算）
    QVector3D m_worldUp;    // 世界的上向量，初始为 (0,1,0)

    // ===== 欧拉角（控制相机朝向）=====
    float m_yaw;            // 偏航角，绕 Y 轴旋转，负值向左，正值向右
    float m_pitch;          // 俯仰角，绕 X 轴旋转，负值向下，正值向上

    // ===== 移动与交互参数 =====
    float m_movementSpeed;  // WASD 移动速度（单位/秒）
    float m_mouseSensitivity; // 鼠标灵敏度，控制视角转动速度
    float m_zoom;           // 缩放/FOV 值，控制透视投影的视野角度

    // ===== 投影参数 =====
    float m_fov;            // 视野角度（Field of View）
    float m_nearPlane;      // 近裁剪面距离
    float m_farPlane;       // 远裁剪面距离
};
