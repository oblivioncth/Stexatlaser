#ifndef ATLAS_H
#define ATLAS_H

#include <QStringList>
#include <QMap>
#include <QImage>
#include <QSize>

struct Atlas
{
    QImage image;
    QMap<QString, QRect> elements;
};

class Atlaser
{
//-Instance Members-------------------------------------------------------------------------------------------------
private:
    const QMap<QString, QImage>& mNamedImages;

//-Constructor-------------------------------------------------------------------------------------------------------
public:
    Atlaser(const QMap<QString, QImage>& namedImages);

//-Instance Functions----------------------------------------------------------------------------------------------
private:
    QMap<QString, QPoint> packMap(const QMap<QString, QSize>& boxesToPack, QSize& size);
public:
    Atlas process();
};

#endif // ATLAS_H
