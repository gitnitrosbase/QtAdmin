QT += gui core network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11 console
CONFIG += app_bundle

SOURCES += \
        main.cpp \

HEADERS += \
    ConnectWindow.hpp \
    MainWindow.hpp \
    TabWindow.hpp \

RESOURCES += res.qrc \

FORMS += mainwindow.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../AppData/Local/nitrosbaseuni/bin/ -lnbclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../AppData/Local/nitrosbaseuni/bin/ -lnbclientd
else:unix: LIBS += -L$$PWD/../../AppData/Local/nitrosbaseuni/bin/ -lnbclient

INCLUDEPATH += $$PWD/../../AppData/Local/nitrosbaseuni/include
DEPENDPATH += $$PWD/../../AppData/Local/nitrosbaseuni/include


TARGET = NBAdmin

#QMAKE_LFLAGS += -static-libgcc -static -enable-stdcall-fixup -Wl,-enable-auto-import -Wl,-enable-runtime-pseudo-reloc
#DEFINES += QT_STATIC_BUILD
