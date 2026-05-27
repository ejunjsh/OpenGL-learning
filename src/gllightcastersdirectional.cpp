#include "header/gllightcastersdirectional.h"

GLLightCastersDirectional::GLLightCastersDirectional(QWidget *parent)
    : GLCameraBase(parent)
{
    setName("GLLightCastersDirectional");
}

void GLLightCastersDirectional::initializeGL()
{
    GLBase::initializeGL();
}

void GLLightCastersDirectional::paintGL()
{
}
