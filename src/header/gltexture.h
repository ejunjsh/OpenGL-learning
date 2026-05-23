#pragma once

#include "glbase.h"

class GLTexture : public GLBase
{
    Q_OBJECT
public:
    explicit GLTexture(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void drawSingle();
    void drawColorMix();
    void drawMix();
    void drawExercise1();
    void drawExercise2();
    void drawExercise3();
    void drawExercise4();

private:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    unsigned int m_texture1 = 0, m_texture2 = 0;
    unsigned int m_textureClamp = 0;
    unsigned int m_textureEx3_1 = 0, m_textureEx3_2 = 0;
    unsigned int VAO_EX2 = 0, VBO_EX2 = 0, EBO_EX2 = 0;
    unsigned int VAO_EX3 = 0, VBO_EX3 = 0, EBO_EX3 = 0;
    QOpenGLShaderProgram m_programColorMix;
    QOpenGLShaderProgram m_programMix;
    QOpenGLShaderProgram m_programEx1;
    QOpenGLShaderProgram m_programEx4;
    float m_mixValue = 0.2f;
    int m_sceneIndex = 0;  // 0=Single, 1=ColorMix, 2=Mix, 3=Exercise1, 4=Exercise2, 5=Exercise3, 6=Exercise4
};
