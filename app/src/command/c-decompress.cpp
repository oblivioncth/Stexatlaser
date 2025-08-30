// Unit Includes
#include "c-decompress.h"

// Qt Includes
#include <QDir>
#include <QFileInfo>
#include <QImageWriter>

// Project Includes
#include "klei/k-tex.h"

//===============================================================================================================
// CDecompressError
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
CDecompressError::CDecompressError(Type t, const QString& s, const QString& d) :
    mType(t),
    mSpecific(s),
    mDetails(d)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool CDecompressError::isValid() const { return mType != NoError; }
QString CDecompressError::specific() const { return mSpecific; }
CDecompressError::Type CDecompressError::type() const { return mType; }

//Private:
Qx::Severity CDecompressError::deriveSeverity() const { return Qx::Critical; }
quint32 CDecompressError::deriveValue() const { return mType; }
QString CDecompressError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString CDecompressError::deriveSecondary() const { return mSpecific; }
QString CDecompressError::deriveDetails() const { return mDetails; }

//===============================================================================================================
// CDecompress
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
CDecompress::CDecompress(Stex& coreRef) : UntexCommand(coreRef) {}

//-Instance Functions-------------------------------------------------------------
//Protected:
QList<const QCommandLineOption*> CDecompress::options() const { return CL_OPTIONS_SPECIFIC + UntexCommand::options(); }
QSet<const QCommandLineOption*> CDecompress::requiredOptions() const { return CL_OPTIONS_REQUIRED; }
QString CDecompress::name() const { return NAME; }

//Public:
Qx::Error CDecompress::perform()
{
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);

    // Get input
    QFileInfo input(mParser.value(CL_OPTION_INPUT));
    if(!input.exists())
    {
        CDecompressError err(CDecompressError::InvalidInput);
        mCore.printError(NAME, err);
        return err;
    }

    // Get output
    QString outputEnd = '.' + STD_OUTPUT_EXT;
    QString outputPath(
        mParser.isSet(CL_OPTION_OUTPUT) ?
        mParser.value(CL_OPTION_OUTPUT) :
        input.absolutePath() + '/' + input.baseName()
    );
    if(!outputPath.endsWith(outputEnd))
        outputPath += outputEnd;
    QFileInfo output(outputPath);

    // Read TEX
    KTex tex;
    QString texPath = input.absoluteFilePath();
    if(auto res = readTex(tex, texPath); res.isFailure())
    {
        CDecompressError err(CDecompressError::CantReadTex, texPath, res.outcomeInfo());
        mCore.printError(NAME, err);
        return err;
    }

    // Extract main image from TEX
    QImage image = extractImage(tex);

    // Write
    if(auto err = writeImage(image, output.absoluteFilePath()); err.isValid())
        return err;

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS);
    return Qx::Error();
}
