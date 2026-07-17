#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>
#include <QVector3D>

class QCheckBox;

class GLFaceCulling : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLFaceCulling(QWidget *parent = nullptr);
    ~GLFaceCulling() override;

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void onCullingToggled(bool checked);
    void onCullFaceToggled(bool checked);
    void onFrontFaceToggled(bool checked);

private:
    void setupMenu();
    void drawCube(const QVector3D &pos);

    GLuint m_cubeVAO = 0, m_cubeVBO = 0;
    GLuint m_planeVAO = 0, m_planeVBO = 0;

    GLuint m_cubeTexture = 0;
    GLuint m_floorTexture = 0;

    QCheckBox *m_enableCullingCheck = nullptr;
    QCheckBox *m_cullFrontCheck = nullptr;   // true = cull FRONT, false = cull BACK
    QCheckBox *m_frontFaceCWCheck = nullptr; // true = CW is front, false = CCW is front

    bool m_cullingEnabled = true;
    bool m_cullFront = false;     // default: cull back
    bool m_frontFaceCW = false;   // default: CCW is front
};
