#pragma once

#include <QMatrix4x4>
#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <vector>
#include <memory>

class Mesh;

class Object3D : public std::enable_shared_from_this<Object3D>
{
public:
    Object3D();
    explicit Object3D(const QString &name);
    ~Object3D();

    // 网格管理
    void addMesh(std::shared_ptr<Mesh> mesh);
    const std::vector<std::shared_ptr<Mesh>> &getMeshes() const { return m_meshes; }

    // 变换操作
    void setPosition(const QVector3D &position);
    void setRotation(const QVector3D &rotation);
    void setScale(const QVector3D &scale);

    QVector3D getPosition() const { return m_position; }
    QVector3D getRotation() const { return m_rotation; }
    QVector3D getScale() const { return m_scale; }

    // 变换操作（相对）
    void translate(const QVector3D &delta);
    void rotate(const QVector3D &delta);
    void scale(const QVector3D &delta);

    // 获取变换矩阵
    QMatrix4x4 getModelMatrix() const;

    // 层级关系
    void addChild(std::shared_ptr<Object3D> child);
    void removeChild(std::shared_ptr<Object3D> child);
    const std::vector<std::shared_ptr<Object3D>> &getChildren() const { return m_children; }
    void setParent(std::shared_ptr<Object3D> parent);
    std::shared_ptr<Object3D> getParent() const { return m_parent.lock(); }

    // 绘制（递归绘制自己和子对象）
    void draw(QOpenGLShaderProgram &program);

    // 名称
    void setName(const QString &name) { m_name = name; }
    QString getName() const { return m_name; }

private:
    void updateModelMatrix();

private:
    QString m_name;

    // 变换
    QVector3D m_position;
    QVector3D m_rotation;  // 欧拉角 (degrees)
    QVector3D m_scale;
    QMatrix4x4 m_modelMatrix;
    bool m_modelMatrixDirty;

    // 网格
    std::vector<std::shared_ptr<Mesh>> m_meshes;

    // 层级关系
    std::weak_ptr<Object3D> m_parent;
    std::vector<std::shared_ptr<Object3D>> m_children;
};
