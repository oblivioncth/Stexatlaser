// Unit Includes
#include "tex-command.h"

// Qt Includes
#include <QImageReader>

// Project Includes
#include "conversion.h"
#include "klei/k-tex-io.h"

//===============================================================================================================
// TexCommandError
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
TexCommandError::TexCommandError(Type t, const QString& s, const QString& d) :
    mType(t),
    mSpecific(s),
    mDetails(d)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool TexCommandError::isValid() const { return mType != NoError; }
QString TexCommandError::specific() const { return mSpecific; }
TexCommandError::Type TexCommandError::type() const { return mType; }

//Private:
Qx::Severity TexCommandError::deriveSeverity() const { return Qx::Critical; }
quint32 TexCommandError::deriveValue() const { return mType; }
QString TexCommandError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString TexCommandError::deriveSecondary() const { return mSpecific; }
QString TexCommandError::deriveDetails() const { return mDetails; }

//===============================================================================================================
// TexCommand
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
TexCommand::TexCommand(Stex& coreRef) : Command(coreRef)
{}

//-Instance Functions-------------------------------------------------------------
//Protected:
QList<const QCommandLineOption*> TexCommand::options() const { return CL_OPTIONS_SPECIFIC + Command::options(); }

TexCommandError TexCommand::getFormat(KTex::Header::PixelFormat& format) const
{
    format = KTex::Header::PixelFormat::DXT5;

    if(!mParser.isSet(CL_OPTION_FORMAT))
        return TexCommandError();

    QString fmtStr = mParser.value(CL_OPTION_FORMAT);
    if(!PIXEL_FORMAT_MAP.contains(mParser.value(CL_OPTION_FORMAT)))
    {
        TexCommandError err(TexCommandError::InvalidFormat);
        mCore.printError(NAME, err);
        return err;
    }

    format = PIXEL_FORMAT_MAP[fmtStr];
    return TexCommandError();
}

KTex TexCommand::createTex(const QImage& image, KTex::Header::PixelFormat format) const
{
    // This could get the format itself, but we want that input validated before any computation takes place
    mCore.printMessage(NAME, MSG_CREATE_TEX);
    ToTexConverter::Options ttco;
    ttco.generateMipMaps = !mParser.isSet(CL_OPTION_UNOPT);
    ttco.premultiplyAlpha = !mParser.isSet(CL_OPTION_STRAIGHT);
    ttco.pixelFormat = format;

    ToTexConverter ttc(image, ttco);
    KTex tex = ttc.convert();

    // Show metadata
    mCore.printMessage(NAME, MSG_TEX_INFO.arg(tex.info(true)));

    return tex;
}

Qx::IoOpReport TexCommand::writeTex(const KTex& tex, const QString& path) const
{
    mCore.printMessage(NAME, MSG_WRITE_TEX);
    KTexWriter texWriter(tex, path);
    return texWriter.write();
}

TexCommandError TexCommand::readImage(QImage& image, const QString& path) const
{
    static QImageReader reader;

    reader.setFileName(path);
    if(!reader.read(&image))
    {
        TexCommandError err(TexCommandError::CantReadImage, path);
        mCore.printError(NAME, err);
        return err;
    }

    return TexCommandError();
}
