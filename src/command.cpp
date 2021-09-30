#include "command.h"


//===============================================================================================================
// Command
//===============================================================================================================

//-Constructor----------------------------------------------------------------------------------------------------------
//Public:
Command::Command(Stex& coreRef) : mCore(coreRef) {}

//-Class Functions------------------------------------------------------------------
//Private:
QMap<QString, Command::Entry>& Command::registry() { static QMap<QString, Entry> registry; return registry; }

//Public:
void Command::registerCommand(const QString& name, CommandFactory* factory, const QString& desc) { registry()[name] = {factory, desc}; }
bool Command::isRegistered(const QString &name) { return registry().contains(name); }
QList<QString> Command::registered() { return registry().keys(); }
QString Command::describe(const QString& name) { return registry().value(name).description; }
std::unique_ptr<Command> Command::acquire(const QString& name, Stex& coreRef) { return registry().value(name).factory->produce(coreRef); }

//-Instance Functions------------------------------------------------------------------------------------------------------
//Protected:
const QList<const QCommandLineOption*> Command::options() { return CL_OPTIONS_STANDARD; }

ErrorCode Command::parse(const QStringList& commandLine)
{
    // Add command options
    for(const QCommandLineOption* clOption : options())
        mParser.addOption(*clOption);

    // Parse
    if(mParser.parse(commandLine))
        return Stex::ErrorCodes::NO_ERR;
    else
    {
        mCore.printError(NAME, Qx::GenericError(Qx::GenericError::Error, Stex::ERR_INVALID_PARAM, mParser.errorText()));
        return Stex::ErrorCodes::INVALID_ARGS;
    }
}

bool Command::checkStandardOptions()
{
    // Show help if requested
    if(mParser.isSet(CL_OPTION_HELP))
    {
        showHelp();
        return true;
    }

    // Default
    return false;
}

void Command::showHelp()
{
    // Help string
    static QString helpStr;

    // One time setup
    if(helpStr.isNull())
    {        
        // Help options
        QString optStr;
        for(const QCommandLineOption* clOption : options())
        {
            // Handle names
            QStringList dashedNames;
            for(const QString& name :  qAsConstR(clOption->names()))
                dashedNames << ((name.length() > 1 ? "--" : "-") + name);

            // Add option
            QString marker = requiredOptions().contains(clOption) ? "*" : "";
            optStr += HELP_OPT_TEMPL.arg(marker, dashedNames.join(" | "), clOption->description());
        }

        // Complete string
        helpStr = HELP_TEMPL.arg(name(), optStr);
    }

    // Show help
    mCore.printVerbatim(helpStr);
}
