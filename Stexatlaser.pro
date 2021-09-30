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

# Qx Lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQxC_static64_0-0-7-7_Qt_5-15-2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQxC_static64_0-0-7-7_Qt_5-15-2d

win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/QxC_static64_0-0-7-7_Qt_5-15-2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/QxC_static64_0-0-7-7_Qt_5-15-2d.lib

# Squish Lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/Squish -lsquishd #EDIT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/Squish -lsquishd

win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/Squish/squishd.lib #EDIT
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/Squish/squishd.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
