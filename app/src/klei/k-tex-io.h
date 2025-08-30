#ifndef K_TEX_IO_H
#define K_TEX_IO_H

// Qt Includes
#include <QFile>

// Qx Includes
#include <qx/io/qx-filestreamwriter.h>
#include <qx/io/qx-filestreamreader.h>
#include <qx/core/qx-bitarray.h>

// Project Includes
#include "k-tex.h"

class KTexWriter
{
//-Class Members----------------------------------------------------------------------------------------------------
private:

//-Instance Members-------------------------------------------------------------------------------------------------
private:
    Qx::FileStreamWriter mStreamWriter;
    const KTex& mSourceTex;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KTexWriter(const KTex& sourceTex, const QString& targetfilePath);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    Qx::IoOpReport writeHeader();
    Qx::IoOpReport writeMipMapMetadata(const KTex::MipMapImage& mipMap);
    Qx::IoOpReport writeMipMapData(const KTex::MipMapImage& mipMap);

public:
    Qx::IoOpReport write();

};

class KTexReader
{
//-Class Members----------------------------------------------------------------------------------------------------
private:

//-Instance Members-------------------------------------------------------------------------------------------------
private:
    Qx::FileStreamReader mStreamReader;
    KTex& mTargetTex;

    // Mipmap helper members
    quint8 mMipMapCount;

    // Status
    //bool* mSupported;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KTexReader(const QString& sourceFilePath, KTex& targetTex);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    Qx::IoOpReport checkFileSupport(QByteArray magicNumberRaw);
    Qx::IoOpReport checkFileSupport(quint8 platformRaw, quint8 pixelFormatRaw, quint8 textureTypeRaw);

    Qx::IoOpReport parsePreCavesSpecs(Qx::BitArray specifcationBits);
    Qx::IoOpReport parsePostCavesSpecs(Qx::BitArray specifcationBits);
    Qx::IoOpReport readHeader();
    Qx::IoOpReport readMipMapMetadata();
    Qx::IoOpReport readMipMapData(int i);

public:
    Qx::IoOpReport read();
};

#endif // K_TEX_IO_H
