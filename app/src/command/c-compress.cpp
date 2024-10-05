// Unit Includes
#include "c-compress.h"

// Qt Includes
#include <QImage>

//===============================================================================================================
// CCompressError
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
CCompressError::CCompressError(Type t, const QString& s, const QString& d) :
    mType(t),
    mSpecific(s),
    mDetails(d)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool CCompressError::isValid() const { return mType != NoError; }
QString CCompressError::specific() const { return mSpecific; }
CCompressError::Type CCompressError::type() const { return mType; }

//Private:
Qx::Severity CCompressError::deriveSeverity() const { return Qx::Critical; }
quint32 CCompressError::deriveValue() const { return mType; }
QString CCompressError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString CCompressError::deriveSecondary() const { return mSpecific; }
QString CCompressError::deriveDetails() const { return mDetails; }

//===============================================================================================================
// CCompress
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
CCompress::CCompress(Stex& coreRef) : TexCommand(coreRef)
{}

//-Instance Functions-------------------------------------------------------------
//Protected:
QList<const QCommandLineOption*> CCompress::options() const { return CL_OPTIONS_SPECIFIC + TexCommand::options(); }
QSet<const QCommandLineOption*> CCompress::requiredOptions() const { return CL_OPTIONS_REQUIRED; }
QString CCompress::name() const { return NAME; }

//Public:
Qx::Error CCompress::perform()
{
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);

    // Get and validate format
    KTex::Header::PixelFormat outputPixelFormat;
    if(auto err = getFormat(outputPixelFormat); err.isValid())
        return err;

    // Get input
    QFileInfo input(mParser.value(CL_OPTION_INPUT));
    if(!input.exists())
    {
        CCompressError err(CCompressError::InvalidInput);
        mCore.printError(NAME, err);
        return err;
    }

    // Get output
    QString outputEnd = '.' + KTex::standardExtension();
    QString outputPath(
        mParser.isSet(CL_OPTION_OUTPUT) ?
        mParser.value(CL_OPTION_OUTPUT) :
        input.absolutePath() + '/' + input.baseName()
    );
    if(!outputPath.endsWith(outputEnd))
        outputPath += outputEnd;
    QFileInfo output(outputPath);

    // Read image
    QImage image;
    if(auto err = readImage(image, input.absoluteFilePath()); err.isValid())
        return err;

    // Create TEX
    KTex tex = createTex(image, outputPixelFormat);

    // Write TEX file
    QString absOutputPath = output.absoluteFilePath();
    if(auto res = writeTex(tex, outputPath); res.isFailure())
    {
        CCompressError err(CCompressError::CantWriteTex, absOutputPath, res.outcomeInfo());
        mCore.printError(NAME, err);
        return err;
    }

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS);
    return Qx::Error();
}
