#pragma once

#include "glbase.h"

class GLTriangle : public GLBase
{
    Q_OBJECT
public:
    explicit GLTriangle(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupTriangle();
    void setupIndexed();
    void setupExercise1();
    void setupExercise2();
    void setupExercise3();
    void drawTriangle();
    void drawIndexed();
    void drawExercise1();
    void drawExercise2();
    void drawExercise3();

private:
    // 三角形
    unsigned int VAO_TRI = 0, VBO_TRI = 0;
    // Indexed
    unsigned int VAO_INDEXED = 0, VBO_INDEXED = 0, EBO_INDEXED = 0;
    // 练习1
    unsigned int VAO_EX1 = 0, VBO_EX1 = 0;
    // 练习2
    unsigned int VAO_EX2[2] = {0, 0}, VBO_EX2[2] = {0, 0};
    // 练习3
    unsigned int VAO_EX3[2] = {0, 0}, VBO_EX3[2] = {0, 0};
    QOpenGLShaderProgram m_program1;

    int m_sceneIndex = 0;  // 0=三角形, 1=Indexed, 2=练习1, 3=练习2, 4=练习3

public slots:
    void setSceneIndex(int index);
};
