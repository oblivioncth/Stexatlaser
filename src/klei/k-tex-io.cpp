#include "k-tex-io.h"

//===============================================================================================================
// K_TEX_WRITER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KTexWriter::KTexWriter(const KTex& sourceTex, QFile& targetFile) :
    mStreamWriter(targetFile, Qx::WriteMode::Overwrite),
    mSourceTex(sourceTex)
{
     mStreamWriter.setByteOrder(QDataStream::LittleEndian);
}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
Qx::IOOpReport KTexWriter::writeHeader()
{
    // Create specifications integer
    Qx::BitArray platformBits = Qx::BitArray::fromInteger(static_cast<int>(mSourceTex.header().platform()));
    platformBits.resize(KTex::Header::BL_PLATFORM_AC);
    Qx::BitArray pixelFormatBits = Qx::BitArray::fromInteger(static_cast<int>(mSourceTex.header().pixelFormat()));
    platformBits.resize(KTex::Header::BL_PLATFORM_AC);
    Qx::BitArray textureTypeBits = Qx::BitArray::fromInteger(static_cast<int>(mSourceTex.header().textureType()));
    platformBits.resize(KTex::Header::BL_PLATFORM_AC);
    Qx::BitArray mipMapCountBits = Qx::BitArray::fromInteger(mSourceTex.mipMapCount());
    platformBits.resize(KTex::Header::BL_MIP_MAP_COUNT_AC);

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

Qx::IOOpReport KTexWriter::writeMipMapMetadata(const KTex::MipMapImage& mipMap)
{
    // Write metadata fields
    mStreamWriter << mipMap.width();
    mStreamWriter << mipMap.height();
    mStreamWriter << mipMap.pitch();
    mStreamWriter << mipMap.imageDataSize();

    // Return writer status
    return mStreamWriter.status();
}

Qx::IOOpReport KTexWriter::writeMipMapData(const KTex::MipMapImage& mipMap)
{
    // Write data
    mStreamWriter.writeRawData(mipMap.imageData());

    // Return writer status
    return mStreamWriter.status();
}

//Public:
Qx::IOOpReport KTexWriter::write()
{
    // Track status
    Qx::IOOpReport status;

    // Open file
    if(!(status = mStreamWriter.openFile()).wasSuccessful())
        return status;

    // Write header
    if(!(status = writeHeader()).wasSuccessful())
        return status;

    // Write mip map metadata
    for(const KTex::MipMapImage& mipMap : mSourceTex.mipMaps())
    {
        if(!(status = writeMipMapMetadata(mipMap)).wasSuccessful())
            return status;
    }

    // Write mip map data
    for(const KTex::MipMapImage& mipMap : mSourceTex.mipMaps())
    {
        if(!(status = writeMipMapData(mipMap)).wasSuccessful())
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
KTexReader::KTexReader(QFile& sourceFile, KTex& targetTex) :
    mSourceFile(sourceFile),
    mStreamReader(sourceFile),
    mTargetTex(targetTex),
    mMipMapCount(0),
    mSupported()
{
    mStreamReader.setByteOrder(QDataStream::LittleEndian);
}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
Qx::IOOpReport KTexReader::checkFileSupport(QByteArray magicNumberRaw)
{
    if(QString::fromUtf8(magicNumberRaw) != KTex::Header::MAGIC_NUM)
    {
        *mSupported = false;
        return Qx::IOOpReport(Qx::IO_OP_READ, Qx::IO_ERR_READ, mSourceFile);
    }

    return mStreamReader.status();
}

Qx::IOOpReport KTexReader::checkFileSupport(quint8 platformRaw, quint8 pixelFormatRaw, quint8 textureTypeRaw)
{
    if(!KTex::supportedPlatform(platformRaw) ||
       !KTex::supportedPixelFormat(pixelFormatRaw)  ||
       !KTex::supportedTextureType(textureTypeRaw))
    {
        *mSupported = false;
        return Qx::IOOpReport(Qx::IO_OP_READ, Qx::IO_ERR_READ, mSourceFile);
    }

    return mStreamReader.status();
}

Qx::IOOpReport KTexReader::parsePreCavesSpecs(Qx::BitArray specifcationBits)
{
    // Split specificatons
    int startBit = 0;

    quint8 platformRaw = specifcationBits.extract(startBit, KTex::Header::BL_PLATFORM_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_BC;
    quint8 pixelFormatRaw = specifcationBits.extract(startBit, KTex::Header::BL_PLATFORM_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_BC;
    quint8 textureTypeRaw = specifcationBits.extract(startBit, KTex::Header::BL_PLATFORM_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_BC;
    quint8 mipMapCountRaw = specifcationBits.extract(startBit, KTex::Header::BL_MIP_MAP_COUNT_BC).toInteger<quint8>();
    startBit += KTex::Header::BL_MIP_MAP_COUNT_BC;
    quint8 flagOneRaw = specifcationBits.extract(startBit, KTex::Header::BL_FLAG_BC).toInteger<quint8>();

    // Check if file is unsupported
    Qx::IOOpReport specsCheck;
    if(!(specsCheck = checkFileSupport(platformRaw, pixelFormatRaw, textureTypeRaw)).wasSuccessful())
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

Qx::IOOpReport KTexReader::parsePostCavesSpecs(Qx::BitArray specifcationBits)
{
    // Split specificatons
    int startBit = 0;

    quint8 platformRaw = specifcationBits.extract(startBit, KTex::Header::BL_PLATFORM_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_AC;
    quint8 pixelFormatRaw = specifcationBits.extract(startBit, KTex::Header::BL_PLATFORM_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_AC;
    quint8 textureTypeRaw = specifcationBits.extract(startBit, KTex::Header::BL_PLATFORM_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_PLATFORM_AC;
    quint8 mipMapCountRaw = specifcationBits.extract(startBit, KTex::Header::BL_MIP_MAP_COUNT_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_MIP_MAP_COUNT_AC;
    quint8 flagOneRaw = specifcationBits.extract(startBit, KTex::Header::BL_FLAG_AC).toInteger<quint8>();
    startBit += KTex::Header::BL_FLAG_AC;
    quint8 flagTwoRaw = specifcationBits.extract(startBit, KTex::Header::BL_FLAG_AC).toInteger<quint8>();

    // Check if file is unsupported
    Qx::IOOpReport specsCheck;
    if(!(specsCheck = checkFileSupport(platformRaw, pixelFormatRaw, textureTypeRaw)).wasSuccessful())
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

Qx::IOOpReport KTexReader::readHeader()
{
    // Read magic number
    QByteArray magicNumber;
    mStreamReader.readRawData(magicNumber, KTex::Header::MAGIC_NUM.size());

    // Make sure file is correct format
    Qx::IOOpReport magicCheck;
    if(!(magicCheck = checkFileSupport(magicNumber)).wasSuccessful())
        return magicCheck;

    // Read specifcation int
    quint32 specifications;
    mStreamReader >> specifications;

    // Parse specifcations based on their version
    Qx::BitArray specifcationBits = Qx::BitArray::fromInteger<quint32>(specifications);

    // This test has a false positive (for pre-caves update) if the input TEX is of the post-caves update variety,
    // has both flags set to high, and has at least 30 mipmaps. This is considered unlikely enough to be reasonable
    // (as it would likely result from an image with an initial size of 73,728 x 73,728) since there is no other way to check
    if(specifcationBits.extract(14, KTex::Header::BL_PADDING_BC).count(true) == KTex::Header::BL_PADDING_BC)
        parsePreCavesSpecs(specifcationBits);
    else
        parsePostCavesSpecs(specifcationBits);

    // Reserve space for known mipmap count
    mTargetTex.mipMaps().reserve(mMipMapCount);

    // Return status
    return mStreamReader.status();
}

Qx::IOOpReport KTexReader::readMipMapMetadata()
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
    mipMap.setPitch(mipMapPitch);

    quint32 mipMapDataSize;
    mStreamReader >> mipMapDataSize;
    mipMap.setImageDataSize(mipMapDataSize);

    // Add mipmap
    mTargetTex.mipMaps().append(mipMap);

    // Return status
    return mStreamReader.status();
}

Qx::IOOpReport KTexReader::readMipMapData(int i)
{
    // Read data chunk for given mipmap
    mStreamReader.readRawData(mTargetTex.mipMaps()[i].imageData(), mTargetTex.mipMaps()[i].imageDataSize());

    // Return status
    return mStreamReader.status();
}

//Public:
Qx::IOOpReport KTexReader::read(bool& supported)
{
    // Initialize supported status flag
    mSupported = &supported;
    *mSupported = true;

    // Track status
    Qx::IOOpReport status;

    // Open file
    if(!(status = mStreamReader.openFile()).wasSuccessful())
        return status;

    // Read header
    if(!(status = readHeader()).wasSuccessful())
        return status;

    // Read mip map metadata
    for(int i = 0; i < mMipMapCount; i++)
    {
        if(!(status = readMipMapMetadata()).wasSuccessful())
            return status;
    }

    // Read mip map data
    for(int i = 0; i < mMipMapCount; i++)
    {
        if(!(status = readMipMapData(i)).wasSuccessful())
            return status;
    }

    // TODO: Implement check/warning for if after reading known data, there is still data leftover
    // i.e. mStreamReader.atEnd(), may need to advance one byte first

    // Close file
    mStreamReader.closeFile();

    // Return status
    return mStreamReader.status();
}
