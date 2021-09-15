#ifndef KATLASKEY_H
#define KATLASKEY_H

#include "atlas.h"

class KAtlasKeyGenerator
{
//-Class Members-------------------------------------------------------------------------------------------------
private:
    static inline const QString ELEMENT_ATLAS = "Atlas";
    static inline const QString ELEMENT_ELEMENTS = "Elements";
    static inline const QString ELEMENT_ELEMENT = "Element";
    static inline const QString ELEMENT_TEXTURE = "Texture";
    static inline const QString ATTRIBUTE_FILENAME = "filename";
    static inline const QString ATTRIBUTE_ELEMENT_NAME = "name";
    static inline const QString ATTRIBUTE_TOP_LEFT_X = "u1";
    static inline const QString ATTRIBUTE_TOP_LEFT_Y = "v1";
    static inline const QString ATTRIBUTE_BOTTOM_RIGHT_X = "u2";
    static inline const QString ATTRIBUTE_BOTTOM_RIGHT_Y = "v2";

//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const Atlas& mAtlas;
    QString mAtlasName;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlasKeyGenerator(const Atlas& atlas, QString atlasName);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QMap<QString, QRectF> translateElements();
    QString writeXml(const QMap<QString, QRectF>& kElements);

public:
    QString process();
};

#endif // KATLASKEY_H
