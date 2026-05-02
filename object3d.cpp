#include "object3d.h"
#include "mesh.h"
#include <QQuaternion>

Object3D::Object3D()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_modelMatrixDirty(true)
{
}

Object3D::Object3D(const QString &name)
    : m_name(name)
    , m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_modelMatrixDirty(true)
{
}

Object3D::~Object3D() = default;

void Object3D::addMesh(std::shared_ptr<Mesh> mesh)
{
    m_meshes.push_back(mesh);
}

void Object3D::setPosition(const QVector3D &position)
{
    m_position = position;
    m_modelMatrixDirty = true;
}

void Object3D::setRotation(const QVector3D &rotation)
{
    m_rotation = rotation;
    m_modelMatrixDirty = true;
}

void Object3D::setScale(const QVector3D &scale)
{
    m_scale = scale;
    m_modelMatrixDirty = true;
}

void Object3D::translate(const QVector3D &delta)
{
    m_position += delta;
    m_modelMatrixDirty = true;
}

void Object3D::rotate(const QVector3D &delta)
{
    m_rotation += delta;
    m_modelMatrixDirty = true;
}

void Object3D::scale(const QVector3D &delta)
{
    m_scale *= delta;
    m_modelMatrixDirty = true;
}

QMatrix4x4 Object3D::getModelMatrix() const
{
    if (m_modelMatrixDirty)
    {
        const_cast<Object3D*>(this)->updateModelMatrix();
    }

    // 如果有父节点，乘以父节点的模型矩阵
    QMatrix4x4 model = m_modelMatrix;
    if (auto parent = m_parent.lock())
    {
        model = parent->getModelMatrix() * model;
    }

    return model;
}

void Object3D::addChild(std::shared_ptr<Object3D> child)
{
    child->setParent(shared_from_this());
    m_children.push_back(child);
}

void Object3D::removeChild(std::shared_ptr<Object3D> child)
{
    auto it = std::remove(m_children.begin(), m_children.end(), child);
    if (it != m_children.end())
    {
        m_children.erase(it, m_children.end());
        child->setParent(nullptr);
    }
}

void Object3D::setParent(std::shared_ptr<Object3D> parent)
{
    m_parent = parent;
}

void Object3D::draw(QOpenGLShaderProgram &program)
{
    // 更新模型矩阵并传递给着色器
    QMatrix4x4 model = getModelMatrix();
    program.setUniformValue("model", model);

    // 绘制所有网格
    for (auto &mesh : m_meshes)
    {
        mesh->draw(program);
    }

    // 递归绘制子对象
    for (auto &child : m_children)
    {
        child->draw(program);
    }
}

void Object3D::updateModelMatrix()
{
    m_modelMatrix.setToIdentity();
    m_modelMatrix.translate(m_position);

    // 使用四元数进行旋转（避免万向锁）
    m_modelMatrix.rotate(QQuaternion::fromEulerAngles(m_rotation));

    m_modelMatrix.scale(m_scale);

    m_modelMatrixDirty = false;
}
