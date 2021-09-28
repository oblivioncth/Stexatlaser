#include "conversion.h"
#include "Squish/squish.h"

// The Qt image formats (QImage::Format) used here are all byte ordered based on the host system, yet squish (and DST?)
// expect the input data to always be RGB(A), which means these won't work on Big Endian ordered systems;
// however this is considered acceptible given the target system is x86 based (LE)

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
FromTexConverter::FromTexConverter(const KTex& sourceTex, const Options& options) :
    mSourceTex(sourceTex),
    mOptions(options)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
const KTex::MipMapImage& FromTexConverter::getMainImage() { return mSourceTex.mipMaps().at(0); } //TODO: Check if mipmap actually exists

QImage FromTexConverter::convertToStandardFormat(const KTex::MipMapImage& mainImage)
{
    QByteArray rawData;
    quint16 pitch;
    QImage::Format rawFormat = mSourceTex.header().pixelFormat() == KTex::Header::PixelFormat::RGB ?
                               QImage::Format_RGB888 : mOptions.demultiplyAlpha ?
                                                       QImage::Format_RGBA8888_Premultiplied :
                                                       QImage::Format_RGBA8888;

    // Uncompressed steps
    if(mSourceTex.header().pixelFormat() == KTex::Header::PixelFormat::RGBA ||
       mSourceTex.header().pixelFormat() == KTex::Header::PixelFormat::RGB)
    {
        rawData.resize(mainImage.imageDataSize());
        pitch = mainImage.pitch();
        std::memcpy(rawData.data(), mainImage.imageData().data(), mainImage.imageDataSize());
    }
    else // Compressed steps
    {
        // Always outputs in RGBA
        int squishFlag = getSquishCompressionFlag(mSourceTex.header().pixelFormat());
        pitch = mainImage.width() * 4;
        rawData.resize(mainImage.width() * mainImage.height() * 4);
        squish::DecompressImage(reinterpret_cast<uchar*>(rawData.data()), mainImage.width(), mainImage.height(), mainImage.pitch(),
                                mainImage.imageData().data(), squishFlag);

    }

    // Create QImage from buffer
    QImage bufferedImage = QImage(reinterpret_cast<uchar*>(rawData.data()), mainImage.width(), mainImage.height(), pitch, rawFormat);

    // Copy and detach image so it isn't reliant on buffer that will be destroyed
    QImage standaloneImage = bufferedImage.copy();
    standaloneImage.detach();

    return standaloneImage;
}

//Public:
QImage FromTexConverter::convert()
{
    // Get primariy image
    const KTex::MipMapImage& mainImage = getMainImage();

    // Convert to QImage
    return convertToStandardFormat(mainImage);
}
