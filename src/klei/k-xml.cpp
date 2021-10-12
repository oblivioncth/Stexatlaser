#include "k-xml.h"
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
        const QString ELEMENT_STR_ALPHA = "StraightAlpha";
        const QString ATTRIBUTE_FILENAME = "filename";
        const QString ATTRIBUTE_ELEMENT_NAME = "name";
        const QString ATTRIBUTE_TOP_LEFT_X = "u1";
        const QString ATTRIBUTE_TOP_LEFT_Y = "v1";
        const QString ATTRIBUTE_BOTTOM_RIGHT_X = "u2";
        const QString ATTRIBUTE_BOTTOM_RIGHT_Y = "v2";
    }
}

//===============================================================================================================
// K_ATLAS_KEY_WRITER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlasKeyWriter::KAtlasKeyWriter(QFile& targetFile, KAtlasKey& sourceAtlasKey) :
    mTargetFile(targetFile),
    mSourceAtlasKey(sourceAtlasKey)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
bool KAtlasKeyWriter::writeElement(const QString& elementName, const QRectF& element)
{
    mStreamWriter.writeEmptyElement(Xml::ELEMENT_ELEMENT);
    mStreamWriter.writeAttribute(Xml::ATTRIBUTE_ELEMENT_NAME, elementName);
    mStreamWriter.writeAttribute(Xml::ATTRIBUTE_TOP_LEFT_X, QString::number(element.left()));
    mStreamWriter.writeAttribute(Xml::ATTRIBUTE_BOTTOM_RIGHT_X, QString::number(element.right()));
    mStreamWriter.writeAttribute(Xml::ATTRIBUTE_TOP_LEFT_Y, QString::number(element.top()));
    mStreamWriter.writeAttribute(Xml::ATTRIBUTE_BOTTOM_RIGHT_Y, QString::number(element.bottom()));

    // Return error status
    return !mStreamWriter.hasError();
}

