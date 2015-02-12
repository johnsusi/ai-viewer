TEMPLATE = app

CONFIG += c++11 c++14

QT += core gui widgets webkit webkitwidgets

HEADERS += \
    mainwindow.h

SOURCES += \
    main.cpp \
    mainwindow.cpp

RESOURCES=contents.qrc

TARGET=ai-viewer

DIST_FILES=assets.rcc

QMAKE_INFO_PLIST=Info.plist

INCLUDEPATH += $$PWD/../deps/include

LIBS += -L/usr/local/lib -ltbb
LIBS += -L$$PWD/../deps/lib -lopencv_core -lopencv_imgproc -lopencv_highgui
LIBS += -L$$PWD/../deps/share/OpenCV/3rdparty/lib -lippicv -lz -framework OpenCL

