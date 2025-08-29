// Unit Includes
#include "k-tex.h"

// Qx Includes
#include <qx/core/qx-json.h>

namespace
{

struct MipMapMeta
{
    quint16 width;
    quint16 height;
    quint16 pitch;
    QX_JSON_STRUCT(width, height, pitch);
};

}

//===============================================================================================================
// K_TEX::HEADER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KTex::Header::Header() :
    mPlatform(KTex::Header::Platform::Default),
    mPixelFormat(KTex::Header::PixelFormat::DXT5),
    mTextureType(KTex::Header::TextureType::TwoD),
    mFlagOne(true),
    mFlagTwo(true)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Public:
KTex::Header::Platform KTex::Header::platform() const { return mPlatform; }
KTex::Header::PixelFormat KTex::Header::pixelFormat() const { return mPixelFormat; }
KTex::Header::TextureType KTex::Header::textureType() const { return mTextureType; }
bool KTex::Header::flagOne() const { return mFlagOne; }
bool KTex::Header::flagTwo() const { return mFlagTwo; }

void KTex::Header::setPlatform(Platform platform) { mPlatform = platform; }
void KTex::Header::setPixelFormat(PixelFormat pixelFormat) { mPixelFormat = pixelFormat; }
void KTex::Header::setTextureType(TextureType textureType) { mTextureType = textureType; }
void KTex::Header::setFlagOne(bool flagOne) { mFlagOne = flagOne; }
void KTex::Header::setFlagTwo(bool flagTwo) { mFlagTwo = flagTwo; }

//===============================================================================================================
// K_TEX::MIP_MAP_IMAGE
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KTex::MipMapImage::MipMapImage() :
    mWidth(0),
    mHeight(0),
    mPitch(0),
    mImageData()
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Public:
quint16 KTex::MipMapImage::width() const { return mWidth; }
quint16 KTex::MipMapImage::height() const { return mHeight; };
quint16 KTex::MipMapImage::pitch() const { return mPitch; }
quint32 KTex::MipMapImage::imageDataSize() const { return mImageData.size(); }
QByteArray& KTex::MipMapImage::imageData() { return mImageData; }
const QByteArray& KTex::MipMapImage::imageData() const { return mImageData; }

QByteArray KTex::MipMapImage::jsonMetadata() const
{
    MipMapMeta meta{mWidth, mHeight, mPitch};
    QByteArray json;
    Qx::serializeJson(json, meta);
    return json;
}

void KTex::MipMapImage::setWidth(quint16 width) { mWidth = width; }
void KTex::MipMapImage::setHeight(quint16 height) { mHeight = height; }
void KTex::MipMapImage::setPitch(quint16 pitch) { mPitch = pitch; }
void KTex::MipMapImage::setImageDataSize(quint32 size) { mImageData.resize(size); }

//===============================================================================================================
// K_TEX
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KTex::KTex() :
    mHeader(),
    mMipMaps()
{}

//-Class Functions---------------------------------------------------------------------------------------------------
//Public:
QString KTex::standardExtension() { return u"tex"_s; }

bool KTex::supportedPlatform(quint8 platformVal)
{
    switch(platformVal)
    {
        case static_cast<quint8>(Header::Platform::Default):
        case static_cast<quint8>(Header::Platform::PC):
        case static_cast<quint8>(Header::Platform::PS3):
        case static_cast<quint8>(Header::Platform::Xbox360):
            return true;
        default:
            return false;
    }
}

bool KTex::supportedPixelFormat(quint8 pixelFormatVal)
{
    switch(pixelFormatVal)
    {
        case static_cast<quint8>(Header::PixelFormat::DXT1):
        case static_cast<quint8>(Header::PixelFormat::DXT3):
        case static_cast<quint8>(Header::PixelFormat::DXT5):
        case static_cast<quint8>(Header::PixelFormat::RGB):
        case static_cast<quint8>(Header::PixelFormat::RGBA):
            return true;
        default:
            return false;
    }
}

bool KTex::supportedTextureType(quint8 textureTypeVal)
{
    switch(textureTypeVal)
    {
        case static_cast<quint8>(Header::TextureType::OneD):
        case static_cast<quint8>(Header::TextureType::TwoD):
        case static_cast<quint8>(Header::TextureType::ThreeD):
        case static_cast<quint8>(Header::TextureType::CubeMapped):
            return true;
        default:
            return false;
    }
}

//-Instance Functions--------------------------------------------------------------------------------------------
//Public:
KTex::Header& KTex::header() { return mHeader; }
const KTex::Header& KTex::header() const { return mHeader; }
quint8 KTex::mipMapCount() const { return mMipMaps.count(); }
bool KTex::hasMipMaps() const { return !mMipMaps.isEmpty(); }
QVector<KTex::MipMapImage>& KTex::mipMaps() { return mMipMaps; }
const QVector<KTex::MipMapImage>& KTex::mipMaps() const { return mMipMaps; }

QString KTex::info(bool indent) const
{
    QStringList infoPoints;
    infoPoints << u"Platform: "_s + PLATFORM_STRINGS[mHeader.platform()];
    auto pxForm = PIXEL_FORMAT_STRINGS.find(mHeader.pixelFormat());
    QString pxFormStr = pxForm != PIXEL_FORMAT_STRINGS.end() ? pxForm.value() : "Unknown";
    infoPoints << u"Pixel Format: "_s + pxFormStr;
    infoPoints << u"Texture Type: "_s + TEXTURE_TYPE_STRINGS[mHeader.textureType()];
    infoPoints << u"Unknown Flag 1: "_s + (mHeader.flagOne() ? "true" : "false");
    infoPoints << u"Unknown Flag 2: "_s + (mHeader.flagTwo() ? "true" : "false");
    infoPoints << u"Mip Maps: "_s + QString::number(mMipMaps.count());

    for(const auto& mm : mMipMaps)
        infoPoints << u"  - "_s + (u"%1 x %2"_s).arg(mm.width()).arg(mm.height());

    if(indent)
    {
        for(auto& ip : infoPoints)
            ip.prepend(u"  "_s);
    }

    return infoPoints.join('\n');
}
