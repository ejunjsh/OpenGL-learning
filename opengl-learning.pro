TEMPLATE = app
TARGET = opengl-learning
QT += widgets opengl openglwidgets
CONFIG += C++17
SOURCES += \
    src/main.cpp \
    src/glwidget.cpp \
    src/gltrianglewidget.cpp \
    src/gltexturewidget.cpp \
    src/gltransformwidget.cpp \
    src/glcoordinatewidget.cpp \
    src/panel.cpp \
    src/actionbutton.cpp \
    src/camera.cpp \
    src/mesh.cpp \
    src/object3d.cpp \

HEADERS += \
    src/header/glwidget.h \
    src/header/gltrianglewidget.h \
    src/header/gltexturewidget.h \
    src/header/gltransformwidget.h \
    src/header/glcoordinatewidget.h \
    src/header/panel.h \
    src/header/actionbutton.h \
    src/header/camera.h \
    src/header/mesh.h \
    src/header/object3d.h

RESOURCES += \
    textures.qrc \
    shaders.qrc
