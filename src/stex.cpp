#include "stex.h"

#include <iostream>

#include "command.h"

//===============================================================================================================
// Stex
//===============================================================================================================

//-Constructor-------------------------------------------------------------
Stex::Stex() {}

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
            for(const QString& name : qAsConstR(clOption->names()))
                dashedNames << ((name.length() > 1 ? "--" : "-") + name);

            // Add option
            optStr += HELP_OPT_TEMPL.arg(dashedNames.join(" | "), clOption->description());
        }

        // Help commands
        QString commandStr;
        for(const QString& command : qAsConstR(Command::registered()))
            commandStr += HELP_COMMAND_TEMPL.arg(command, Command::describe(command));

        // Complete string
        helpStr = HELP_TEMPL.arg(optStr, commandStr);
    }

    // Show help
    printVerbatim(helpStr);
}


void Stex::showVersion() { printVerbatim(CL_VERSION_MESSAGE); }

//Public:
ErrorCode Stex::initialize(QStringList& commandLine)
{
    // Setup CLI Parser
    QCommandLineParser clParser;
    clParser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    for(const QCommandLineOption* clOption : CL_OPTIONS_ALL)
        clParser.addOption(*clOption);

    // Parse
    bool validArgs = clParser.parse(commandLine);

    // Remove app name from command line string
    commandLine.removeFirst();

    // Check for valid arguments
    if(validArgs)
    {
        if(clParser.isSet(CL_OPTION_VERSION))
        {
            showVersion();
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
        return ErrorCodes::NO_ERR;
    }
    else
    {
        commandLine.clear(); // Clear remaining options since they are now irrelavent
        printError(NAME, Qx::GenericError(Qx::GenericError::Error, ERR_INVALID_PARAM, clParser.errorText()));
        return ErrorCodes::INVALID_ARGS;
    }

}

void Stex::printError(QString src, Qx::GenericError error)
{
    QString message = "(" + error.errorLevelString() + ") " + error.primaryInfo();

    if(!error.secondaryInfo().isNull())
        message += " | " + error.secondaryInfo();

    if(!error.detailedInfo().isNull())
        message += "\n" + error.detailedInfo();

    printMessage(src, message);
}

void Stex::printMessage(QString src, QString message)
{
    QString text =  "[" + src + "] " + message + "\n";
    printVerbatim(text);
}

void Stex::printVerbatim(QString text)
{
    qcout << text;
    qcout.flush();
}