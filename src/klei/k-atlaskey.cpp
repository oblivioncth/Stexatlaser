#include "k-atlaskey.h"

#include <QXmlStreamWriter>
#include <QFileInfo>
#include <cmath>

#include "k-tex.h"

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
    // Determine largest coordinate
    QPoint imageMaxCoord = {mAtlas.image.width() - 1, mAtlas.image.height() - 1};

    // Translate
    QMap<QString, QRectF> translatedElements;

    QMap<QString, QRect>::const_iterator i;
    for(i = mAtlas.elements.constBegin(); i != mAtlas.elements.constEnd(); i++)
    {
        // Convert coordinates to relative
        QPointF topLeft{
            static_cast<qreal>(i->topLeft().x())/static_cast<qreal>(imageMaxCoord.x()),
            static_cast<qreal>(i->topLeft().y())/static_cast<qreal>(imageMaxCoord.y())
        };
        QPointF bottomRight{
            static_cast<qreal>(i->bottomRight().x())/static_cast<qreal>(imageMaxCoord.x()),
            static_cast<qreal>(i->bottomRight().y())/static_cast<qreal>(imageMaxCoord.y())
        };

        // Convert name if needed
        QString elementName = ensureElementExtension(i.key());

        // Add translated element
        translatedElements[elementName] = {topLeft, bottomRight};
    }

    return translatedElements;
}

QString KAtlasKeyGenerator::ensureElementExtension(const QString& elementName) const
{
    QFileInfo nameInfo(elementName);
    return nameInfo.suffix() == KTex::FILE_EXT ? elementName : elementName + "." + KTex::FILE_EXT;
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
    // Determine largest coordinate
    QPoint imageMaxCoord = {mAtlasImage.width() - 1, mAtlasImage.height() - 1};

    // Translate
    QMap<QString, QRect> translatedElements;

    QMap<QString, QRectF>::const_iterator i;
    for(i = mAtlasKey.elements().constBegin(); i != mAtlasKey.elements().constEnd(); i++)
    {
        // Convert to coordinates to absolute
        QPoint topLeft{
            static_cast<int>(std::round(i->topLeft().x() * imageMaxCoord.x())),
            static_cast<int>(std::round(i->topLeft().y() * imageMaxCoord.y()))
        };
        QPoint bottomRight{
            static_cast<int>(std::round(i->bottomRight().x() * imageMaxCoord.x())),
            static_cast<int>(std::round(i->bottomRight().y() * imageMaxCoord.y()))
        };

        // Convert name if needed
        QString elementName = peelElementExtension(i.key());

        // Add translated element
        translatedElements[elementName] = {topLeft, bottomRight};
    }

    return translatedElements;
}

QString KAtlasKeyParser::peelElementExtension(const QString& elementName) const
{
    QFileInfo nameInfo(elementName);
    return nameInfo.suffix() == KTex::FILE_EXT ? elementName.chopped(KTex::FILE_EXT.length() + 1) : elementName;
}

//Public:
KAtlas KAtlasKeyParser::process()
{
    KAtlas atlas;
    atlas.image = mAtlasImage;
    atlas.elements = translateElements();

    return atlas;
}



