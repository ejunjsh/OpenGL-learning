#include "header/mesh.h"
#include <QOpenGLFunctions>
#include <QOpenGLContext>

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices,
           const std::vector<Texture> &textures)
    : m_vertices(vertices)
    , m_indices(indices)
    , m_textures(textures)
    , m_vbo(QOpenGLBuffer::VertexBuffer)
    , m_ebo(QOpenGLBuffer::IndexBuffer)
    , m_initialized(false)
    , m_hasNormals(false)
    , m_hasTexCoords(false)
{
    detectAttributes();
}

Mesh::~Mesh()
{
    if (m_initialized)
    {
        m_vao.destroy();
        m_vbo.destroy();
        m_ebo.destroy();
    }
}

void Mesh::detectAttributes()
{
    m_hasNormals = false;
    m_hasTexCoords = false;

    for (const auto &vertex : m_vertices)
    {
        if (vertex.normal != QVector3D(0, 0, 0))
        {
            m_hasNormals = true;
        }
        if (vertex.texCoords != QVector2D(0, 0))
        {
            m_hasTexCoords = true;
        }
        if (m_hasNormals && m_hasTexCoords)
            break;
    }
}

void Mesh::setup()
{
    if (m_initialized)
        return;

    m_vao.create();
    m_vao.bind();

    // 设置 VBO
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(m_vertices.data(), static_cast<int>(m_vertices.size() * sizeof(Vertex)));

    // 设置 EBO
    if (!m_indices.empty())
    {
        m_ebo.create();
        m_ebo.bind();
        m_ebo.allocate(m_indices.data(), static_cast<int>(m_indices.size() * sizeof(unsigned int)));
    }

    // 顶点位置属性 (location = 0) - 必需
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(0);

    // 法线属性 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);

    // 纹理坐标属性 (location = 2) - 可选
    if (m_hasTexCoords)
    {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                             reinterpret_cast<void*>(offsetof(Vertex, texCoords)));
        glEnableVertexAttribArray(2);
    }

    m_vao.release();
    m_vbo.release();
    if (m_ebo.isCreated())
        m_ebo.release();

    m_initialized = true;
}

void Mesh::draw(QOpenGLShaderProgram &program)
{
    if (!m_initialized)
        setup();

    // 激活并绑定纹理（仅当存在纹理时）
    if (hasTextures())
    {
        for (unsigned int i = 0; i < m_textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);

            QString name = m_textures[i].name;
            program.setUniformValue(name.toUtf8().constData(), static_cast<int>(i));

            glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
        }
    }

    // 绘制
    m_vao.bind();
    if (!m_indices.empty())
    {
        glDrawElements(GL_TRIANGLES, static_cast<int>(m_indices.size()), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(m_vertices.size()));
    }
    m_vao.release();

    // 解绑纹理
    if (hasTextures())
        glActiveTexture(GL_TEXTURE0);
}
