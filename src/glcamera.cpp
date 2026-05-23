#include "header/glcamera.h"
#include <QMatrix4x4>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QtMath>

GLCamera::GLCamera(QWidget *parent)
    : GLBase(parent)
{
    setName("GLCamera");

    // world space positions of our cubes
    m_cubePositions = {
        QVector3D( 0.0f,  0.0f,  0.0f),
        QVector3D( 2.0f,  5.0f, -15.0f),
        QVector3D(-1.5f, -2.2f, -2.5f),
        QVector3D(-3.8f, -2.0f, -12.3f),
        QVector3D( 2.4f, -0.4f, -3.5f),
        QVector3D(-1.7f,  3.0f, -7.5f),
        QVector3D( 1.3f, -2.0f, -2.5f),
        QVector3D( 1.5f,  2.0f, -2.5f),
        QVector3D( 1.5f,  0.2f, -1.5f),
        QVector3D(-1.3f,  1.0f, -1.5f)
    };

    setupMenu();
}

void GLCamera::setupMenu()
{
    QFrame *menu = getMenuPanel();
    QVBoxLayout *menuLayout = new QVBoxLayout(menu);

    QRadioButton *scene1Btn = new QRadioButton("Rotating Camera", menu);
    QRadioButton *scene2Btn = new QRadioButton("WASD Camera", menu);
    QRadioButton *scene3Btn = new QRadioButton("Mouse Look Camera", menu);
    QRadioButton *scene4Btn = new QRadioButton("Camera Class", menu);
    scene1Btn->setChecked(true);
    scene1Btn->setStyleSheet("color: white;");
    scene2Btn->setStyleSheet("color: white;");
    scene3Btn->setStyleSheet("color: white;");
    scene4Btn->setStyleSheet("color: white;");

    menuLayout->addWidget(scene1Btn);
    menuLayout->addWidget(scene2Btn);
    menuLayout->addWidget(scene3Btn);
    menuLayout->addWidget(scene4Btn);
    menuLayout->addStretch();

    connect(scene1Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 0;
    });
    connect(scene2Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 1;
    });
    connect(scene3Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 2;
    });
    connect(scene4Btn, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) m_sceneIndex = 3;
    });
}

void GLCamera::initializeGL()
{
    GLBase::initializeGL();

    // 编译着色器
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/camera/camera.vert") ||
        !m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/camera/camera.frag"))
    {
        qFatal("Failed to compile camera shader");
    }
    if (!m_program.link())
    {
        qFatal("Failed to link camera shader program");
    }

    // 立方体顶点数据：位置 + 纹理坐标 (36 顶点)
    float vertices[] = {
        // 背面
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        // 前面
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        // 左面
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // 右面
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        // 底面
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        // 顶面
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 加载纹理
    m_texture1 = loadTexture(":/textures/container.jpg");
    m_texture2 = loadTexture(":/textures/awesomeface.png", true);

    // 设置每个采样器对应的纹理单元
    m_program.bind();
    m_program.setUniformValue("texture1", 0);
    m_program.setUniformValue("texture2", 1);
    m_program.release();
}

void GLCamera::updateCamera(float dt)
{
    Q_UNUSED(dt);

    if (m_sceneIndex != 1 && m_sceneIndex != 2 && m_sceneIndex != 3)
        return;

    float currentFrame = elapsedTime();
    float deltaTime = currentFrame - m_lastFrameTime;
    m_lastFrameTime = currentFrame;

    if (m_sceneIndex == 3)
    {
        // 场景3: 使用 Camera 类处理键盘输入
        m_camera.processKeyboard(m_keys, deltaTime);
        return;
    }

    // 场景1 & 2: WASD 键盘移动（手动实现）
    float cameraSpeed = 2.5f * deltaTime;

    if (m_keys.contains(Qt::Key_W))
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (m_keys.contains(Qt::Key_S))
        m_cameraPos -= cameraSpeed * m_cameraFront;
    if (m_keys.contains(Qt::Key_A))
        m_cameraPos -= QVector3D::crossProduct(m_cameraFront, m_cameraUp).normalized() * cameraSpeed;
    if (m_keys.contains(Qt::Key_D))
        m_cameraPos += QVector3D::crossProduct(m_cameraFront, m_cameraUp).normalized() * cameraSpeed;
}

void GLCamera::keyPressEvent(QKeyEvent *event)
{
    m_keys.insert(event->key());
    QOpenGLWidget::keyPressEvent(event);
}

void GLCamera::keyReleaseEvent(QKeyEvent *event)
{
    m_keys.remove(event->key());
    QOpenGLWidget::keyReleaseEvent(event);
}

void GLCamera::mousePressEvent(QMouseEvent *event)
{
    if (m_sceneIndex == 2 || m_sceneIndex == 3)
    {
        m_mousePressed = true;
        m_firstMouse = true;
        setCursor(Qt::BlankCursor);
    }
    QOpenGLWidget::mousePressEvent(event);
}

void GLCamera::mouseMoveEvent(QMouseEvent *event)
{
    if ((m_sceneIndex != 2 && m_sceneIndex != 3) || !m_mousePressed)
    {
        QOpenGLWidget::mouseMoveEvent(event);
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
    float yoffset = m_lastY - ypos; // y 轴反向
    m_lastX = xpos;
    m_lastY = ypos;

    if (m_sceneIndex == 3)
    {
        // 场景3: 使用 Camera 类处理鼠标
        m_camera.processMouseMovement(xoffset, yoffset);
        return;
    }

    // 场景2: 手动处理
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    // 限制俯仰角
    if (m_pitch > 89.0f)
        m_pitch = 89.0f;
    if (m_pitch < -89.0f)
        m_pitch = -89.0f;

    // 根据 yaw/pitch 重新计算相机前向量
    QVector3D front;
    front.setX(qCos(qDegreesToRadians(m_yaw)) * qCos(qDegreesToRadians(m_pitch)));
    front.setY(qSin(qDegreesToRadians(m_pitch)));
    front.setZ(qSin(qDegreesToRadians(m_yaw)) * qCos(qDegreesToRadians(m_pitch)));
    m_cameraFront = front.normalized();
}

void GLCamera::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_sceneIndex == 2 || m_sceneIndex == 3)
    {
        m_mousePressed = false;
        setCursor(Qt::ArrowCursor);
    }
    QOpenGLWidget::mouseReleaseEvent(event);
}

