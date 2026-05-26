#pragma once

#include "glcamerabase.h"
#include "materialproperties.h"
#include <QComboBox>
#include <QLabel>
#include <QOpenGLShaderProgram>
#include <QVector>

class GLMaterials : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLMaterials(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupMenu();
    void drawLightCube(const QMatrix4x4 &projection, const QMatrix4x4 &view);

private:
    GLuint m_cubeVAO = 0;
    GLuint m_lightVAO = 0;
    GLuint m_vbo = 0;
    QOpenGLShaderProgram m_materialProgram;
    QOpenGLShaderProgram m_lightProgram;
    QVector3D m_lightPos = QVector3D(1.2f, 1.0f, 2.0f);

    int m_sceneIndex = 0;
    int m_materialIndex = 0;

    QComboBox *m_materialCombo = nullptr;
    QLabel *m_enhancedLabel = nullptr;
    QVector<MaterialProperties> m_materials;

    bool m_animateLight = false;
};
