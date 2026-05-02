TEMPLATE = app
TARGET = opengl-learning
QT += widgets opengl openglwidgets
CONFIG += C++17
SOURCES += \
    main.cpp \
    glwidget.cpp \
    glwidgetex.cpp \
    panel.cpp \
    actionbutton.cpp \
    camera.cpp \
    mesh.cpp \
    object3d.cpp \

HEADERS += \
    glwidget.h \
    glwidgetex.h \
    panel.h \
    actionbutton.h \
    camera.h \
    mesh.h \
    object3d.h 

RESOURCES += \
    resources.qrc
