#include "k-atlaskey.h"

#include <QXmlStreamWriter>
#include <cmath>

#include "k-tex.h"

//===============================================================================================================
// UNIT ONLY
//===============================================================================================================
namespace
{
    namespace Xml
    {
        const QString ELEMENT_ATLAS = "Atlas";
        const QString ELEMENT_ELEMENTS = "Elements";
        const QString ELEMENT_ELEMENT = "Element";
        const QString ELEMENT_TEXTURE = "Texture";
        const QString ATTRIBUTE_FILENAME = "filename";
        const QString ATTRIBUTE_ELEMENT_NAME = "name";
        const QString ATTRIBUTE_TOP_LEFT_X = "u1";
        const QString ATTRIBUTE_TOP_LEFT_Y = "v1";
        const QString ATTRIBUTE_BOTTOM_RIGHT_X = "u2";
        const QString ATTRIBUTE_BOTTOM_RIGHT_Y = "v2";
    }
}

//===============================================================================================================
// K_ATLAS_KEY
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlasKey::KAtlasKey() :
    mAtlasFilename(),
    mStraightAlpha(false), // Assume premultiplied by default
    mRelativeElements()
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Public:
QString KAtlasKey::atlasFilename() const { return mAtlasFilename; }
void KAtlasKey::setAtlasFilename(QString atlasFilename) { mAtlasFilename = atlasFilename; }
bool KAtlasKey::straightAlpha() const { return mStraightAlpha; }
void KAtlasKey::setStraightAlpha(bool straightAlpha) { mStraightAlpha = straightAlpha; }
int KAtlasKey::elementCount() const { return mRelativeElements.count(); }

QMap<QString, QRectF>& KAtlasKey::elements() { return mRelativeElements; }
const QMap<QString, QRectF>& KAtlasKey::elements() const { return mRelativeElements; }

void KAtlasKey::insertElement(QString elementName, QRectF element) { mRelativeElements[elementName] = element; }


//===============================================================================================================
// K_ATLAS_KEY_GENERATOR
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlasKeyGenerator::KAtlasKeyGenerator(const KAtlas& atlas, const QString& atlasName, bool straightAlpha) :
    mAtlas(atlas),
    mAtlasName(atlasName),
    mStraightAlpha(straightAlpha)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
QMap<QString, QRectF> KAtlasKeyGenerator::translateElements() const
{
    QMap<QString, QRectF> translatedElements;

    QMap<QString, QRect>::const_iterator i;
    for(i = mAtlas.elements.constBegin(); i != mAtlas.elements.constEnd(); i++)
    {
        QPointF topLeft{
            static_cast<qreal>(i->topLeft().x())/static_cast<qreal>(mAtlas.image.width() - 1),
            static_cast<qreal>(i->topLeft().y())/static_cast<qreal>(mAtlas.image.height() - 1)
        };
        QPointF bottomRight{
            static_cast<qreal>(i->bottomRight().x())/static_cast<qreal>(mAtlas.image.width() - 1),
            static_cast<qreal>(i->bottomRight().y())/static_cast<qreal>(mAtlas.image.height() - 1)
        };
        translatedElements[i.key()] = {topLeft, bottomRight};
    }

    return translatedElements;
}


//Public:
KAtlasKey KAtlasKeyGenerator::process() const
{
    // Create atlas key
    KAtlasKey atlasKey;
    atlasKey.setAtlasFilename(mAtlasName + "." + KTex::FILE_EXT);
    atlasKey.setStraightAlpha(mStraightAlpha);
    atlasKey.elements() = translateElements();

    return atlasKey;
}

//===============================================================================================================
// K_ATLAS_KEY_PARSER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlasKeyParser::KAtlasKeyParser(const KAtlasKey& atlasKey, const QImage& atlasImage) :
    mAtlasKey(atlasKey),
    mAtlasImage(atlasImage)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
QMap<QString, QRect> KAtlasKeyParser::translateElements() const
{
    QMap<QString, QRect> translatedElements;

    QMap<QString, QRectF>::const_iterator i;
    for(i = mAtlasKey.elements().constBegin(); i != mAtlasKey.elements().constEnd(); i++)
    {
        QPoint topLeft{
            static_cast<int>(std::round(i->topLeft().x() * mAtlasImage.width())),
            static_cast<int>(std::round(i->topLeft().y() * mAtlasImage.height()))
        };
        QPoint bottomRight{
            static_cast<int>(std::round(i->bottomRight().x() * mAtlasImage.width())),
            static_cast<int>(std::round(i->bottomRight().y() * mAtlasImage.height()))
        };
        translatedElements[i.key()] = {topLeft, bottomRight};
    }

    return translatedElements;
}

//Public:
KAtlas KAtlasKeyParser::process()
{
    KAtlas atlas;
    atlas.image = mAtlasImage;
    atlas.elements = translateElements();

    return atlas;
}



