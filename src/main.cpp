#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>

#include "qx-io.h"
#include "ImageMagick/Magick++.h"

#include "klei/k-atlas.h"
#include "klei/k-atlaskey.h"

// Required for use of static IM lib
#define STATIC_MAGICK

int main(int argc, char *argv[])
{
    // Initialize Qt application
    QCoreApplication app(argc, argv);

    // Initialize ImageMagick
    Magick::InitializeMagick(argv[0]);

    QString hardCode = "C:/Users/Player/Desktop/test";
    QDir inputFolder(hardCode);
    QString atlasName = inputFolder.dirName();

    QStringList fileList;
    Qx::IOOpReport dirQuery = Qx::getDirFileList(fileList, inputFolder, {"png"});

    // Generate named image map
    QImageReader imageReader;
    QMap<QString, QImage> namedImages;

    for(const QString& filePath : qAsConst(fileList))
    {
        QString elementName = QFileInfo(filePath).baseName();
        imageReader.setFileName(filePath);
        namedImages[elementName] = imageReader.read();
    }

    // Generate atlas
    KAtlaser atlaser(namedImages);
    KAtlas atlas = atlaser.process();

    // Generate atals key
    KAtlasKeyGenerator keyGenerator(atlas, atlasName);
    QString key = keyGenerator.process();

    // TEST
//    QImage testImage(QSize(128,128), QImage::Format_ARGB32);
//    testImage.fill(Qt::transparent);

//    Atlas testAtlas;
//    testAtlas.image = testImage;
//    testAtlas.elements.insert("ear-0.tex", QRect(0,0, 84, 128));
//    KAtlasKeyGenerator keyGen(testAtlas, "test");
//    QString keyTest = keyGen.process();

//    QFile testFile("experiment.xml");
//    Qx::writeStringAsFile(testFile, keyTest, true);
    // TEST

    // Save image (scope, because QImageWriter holds lock on output during its lifetime)
    {
        // Check for error, maybe make this a function
        QImageWriter writer(atlasName + ".png");
        qDebug() << writer.write(atlas.image);
    }

    // Save key
    QFile keyFile(atlasName + ".xml");
    Qx::IOOpReport writeReport = Qx::writeStringAsFile(keyFile, key, true);
    qDebug() << writeReport.getOutcome();
}
