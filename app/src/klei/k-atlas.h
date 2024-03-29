#ifndef ATLAS_H
#define ATLAS_H

// Qt Includes
#include <QStringList>
#include <QMap>
#include <QImage>
#include <QSize>

struct KAtlas
{
    QImage image;
    QMap<QString, QRect> elements;
};

class KAtlaser
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const QMap<QString, QImage>& mNamedImages;
    bool mUseMargin;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlaser(const QMap<QString, QImage>& namedImages, bool useMargin);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QMap<QString, QPoint> packMap(const QMap<QString, QSize>& boxesToPack, QSize& size) const;
    KAtlas processSingleImage() const;
    KAtlas processMultiImage() const;

public:
    KAtlas process() const;
};

class KDeatlaser
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const KAtlas& mAtlas;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KDeatlaser(const KAtlas& atlas);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QMap<QString, QRect> flipElements() const;
    QMap<QString, QImage> extractElements(const QImage& normalizedAtlas, const QMap<QString, QRect> normalizedElements) const;

public:
    QMap<QString, QImage> process() const;
};

#endif // ATLAS_H
