QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TEMPLATE = app
TARGET = copypanel

SRC_DIR = code/src
INC_DIR = code/inc
MUI_DIR = code/ui
RES_DIR = code/res

INCLUDEPATH += $$INC_DIR $$SPDLOG_DIR
OBJECTS_DIR = build/obj/
MOC_DIR = build/
RCC_DIR = build/
UI_DIR = build/

CONFIG(debug,debug|release) {
    DESTDIR = $$absolute_path(bin/debug)
} else {
    DESTDIR = $$absolute_path(bin/release)
}
win32  {
    LIBS += -luser32
}

SOURCES += \
    $$SRC_DIR/main.cpp \
    $$SRC_DIR/desktopmenu.cpp \
    $$SRC_DIR/qtinformationstorage.cpp \
    $$SRC_DIR/basepanel.cpp \

HEADERS += \
    $$INC_DIR/desktopmenu.h \
    $$INC_DIR/qtinformationstorage.h  \
    $$INC_DIR/basepanel.h \


RESOURCES += \
    $$RES_DIR/res.qrc

RC_ICONS = code/res/ui/logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

