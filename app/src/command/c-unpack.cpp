// Unit Includes
#include "c-unpack.h"

// Qt Includes
#include <QDir>
#include <QFileInfo>
#include <QImageWriter>

// Project Includes
#include "conversion.h"
#include "klei/k-atlas.h"
#include "klei/k-atlaskey.h"
#include "klei/k-tex-io.h"
#include "klei/k-xml.h"

//===============================================================================================================
// CUnpackError
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
CUnpackError::CUnpackError(Type t, const QString& s, const QString& d) :
    mType(t),
    mSpecific(s),
    mDetails(d)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool CUnpackError::isValid() const { return mType != NoError; }
QString CUnpackError::specific() const { return mSpecific; }
CUnpackError::Type CUnpackError::type() const { return mType; }

//Private:
Qx::Severity CUnpackError::deriveSeverity() const { return Qx::Critical; }
quint32 CUnpackError::deriveValue() const { return mType; }
QString CUnpackError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString CUnpackError::deriveSecondary() const { return mSpecific; }
QString CUnpackError::deriveDetails() const { return mDetails; }

//===============================================================================================================
// CPack
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
CUnpack::CUnpack(Stex& coreRef) : Command(coreRef) {}

//-Instance Functions-------------------------------------------------------------
//Protected:
QList<const QCommandLineOption*> CUnpack::options() { return CL_OPTIONS_SPECIFIC + Command::options(); }
QSet<const QCommandLineOption*> CUnpack::requiredOptions() { return CL_OPTIONS_REQUIRED; }
QString CUnpack::name() { return NAME; }

//Public:
Qx::Error CUnpack::perform()
{
    // Get input and output
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);
    QFileInfo inputKey(mParser.value(CL_OPTION_INPUT));
    QDir outputDir(mParser.value(CL_OPTION_OUTPUT));

    // Make sure the provided input and output are valid
    if(!inputKey.exists() || !inputKey.isFile() || inputKey.suffix() != "xml")
    {
        CUnpackError err(CUnpackError::InvalidInput);
        mCore.printError(NAME, err);
        return err;
    }
    if(!outputDir.exists())
    {
        if(!outputDir.mkpath(outputDir.absolutePath()))
        {
            CUnpackError err(CUnpackError::InvalidOutput);
            mCore.printError(NAME, err);
            return err;
        }
    }

    // Read atlas key
    mCore.printMessage(NAME, MSG_READ_KEY);
    QFile atlasKeyFile(inputKey.absoluteFilePath());
    KAtlasKey atlasKey;
    KAtlasKeyReader keyReader(atlasKey, atlasKeyFile);
    Qx::XmlStreamReaderError keyReadReport = keyReader.read();

    if(keyReadReport.isValid())
    {
        CUnpackError err(CUnpackError::CantReadKey, atlasKeyFile.fileName(), keyReadReport.text());
        mCore.printError(NAME, err);
        return err;
    }

    // Ensure TEX atlas Exists
    QFileInfo texFileInfo(inputKey.absoluteDir().filePath(atlasKey.atlasFilename()));
    if(!texFileInfo.exists() || !texFileInfo.isFile())
    {
        CUnpackError err(CUnpackError::AtlasDoesntExist, texFileInfo.fileName());
        mCore.printError(NAME, err);
        return err;
    }

    // Create final output directory
    QDir finalOutputDir(outputDir.absoluteFilePath(texFileInfo.baseName()));
    if(!finalOutputDir.exists() && !outputDir.mkpath(finalOutputDir.absolutePath()))
    {
        CUnpackError err(CUnpackError::CantCreateDir, finalOutputDir.absolutePath());
        mCore.printError(NAME, err);
        return err;
    }

    // Read TEX atlas
    mCore.printMessage(NAME, MSG_READ_TEX);
    KTex tex;
    KTexReader texReader(texFileInfo.absoluteFilePath(), tex);
    bool supported;
    Qx::IoOpReport texReadReport = texReader.read(supported);

    if(texReadReport.isFailure())
    {
        CUnpackError err(CUnpackError::CantReadAtlas, texFileInfo.absoluteFilePath(), texReadReport.outcomeInfo());
        mCore.printError(NAME, err);
        return err;
    }
    else if(!supported)
    {
        CUnpackError err(CUnpackError::AtlasUnsupported, texFileInfo.fileName());
        mCore.printError(NAME, err);
        return err;
    }

    // Extract atlas image from TEX
    mCore.printMessage(NAME, MSG_EXTRACT_IMAGE);
    FromTexConverter::Options ftco;
    ftco.demultiplyAlpha = !mParser.isSet(CL_OPTION_STRAIGHT) && !atlasKey.straightAlpha();
    FromTexConverter ftc(tex, ftco);
    QImage atlasImage = ftc.convert();

    // Create atlas
    mCore.printMessage(NAME, MSG_FORM_ATLAS);
    KAtlasKeyParser akp(atlasKey, atlasImage);
    KAtlas atlas = akp.process();

    // Deatlas
    mCore.printMessage(NAME, MSG_DEATLAS);
    KDeatlaser deatlaser(atlas);
    QMap<QString, QImage> namedImages = deatlaser.process();

    // Write images
    mCore.printMessage(NAME, MSG_WRITE_IMAGES);
    QMap<QString, QImage>::const_iterator i;
    for(i = namedImages.constBegin(); i != namedImages.constEnd(); i++)
    {
        QString filename = finalOutputDir.absoluteFilePath(i.key() + ".png");
        QImageWriter writer(filename);
        if(!writer.write(i.value()))
        {
            CUnpackError err(CUnpackError::CantWriteImage, filename, writer.errorString());
            mCore.printError(NAME, err);
            return err;
        }
    }

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS.arg(namedImages.count()));
    return Qx::Error();
}
