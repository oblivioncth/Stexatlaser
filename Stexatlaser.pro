CONFIG += c++17 console

TARGET = stex

SOURCES += \
    src/conversion.cpp \
    src/klei/k-atlas.cpp \
    src/klei/k-atlaskey.cpp \
    src/klei/k-tex-io.cpp \
    src/klei/k-tex.cpp \
    src/main.cpp

HEADERS += \
    src/conversion.h \
    src/klei/k-atlas.h \
    src/klei/k-atlaskey.h \
    src/klei/k-tex-io.h \
    src/klei/k-tex.h

INCLUDEPATH += $$PWD/include $$PWD/include/ImageMagick $$PWD/include/Squish
DEPENDPATH += $$PWD/include $$PWD/include/ImageMagick $$PWD/include/Squish

# Qx Lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQxC_static64_0-0-7-2_Qt_5-15-2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQxC_static64_0-0-7-2_Qt_5-15-2d

win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/QxC_static64_0-0-7-2_Qt_5-15-2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/QxC_static64_0-0-7-2_Qt_5-15-2d.lib

# Squish Lib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/Squish -lsquishd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/Squish -lsquishd

win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/Squish/squishd.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/Squish/squishd.lib

# Image Magick Lib
win32:CONFIG(release, debug|release):
    #LIBS += -L$$PWD/lib/ImageMagick
else:win32:CONFIG(debug, debug|release): \
    LIBS += -L$$PWD/lib/ImageMagick \
    -lCORE_DB_Magick++_ \
    -lCORE_DB_MagickCore_ \
    -lCORE_DB_MagickWand_ \
    -lCORE_DB_aom_ \
    -lCORE_DB_brotli_ \
    -lCORE_DB_bzlib_ \
    -lCORE_DB_cairo_ \
    -lCORE_DB_coders_ \
    -lCORE_DB_croco_ \
    -lCORE_DB_exr_ \
    -lCORE_DB_ffi_ \
    -lCORE_DB_filters_ \
    -lCORE_DB_flif_ \
    -lCORE_DB_freetype_ \
    -lCORE_DB_fribidi_ \
    -lCORE_DB_glib_ \
    -lCORE_DB_harfbuzz_ \
    -lCORE_DB_highway_ \
    -lCORE_DB_jp2_ \
    -lCORE_DB_jpeg-turbo_ \
    -lCORE_DB_jpeg-xl_ \
    -lCORE_DB_lcms_ \
    -lCORE_DB_libde265_ \
    -lCORE_DB_libheif_ \
    -lCORE_DB_liblzma_ \
    -lCORE_DB_libraw_ \
    -lCORE_DB_librsvg_ \
    -lCORE_DB_libxml_ \
    -lCORE_DB_libzip_ \
    -lCORE_DB_lqr_ \
    -lCORE_DB_openjpeg_ \
    -lCORE_DB_pango_ \
    -lCORE_DB_pixman_ \
    -lCORE_DB_png_ \
    -lCORE_DB_raqm_ \
    -lCORE_DB_tiff_ \
    -lCORE_DB_webp_ \
    -lCORE_DB_zlib_

win32:!win32-g++:CONFIG(release, debug|release):
    #PRE_TARGETDEPS += $$PWD/lib/ImageMagick
else:win32:!win32-g++:CONFIG(debug, debug|release): \
    PRE_TARGETDEPS += \
    $$PWD/lib/ImageMagick/CORE_DB_Magick++_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_MagickCore_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_MagickWand_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_aom_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_brotli_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_bzlib_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_cairo_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_coders_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_croco_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_exr_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_ffi_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_filters_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_flif_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_freetype_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_fribidi_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_glib_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_harfbuzz_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_highway_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_jp2_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_jpeg-turbo_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_jpeg-xl_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_lcms_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_libde265_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_libheif_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_liblzma_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_libraw_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_librsvg_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_libxml_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_libzip_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_lqr_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_openjpeg_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_pango_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_pixman_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_png_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_raqm_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_tiff_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_webp_.lib \
    $$PWD/lib/ImageMagick/CORE_DB_zlib_.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
