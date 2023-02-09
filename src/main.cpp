// Qt Includes
#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

// Project Includes
#include "stex.h"
#include "command.h"

// Error Messages
const QString ERR_INVALID_COMMAND = R"("%1" is not a valid command)";

// Meta
const QString NAME = "main";

int main(int argc, char *argv[])
{
    // Initialize Qt application
    QCoreApplication app(argc, argv);

    // Set application name
    QCoreApplication::setApplicationName(VER_PRODUCTNAME_STR);
    QCoreApplication::setApplicationVersion(VER_FILEVERSION_STR);

    // Error status tracker
    ErrorCode errorStatus = Stex::ErrorCodes::NO_ERR;

    // Create Core instance
    Stex coreCLI;

    //-Setup Core--------------------------------------------------------------------------
    QStringList clArgs = app.arguments();
    if((errorStatus = coreCLI.initialize(clArgs)) || clArgs.empty()) // Terminate if error or no command
        return errorStatus;

    //-Handle Command and Command Options----------------------------------------------------------
    QString commandStr = clArgs.first().toLower();

    // Check for valid command
    if(!Command::isRegistered(commandStr))
    {
        coreCLI.printError(NAME, Qx::GenericError(Qx::GenericError::Critical, ERR_INVALID_COMMAND.arg(commandStr)));
        return Stex::ErrorCodes::INVALID_ARGS;
    }

    // Create command instance
    std::unique_ptr<Command> commandProcessor = Command::acquire(commandStr, coreCLI);

    // Process command
    return commandProcessor->process(clArgs);
}
