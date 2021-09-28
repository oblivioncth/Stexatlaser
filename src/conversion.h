#ifndef CONVERSION_H
#define CONVERSION_H

#include <QImage>

#include "klei/k-tex.h"

class ToTexConverter
{
//-Structs----------------------------------------------------------------------------------------------------------
public:
    struct Options
    {
        KTex::Header::Platform platform = KTex::Header::Platform::PC;
        KTex::Header::TextureType textureType = KTex::Header::TextureType::TwoD;
        KTex::Header::PixelFormat pixelFormat = KTex::Header::PixelFormat::DXT5;
        bool generateMipMaps = true;
        bool premultiplyAlpha = true;
    };

//-Class Members----------------------------------------------------------------------------------------------------
private:

//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const QImage& mSourceImage;
    const Options& mOptions;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    ToTexConverter(const QImage& sourceImage, const Options& options);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QImage convertToBasePixelFormat();
    QVector<QImage> generateMipMaps(const QImage& baseImage);
    QVector<KTex::MipMapImage> convertToTargetFormat(const QVector<QImage>& images);

public:
    KTex convert();
};

class FromTexConverter
{
//-Class Members----------------------------------------------------------------------------------------------------
private:
    struct Options
    {
        bool demultiplyAlpha = true;
    };
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const KTex& mSourceTex;
    const Options& mOptions;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    FromTexConverter(const KTex& sourceTex, const Options& options);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    const KTex::MipMapImage& getMainImage();
    QImage convertToStandardFormat(const KTex::MipMapImage& mainImage);

public:
    QImage convert();
};

#endif // CONVERSION_H