void GLCamera::wheelEvent(QWheelEvent *event)
{
    if (m_sceneIndex == 2)
    {
        m_fov -= event->angleDelta().y() / 120.0f;
        m_fov = qBound(1.0f, m_fov, 45.0f);
    }
    else if (m_sceneIndex == 3)
    {
        // 场景3: 使用 Camera 类处理滚轮
        m_camera.processMouseScroll(event->angleDelta().y() / 120.0f);
    }
    QOpenGLWidget::wheelEvent(event);
}

void GLCamera::paintGL()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (m_sceneIndex == 0)
        drawScene1();
    else if (m_sceneIndex == 1)
        drawScene2();
    else if (m_sceneIndex == 2)
        drawScene3();
    else
        drawScene4();
}

void GLCamera::drawScene1()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    // 旋转相机
    float radius = 10.0f;
    float camX = static_cast<float>(qSin(elapsedTime()) * radius);
    float camZ = static_cast<float>(qCos(elapsedTime()) * radius);
    QMatrix4x4 view;
    view.lookAt(QVector3D(camX, 0.0f, camZ), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    m_program.setUniformValue("view", view);

    // projection 矩阵
    const float aspect = static_cast<float>(width()) / height();
    QMatrix4x4 projection;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);
    m_program.setUniformValue("projection", projection);

    // 绘制 10 个立方体
    glBindVertexArray(m_vao);
    for (int i = 0; i < m_cubePositions.size(); i++)
    {
        QMatrix4x4 model;
        model.translate(m_cubePositions[i]);
        float angle = 20.0f * i;
        model.rotate(angle, 1.0f, 0.3f, 0.5f);
        m_program.setUniformValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_program.release();
}

void GLCamera::drawScene2()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    // camera/view transformation -- 自由移动相机
    QMatrix4x4 view;
    view.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    m_program.setUniformValue("view", view);

    // projection 矩阵
    const float aspect = static_cast<float>(width()) / height();
    QMatrix4x4 projection;
    projection.perspective(45.0f, aspect, 0.1f, 100.0f);
    m_program.setUniformValue("projection", projection);

    // 绘制 10 个立方体
    glBindVertexArray(m_vao);
    for (int i = 0; i < m_cubePositions.size(); i++)
    {
        QMatrix4x4 model;
        model.translate(m_cubePositions[i]);
        float angle = 20.0f * i;
        model.rotate(angle, 1.0f, 0.3f, 0.5f);
        m_program.setUniformValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_program.release();
}

void GLCamera::drawScene3()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    // 鼠标视角 + WASD 自由相机
    QMatrix4x4 view;
    view.lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    m_program.setUniformValue("view", view);

    // 动态 FOV（滚轮缩放）
    const float aspect = static_cast<float>(width()) / height();
    QMatrix4x4 projection;
    projection.perspective(m_fov, aspect, 0.1f, 100.0f);
    m_program.setUniformValue("projection", projection);

    // 绘制 10 个立方体
    glBindVertexArray(m_vao);
    for (int i = 0; i < m_cubePositions.size(); i++)
    {
        QMatrix4x4 model;
        model.translate(m_cubePositions[i]);
        float angle = 20.0f * i;
        model.rotate(angle, 1.0f, 0.3f, 0.5f);
        m_program.setUniformValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_program.release();
}

void GLCamera::drawScene4()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2);

    m_program.bind();

    // 使用 Camera 类的 view/projection 矩阵
    m_program.setUniformValue("view", m_camera.getViewMatrix());

    const float aspect = static_cast<float>(width()) / height();
    m_program.setUniformValue("projection", m_camera.getProjectionMatrix(aspect));

    // 绘制 10 个立方体
    glBindVertexArray(m_vao);
    for (int i = 0; i < m_cubePositions.size(); i++)
    {
        QMatrix4x4 model;
        model.translate(m_cubePositions[i]);
        float angle = 20.0f * i;
        model.rotate(angle, 1.0f, 0.3f, 0.5f);
        m_program.setUniformValue("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    m_program.release();
}
