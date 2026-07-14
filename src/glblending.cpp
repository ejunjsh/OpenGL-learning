#include "header/glblending.h"
#include <QMatrix4x4>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QDebug>
#include <algorithm>
#include <map>

GLBlending::GLBlending(QWidget *parent)
    : GLCameraBase(parent),
      m_discardProgram(new QOpenGLShaderProgram(this)),
      m_noDiscardProgram(new QOpenGLShaderProgram(this)),
      m_windowProgram(new QOpenGLShaderProgram(this))
{
    setName("Blending");
    setupMenu();

    m_camera->setPosition(QVector3D(0.0f, 0.5f, 5.0f));
    m_camera->setPitch(-12.0f);
    m_camera->setYaw(-90.0f);
    m_camera->updateCameraVectors();

    // 草的位置
    m_grassPositions = {
        {-1.5f,  0.0f, -0.48f},
        { 1.5f,  0.0f,  0.51f},
        { 0.0f,  0.0f,  0.7f},
        {-0.3f,  0.0f, -2.3f},
        { 0.5f,  0.0f, -0.6f},
    };

    // 窗口位置
    m_windowPositions = {
        {-1.5f,  0.0f, -0.48f},
        { 1.5f,  0.0f,  0.51f},
        { 0.0f,  0.0f,  0.7f},
        {-0.3f,  0.0f, -2.3f},
        { 0.5f,  0.0f, -0.6f},
    };
}

GLBlending::~GLBlending()
{
    makeCurrent();
    m_discardProgram->removeAllShaders();
    m_noDiscardProgram->removeAllShaders();
    m_windowProgram->removeAllShaders();
    doneCurrent();
}

void GLBlending::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    m_grassCheck = new QCheckBox(menu);
    m_grassCheck->setChecked(true);
    m_grassCheck->setText("Show Grass");

    m_windowCheck = new QCheckBox(menu);
    m_windowCheck->setChecked(false);
    m_windowCheck->setText("Show Windows");

    m_noDiscardCheck = new QCheckBox(menu);
    m_noDiscardCheck->setChecked(false);
    m_noDiscardCheck->setText("Grass No Discard");

    m_noSortCheck = new QCheckBox(menu);
    m_noSortCheck->setChecked(false);
    m_noSortCheck->setText("Windows No Sort");

    menuLayout->addWidget(m_grassCheck);
    menuLayout->addWidget(m_noDiscardCheck);
    menuLayout->addWidget(m_windowCheck);
    menuLayout->addWidget(m_noSortCheck);
    menuLayout->addStretch();

    connect(m_grassCheck, &QCheckBox::toggled,
            this, &GLBlending::onGrassToggled);
    connect(m_noDiscardCheck, &QCheckBox::toggled,
            this, &GLBlending::onNoDiscardToggled);
    connect(m_windowCheck, &QCheckBox::toggled,
            this, &GLBlending::onWindowToggled);
    connect(m_noSortCheck, &QCheckBox::toggled,
            this, &GLBlending::onNoSortToggled);
}

void GLBlending::onGrassToggled(bool checked)
{
    m_showGrass = checked;
}

void GLBlending::onWindowToggled(bool checked)
{
    m_showWindows = checked;
}

void GLBlending::onNoDiscardToggled(bool checked)
{
    m_noDiscard = checked;
}

void GLBlending::onNoSortToggled(bool checked)
{
    m_noSort = checked;
}

