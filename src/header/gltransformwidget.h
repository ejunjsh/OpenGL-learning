#pragma once

#include "glwidget.h"

class GLTransformWidget : public GLWidget
{
    Q_OBJECT
public:
    explicit GLTransformWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void drawTransform();
    void drawExercise1();
    void drawExercise2();

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLuint m_texture1 = 0;
    GLuint m_texture2 = 0;
    int m_sceneIndex = 0;  // 0=Transform, 1=Exercise1, 2=Exercise2
};
