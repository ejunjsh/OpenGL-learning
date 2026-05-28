TEMPLATE = app
TARGET = opengl-learning
QT += widgets opengl openglwidgets
CONFIG += C++17
SOURCES += \
    src/main.cpp \
    src/glbase.cpp \
    src/glcamera.cpp \
    src/glcamerabase.cpp \
    src/gltriangle.cpp \
    src/gltexture.cpp \
    src/gltransform.cpp \
    src/glcamerax.cpp \
    src/glcoordinate.cpp \
    src/glcolor.cpp \
    src/glbasiclighting.cpp \
    src/glmaterials.cpp \
    src/gllightingmaps.cpp \
    src/gllightcasters.cpp \
    src/glmultiplelights.cpp \
    src/panel.cpp \
    src/actionbutton.cpp \
    src/camera.cpp \
    src/mesh.cpp \
    src/object3d.cpp \
    src/glpbrlighting.cpp \

HEADERS += \
    src/header/glbase.h \
    src/header/glcamera.h \
    src/header/glcamerabase.h \
    src/header/gltriangle.h \
    src/header/gltexture.h \
    src/header/gltransform.h \
    src/header/glcamerax.h \
    src/header/glcoordinate.h \
    src/header/glcolor.h \
    src/header/glbasiclighting.h \
    src/header/glmaterials.h \
    src/header/gllightingmaps.h \
    src/header/gllightcasters.h \
    src/header/glmultiplelights.h \
    src/header/panel.h \
    src/header/actionbutton.h \
    src/header/camera.h \
    src/header/mesh.h \
    src/header/object3d.h \
    src/header/glpbrlighting.h

RESOURCES += \
    textures.qrc \
    shaders.qrc
