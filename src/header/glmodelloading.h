#pragma once

#include "glcamerabase.h"
#include "model.h"

#include <QOpenGLShaderProgram>
#include <QComboBox>

class GLModelLoading : public GLCameraBase
{
    Q_OBJECT
public:
    explicit GLModelLoading(QWidget *parent = nullptr);
    ~GLModelLoading() override;

protected:
    void initializeGL() override;
    void paintGL() override;

private slots:
    void onModelSelected(int index);

private:
    void setupMenu();
    void loadModelFile(const QString &filePath);

    QOpenGLShaderProgram m_program;
    std::unique_ptr<Model> m_model;

    QComboBox *m_modelSelector = nullptr;
    QStringList m_modelPaths;
};
