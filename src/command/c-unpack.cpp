#include "c-unpack.h"

#include <QDir>
#include <QFileInfo>
#include <QImageWriter>

#include "../conversion.h"
#include "../klei/k-atlas.h"
#include "../klei/k-atlaskey.h"
#include "../klei/k-tex-io.h"
#include "../klei/k-xml.h"

//===============================================================================================================
// CPack
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
CUnpack::CUnpack(Stex& coreRef) : Command(coreRef) {}

//-Instance Functions-------------------------------------------------------------
//Protected:
const QList<const QCommandLineOption*> CUnpack::options() { return CL_OPTIONS_SPECIFIC + Command::options(); }
const QSet<const QCommandLineOption*> CUnpack::requiredOptions() { return CL_OPTIONS_REQUIRED; }
const QString CUnpack::name() { return NAME; }

//Public:
ErrorCode CUnpack::process(const QStringList& commandLine)
{
    ErrorCode errorStatus;

    // Parse and check for valid arguments
    if((errorStatus = parse(commandLine)))
        return errorStatus;

    // Handle standard options
    if(checkStandardOptions())
        return Stex::ErrorCodes::NO_ERR;

    // Make sure input and output were provided
    if(!mParser.isSet(CL_OPTION_INPUT))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_NO_INPUT));
        return ErrorCodes::NO_INPUT;
    }
    if(!mParser.isSet(CL_OPTION_OUTPUT))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_NO_OUTPUT));
        return ErrorCodes::NO_OUTPUT;
    }

    // Get input and output
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);
    QFileInfo inputKey(mParser.value(CL_OPTION_INPUT));
    QDir outputDir(mParser.value(CL_OPTION_OUTPUT));

    // Make sure the provided input and output are valid
    if(!inputKey.exists() || !inputKey.isFile() || inputKey.suffix() != "xml")
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_INVALID_INPUT));
        return ErrorCodes::INVALID_INPUT;
    }
    if(!outputDir.exists())
    {
        if(!outputDir.mkpath(outputDir.absolutePath()))
        {
            mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_INVALID_OUTPUT));
            return ErrorCodes::INVALID_OUTPUT;
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
                                                keyReadReport.getText()));
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
    if(outputDir.mkdir(texFileInfo.baseName()))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_CREATE_DIR.arg(finalOutputDir.absolutePath())));
        return ErrorCodes::CANT_CREATE_DIR;
    }

    // Read TEX atlas
    mCore.printMessage(NAME, MSG_READ_TEX);
    QFile texFile(texFileInfo.absoluteFilePath());
    KTex tex;
    KTexReader texReader(texFile, tex);
    bool supported;
    Qx::IOOpReport texReadReport = texReader.read(supported);

    if(!texReadReport.wasSuccessful())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_READ_ATLAS.arg(texFile.fileName()),
                                                texReadReport.getOutcomeInfo()));
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
        if(writer.write(i.value()))
        {
            mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_WRITE_IMAGE.arg(filename)));
            return ErrorCodes::CANT_WRITE_IMAGE;
        }
    }

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS.arg(namedImages.count()));
    return Stex::ErrorCodes::NO_ERR;
}
