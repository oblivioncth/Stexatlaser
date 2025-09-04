// Unit Includes
#include "c-pack.h"

// Qt Includes
#include <QDir>

// Project Includes
#include "klei/k-atlas.h"
#include "klei/k-atlaskey.h"
#include "klei/k-xml.h"

//===============================================================================================================
// CPackError
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
CPackError::CPackError(Type t, const QString& s, const QString& d) :
    mType(t),
    mSpecific(s),
    mDetails(d)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool CPackError::isValid() const { return mType != NoError; }
QString CPackError::specific() const { return mSpecific; }
CPackError::Type CPackError::type() const { return mType; }

//Private:
Qx::Severity CPackError::deriveSeverity() const { return Qx::Critical; }
quint32 CPackError::deriveValue() const { return mType; }
QString CPackError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString CPackError::deriveSecondary() const { return mSpecific; }
QString CPackError::deriveDetails() const { return mDetails; }

//===============================================================================================================
// CPack
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
CPack::CPack(Stex& coreRef) : TexCommand(coreRef)
{}

//-Class Functions----------------------------------------------------------------
//Private:
bool CPack::hasBasenameCollision(const QFileInfoList& imageFiles)
{
    QSet<QString> baseNames;

    for(const QFileInfo& fileInfo : std::as_const(imageFiles))
    {
        QString baseName = fileInfo.baseName();

        if(!baseNames.contains(baseName))
            baseNames.insert(baseName);
        else
            return true;
    }

    // No collisions occurred
    return false;
}

//-Instance Functions-------------------------------------------------------------
//Protected:
QList<const QCommandLineOption*> CPack::options() const { return CL_OPTIONS_SPECIFIC + TexCommand::options(); }
QSet<const QCommandLineOption*> CPack::requiredOptions() const { return CL_OPTIONS_REQUIRED; }
QString CPack::name() const { return NAME; }

//Public:
Qx::Error CPack::perform()
{
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);

    // Get and validate format
    KTex::Header::PixelFormat outputPixelFormat;
    if(auto err = getFormat(outputPixelFormat); err.isValid())
        return err;

    // Get input and output
    QDir inputDir(mParser.value(CL_OPTION_INPUT));
    QDir outputDir(mParser.value(CL_OPTION_OUTPUT));

    // Make sure the provided input and output are valid
    if(!inputDir.exists())
    {
        CPackError err(CPackError::InvalidInput);
        mCore.printError(NAME, err);
        return err;
    }
    if(!outputDir.exists())
    {
        if(!outputDir.mkpath(outputDir.absolutePath()))
        {
            CPackError err(CPackError::InvalidOutput);
            mCore.printError(NAME, err);
            return err;
        }
    }

    // Get source images
    mCore.printMessage(NAME, MSG_READ_IMAGES);
    QFileInfoList imageFiles = inputDir.entryInfoList(mCore.imageFormatFilter());

    // Make sure there is at least one image
    if(imageFiles.isEmpty())
    {
        CPackError err(CPackError::NoImages);
        mCore.printError(NAME, err);
        return err;
    }

    // Make sure there are no basename collisions
    if(hasBasenameCollision(imageFiles))
    {
        CPackError err(CPackError::DupeBasename);
        mCore.printError(NAME, err);
        return err;
    }

    // Generate named image map
    QMap<QString, QImage> namedImages;

    for(const QFileInfo& imageInfo : std::as_const(imageFiles))
    {
        QString elementName = imageInfo.baseName();
        QImage image;
        if(auto err = readImage(image, imageInfo.absoluteFilePath()); err.isValid())
            return err;

        namedImages[elementName] = image;
    }

    // Create atlas
    mCore.printMessage(NAME, MSG_CREATE_ATLAS);
    KAtlaser atlaser(namedImages, mParser.isSet(CL_OPTION_MARGIN));
    KAtlas atlas = atlaser.process();

    // Create atlas key
    mCore.printMessage(NAME, MSG_CREATE_KEY);
    KAtlasKeyGenerator akg(atlas, inputDir.dirName(), mParser.isSet(CL_OPTION_STRAIGHT));
    KAtlasKey atlasKey = akg.process();

    // Create TEX
    KTex tex = createTex(atlas.image, outputPixelFormat);

    // Write TEX file
    QString outputTexFilePath(outputDir.absoluteFilePath(atlasKey.atlasFilename()));
    if(auto res = writeTex(tex, outputTexFilePath); res.isFailure())
    {
        CPackError err(CPackError::CantWriteAtlas, outputTexFilePath, res.outcomeInfo());
        mCore.printError(NAME, err);
        return err;
    }

    // Write atlas key
    mCore.printMessage(NAME, MSG_WRITE_KEY);
    QFile outputKeyFile(outputDir.absoluteFilePath(inputDir.dirName() + '.' + KAtlasKey::standardExtension()));
    KAtlasKeyWriter keyWriter(outputKeyFile, atlasKey);
    Qx::XmlStreamWriterError keyWriteReport;
    if((keyWriteReport = keyWriter.write()).isValid())
    {
        CPackError err(CPackError::CantWriteKey, outputKeyFile.fileName(), keyWriteReport.text());
        mCore.printError(NAME, err);
        return err;
    }

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS.arg(namedImages.count()));
    return Qx::Error();
}
