CONFIG += c++17 console
CONFIG -= app_bundle

QT += xml

TARGET = Stex

SOURCES += \
    src/atlas.cpp \
    src/k_atlaskey.cpp \
    src/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/atlas.h \
    src/k_atlaskey.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQxC_static64_0-0-6-6_Qt_5-15-2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQxC_static64_0-0-6-6_Qt_5-15-2d

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQxC_static64_0-0-6-6_Qt_5-15-2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libQxC_static64_0-0-6-6_Qt_5-15-2d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/QxC_static64_0-0-6-6_Qt_5-15-2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/QxC_static64_0-0-6-6_Qt_5-15-2d.lib
