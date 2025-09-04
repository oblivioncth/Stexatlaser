// Unit Includes
#include "conversion.h"

// Standard Library Includes
#include <cstring>

// Squish Includes
#include <squish/squish.h>

// etc2comp Includes
#include <Etc/EtcImage.h>

// NOTE: The Qt image formats (QImage::Format) used here are all byte ordered based on the host system, yet squish (and DST?)
// expect the input data to always be RGB(A), which means these won't work on Big Endian ordered systems;
// however this is considered acceptable given the target system is x86 based (LE)

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
    // Ideally this would perform a bit more image processing but Qt doesn't have much
    // and for now the priority is to keep lib dependency low (ImageMagick feature disabling
    // on Windows in particular is really troublesome and it has a conflict with harfbuzz in Qt)

    QVector<QImage> mipMaps;
    QSize mipMapSize = baseImage.size();

    while(mipMapSize != QSize(1,1))
    {
        mipMapSize /= 2; // This always rounds up to the nearest integer, so it won't drop the dimensions below 1x1
        mipMaps.append(baseImage.scaled(mipMapSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }

    return mipMaps;
}

QVector<KTex::MipMapImage> ToTexConverter::convertToTargetFormat(const QVector<QImage>& images)
{
    auto pxFormat = mOptions.pixelFormat;
    QVector<KTex::MipMapImage> outputImages;

    for(const QImage& image : images)
    {
        KTex::MipMapImage mipMap;

        // Common steps
        mipMap.setWidth(image.width());
        mipMap.setHeight(image.height());

        // Encoder specific steps
        switch(pxFormat) // Use variants, inheritance, or other functions for this if many more types are added
        {
            using enum KTex::Header::PixelFormat;

            case RGB:
            case RGBA:
                mipMap.setPitch(image.bytesPerLine());
                mipMap.setImageDataSize(image.sizeInBytes());
                std::memcpy(mipMap.imageData().data(), image.bits(), image.sizeInBytes());
                break;

            case DXT1:
            case DXT3:
            case DXT5:
            {
                int squishFlag = getSquishCompressionFlag(pxFormat);
                mipMap.setPitch(squish::GetStorageRequirements(image.width(), 1, squishFlag)); // Space for one row of blocks
                mipMap.setImageDataSize(squish::GetStorageRequirements(image.width(), image.height(), squishFlag));
                squish::CompressImage(image.bits(), image.width(), image.height(), image.bytesPerLine(),
                                      mipMap.imageData().data(), squishFlag);
            }

            case ETC2EAC:
            {
                /* The pitch calculation below boils down to:
                 *     p = blocks_per_row * block_size
                 *     where
                 *     block_size varies with format
                 *     blocks_per_row = ceil(width/4)
                 */

                // Get texture info
                auto etcFormat = Etc::Image::Format::RGBA8; // Make function for this, like for squish, if more ETC formats are supported

                // Create ETC image
                auto errMetric = image.isGrayscale() ? Etc::ErrorMetric::GRAY : Etc::ErrorMetric::NUMERIC; // Could try the Rec 709 option for color
                /* The standard allows viewing a POD struct as a sequence of bytes (e.g. auto data = reinterpret_cast<uchar*>(myStruct)),
                 * but does not allow the other way around; however, in the case of a very simple struct of ints, almost no known compiler
                 * inserts padding between the members, so here we're gonna try what is technically UB, casting an array to a struct, since
                 * it generally works and this application is non-critical. This is possible because each struct member is exactly 1-byte in
                 * size and is laid out in the correct R-G-B-A order.
                 *
                 * This lib has a really strange interface, as it was hacked together by someone else after its initial creation.
                 * You make an image with uncompressed pixel data, despite the type (Etc::Image) being named like you already have
                 * a compressed image, and then call Encode.
                 */
                Etc::Image etcImage(etcFormat, (const Etc::ColorR8G8B8A8*)image.bits(), image.width(), image.height(), errMetric);

                // Prepare mip-map
                mipMap.setPitch(etcImage.GetNumberOfBlockColumns() * etcImage.GetBlockSize()); // Space for one row of blocks
                mipMap.setImageDataSize(etcImage.GetEncodingBitsBytes());

                // Encode
                constexpr float quality = 90; /* (0-100) could add flag to allow adjusting, but awkward since its just for this format, though we could just
                                               * say "for formats where it applies" and for now it's only this one. Kram uses 49 by default and states that
                                               * unity uses "80".
                                               */
                auto status = etcImage.EncodeSinglepass(quality, reinterpret_cast<uchar*>(mipMap.imageData().data()));
                if(status != Etc::Image::SUCCESS)
                    qWarning("Unexpected ETC2 encode error: 0x%x", status);
                break;
            }

            default:
                qCritical("Unhandled encoding pixel format!");
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

    // Flip
    baseImage.mirror(); // .flip() in >= Qt 6.9.0

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
    QByteArray decodedData;
    quint16 decodedPitch{};
    QImage::Format decodedFormat{};

    auto pxFormat = mSourceTex.header().pixelFormat();
    switch(pxFormat) // Use variants, inheritance, or other functions for this if many more types are added
    {
        using enum KTex::Header::PixelFormat;

        case RGB:
            decodedFormat = QImage::Format_RGB888;
            decodedPitch = mainImage.pitch();
            decodedData = mainImage.imageData(); // Implicit sharing avoids copy
            break;

        case RGBA:
            decodedFormat = mOptions.demultiplyAlpha ? QImage::Format_RGBA8888_Premultiplied : QImage::Format_RGBA8888;
            decodedPitch = mainImage.pitch();
            decodedData = mainImage.imageData(); // Implicit sharing avoids copy
            break;

        case DXT1:
        case DXT3:
        case DXT5:
        {
            decodedFormat = mOptions.demultiplyAlpha ? QImage::Format_RGBA8888_Premultiplied : QImage::Format_RGBA8888;
            decodedPitch = mainImage.width() * 4;
            int squishFlag = getSquishCompressionFlag(pxFormat);
            decodedData.resize(mainImage.width() * mainImage.height() * 4);
            squish::DecompressImage(reinterpret_cast<uchar*>(decodedData.data()), mainImage.width(), mainImage.height(), decodedPitch,
                                    mainImage.imageData().data(), squishFlag);
            break;
        }

        case ETC2EAC:
        {
            decodedFormat = mOptions.demultiplyAlpha ? QImage::Format_RGBA8888_Premultiplied : QImage::Format_RGBA8888;
            decodedPitch = mainImage.width() * 4;
            auto etcFormat = Etc::Image::Format::RGBA8; // Make function for this, like for squish, if more ETC formats are supported
            decodedData.resize(mainImage.width() * mainImage.height() * 4);
            /* This lib has a really strange interface, as it was hacked together by someone else after its initial creation.
             * You make an image with no pixel data set and then pass the pixel data as part of the Encode call
             */
            Etc::Image etcImage(etcFormat, nullptr, 1024, 1024, Etc::ErrorMetric::NUMERIC);
            auto status = etcImage.Decode(reinterpret_cast<const uchar*>(mainImage.imageData().data()),
                                          reinterpret_cast<uchar*>(decodedData.data()));
            if(status != Etc::Image::SUCCESS)
                qWarning("Unexpected ETC2 decode error: 0x%x", status);
            break;
        }

        default:
            qCritical("Unhandled decoding pixel format!");
    }

    // Create QImage from buffer
    QImage bufferedImage = QImage(reinterpret_cast<uchar*>(decodedData.data()), mainImage.width(), mainImage.height(), decodedPitch, decodedFormat);

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
    QImage img = convertToStandardFormat(mainImage);

    // Flip
    img.mirror(); // .flip() in >= Qt 6.9.0

    return img;
}
