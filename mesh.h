#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QVector2D>
#include <QString>
#include <vector>
#include <memory>

struct Vertex
{
    QVector3D position;
    QVector3D normal;      // 保留，用于光照
    QVector2D texCoords;  // 可选，未使用时为 (0,0)

    Vertex() : position(0, 0, 0), normal(0, 0, 0), texCoords(0, 0) {}
    Vertex(const QVector3D &pos) : position(pos), normal(0, 0, 0), texCoords(0, 0) {}
    Vertex(const QVector3D &pos, const QVector3D &norm)
        : position(pos), normal(norm), texCoords(0, 0) {}
    Vertex(const QVector3D &pos, const QVector2D &tex)
        : position(pos), normal(0, 0, 0), texCoords(tex) {}
    Vertex(const QVector3D &pos, const QVector3D &norm, const QVector2D &tex)
        : position(pos), normal(norm), texCoords(tex) {}
};

struct Texture
{
    unsigned int id;
    QString name;

    Texture() : id(0) {}
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices,
         const std::vector<Texture> &textures = {});
    ~Mesh();

    // 初始化 OpenGL 资源（需要在 OpenGL 上下文下调用）
    void setup();

    // 绘制网格
    void draw(QOpenGLShaderProgram &program);

    // 获取网格数据
    const std::vector<Vertex> &getVertices() const { return m_vertices; }
    const std::vector<unsigned int> &getIndices() const { return m_indices; }

    // 检查是否有法向量或纹理
    bool hasNormals() const { return m_hasNormals; }
    bool hasTexCoords() const { return m_hasTexCoords; }
    bool hasTextures() const { return !m_textures.empty(); }

private:
    void setupMesh();
    void detectAttributes();

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    std::vector<Texture> m_textures;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_ebo;  // Element Buffer Object

    bool m_initialized;

    // 属性标记
    bool m_hasNormals;
    bool m_hasTexCoords;
};