//Public:
Qx::XmlStreamWriterError KAtlasKeyWriter::write()
{
    // Open file
    if(!mTargetFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return Qx::XmlStreamWriterError(mTargetFile.errorString());

    // Setup Writer
    mStreamWriter.setDevice(&mTargetFile);
    mStreamWriter.setAutoFormatting(true);

    // Write header
    mStreamWriter.writeStartDocument();

    // Start atlas element
    mStreamWriter.writeStartElement(Xml::ELEMENT_ATLAS);

    // Write texture element
    mStreamWriter.writeEmptyElement(Xml::ELEMENT_TEXTURE);
    mStreamWriter.writeAttribute(Xml::ATTRIBUTE_FILENAME, mSourceAtlasKey.atlasFilename());

    // Write elements
    mStreamWriter.writeStartElement(Xml::ELEMENT_ELEMENTS);

    QMap<QString, QRectF>::const_iterator i;
    for(i = mSourceAtlasKey.elements().constBegin(); i != mSourceAtlasKey.elements().constEnd(); i++)
    {
        if(!writeElement(i.key(), i.value()))
            return Qx::XmlStreamWriterError(mStreamWriter.device()->errorString());
    }

    mStreamWriter.writeEndElement();

    // Write straight alpha
    mStreamWriter.writeTextElement(Xml::ELEMENT_STR_ALPHA, mSourceAtlasKey.straightAlpha() ? "true" : "false");

    // End atlas element
    mStreamWriter.writeEndDocument();

    // Complete document
    mStreamWriter.writeEndDocument();

    // Close file
    mTargetFile.close();

    // Return writer status
    return mStreamWriter.hasError() ? Qx::XmlStreamWriterError(mStreamWriter.device()->errorString()) :
                                      Qx::XmlStreamWriterError();
}

//===============================================================================================================
// K_ATLAS_KEY_READER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlasKeyReader::KAtlasKeyReader(KAtlasKey& targetAtlasKey, QFile& sourceFile) :
    mTargetAtlasKey(targetAtlasKey),
    mSourceFile(sourceFile)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
bool KAtlasKeyReader::readAtlasKey()
{
    bool hasFilename = false;
    bool hasElements = false;

    while(mStreamReader.readNextStartElement())
    {
        if(mStreamReader.name() == Xml::ELEMENT_TEXTURE)
        {
            parseTexture();
            hasFilename = true;
        }
        else if(mStreamReader.name() == Xml::ELEMENT_ELEMENTS)
        {
            if(!readElements())
                return true; // Error

            hasElements = true;
        }
        else if(mStreamReader.name() == Xml::ELEMENT_STR_ALPHA)
            parseStraightAlpha();
        else
            mStreamReader.skipCurrentElement();
    }

    if(!hasFilename)
        mStreamReader.raiseError(ERR_NO_FILENAME);
    else if(!hasElements)
        mStreamReader.raiseError(ERR_NO_ELMENTS);

    // Return status
    return !mStreamReader.hasError();
}

bool KAtlasKeyReader::readElements()
{
    bool hasElements = false;

    while(mStreamReader.readNextStartElement())
    {
        if(mStreamReader.name() == Xml::ELEMENT_ELEMENT)
        {
            parseElement();
            hasElements = true;
        }
        else
            mStreamReader.skipCurrentElement();
    }

    if(!hasElements)
        mStreamReader.raiseError(ERR_NO_ELMENTS);

    // Return status
    return !mStreamReader.hasError();
}

void KAtlasKeyReader::parseTexture()
{
    QXmlStreamAttributes textureAttrib = mStreamReader.attributes();

    // Check for require attributes
    if(!hasAttributes(textureAttrib, {Xml::ATTRIBUTE_FILENAME}))
        return;

    // Read attributes
    mTargetAtlasKey.setAtlasFilename(textureAttrib.value("", Xml::ATTRIBUTE_FILENAME).toString());

    // Go to end of element
    mStreamReader.skipCurrentElement();
}

void KAtlasKeyReader::parseElement()
{
    QXmlStreamAttributes elementAttriv = mStreamReader.attributes();

    // Check for require attributes
    if(!hasAttributes(elementAttriv, {Xml::ATTRIBUTE_ELEMENT_NAME,
                                      Xml::ATTRIBUTE_TOP_LEFT_X, Xml::ATTRIBUTE_TOP_LEFT_Y,
                                      Xml::ATTRIBUTE_BOTTOM_RIGHT_X, Xml::ATTRIBUTE_BOTTOM_RIGHT_Y}))
        return;

    // Read attributes
    QString elementName = elementAttriv.value("", Xml::ATTRIBUTE_ELEMENT_NAME).toString();

    QRectF element;
    element.setTopLeft(QPointF(elementAttriv.value("", Xml::ATTRIBUTE_TOP_LEFT_X).toDouble(),
                               elementAttriv.value("", Xml::ATTRIBUTE_TOP_LEFT_Y).toDouble()));
    element.setBottomRight(QPointF(elementAttriv.value("", Xml::ATTRIBUTE_BOTTOM_RIGHT_X).toDouble(),
                                   elementAttriv.value("", Xml::ATTRIBUTE_BOTTOM_RIGHT_Y).toDouble()));

    mTargetAtlasKey.insertElement(elementName, element);

    // Go to end of element
    mStreamReader.skipCurrentElement();
}

void KAtlasKeyReader::parseStraightAlpha()
{
    QString text = mStreamReader.readElementText(); // Already goes to end of element
    bool straightAlpha = false;

    if(text == "true")
        straightAlpha = true;
    else if(text == "false")
        straightAlpha = false;
    else
        mStreamReader.raiseError(ERR_INVALID_STR_ALPHA);

    mTargetAtlasKey.setStraightAlpha(straightAlpha);
}

bool KAtlasKeyReader::hasAttributes(const QXmlStreamAttributes& attributes, const QStringList& checkList)
{
    for(const QString& attrib : checkList)
    {
        if(!attributes.hasAttribute("", attrib))
        {
            mStreamReader.raiseError(ERR_INVALID_ATTRIBS);
            return false;
        }
    }

    return true;
}

//Public:
Qx::XmlStreamReaderError KAtlasKeyReader::read()
{
    // Open file
    if(!mSourceFile.open(QIODevice::ReadOnly))
        return Qx::XmlStreamReaderError(mSourceFile.errorString());

    // Setup reader
    mStreamReader.setDevice(&mSourceFile);

    // Prepare error tracker
    Qx::XmlStreamReaderError readError;

    if(mStreamReader.readNextStartElement())
    {
        if(mStreamReader.name() == Xml::ELEMENT_ATLAS)
        {
            // Return no error on success
            if(!readAtlasKey())
            {
                if(mStreamReader.error() == QXmlStreamReader::CustomError)
                    return Qx::XmlStreamReaderError(mStreamReader.errorString());
                else
                    return Qx::XmlStreamReaderError(mStreamReader.error());
            }
            else
                return Qx::XmlStreamReaderError();
        }
        else
            readError = Qx::XmlStreamReaderError(ERR_NOT_ATLAS_KEY);
    }
    else
        readError = Qx::XmlStreamReaderError(mStreamReader.error());

    // Close file
    mSourceFile.close();

    // Return status
    return readError;
}
