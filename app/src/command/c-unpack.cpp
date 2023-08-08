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
ErrorCode CUnpack::perform()
{
    ErrorCode errorStatus;

    // Get input and output
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);
    QFileInfo inputKey(mParser.value(CL_OPTION_INPUT));
    QDir outputDir(mParser.value(CL_OPTION_OUTPUT));

    // Make sure the provided input and output are valid
    if(!inputKey.exists() || !inputKey.isFile() || inputKey.suffix() != "xml")
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_INVALID_INPUT));
        return Stex::ErrorCodes::INVALID_INPUT;
    }
    if(!outputDir.exists())
    {
        if(!outputDir.mkpath(outputDir.absolutePath()))
        {
            mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_INVALID_OUTPUT));
            return Stex::ErrorCodes::INVALID_OUTPUT;
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
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_READ_KEY.arg(atlasKeyFile.fileName()),
                                                keyReadReport.text()));
        return ErrorCodes::CANT_READ_KEY;
    }

    // Ensure TEX atlas Exists
    QFileInfo texFileInfo(inputKey.absoluteDir().filePath(atlasKey.atlasFilename()));
    if(!texFileInfo.exists() || !texFileInfo.isFile())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_ATLAS_DNE.arg(texFileInfo.fileName())));
        return ErrorCodes::ATLAS_DOESNT_EXIST;
    }

    // Create final output directory
    QDir finalOutputDir(outputDir.absoluteFilePath(texFileInfo.baseName()));
    if(!finalOutputDir.exists() && !outputDir.mkpath(finalOutputDir.absolutePath()))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_CREATE_DIR.arg(finalOutputDir.absolutePath())));
        return ErrorCodes::CANT_CREATE_DIR;
    }

    // Read TEX atlas
    mCore.printMessage(NAME, MSG_READ_TEX);
    KTex tex;
    KTexReader texReader(texFileInfo.absoluteFilePath(), tex);
    bool supported;
    Qx::IoOpReport texReadReport = texReader.read(supported);

    if(texReadReport.isFailure())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_READ_ATLAS.arg(texFileInfo.absoluteFilePath()),
                                                texReadReport.outcomeInfo()));
        return ErrorCodes::CANT_READ_ATALAS;
    }
    else if(!supported)
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_ATLAS_NOT_SUPPORTED));
        return ErrorCodes::ATLAS_UNSUPPORTED;
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
            mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_WRITE_IMAGE.arg(filename), writer.errorString()));
            return ErrorCodes::CANT_WRITE_IMAGE;
        }
    }

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS.arg(namedImages.count()));
    return Stex::ErrorCodes::NO_ERR;
}
