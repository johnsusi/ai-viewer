TEMPLATE = app

CONFIG += c++14

linux:QMAKE_CXXFLAGS_CXX11 = -std=c++1y



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

LIBS += -lz
LIBS += -L/usr/local/lib -ltbb
LIBS += -L$$PWD/../deps/lib -lopencv_core -lopencv_imgproc -lopencv_highgui
linux-g++-64: ../3rdparty/opencv/3rdparty/ippicv/unpack/ippicv_lnx/lib/intel64/libippicv.a
#LIBS += -L$$PWD/../deps/share/OpenCV/3rdparty/lib -lippicv -lz
macx:LIBS += -framework OpenCL

