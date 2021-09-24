#ifndef ATLAS_H
#define ATLAS_H

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

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    KAtlaser(const QMap<QString, QImage>& namedImages);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QMap<QString, QPoint> packMap(const QMap<QString, QSize>& boxesToPack, QSize& size) const;

public:
    KAtlas process() const;
};

#endif // ATLAS_H
