#include "c-pack.h"

#include <QDir>
#include <QFileInfo>
#include <QImageReader>

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
CPack::CPack(Stex& coreRef) : Command(coreRef) {}

//-Instance Functions-------------------------------------------------------------
//Protected:
const QList<const QCommandLineOption*> CPack::options() { return CL_OPTIONS_SPECIFIC + Command::options(); }
const QString CPack::name() { return NAME; }

//Public:
ErrorCode CPack::process(const QStringList& commandLine)
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
    QDir inputDir(mParser.value(CL_OPTION_INPUT));
    QDir outputDir(mParser.value(CL_OPTION_OUTPUT));

    // Make sure the provided input and output are valid
    if(!inputDir.exists())
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

    // Make sure output pixel format is valid
    if(!PIXEL_FORMAT_MAP.contains(mParser.value(CL_OPTION_FORMAT)))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_INVALID_FORMAT));
        return ErrorCodes::INVALID_FORMAT;
    }

    // Get output pixel format
    KTex::Header::PixelFormat outputPixelFormat = PIXEL_FORMAT_MAP[mParser.value(CL_OPTION_FORMAT)];

    // Get source images
    QFileInfoList imageFiles = inputDir.entryInfoList(IMAGE_FORMATS);

    // Make sure there is at least one image
    if(imageFiles.isEmpty())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_NO_IMAGES));
        return ErrorCodes::NO_IMAGES;
    }

    // Generate named image map
    QImageReader imageReader;
    QMap<QString, QImage> namedImages;

    for(const QFileInfo& imageInfo : qAsConst(imageFiles))
    {
        QString elementName = imageInfo.baseName();
        imageReader.setFileName(imageInfo.absoluteFilePath());

        QImage image;
        if(!imageReader.read(&image))
        {
            mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_READ_IMAGE.arg(imageInfo.absoluteFilePath())));
            return ErrorCodes::CANT_READ_IMAGE;
        }

        namedImages[elementName] = image;
    }

    // Create atlas
    KAtlaser atlaser(namedImages);
    KAtlas atlas = atlaser.process();

    // Create atlas key
    KAtlasKeyGenerator akg(atlas, inputDir.dirName());
    KAtlasKey atlasKey = akg.process();

    // Create TEX
    ToTexConverter::Options ttco;
    ttco.generateMipMaps = !mParser.isSet(CL_OPTION_UNOPT);
    ttco.premultiplyAlpha = !mParser.isSet(CL_OPTION_STRAIGHT);
    ttco.pixelFormat = outputPixelFormat;

    ToTexConverter ttc(atlas.image, ttco);
    KTex tex = ttc.convert();

    // Write TEX file
    QFile outputTexFile(outputDir.absoluteFilePath(atlasKey.atlasFilename()));
    KTexWriter texWriter(tex, outputTexFile);
    Qx::IOOpReport texWriteReport;
    if(!(texWriteReport = texWriter.write()).wasSuccessful())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_WRITE_ATLAS.arg(outputTexFile.fileName()),
                                                texWriteReport.getOutcomeInfo()));
        return ErrorCodes::CANT_WRITE_ATLAS;
    }

    // Write atlas key
    QFile outputKeyFile(outputDir.absoluteFilePath(inputDir.dirName() + ".xml"));
    KAtlasKeyWriter keyWriter(outputKeyFile, atlasKey);
    Qx::XmlStreamWriterError keyWriteReport;
    if(!(keyWriteReport = keyWriter.write()).isValid())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_WRITE_KEY.arg(outputKeyFile.fileName()),
                                                keyWriteReport.getText()));
        return ErrorCodes::CANT_WRITE_KEY;
    }

    // Return success
    return Stex::ErrorCodes::NO_ERR;
}
