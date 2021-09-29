#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

#include "qx-io.h"

#include "version.h"
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


//    QString hardCode = "C:/Users/Player/Desktop/test";
//    QDir inputFolder(hardCode);
//    QString atlasName = inputFolder.dirName();

//    QStringList fileList;
//    Qx::IOOpReport dirQuery = Qx::getDirFileList(fileList, inputFolder, {"png"});

//    // Generate named image map
//    QImageReader imageReader;
//    QMap<QString, QImage> namedImages;

//    for(const QString& filePath : qAsConst(fileList))
//    {
//        QString elementName = QFileInfo(filePath).baseName();
//        imageReader.setFileName(filePath);
//        namedImages[elementName] = imageReader.read();
//    }

//    // Generate atlas
//    KAtlaser atlaser(namedImages);
//    KAtlas atlas = atlaser.process();

//    // Generate atals key
//    KAtlasKeyGenerator keyGenerator(atlas, atlasName);
//    QString key = keyGenerator.process();

//    // Save image (scope, because QImageWriter holds lock on output during its lifetime)
//    {
//        // Check for error, maybe make this a function
//        QImageWriter writer(atlasName + ".png");
//        qDebug() << writer.write(atlas.image);
//    }

//    // Save key
//    QFile keyFile(atlasName + ".xml");
//    Qx::IOOpReport writeReport = Qx::writeStringAsFile(keyFile, key, true);
//    qDebug() << writeReport.getOutcome();
}
