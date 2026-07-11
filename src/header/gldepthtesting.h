#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>

class QCheckBox;

class GLDepthTesting : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLDepthTesting(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void onDepthToggled(bool checked);

private:
    void setupMenu();

    GLuint m_cubeVAO = 0;
    GLuint m_planeVAO = 0;
    GLuint m_cubeVBO = 0;
    GLuint m_planeVBO = 0;
    GLuint m_cubeTexture = 0;
    GLuint m_floorTexture = 0;

    QCheckBox *m_depthCheck = nullptr;
    bool m_depthTest = true;
};
