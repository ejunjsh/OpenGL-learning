TEMPLATE = app
TARGET = opengl-learning
QT += widgets opengl openglwidgets
CONFIG += C++17
SOURCES += \
    main.cpp \
    glwidget.cpp \
    gltrianglewidget.cpp \
    gltexturewidget.cpp \
    gltransformwidget.cpp \
    panel.cpp \
    actionbutton.cpp \
    camera.cpp \
    mesh.cpp \
    object3d.cpp \

HEADERS += \
    glwidget.h \
    gltrianglewidget.h \
    gltexturewidget.h \
    gltransformwidget.h \
    panel.h \
    actionbutton.h \
    camera.h \
    mesh.h \
    object3d.h

RESOURCES += \
    textures.qrc \
    shaders.qrc
