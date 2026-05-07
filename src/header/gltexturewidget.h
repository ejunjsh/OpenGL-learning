#pragma once

#include "glwidget.h"

class GLTextureWidget : public GLWidget
{
    Q_OBJECT
public:
    explicit GLTextureWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void setupScene();
    unsigned int loadTexture(const QString &path, bool flipVertically = false, bool flipHorizontally = false);
};
