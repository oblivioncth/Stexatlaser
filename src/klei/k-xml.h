#ifndef KXML_H
#define KXML_H

#include <QFile>

#include "k-atlaskey.h"

class KAtlasKeyWriter
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    QFile& mTargetFile;
    const KAtlasKey& mSourceAtlasKey;
    QXmlStreamWriter mStreamWriter;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlasKeyWriter(QFile& targetFile, KAtlasKey& sourceAtlasKey);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    bool writeElement(const QString& elementName, const QRectF& element);

public:
    Qx::XmlStreamWriterError write();
};

class KAtlasKeyReader
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    static inline const QString ERR_NOT_ATLAS_KEY = "The provided XML file is not an atlas key!";
    static inline const QString ERR_NO_FILENAME = "The provided atlas key has no atlas filename!";
    static inline const QString ERR_NO_ELMENTS = "The provided atlas key has no elements!";
    static inline const QString ERR_INVALID_ATTRIBS = "The provided atlas key has a key with an invalid attributes!";
    static inline const QString ERR_INVALID_STR_ALPHA = "The value for StraightAlpha was not of boolean type!";

//-Instance Members-------------------------------------------------------------------------------------------------
private:
    KAtlasKey& mTargetAtlasKey;
    QFile& mSourceFile;
    QXmlStreamReader mStreamReader;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlasKeyReader(KAtlasKey& targetAtlasKey, QFile& sourceFile);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    bool readAtlasKey();
    bool readElements();

    void parseTexture();
    void parseElement();
    void parseStraightAlpha();

    bool hasAttributes(const QXmlStreamAttributes& attributes, const QStringList& checkList);

public:
    Qx::XmlStreamReaderError read();
};

#endif // KXML_H
