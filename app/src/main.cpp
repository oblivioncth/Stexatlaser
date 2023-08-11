// Qt Includes
#include <QCoreApplication>

// Project Includes
#include "stex.h"
#include "command/command.h"
#include "project_vars.h"

// Meta
const QString NAME = u"main"_s;

int main(int argc, char *argv[])
{
    // Initialize Qt application
    QCoreApplication app(argc, argv);

    // Set application name
    QCoreApplication::setApplicationName(PROJECT_APP_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION_STR);

    // Create Core instance
    Stex coreCLI;

    //-Setup Core--------------------------------------------------------------------------
    QStringList clArgs = app.arguments();
    Qx::Error initError = coreCLI.initialize(clArgs);
    if(initError.isValid() || clArgs.empty()) // Terminate if error or no command
        return initError.value();

    //-Handle Command and Command Options----------------------------------------------------------
    QString commandStr = clArgs.first().toLower();

    // Check for valid command
    if(CommandError ce = Command::isRegistered(commandStr); ce.isValid())
    {
        coreCLI.printError(NAME, ce);
        return Qx::Error(ce).value();
    }

    // Create command instance
    std::unique_ptr<Command> commandProcessor = Command::acquire(commandStr, coreCLI);

    // Process command
    return commandProcessor->process(clArgs).value();
}
