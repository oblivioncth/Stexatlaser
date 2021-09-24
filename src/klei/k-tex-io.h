#ifndef K_TEX_IO_H
#define K_TEX_IO_H

#include <QFile>

#include "k-tex.h"
#include "qx-io.h"

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
    KTexWriter(const KTex& sourceTex, QFile& targetFile);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    Qx::IOOpReport writeHeader();
    Qx::IOOpReport writeMipMapMetadata(const KTex::MipMapImage& mipMap);
    Qx::IOOpReport writeMipMapData(const KTex::MipMapImage& mipMap);

public:
    Qx::IOOpReport write();

};

class KTexReader
{
//-Class Members----------------------------------------------------------------------------------------------------
private:

//-Instance Members-------------------------------------------------------------------------------------------------
private:
    QFile& mSourceFile;
    Qx::FileStreamReader mStreamReader;
    KTex& mTargetTex;

    // Mipmap helper members
    quint8 mMipMapCount;

    // Status
    bool* mSupported;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KTexReader(QFile& sourceFile, KTex& targetTex);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    Qx::IOOpReport checkFileSupport(QByteArray magicNumberRaw);
    Qx::IOOpReport checkFileSupport(quint8 platformRaw, quint8 pixelFormatRaw, quint8 textureTypeRaw);

    Qx::IOOpReport parsePreCavesSpecs(Qx::BitArray specifcationBits);
    Qx::IOOpReport parsePostCavesSpecs(Qx::BitArray specifcationBits);
    Qx::IOOpReport readHeader();
    Qx::IOOpReport readMipMapMetadata();
    Qx::IOOpReport readMipMapData(int i);

public:
    Qx::IOOpReport read(bool& supported);
};

#endif // K_TEX_IO_H
