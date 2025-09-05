// Unit Includes
#include "c-dump.h"

// Project Includes
#include "klei/k-tex.h"
#include "klei/k-tex-io.h"

//===============================================================================================================
// CDump
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
CDumpError::CDumpError(Type t, const QString& s, const QString& d) :
    mType(t),
    mSpecific(s),
    mDetails(d)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool CDumpError::isValid() const { return mType != NoError; }
QString CDumpError::specific() const { return mSpecific; }
CDumpError::Type CDumpError::type() const { return mType; }

//Private:
Qx::Severity CDumpError::deriveSeverity() const { return Qx::Critical; }
quint32 CDumpError::deriveValue() const { return mType; }
QString CDumpError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString CDumpError::deriveSecondary() const { return mSpecific; }
QString CDumpError::deriveDetails() const { return mDetails; }

//===============================================================================================================
// CDump
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Public:
CDump::CDump(Stex& coreRef) : Command(coreRef) {}

//-Instance Functions-------------------------------------------------------------
//Private:
QList<const QCommandLineOption*> CDump::options() const { return CL_OPTIONS_SPECIFIC + Command::options(); }
QSet<const QCommandLineOption*> CDump::requiredOptions() const { return CL_OPTIONS_REQUIRED; }
QString CDump::name() const { return NAME; }

Qx::IoOpReport CDump::readTex(KTex& tex, const QString& path) const
{
    mCore.printMessage(NAME, MSG_READ_TEX);
    KTexReader texReader(path, tex, true);
    Qx::IoOpReport res = texReader.read();

    if(!res.isFailure())
        mCore.printMessage(NAME, MSG_TEX_INFO.arg(tex.info(true)));

    return res;
}

//Public:
Qx::Error CDump::perform()
{
    mCore.printMessage(NAME, MSG_INPUT_VALIDATION);

    // Get input
    QFileInfo input(mParser.value(CL_OPTION_INPUT));
    if(!input.exists())
    {
        CDumpError err(CDumpError::InvalidInput);
        mCore.printError(NAME, err);
        return err;
    }

    // Get output
    QDir outputDir(
        mParser.isSet(CL_OPTION_OUTPUT) ?
            mParser.value(CL_OPTION_OUTPUT) :
            input.absolutePath() + '/' + input.baseName()
    );

    // Read TEX
    KTex tex;
    QString texPath = input.absoluteFilePath();
    if(auto res = readTex(tex, texPath); res.isFailure())
    {
        CDumpError err(CDumpError::CantReadTex, texPath, res.outcomeInfo());
        mCore.printError(NAME, err);
        return err;
    }

    // Check for empty TEX
    if(!tex.hasMipMaps())
    {
        CDumpError err(CDumpError::TexEmpty, texPath);
        mCore.printError(NAME, err);
        return err;
    }

    // Make root folder
    if(!outputDir.mkpath("."))
    {
        CDumpError err(CDumpError::CantMakeDir, outputDir.absolutePath());
        mCore.printError(NAME, err);
        return err;
    }

    // Dump
    for(auto i = 0; i < tex.mipMapCount(); i++)
    {
        // Get MM
        const auto& mm = tex.mipMaps().at(i);

        // Save meta
        QFile dump(outputDir.absoluteFilePath(META_OUTPUT_TEMPLATE.arg(i)));
        if(auto res = Qx::writeBytesToFile(dump, mm.jsonMetadata()); res.isFailure())
            return res;


        // Save data
        dump.setFileName(outputDir.absoluteFilePath(DATA_OUTPUT_TEMPLATE.arg(i)));
        if(auto res = Qx::writeBytesToFile(dump, mm.imageData()); res.isFailure())
            return res;
    }

    // Return success
    mCore.printMessage(NAME, MSG_SUCCESS);
    return Qx::Error();
}
