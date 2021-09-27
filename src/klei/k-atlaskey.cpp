#include "k-atlaskey.h"
#include <QXmlStreamWriter>

//===============================================================================================================
// K_ATLAS_KEY_GENERATOR
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlasKeyGenerator::KAtlasKeyGenerator(const KAtlas& atlas, QString atlasName) : mAtlas(atlas), mAtlasName(atlasName) {}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
QMap<QString, QRectF> KAtlasKeyGenerator::translateElements() const
{
    QMap<QString, QRectF> translatedElements;

    QMap<QString, QRect>::const_iterator i;
    for(i = mAtlas.elements.constBegin(); i != mAtlas.elements.constEnd(); i++)
    {
        QPointF topLeft{
            static_cast<qreal>(i->topLeft().x())/static_cast<qreal>(mAtlas.image.width()),
            static_cast<qreal>(i->topLeft().y())/static_cast<qreal>(mAtlas.image.height())
        };
        QPointF bottomRight{
            static_cast<qreal>(i->bottomRight().x())/static_cast<qreal>(mAtlas.image.width()),
            static_cast<qreal>(i->bottomRight().y())/static_cast<qreal>(mAtlas.image.height())
        };
        translatedElements[i.key()] = {topLeft, bottomRight};
    }

    return translatedElements;
}

QString KAtlasKeyGenerator::produceXml(const QMap<QString, QRectF>& kElements) const
{
    // Setup Writer
    QString xmlOut;
    QXmlStreamWriter streamWriter(&xmlOut);
    streamWriter.setAutoFormatting(true);

    // Write header
    streamWriter.writeStartDocument();

    // Start atlas element
    streamWriter.writeStartElement(ELEMENT_ATLAS);

    // Write texture element
    streamWriter.writeEmptyElement(ELEMENT_TEXTURE);
    streamWriter.writeAttribute(ATTRIBUTE_FILENAME, mAtlasName + ".tex"); // TODO: Get this extension from eventual TEX class

    // Write elements
    streamWriter.writeStartElement(ELEMENT_ELEMENTS);

    QMap<QString, QRectF>::const_iterator i;
    for(i = kElements.constBegin(); i != kElements.constEnd(); i++)
    {
        streamWriter.writeEmptyElement(ELEMENT_ELEMENT);
        streamWriter.writeAttribute(ATTRIBUTE_ELEMENT_NAME, i.key());
        streamWriter.writeAttribute(ATTRIBUTE_TOP_LEFT_X, QString::number(i->left()));
        streamWriter.writeAttribute(ATTRIBUTE_BOTTOM_RIGHT_X, QString::number(i->right()));
        streamWriter.writeAttribute(ATTRIBUTE_TOP_LEFT_Y, QString::number(i->top()));
        streamWriter.writeAttribute(ATTRIBUTE_BOTTOM_RIGHT_Y, QString::number(i->bottom()));
    }

    streamWriter.writeEndElement();

    // End atlas element
    streamWriter.writeEndDocument();

    // Complete document
    streamWriter.writeEndDocument();

    // Return as string
    return xmlOut;
}

//Public:
QString KAtlasKeyGenerator::process() const
{
    // Translate to proportional coordinates
    QMap<QString, QRectF> translatedElements = translateElements();

    // Create XML key
    return produceXml(translatedElements);
}
