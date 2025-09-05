#ifndef KTEX_H
#define KTEX_H

// Qt Includes
#include <QString>
#include <QVector>
#include <QHash>

using namespace Qt::Literals::StringLiterals;

class KTex
{
//-Inner Classes----------------------------------------------------------------------------------------------------
public:
    class Header
    {
    //-Enums---------------------------------------------------------------------------------------------------------
    public:
        enum class Platform : quint8{
            Default = 0x00,
            PC = 0x0C,
            PS3 = 0x0A,
            Xbox360 = 0x0B
        };
        enum class PixelFormat : quint8{
            DXT1 = 0x00,
            DXT3 = 0x01,
            DXT5 = 0x02,
            RGBA = 0x04,
            RGB = 0x05,
            ETC2EAC = 0x11
        };
        enum class TextureType : quint8{
            OneD = 0x00,
            TwoD = 0x01,
            ThreeD = 0x02,
            CubeMapped = 0x03
        };

    //-Class Members-------------------------------------------------------------------------------------------------
    public:
        static inline const QString MAGIC_NUM = u"KTEX"_s;

        // Sizes
        static const int BL_PLATFORM_BC = 3;
        static const int BL_PIXEL_FORMAT_BC = 3;
        static const int BL_TEXTURE_TYPE_BC = 3;
        static const int BL_MIP_MAP_COUNT_BC = 4;
        static const int BL_FLAG_BC = 1;
        static const int BL_PADDING_BC = 18;

        static const int BL_PLATFORM_AC = 4;
        static const int BL_PIXEL_FORMAT_AC = 5;
        static const int BL_TEXTURE_TYPE_AC = 4;
        static const int BL_MIP_MAP_COUNT_AC = 5;
        static const int BL_FLAG_AC = 1;
        static const int BL_PADDING_AC = 12;

    //-Instance Members------------------------------------------------------------------------------------------------
    private:
        Platform mPlatform;
        PixelFormat mPixelFormat;
        TextureType mTextureType;
        bool mFlagOne;
        bool mFlagTwo;

    //-Constructor-------------------------------------------------------------------------------------------------------
    public:
        Header();

    //-Instance Functions------------------------------------------------------------------------------------------------
    public:
        Platform platform() const;
        PixelFormat pixelFormat() const;
        TextureType textureType() const;
        bool flagOne() const;
        bool flagTwo() const;

        void setPlatform(Platform platform);
        void setPixelFormat(PixelFormat pixelFormat);
        void setTextureType(TextureType textureType);
        void setFlagOne(bool flagOne);
        void setFlagTwo(bool flagTwo);
    };

    class MipMapImage
    {
    //-Class Members-------------------------------------------------------------------------------------------------
    public:

    //-Instance Members------------------------------------------------------------------------------------------------
    private:
        quint16 mWidth;
        quint16 mHeight;
        quint16 mPitch;
        QByteArray mImageData;

    //-Constructor-------------------------------------------------------------------------------------------------------
    public:
        MipMapImage();

    //-Instance Functions----------------------------------------------------------------------------------------------
    public:
        quint16 width() const;
        quint16 height() const;
        quint16 pitch() const;
        quint32 imageDataSize() const;
        QByteArray& imageData();
        const QByteArray& imageData() const;
        QByteArray jsonMetadata() const;

        void setWidth(quint16 width);
        void setHeight(quint16 height);
        void setPitch(quint16 pitch);
        void setImageDataSize(quint32 size);
    };

//-Class Members----------------------------------------------------------------------------------------------------
private:
    static inline const QHash<Header::Platform, QString> PLATFORM_STRINGS = {
        {KTex::Header::Platform::Default, u"Default"_s},
        {KTex::Header::Platform::PC, u"PC"_s},
        {KTex::Header::Platform::PS3, u"PS3"_s},
        {KTex::Header::Platform::Xbox360, u"Xbox360"_s},
        };
    static inline const QHash<Header::PixelFormat, QString> PIXEL_FORMAT_STRINGS = {
        {KTex::Header::PixelFormat::DXT1, u"DXT1"_s},
        {KTex::Header::PixelFormat::DXT3, u"DXT3"_s},
        {KTex::Header::PixelFormat::DXT5, u"DXT5"_s},
        {KTex::Header::PixelFormat::RGB, u"RGB"_s},
        {KTex::Header::PixelFormat::RGBA, u"RGBA"_s}
    };
    static inline const QHash<Header::TextureType, QString> TEXTURE_TYPE_STRINGS = {
        {KTex::Header::TextureType::OneD, u"1D"_s},
        {KTex::Header::TextureType::TwoD, u"2D"_s},
        {KTex::Header::TextureType::ThreeD, u"3D"_s},
        {KTex::Header::TextureType::CubeMapped, u"Cube Mapped"_s},
    };

//-Instance Members-------------------------------------------------------------------------------------------------
private:
    Header mHeader;
    QVector<MipMapImage> mMipMaps;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KTex();

//-Class Functions---------------------------------------------------------------------------------------------------
public:
    static QString standardExtension();
    static bool supportedPlatform(quint8 platformVal);
    static bool supportedPixelFormat(quint8 pixelFormatVal);
    static bool supportedTextureType(quint8 textureTypeVal);

//-Instance Functions----------------------------------------------------------------------------------------------
public:
    Header& header();
    const Header& header() const;
    quint8 mipMapCount() const;
    bool hasMipMaps() const;
    QVector<MipMapImage>& mipMaps();
    const QVector<MipMapImage>& mipMaps() const;

    QString info(bool indent = false) const;

};

#endif // KTEX_H
