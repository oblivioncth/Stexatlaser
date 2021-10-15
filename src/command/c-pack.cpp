#include "c-pack.h"

#include <QDir>
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
CPack::CPack(Stex& coreRef) : Command(coreRef)
{ }

//-Class Functions----------------------------------------------------------------
//Private:
bool CPack::hasBasenameCollision(const QFileInfoList& imageFiles)
{
    QSet<QString> baseNames;

    for(const QFileInfo& fileInfo : qAsConst(imageFiles))
    {
        QString baseName = fileInfo.baseName();

        if(!baseNames.contains(baseName))
            baseNames.insert(baseName);
        else
            return true;
    }

    // No collisons occured
    return false;
}

//-Instance Functions-------------------------------------------------------------
//Protected:
const QList<const QCommandLineOption*> CPack::options() { return CL_OPTIONS_SPECIFIC + Command::options(); }
const QSet<const QCommandLineOption*> CPack::requiredOptions() { return CL_OPTIONS_REQUIRED; }
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
        return Stex::ErrorCodes::NO_INPUT;
    }
    if(!mParser.isSet(CL_OPTION_OUTPUT))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_NO_OUTPUT));
        return Stex::ErrorCodes::NO_OUTPUT;
    }

    // Make sure output pixel format is valid if provided
    if(mParser.isSet(CL_OPTION_FORMAT) && !PIXEL_FORMAT_MAP.contains(mParser.value(CL_OPTION_FORMAT)))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_INVALID_FORMAT));
        return ErrorCodes::INVALID_FORMAT;
    }

    // Get input and output
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);
    QDir inputDir(mParser.value(CL_OPTION_INPUT));
    QDir outputDir(mParser.value(CL_OPTION_OUTPUT));

    // Make sure the provided input and output are valid
    if(!inputDir.exists())
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

    // Get output pixel format
    KTex::Header::PixelFormat outputPixelFormat = mParser.isSet(CL_OPTION_FORMAT) ? PIXEL_FORMAT_MAP[mParser.value(CL_OPTION_FORMAT)] :
                                                                                    KTex::Header::PixelFormat::DXT5;

    // Get source images
    mCore.printMessage(NAME, MSG_READ_IMAGES);
    QFileInfoList imageFiles = inputDir.entryInfoList(mCore.imageFormatFilter());

    // Make sure there is at least one image
    if(imageFiles.isEmpty())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, ERR_NO_IMAGES));
        return ErrorCodes::NO_IMAGES;
    }

    // Make sure there are no basename collisions
    if(hasBasenameCollision(imageFiles))
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_DUPE_BASENAME));
        return ErrorCodes::DUPLICATE_NAME;
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
    mCore.printMessage(NAME, MSG_CREATE_ATLAS);
    KAtlaser atlaser(namedImages);
    KAtlas atlas = atlaser.process();

    // Create atlas key
    mCore.printMessage(NAME, MSG_CREATE_KEY);
    KAtlasKeyGenerator akg(atlas, inputDir.dirName(), mParser.isSet(CL_OPTION_STRAIGHT));
    KAtlasKey atlasKey = akg.process();

    // Create TEX
    mCore.printMessage(NAME, MSG_CREATE_TEX);
    ToTexConverter::Options ttco;
    ttco.generateMipMaps = !mParser.isSet(CL_OPTION_UNOPT);
    ttco.premultiplyAlpha = !mParser.isSet(CL_OPTION_STRAIGHT);
    ttco.pixelFormat = outputPixelFormat;

    ToTexConverter ttc(atlas.image, ttco);
    KTex tex = ttc.convert();

    // Write TEX file
    mCore.printMessage(NAME, MSG_WRITE_TEX);
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
    mCore.printMessage(NAME, MSG_WRITE_KEY);
    QFile outputKeyFile(outputDir.absoluteFilePath(inputDir.dirName() + ".xml"));
    KAtlasKeyWriter keyWriter(outputKeyFile, atlasKey);
    Qx::XmlStreamWriterError keyWriteReport;
    if((keyWriteReport = keyWriter.write()).isValid())
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_CANT_WRITE_KEY.arg(outputKeyFile.fileName()),
                                                keyWriteReport.getText()));
        return ErrorCodes::CANT_WRITE_KEY;
    }

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS.arg(namedImages.count()));
    return Stex::ErrorCodes::NO_ERR;
}
