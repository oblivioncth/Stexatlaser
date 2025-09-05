// Unit Includes
#include "stex.h"

// Qt Includes
#include <QImageReader>

// Qx Includes
#include <qx/utility/qx-helpers.h>
#include <qx/core/qx-iostream.h>

// Project Includes
#include "command/command.h"

//===============================================================================================================
// StexError
//===============================================================================================================

//-Constructor-------------------------------------------------------------
//Private:
StexError::StexError(Type t, const QString& s, Qx::Severity sv) :
    mType(t),
    mSpecific(s),
    mSeverity(sv)
{}

//-Instance Functions-------------------------------------------------------------
//Public:
bool StexError::isValid() const { return mType != NoError; }
QString StexError::specific() const { return mSpecific; }
StexError::Type StexError::type() const { return mType; }

//Private:
Qx::Severity StexError::deriveSeverity() const { return mSeverity; }
quint32 StexError::deriveValue() const { return mType; }
QString StexError::derivePrimary() const { return ERR_STRINGS.value(mType); }
QString StexError::deriveSecondary() const { return mSpecific; }

//===============================================================================================================
// Stex
//===============================================================================================================

//-Constructor-------------------------------------------------------------
Stex::Stex()
{
    for(const QByteArray& format : qxAsConst(QImageReader::supportedImageFormats()))
    {
        QString formatExt = QString::fromUtf8(format);
        mImageFormats.append(formatExt);
        mImageFormatFilter.append(u"*."_s + formatExt);
    }
}

//-Instance Functions-------------------------------------------------------------
//Private:
void Stex::showHelp()
{
    // Help string
    static QString helpStr;

    // One time setup
    if(helpStr.isNull())
    {
        // Help options
        QString optStr;
        for(const QCommandLineOption* clOption : CL_OPTIONS_ALL)
        {
            // Handle names
            QStringList dashedNames;
            for(const QString& name : qxAsConst(clOption->names()))
                dashedNames << ((name.length() > 1 ? u"--"_s : u"-"_s) + name);

            // Add option
            optStr += HELP_OPT_TEMPL.arg(dashedNames.join(u" | "_s), clOption->description());
        }

        // Help commands
        QString commandStr;
        for(const QString& command : qxAsConst(Command::registered()))
            commandStr += HELP_COMMAND_TEMPL.arg(command, Command::describe(command));

        // Complete string
        helpStr = HELP_TEMPL.arg(optStr, commandStr);
    }

    // Show help
    printVerbatim(helpStr);
}


void Stex::showVersion() { printVerbatim(MSG_VERSION); }
void Stex::showFormats() { printVerbatim(MSG_FORMATS.arg(mImageFormats.join('\n'))); }

//Public:
Qx::Error Stex::initialize(QStringList& commandLine)
{
    // Setup CLI Parser
    QCommandLineParser clParser;
    clParser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    for(const QCommandLineOption* clOption : CL_OPTIONS_ALL)
        clParser.addOption(*clOption);

    // Parse
    bool validArgs = clParser.parse(commandLine);

    // Check for valid arguments
    if(validArgs)
    {
        if(clParser.isSet(CL_OPTION_VERSION))
        {
            showVersion();
            commandLine.clear(); // Clear args so application terminates after core setup
        }
        else if(clParser.isSet(CL_OPTION_FORMATS))
        {
            showFormats();
            commandLine.clear(); // Clear args so application terminates after core setup
        }
        else if(clParser.isSet(CL_OPTION_HELP) || clParser.positionalArguments().count() == 0) // Also when no parameters
        {
            showHelp();
            commandLine.clear(); // Clear args so application terminates after core setup
        }
        else
            commandLine = clParser.positionalArguments(); // Remove core options from command line list

        // Return success
        return Qx::Error();
    }
    else
    {
        commandLine.clear(); // Clear remaining options since they are now irrelevant

        StexError err(StexError::InvalidOptions, clParser.errorText());
        printError(NAME, err);
        return err;
    }

}

QStringList Stex::imageFormatFilter() const { return mImageFormatFilter; }
QStringList Stex::supportedImageFormats() const { return mImageFormats; }

void Stex::printError(const QString& src, const Qx::Error& error)
{
    QString message = u"("_s + error.severityString() + u") "_s + error.primary();

    if(!error.secondary().isNull())
        message += u" | "_s + error.secondary();

    if(!error.details().isNull())
        message += u"\n"_s + error.details();

    printMessage(src, message);
}

void Stex::printMessage(const QString& src, const QString& message)
{
    QString text =  u"["_s + src + u"] "_s + message + u"\n"_s;
    printVerbatim(text);
}

void Stex::printVerbatim(const QString& text)
{
    Qx::cout << text;
    Qx::cout.flush();
}
