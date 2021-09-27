#include "conversion.h"
#include "Squish/squish.h"

//===============================================================================================================
// UNIT ONLY
//===============================================================================================================
namespace
{
    int getSquishCompressionFlag(KTex::Header::PixelFormat pixelFormat)
    {
        switch(pixelFormat)
        {
            case KTex::Header::PixelFormat::DXT5:
                return squish::kDxt5;

            case KTex::Header::PixelFormat::DXT3:
                return squish::kDxt3;

            case KTex::Header::PixelFormat::DXT1:
                return squish::kDxt1;

            default:
                return -1; // Should never occur
        }
    }
}

//===============================================================================================================
// TO_TEX_CONVERTER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
ToTexConverter::ToTexConverter(const QImage& sourceImage, const Options& options) :
    mSourceImage(sourceImage),
    mOptions(options)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
QImage ToTexConverter::convertToBasePixelFormat()
{
    QImage::Format newFormat;

    if(mOptions.pixelFormat == KTex::Header::PixelFormat::RGB)
        newFormat = QImage::Format_RGB888;
    else // The S3TC formats need an RGBA input for compression as well
    {
        if(mOptions.premultiplyAlpha)
            newFormat = QImage::Format_RGBA8888_Premultiplied;
        else
            newFormat = QImage::Format_RGBA8888;
    }

    return mSourceImage.convertToFormat(newFormat);
}

QVector<QImage> ToTexConverter::generateMipMaps(const QImage& baseImage)
{
    QVector<QImage> mipMaps;
    QSize mipMapSize = baseImage.size()/2;

    // Ideally this would perform a bit more image processing but Qt doesn't have much
    // and for now the priority is to keep lib dependency low (ImageMagick feature disabling
    // on Windows in particular is really troublesome and it has a conflict with harfbuzz in Qt)
    while(mipMapSize.isValid())
        mipMaps.append(baseImage.scaled(mipMapSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    return mipMaps;
}

QVector<KTex::MipMapImage> ToTexConverter::convertToTargetFormat(const QVector<QImage>& images)
{
    QVector<KTex::MipMapImage> outputImages;

    for(const QImage& image : images)
    {
        KTex::MipMapImage mipMap;

        // Common steps
        mipMap.setWidth(image.width());
        mipMap.setHeight(image.height());

        // Uncompressed steps
        if(mOptions.pixelFormat == KTex::Header::PixelFormat::RGBA ||
           mOptions.pixelFormat == KTex::Header::PixelFormat::RGB)
        {
            mipMap.setPitch(image.bytesPerLine());
            mipMap.setImageDataSize(image.sizeInBytes());
            std::memcpy(mipMap.imageData().data(), image.bits(), image.sizeInBytes());
        }
        else // Compressed steps
        {
            int squishFlag = getSquishCompressionFlag(mOptions.pixelFormat);
            mipMap.setPitch(squish::GetStorageRequirements(image.width(), 1, squishFlag));
            mipMap.setImageDataSize(squish::GetStorageRequirements(image.width(), image.height(), squishFlag));
            squish::CompressImage(image.bits(), image.width(), image.height(), image.bytesPerLine(),
                                  mipMap.imageData().data(), squishFlag);
        }

        outputImages.append(mipMap);
    }

    return outputImages;
}

//Public:
KTex ToTexConverter::convert()
{
    // Convert to base pixel format to work with
    QImage baseImage = convertToBasePixelFormat();

    // Working images
    QVector<QImage> workingImages = {baseImage};

    // Generate mipmaps
    if(mOptions.generateMipMaps)
        workingImages = workingImages + generateMipMaps(baseImage);

    // Convert to KTex image/mipmap format
    QVector<KTex::MipMapImage> outputImages = convertToTargetFormat(workingImages);

    // Create KTex
    KTex tex;

    // Set header specs
    tex.header().setTextureType(mOptions.textureType);
    tex.header().setPixelFormat(mOptions.pixelFormat);

    // Set imags
    tex.mipMaps().swap(outputImages);

    // Return finished tex
    return tex;
}

//===============================================================================================================
// FROM_TEX_CONVERTER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
FromTexConverter::FromTexConverter(const KTex& sourceTex) :
    mSourceTex(sourceTex)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:

//Public:
QImage FromTexConverter::convert()
{
    return QImage();
}
