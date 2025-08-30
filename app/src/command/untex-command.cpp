// Unit Includes
#include "untex-command.h"

// Qt Includes
#include <QImageWriter>

// Project Includes
#include "klei/k-tex-io.h"
#include "conversion.h"

//===============================================================================================================
// UntexCommandError
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
UntexCommandError::UntexCommandError(Type t, const QString& s, const QString& d) :
    mType(t),
    mSpecific(s),
    mDetails(d)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool UntexCommandError::isValid() const { return mType != NoError; }
QString UntexCommandError::specific() const { return mSpecific; }
UntexCommandError::Type UntexCommandError::type() const { return mType; }

//Private:
Qx::Severity UntexCommandError::deriveSeverity() const { return Qx::Critical; }
quint32 UntexCommandError::deriveValue() const { return mType; }
QString UntexCommandError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString UntexCommandError::deriveSecondary() const { return mSpecific; }
QString UntexCommandError::deriveDetails() const { return mDetails; }

//===============================================================================================================
// UntexCommand
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
UntexCommand::UntexCommand(Stex& coreRef) : Command(coreRef)
{}

//-Instance Functions-------------------------------------------------------------
//Protected:
QList<const QCommandLineOption*> UntexCommand::options() const { return CL_OPTIONS_SPECIFIC + Command::options(); }

Qx::IoOpReport UntexCommand::readTex(KTex& tex, const QString& path) const
{
    mCore.printMessage(NAME, MSG_READ_TEX);
    KTexReader texReader(path, tex);
    Qx::IoOpReport res = texReader.read();

    if(!res.isFailure())
        mCore.printMessage(NAME, MSG_TEX_INFO.arg(tex.info(true)));

    return res;
}

QImage UntexCommand::extractImage(const KTex& tex, bool forceStraight) const
{
    mCore.printMessage(NAME, MSG_EXTRACT_IMAGE);
    FromTexConverter::Options ftco;
    ftco.demultiplyAlpha = !mParser.isSet(CL_OPTION_STRAIGHT) && !forceStraight;
    FromTexConverter ftc(tex, ftco);
    return ftc.convert();
}

UntexCommandError UntexCommand::writeImage(const QImage& image, const QString& path) const
{
    static QImageWriter writer;

    writer.setFileName(path);
    if(!writer.write(image))
    {
        UntexCommandError err(UntexCommandError::CantWriteImage, path);
        mCore.printError(NAME, err);
        return err;
    }

    return UntexCommandError();
}
