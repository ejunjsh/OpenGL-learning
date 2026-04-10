#include "glwidget.h"
#include <QMatrix4x4>
#include <QtMath>
#include <QDebug>
#include <algorithm>
#include <cmath>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), m_vbo(QOpenGLBuffer::VertexBuffer)
{

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_timer.start();
    connect(&m_frameTimer, &QTimer::timeout, this, [this]()
{
const qint64 ms = m_timer.restart();
const float dt - static_cast<float>(ms) / 1000.0f;
updateCameraByKeyboard(dt);
update(); });

    m_frameTimer.start(16);
}

GLWidget : ~GLWidget()
{
    makeCurrent();
    m_vao.destroy();
    m_vbo.destroy();
    doneCurrent();
}