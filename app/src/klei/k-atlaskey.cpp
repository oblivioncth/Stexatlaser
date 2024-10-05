// Unit Includes
#include "k-atlaskey.h"

// Qt Includes
#include <QXmlStreamWriter>
#include <QFileInfo>

// Project Includes
#include "k-tex.h"

/*
 * Pixel coordinate to UV coordinate conversion has some degree of variation depending on
 * the context in which it is performed. Since in this case the conversion is from a 2D
 * space to another 2D space and essentially is only for the purpose of going from absolute
 * coordinates to relative coordinates, the conversions are performed as directly as possible
 * with no interpolation whatsoever.
 *
 * Remember that in the UV space 1 is the right/bottom most edge of the texture so the UV coordinates
 * of an element within an atlas need to span from the upper (or lower if in regards to systmes
 * with a lower left origin) left corner of its pixels to its lower right. Normally, this would mean
 * the left-hand coordinate can be taken as is, and the right-hand coordinate of an element needs to be
 * +1'ed before converting to UV space so that the resultant UV coordinate is at the extent of that pixel;
 * however, standard convention instead views pixel coordinates as being located at the center of each
 * pixel in a pixel-grid, with the goal of direct UV mapping being to have the UV bounding-box lie such
 * that its edges intersect each of these center points for the pixels that should be included.
 *
 * This is covered in the following article, though note that the bit about the upper left actually being
 * (-0.5,-0.5) does not apply in this context and the origin is still 0,0:
 * https://learn.microsoft.com/en-us/windows/win32/direct3d9/directly-mapping-texels-to-pixels
 *
 * To acheive this, pixel coordinates are shifted by +0.5 before coverting to the UV coordinate space
 * and by -0.5 when coming from the UV coordinate space.
 *
 * NOTE: In the event that the game has issues with the UV coordinates being too close to edges,
 * it may be necessary to inset the bounding-box of each atlas element very slightly (by fractions
 * of a pixel).
 */

namespace
{

QRect flipElement(const QRect& element, const QImage& image)
{
    return {QPoint(element.x(), (image.height() - 1) - element.bottom()), element.size()};
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

//-Class Function---s--------------------------------------------------------------------------------------------
//Public:
QString KAtlasKey::standardExtension() { return u"xml"_s; }

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


//-Class Functions------------------------------------------------------------------------------------------------
//Private:
QString KAtlasKeyGenerator::ensureElementExtension(const QString& elementName)
{
    QFileInfo nameInfo(elementName);
    QString ext = KTex::standardExtension();
    return nameInfo.suffix() == ext ? elementName : elementName + u"."_s + ext;
}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
QMap<QString, QRectF> KAtlasKeyGenerator::translateElements() const
{
    // Translate
    QMap<QString, QRectF> translatedElements;

    qreal xMax = mAtlas.image.width();
    qreal yMax = mAtlas.image.height();

    for(auto i = mAtlas.elements.constBegin(); i != mAtlas.elements.constEnd(); i++)
    {
        // Flip
        QRect fi = flipElement(*i, mAtlas.image);

        // Map to UV coordinate space
        // + 0.5 to correspond to center of edge pixels
        QPointF topLeftUV((fi.topLeft().x() + 0.5)/xMax, (fi.topLeft().y() + 0.5)/yMax);
        QPointF bottomRightUV((fi.bottomRight().x() + 0.5)/xMax, (fi.bottomRight().y() + 0.5)/yMax);

        // Convert name if needed
        QString elementName = ensureElementExtension(i.key());

        // Add translated element
        translatedElements[elementName] = QRectF(topLeftUV, bottomRightUV);
    }

    return translatedElements;
}

//Public:
KAtlasKey KAtlasKeyGenerator::process() const
{
    // Create atlas key
    KAtlasKey atlasKey;
    atlasKey.setAtlasFilename(mAtlasName + u"."_s + KTex::standardExtension());
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
    // Translate
    QMap<QString, QRect> translatedElements;

    qreal xMax = mAtlasImage.width();
    qreal yMax = mAtlasImage.height();

    for(auto i = mAtlasKey.elements().constBegin(); i != mAtlasKey.elements().constEnd(); i++)
    {
        // Map to pixel coordinate space
        QPoint topLeft(std::round(i->topLeft().x() * xMax - 0.5), std::round(i->topLeft().y() * yMax - 0.5));
        QPoint bottomRight(std::round(i->bottomRight().x() * xMax - 0.5), std::round(i->bottomRight().y() * yMax - 0.5));

        // Flip
        QRect flipped = flipElement({topLeft, bottomRight}, mAtlasImage);


        // Convert name if needed
        QString elementName = peelElementExtension(i.key());

        // Add translated element
        translatedElements[elementName] = flipped;
    }

    return translatedElements;
}

QString KAtlasKeyParser::peelElementExtension(const QString& elementName) const
{
    QFileInfo nameInfo(elementName);
    QString ext = KTex::standardExtension();
    return nameInfo.suffix() == ext ? elementName.chopped(ext.length() + 1) : elementName;
}

//Public:
KAtlas KAtlasKeyParser::process()
{
    KAtlas atlas;
    atlas.image = mAtlasImage;
    atlas.elements = translateElements();

    return atlas;
}



