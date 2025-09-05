// Unit Includes
#include "k-tex-io.h"

//===============================================================================================================
// K_TEX_WRITER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KTexWriter::KTexWriter(const KTex& sourceTex, const QString& targetFilePath) :
    mStreamWriter(targetFilePath, Qx::WriteMode::Overwrite),
    mSourceTex(sourceTex)
{
     mStreamWriter.setByteOrder(QDataStream::LittleEndian);
}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
Qx::IoOpReport KTexWriter::writeHeader()
{
    // Create specifications integer
    Qx::BitArray platformBits = Qx::BitArray::fromInteger(static_cast<int>(mSourceTex.header().platform()));
    platformBits.resize(KTex::Header::BL_PLATFORM_AC);
    Qx::BitArray pixelFormatBits = Qx::BitArray::fromInteger(static_cast<int>(mSourceTex.header().pixelFormat()));
    pixelFormatBits.resize(KTex::Header::BL_PIXEL_FORMAT_AC);
    Qx::BitArray textureTypeBits = Qx::BitArray::fromInteger(static_cast<int>(mSourceTex.header().textureType()));
    textureTypeBits.resize(KTex::Header::BL_TEXTURE_TYPE_AC);
    Qx::BitArray mipMapCountBits = Qx::BitArray::fromInteger(mSourceTex.mipMapCount());
    mipMapCountBits.resize(KTex::Header::BL_MIP_MAP_COUNT_AC);

    Qx::BitArray flagOneBit(KTex::Header::BL_FLAG_AC, mSourceTex.header().flagOne());
    Qx::BitArray flagTwoBit(KTex::Header::BL_FLAG_AC, mSourceTex.header().flagTwo());
    Qx::BitArray paddingBits(KTex::Header::BL_PADDING_AC, true);

    Qx::BitArray specifcationBits(platformBits +
                                  pixelFormatBits +
                                  textureTypeBits +
                                  mipMapCountBits +
                                  flagOneBit +
                                  flagTwoBit +
                                  paddingBits);

    quint32 specifications = specifcationBits.toInteger<quint32>();

    // Write magic number
    mStreamWriter.writeRawData(KTex::Header::MAGIC_NUM.toUtf8());

    // Write specs
    mStreamWriter << specifications;

    // Return writer status
    return mStreamWriter.status();
}

Qx::IoOpReport KTexWriter::writeMipMapMetadata(const KTex::MipMapImage& mipMap)
{
    // Write metadata fields
    mStreamWriter << mipMap.width();
    mStreamWriter << mipMap.height();
    mStreamWriter << mipMap.pitch();
    mStreamWriter << mipMap.imageDataSize();

    // Return writer status
    return mStreamWriter.status();
}

Qx::IoOpReport KTexWriter::writeMipMapData(const KTex::MipMapImage& mipMap)
{
    // Write data
    mStreamWriter.writeRawData(mipMap.imageData());

    // Return writer status
    return mStreamWriter.status();
}

//Public:
Qx::IoOpReport KTexWriter::write()
{
    // Track status
    Qx::IoOpReport status;

    // Open file
    if((status = mStreamWriter.openFile()).isFailure())
        return status;

    // Write header
    if((status = writeHeader()).isFailure())
        return status;

    // Write mip map metadata
    for(const KTex::MipMapImage& mipMap : mSourceTex.mipMaps())
    {
        if((status = writeMipMapMetadata(mipMap)).isFailure())
            return status;
    }

    // Write mip map data
    for(const KTex::MipMapImage& mipMap : mSourceTex.mipMaps())
    {
        if((status = writeMipMapData(mipMap)).isFailure())
            return status;
    }

    // Close file
    mStreamWriter.closeFile();

    // Return status
    return mStreamWriter.status();
}

//===============================================================================================================
// K_TEX_READER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KTexReader::KTexReader(const QString& sourceFilePath, KTex& targetTex, bool anyPixelFormat) :
    mStreamReader(sourceFilePath),
    mTargetTex(targetTex),
    mAnyPixelFormat(anyPixelFormat),
    mMipMapCount(0)
{
    mStreamReader.setByteOrder(QDataStream::LittleEndian);
}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
Qx::IoOpReport KTexReader::checkFileSupport(QByteArrayView magicNumberRaw)
{
    if(QString::fromUtf8(magicNumberRaw) != KTex::Header::MAGIC_NUM)
    {
        qWarning("Incorrect magic number.");
        return Qx::IoOpReport(Qx::IO_OP_READ, Qx::IO_ERR_READ, mStreamReader.file());
    }

    return mStreamReader.status();
}

