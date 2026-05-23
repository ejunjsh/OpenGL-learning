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
};
