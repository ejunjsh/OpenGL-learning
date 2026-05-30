TEMPLATE = app
TARGET = opengl-learning
QT += widgets opengl openglwidgets
CONFIG += c++17

# Assimp
macx {
    INCLUDEPATH += /opt/homebrew/include
    LIBS += -L/opt/homebrew/lib -lassimp
} else {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lassimp
}
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
    src/glpbrlighting.cpp \
    src/glmodelloading.cpp \

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
    src/header/glpbrlighting.h \
    src/header/glmodelloading.h \
    src/header/materialproperties.h \
    src/header/model.h


# 自动把资源复制到 exe 旁边（每次 make 都会执行，不只限于重新链接）
macx {
    RES_DEST = $$OUT_PWD/opengl-learning.app/Contents/MacOS
} else {
    RES_DEST = $$OUT_PWD
}

# 把 shadow build 目录的 .pro 文件也纳入拷贝触发条件
copy_res.commands += $$QMAKE_COPY_DIR \"$$PWD/shaders\" \"$$RES_DEST/shaders\" && $$QMAKE_COPY_DIR \"$$PWD/textures\" \"$$RES_DEST/textures\" && $$QMAKE_COPY_DIR \"$$PWD/objects\" \"$$RES_DEST/objects\"
copy_res.target = copy_res
copy_res.CONFIG += phony
QMAKE_EXTRA_TARGETS += copy_res
PRE_TARGETDEPS += copy_res