void GLBlending::initializeGL()
{
    GLBase::initializeGL();

    // ---- 正常纹理渲染 program（父类 m_program，用于立方体和地板） ----
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/blending/blending.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/blending/blending_no_discard.frag"))
    {
        qFatal("Failed to compile blending shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link blending shader program");
    }

    // ---- discard program（草） ----
    if (!m_discardProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/blending/blending.vert") ||
        !m_discardProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/blending/blending_discard.frag"))
    {
        qFatal("Failed to compile discard shader");
    }
    if (!m_discardProgram->link())
    {
        qFatal("Failed to link discard shader program");
    }

    // ---- no-discard program（草，不 discard） ----
    if (!m_noDiscardProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/blending/blending.vert") ||
        !m_noDiscardProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/blending/blending_no_discard.frag"))
    {
        qFatal("Failed to compile no-discard shader");
    }
    if (!m_noDiscardProgram->link())
    {
        qFatal("Failed to link no-discard shader program");
    }

    // ---- window program（半透明窗口） ----
    if (!m_windowProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           "shaders/blending/blending.vert") ||
        !m_windowProgram->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           "shaders/blending/blending_window.frag"))
    {
        qFatal("Failed to compile window shader");
    }
    if (!m_windowProgram->link())
    {
        qFatal("Failed to link window shader program");
    }

    // 加载纹理
    m_cubeTexture   = loadTexture("textures/marble.jpg");
    m_floorTexture  = loadTexture("textures/metal.png");
    m_grassTexture  = loadTexture("textures/grass.png", true);
    m_windowTexture = loadTexture("textures/window.png", true);

    // ---- 立方体顶点 ----
    float cubeVertices[] = {
        // back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        // front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // top face
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    };

    // ---- 地板顶点 ----
    float planeVertices[] = {
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f,
    };

    // ---- 透明面片顶点（草 / 窗口共用） ----
    float quadVertices[] = {
         0.0f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.0f, -0.5f,  0.0f,  0.0f, 0.0f,
         1.0f, -0.5f,  0.0f,  1.0f, 0.0f,
         0.0f,  0.5f,  0.0f,  0.0f, 1.0f,
         1.0f, -0.5f,  0.0f,  1.0f, 0.0f,
         1.0f,  0.5f,  0.0f,  1.0f, 1.0f,
    };

    // Cube VAO + VBO
    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);
    glBindVertexArray(m_cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Plane VAO + VBO
    glGenVertexArrays(1, &m_planeVAO);
    glGenBuffers(1, &m_planeVBO);
    glBindVertexArray(m_planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Quad VAO + VBO
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // 设置 texture sampler uniform
    m_program.bind();
    m_program.setUniformValue("uTexture", 0);
    m_program.release();

    m_discardProgram->bind();
    m_discardProgram->setUniformValue("uTexture", 0);
    m_discardProgram->release();

    m_noDiscardProgram->bind();
    m_noDiscardProgram->setUniformValue("uTexture", 0);
    m_noDiscardProgram->release();

    m_windowProgram->bind();
    m_windowProgram->setUniformValue("uTexture", 0);
    m_windowProgram->release();
}

void GLBlending::drawCube(const QMatrix4x4 &projection, const QMatrix4x4 &view,
                          const QVector3D &pos)
{
    QMatrix4x4 model;
    model.translate(pos);
    m_program.setUniformValue("model", model);
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void GLBlending::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const float aspect = static_cast<float>(width()) / height();
    const QMatrix4x4 projection = m_camera->getProjectionMatrix(aspect);
    const QMatrix4x4 view = m_camera->getViewMatrix();
    const QVector3D camPos = m_camera->getPosition();

    // ================================================================
    // 第一步：绘制不透明物体
    // ================================================================
    glDisable(GL_BLEND);

    m_program.bind();

    // 地板
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floorTexture);
    glBindVertexArray(m_planeVAO);
    m_program.setUniformValue("model", QMatrix4x4());
    m_program.setUniformValue("projection", projection);
    m_program.setUniformValue("view", view);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 立方体
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_cubeTexture);
    glBindVertexArray(m_cubeVAO);
    drawCube(projection, view, QVector3D(-1.0f, 0.0f, -1.0f));
    drawCube(projection, view, QVector3D( 2.0f, 0.0f,  0.0f));

    m_program.release();

    // ================================================================
    // 第二步：绘制草
    // ================================================================
    if (m_showGrass)
    {
        QOpenGLShaderProgram *grassProg = m_noDiscard ? m_noDiscardProgram : m_discardProgram;
        grassProg->bind();
        grassProg->setUniformValue("projection", projection);
        grassProg->setUniformValue("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_grassTexture);
        glBindVertexArray(m_quadVAO);

        for (const auto &pos : m_grassPositions)
        {
            QMatrix4x4 model;
            model.translate(pos);
            grassProg->setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        grassProg->release();
    }

    // ================================================================
    // 第三步：绘制半透明窗口（按距离排序，从远到近）
    // ================================================================
    if (m_showWindows)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_windowProgram->bind();
        m_windowProgram->setUniformValue("projection", projection);
        m_windowProgram->setUniformValue("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_windowTexture);
        glBindVertexArray(m_quadVAO);

        if (m_noSort)
        {
            // 不排序：按原始顺序绘制
            for (const auto &pos : m_windowPositions)
            {
                QMatrix4x4 model;
                model.translate(pos);
                m_windowProgram->setUniformValue("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        else
        {
            // 按距离排序（远处 → 近处）
            std::multimap<float, QVector3D> sorted;
            for (const auto &pos : m_windowPositions)
            {
                float dist = (camPos - pos).lengthSquared();
                sorted.emplace(dist, pos);
            }

            for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
            {
                QMatrix4x4 model;
                model.translate(it->second);
                m_windowProgram->setUniformValue("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        m_windowProgram->release();
        glDisable(GL_BLEND);
    }

    glBindVertexArray(0);
}
