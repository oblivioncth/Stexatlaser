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
        KTex::Header::PixelFormat pixelType = KTex::Header::PixelFormat::DXT5;
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

public:
    KTex convert();

};

#endif // CONVERSION_H
