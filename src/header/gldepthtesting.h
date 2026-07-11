#pragma once

#include "glcamerabase.h"
#include <QOpenGLShaderProgram>

class QCheckBox;
class QComboBox;

class GLDepthTesting : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLDepthTesting(QWidget *parent = nullptr);
    ~GLDepthTesting() override;

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void onDepthToggled(bool checked);
    void onViewModeChanged(int index);

private:
    void setupMenu();

    enum ViewMode { Normal = 0, DepthNonLinear = 1, DepthLinear = 2 };

    GLuint m_cubeVAO = 0;
    GLuint m_planeVAO = 0;
    GLuint m_cubeVBO = 0;
    GLuint m_planeVBO = 0;
    GLuint m_cubeTexture = 0;
    GLuint m_floorTexture = 0;

    QOpenGLShaderProgram *m_depthViewProgram = nullptr;   // view1: 非线性深度
    QOpenGLShaderProgram *m_depthView2Program = nullptr;  // view2: 线性深度

    QCheckBox *m_depthCheck = nullptr;
    QComboBox *m_viewCombo = nullptr;
    bool m_depthTest = true;
    ViewMode m_viewMode = Normal;
};
