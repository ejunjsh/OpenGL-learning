TEMPLATE = app
TARGET = opengl-learning
QT += widgets opengl openglwidgets
CONFIG += C++17
SOURCES += \ 
    main.cpp \
    glwidget.cpp \
    glwidgetex.cpp \
    panel.cpp \
    actionbutton.cpp

HEADERS += \ 
    glwidget.h \
    glwidgetex.h \
    panel.h \
    actionbutton.h

RESOURCES += \
    resources.qrc
