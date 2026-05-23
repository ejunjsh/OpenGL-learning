#pragma once

#include "glbase.h"

class GLCoordinate : public GLBase
{
    Q_OBJECT
public:
    explicit GLCoordinate(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void drawQuad();
    void drawDepthCube();
    void drawMultipleCubes();
    void drawExercise1();

    // 2D quad
    unsigned int m_VAO_QUAD = 0;
    unsigned int m_VBO_QUAD = 0;
    unsigned int m_EBO_QUAD = 0;

    // 3D cube (depth / multiple)
    unsigned int m_VAO_CUBE = 0;
    unsigned int m_VBO_CUBE = 0;

    unsigned int m_texture1 = 0;
    unsigned int m_texture2 = 0;

    int m_sceneIndex = 0;  // 0=quad, 1=depth cube, 2=multiple cubes, 3=exercise 1
};
