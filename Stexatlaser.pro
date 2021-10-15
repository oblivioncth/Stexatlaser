################# Common #################

CONFIG += c++17 console

TARGET = stex

SOURCES += \
    src/command.cpp \
    src/command/c-pack.cpp \
    src/command/c-unpack.cpp \
    src/stex.cpp \
    src/conversion.cpp \
    src/klei/k-atlas.cpp \
    src/klei/k-atlaskey.cpp \
    src/klei/k-tex-io.cpp \
    src/klei/k-tex.cpp \
    src/klei/k-xml.cpp \
    src/main.cpp

HEADERS += \
    src/command.h \
    src/command/c-pack.h \
    src/command/c-unpack.h \
    src/conversion.h \
    src/stex.h \
    src/klei/k-atlas.h \
    src/klei/k-atlaskey.h \
    src/klei/k-tex-io.h \
    src/klei/k-tex.h \
    src/klei/k-xml.h \
    src/version.h

INCLUDEPATH += $$PWD/include $$PWD/include/Squish
DEPENDPATH += $$PWD/include $$PWD/include/Squish

################# Windows Build #################
win32 {
    RC_FILE = resources.rc

    contains(QT_ARCH, i386) {
        CONFIG(release, debug|release) {
            LIBS += \
                -L$$PWD/lib/Qx/x32 -lQxC_static32_0-0-7-10_Qt_5-15-2 \
                -L$$PWD/lib/Squish/x32 -lsquish

            !win32-g++: PRE_TARGETDEPS += \
                $$PWD/lib/Qx/x32/QxC_static32_0-0-7-10_Qt_5-15-2.lib \
                $$PWD/lib/Squish/x32/squish.lib

        } else:CONFIG(debug, debug|release) {
            LIBS += \
                -L$$PWD/lib/Qx/x32 -lQxC_static32_0-0-7-10_Qt_5-15-2d \
                -L$$PWD/lib/Squish/x32 -lsquishd

            !win32-g++: PRE_TARGETDEPS += \
                $$PWD/lib/Qx/x32/QxC_static32_0-0-7-10_Qt_5-15-2d.lib \
                $$PWD/lib/Squish/x32/squishd.lib
        }
    } else {
        CONFIG(release, debug|release) {
            LIBS += \
                -L$$PWD/lib/Qx/x64 -lQxC_static64_0-0-7-10_Qt_5-15-2 \
                -L$$PWD/lib/Squish/x64 -lsquish

            !win32-g++: PRE_TARGETDEPS += \
                $$PWD/lib/Qx/x64/QxC_static64_0-0-7-10_Qt_5-15-2.lib \
                $$PWD/lib/Squish/x64/squish.lib

        } else:CONFIG(debug, debug|release) {
            LIBS += \
                -L$$PWD/lib/Qx/x64 -lQxC_static64_0-0-7-10_Qt_5-15-2d \
                -L$$PWD/lib/Squish/x64 -lsquishd

            !win32-g++: PRE_TARGETDEPS += \
                $$PWD/lib/Qx/x64/QxC_static64_0-0-7-10_Qt_5-15-2d.lib \
                $$PWD/lib/Squish/x64/squishd.lib
        }
    }
}

################# Linux Build #################
unix:!macx {
    CONFIG(release, debug|release) {
        LIBS += \
            -L$$PWD/lib/Qx/x64 -llibQxC_static64_0-0-7-10_Qt_5-15-2 \
            -L$$PWD/lib/Squish/x64 -llibsquish

        !win32-g++: PRE_TARGETDEPS += \
            $$PWD/lib/Qx/x64/libQxC_static64_0-0-7-10_Qt_5-15-2.a \
            $$PWD/lib/Squish/x64/libsquish.a

    } else:CONFIG(debug, debug|release) {
        LIBS += \
            -L$$PWD/lib/Qx/x64 -llibQxC_static64_0-0-7-10_Qt_5-15-2d \
            -L$$PWD/lib/Squish/x64 -llibsquishd

        !win32-g++: PRE_TARGETDEPS += \
            $$PWD/lib/Qx/x64/libQxC_static64_0-0-7-10_Qt_5-15-2d.a \
            $$PWD/lib/Squish/x64/libsquishd.a
    }
}

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
