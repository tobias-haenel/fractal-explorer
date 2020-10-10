QT -= gui

TEMPLATE = lib
DEFINES += LOGIC_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/controllers/MainController.cpp

HEADERS += \
    public/controllers/MainController.h \
    public/logic_global.h

INCLUDEPATH += $$PWD/public
DEPENDPATH += $PWD/public
INCLUDEPATH += $PWD/src

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
