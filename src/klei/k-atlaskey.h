#ifndef KATLASKEY_H
#define KATLASKEY_H

#include "k-atlas.h"
#include "qx.h"
#include "qx-xml.h"

class KAtlasKey
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    QString mAtlasFilename;
    QMap<QString, QRectF> mRelativeElements;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlasKey();

//-Instance Functions----------------------------------------------------------------------------------------------
public:
    QString atlasFilename() const;
    void setAtlasFilename(QString atlasFilename);
    int elementCount() const;

    QMap<QString, QRectF>& elements();
    const QMap<QString, QRectF>& elements() const;

    void insertElement(QString elementName, QRectF element);
};

class KAtlasKeyGenerator
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const KAtlas& mAtlas;
    const QString& mAtlasName;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlasKeyGenerator(const KAtlas& atlas, const QString& atlasName);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QMap<QString, QRectF> translateElements() const;

public:
    KAtlasKey process() const;
};

class KAtlasKeyParser
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const KAtlasKey& mAtlasKey;
    const QImage& mAtlasImage;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlasKeyParser(const KAtlasKey& atlasKey, const QImage& atlasImage);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QMap<QString, QRect> translateElements() const;

public:
    KAtlas process();
};

#endif // KATLASKEY_H