Qx::IoOpReport KTexReader::checkFileSupport(quint8 platformRaw, quint8 pixelFormatRaw, quint8 textureTypeRaw)
{
    if(!KTex::supportedPlatform(platformRaw) ||
       (!mAnyPixelFormat &&!KTex::supportedPixelFormat(pixelFormatRaw)) ||
       !KTex::supportedTextureType(textureTypeRaw))
    {
        qWarning("TEX is unsupported.");
        return Qx::IoOpReport(Qx::IO_OP_READ, Qx::IO_ERR_READ, mStreamReader.file());
    }

    return mStreamReader.status();
}

Qx::IoOpReport KTexReader::parsePreCavesSpecs(const Qx::BitArray& specifcationBits)
{
    // Split specifications
    int startBit = 0;

    quint8 platformRaw = specifcationBits.subArray(startBit, KTex::Header::BL_PLATFORM_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_BC;
    quint8 pixelFormatRaw = specifcationBits.subArray(startBit, KTex::Header::BL_PIXEL_FORMAT_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_PIXEL_FORMAT_BC;
    quint8 textureTypeRaw = specifcationBits.subArray(startBit, KTex::Header::BL_TEXTURE_TYPE_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_TEXTURE_TYPE_BC;
    quint8 mipMapCountRaw = specifcationBits.subArray(startBit, KTex::Header::BL_MIP_MAP_COUNT_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_MIP_MAP_COUNT_BC;
    quint8 flagOneRaw = specifcationBits.subArray(startBit, KTex::Header::BL_FLAG_BC).toInteger<quint8>();

    // Check if file is unsupported
    Qx::IoOpReport specsCheck;
    if((specsCheck = checkFileSupport(platformRaw, pixelFormatRaw, textureTypeRaw)).isFailure())
        return specsCheck;

    // Assign values
    mTargetTex.header().setPlatform(static_cast<KTex::Header::Platform>(platformRaw));
    mTargetTex.header().setPixelFormat(static_cast<KTex::Header::PixelFormat>(pixelFormatRaw));
    mTargetTex.header().setTextureType(static_cast<KTex::Header::TextureType>(textureTypeRaw));
    mTargetTex.header().setFlagOne(flagOneRaw);
    mTargetTex.header().setFlagTwo(false);
    mMipMapCount = mipMapCountRaw; // Track within reader for later

    // Return status
    return mStreamReader.status();
}

Qx::IoOpReport KTexReader::parsePostCavesSpecs(const Qx::BitArray& specifcationBits)
{
    // Split specifications
    int startBit = 0;

    quint8 platformRaw = specifcationBits.subArray(startBit, KTex::Header::BL_PLATFORM_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_AC;
    quint8 pixelFormatRaw = specifcationBits.subArray(startBit, KTex::Header::BL_PIXEL_FORMAT_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_PIXEL_FORMAT_AC;
    quint8 textureTypeRaw = specifcationBits.subArray(startBit, KTex::Header::BL_TEXTURE_TYPE_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_TEXTURE_TYPE_AC;
    quint8 mipMapCountRaw = specifcationBits.subArray(startBit, KTex::Header::BL_MIP_MAP_COUNT_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_MIP_MAP_COUNT_AC;
    quint8 flagOneRaw = specifcationBits.subArray(startBit, KTex::Header::BL_FLAG_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_FLAG_AC;
    quint8 flagTwoRaw = specifcationBits.subArray(startBit, KTex::Header::BL_FLAG_AC).toInteger<quint8>();

    // Check if file is unsupported
    Qx::IoOpReport specsCheck;
    if((specsCheck = checkFileSupport(platformRaw, pixelFormatRaw, textureTypeRaw)).isFailure())
        return specsCheck;

    // Assign values
    mTargetTex.header().setPlatform(static_cast<KTex::Header::Platform>(platformRaw));
    mTargetTex.header().setPixelFormat(static_cast<KTex::Header::PixelFormat>(pixelFormatRaw));
    mTargetTex.header().setTextureType(static_cast<KTex::Header::TextureType>(textureTypeRaw));
    mTargetTex.header().setFlagOne(flagOneRaw);
    mTargetTex.header().setFlagTwo(flagTwoRaw);
    mMipMapCount = mipMapCountRaw; // Track within reader for later

    // Return status
    return mStreamReader.status();
}

Qx::IoOpReport KTexReader::readHeader()
{
    // Read magic number
    QByteArray magicNumber;
    mStreamReader.readRawData(magicNumber, KTex::Header::MAGIC_NUM.size());

    // Make sure file is correct format
    Qx::IoOpReport magicCheck;
    if((magicCheck = checkFileSupport(magicNumber)).isFailure())
        return magicCheck;

    // Read specification int
    quint32 specifications;
    mStreamReader >> specifications;

    // Parse specifications based on their version
    Qx::BitArray specificationBits = Qx::BitArray::fromInteger<quint32>(specifications);

    // This test has a false positive (for pre-caves update) if the input TEX is of the post-caves update variety,
    // has both flags set to high, and has at least 30 mipmaps. This is considered unlikely enough to be reasonable
    // (as it would likely result from an image with an initial size of 73,728 x 73,728) since there is no other way to check
    bool preCaves = specificationBits.subArray(14, KTex::Header::BL_PADDING_BC).count(true) == KTex::Header::BL_PADDING_BC;

    if(auto specCheck = preCaves ? parsePreCavesSpecs(specificationBits) : parsePostCavesSpecs(specificationBits); specCheck.isFailure())
        return specCheck;

    // Reserve space for known mipmap count
    mTargetTex.mipMaps().reserve(mMipMapCount);

    // Return status
    return mStreamReader.status();
}

Qx::IoOpReport KTexReader::readMipMapMetadata()
{
    // Mipmap to add
    KTex::MipMapImage mipMap;

    // Read metadata
    quint16 mipMapWidth;
    mStreamReader >> mipMapWidth;
    mipMap.setWidth(mipMapWidth);

    quint16 mipMapHeight;
    mStreamReader >> mipMapHeight;
    mipMap.setHeight(mipMapHeight);

    quint16 mipMapPitch;
    mStreamReader >> mipMapPitch;

    // Manually correct non-compliant mipmaps that report 0 pitch (*cough* matt's tools *cough*)
    if(mipMapPitch == 0)
    {
        auto pf = mTargetTex.header().pixelFormat();
        bool isCompressed = pf != KTex::Header::PixelFormat::RGB && pf != KTex::Header::PixelFormat::RGBA;

        if(isCompressed)
        {
            // Equivalent to squish::GetStorageRequirements(mipMapWidth, 1, squishFlag), but avoids squish include
            mipMapPitch = ((mipMapWidth + 3) / 4) * (pf == KTex::Header::PixelFormat::DXT1 ? 8 : 16);
        }
        else
            mipMapPitch = mipMapWidth * 4; // Used QImage formats are always 32-bit aligned.
    }
    mipMap.setPitch(mipMapPitch);

    quint32 mipMapDataSize;
    mStreamReader >> mipMapDataSize;
    mipMap.setImageDataSize(mipMapDataSize);

    // Add mipmap
    mTargetTex.mipMaps().append(mipMap);

    // Return status
    return mStreamReader.status();
}

Qx::IoOpReport KTexReader::readMipMapData(int i)
{
    // Read data chunk for given mipmap
    mStreamReader.readRawData(mTargetTex.mipMaps()[i].imageData(), mTargetTex.mipMaps()[i].imageDataSize());

    // Return status
    return mStreamReader.status();
}

//Public:
Qx::IoOpReport KTexReader::read()
{
    // Track status
    Qx::IoOpReport status;

    // Open file
    if((status = mStreamReader.openFile()).isFailure())
        return status;

    // Read header
    if((status = readHeader()).isFailure())
        return status;

    // Read mip map metadata
    for(int i = 0; i < mMipMapCount; i++)
    {
        if((status = readMipMapMetadata()).isFailure())
            return status;
    }

    // Read mip map data
    for(int i = 0; i < mMipMapCount; i++)
    {
        if((status = readMipMapData(i)).isFailure())
            return status;
    }

    if(!mStreamReader.atEnd())
        qWarning("There was still data left in the file after reading all mipmaps!");

    // Close file
    mStreamReader.closeFile();

    // Return status
    return mStreamReader.status();
}
