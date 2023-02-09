#include "k-atlas.h"

#include <QImageReader>
#include <QPainter>
#include <cmath>

#include <qx/core/qx-algorithm.h>

//===============================================================================================================
// K_ATLASER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlaser::KAtlaser(const QMap<QString, QImage>& namedImages, bool useMargin) :
    mNamedImages(namedImages),
    mUseMargin(useMargin)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
QMap<QString, QPoint> KAtlaser::packMap(const QMap<QString, QSize>& boxesToPack, QSize& size) const // TODO: Make a visualizer for this function
{
    // Final arrangement to return
    QMap<QString, QPoint> packedMap;

    while(packedMap.isEmpty())
    {
        // Need to use std::list because its iterators arent invalidated due to insertions unlike QList/QVector
        std::list<QRect> possibleContainers = {QRect(QPoint(0,0), size)}; // Start with entire area

        // Box pool
        QMap<QString, QSize> remainingBoxes(boxesToPack);

        // Process all boxes
        while(!remainingBoxes.empty())
        {
            int requiredMargin = std::max(size.width(), size.height()); // Start with length of entire area

            QMap<QString, QSize>::iterator iSmallestBox = remainingBoxes.end();
            std::list<QRect>::iterator iSmallestContainer = possibleContainers.end();

            // Check each box agaisnt available containers to find the tightest combination (smallest container that fits smallest box)
            for(QMap<QString, QSize>::iterator iBoxes = remainingBoxes.begin(); iBoxes != remainingBoxes.end(); iBoxes++)
            {
                for(std::list<QRect>::iterator iContainers = possibleContainers.begin(); iContainers != possibleContainers.end(); iContainers++)
                {
                    int smallestMargin = std::min(iContainers->height() - iBoxes->height(), iContainers->width() - iBoxes->width());

                    if(smallestMargin >= 0 && smallestMargin < requiredMargin)
                    {
                        requiredMargin = smallestMargin; // Set new margin to beat
                        iSmallestBox = iBoxes; // Record current smallest box
                        iSmallestContainer = iContainers; // Record current smallest container
                    }

                }
            }

            // Check if no combination was valid
            if(iSmallestBox == remainingBoxes.end() || iSmallestContainer == possibleContainers.end())
            {
                // Reset results and reattempt with larger area
                packedMap.clear();
                if(size.width() <= size.height())
                    size.setWidth(size.width() * 2);
                else
                    size.setHeight(size.height() * 2);
                break;
            }
            else // There is a fit
            {
                // Add smallest box to map
                QRect mappedBox(iSmallestContainer->topLeft(), *iSmallestBox);
                packedMap[iSmallestBox.key()] = mappedBox.topLeft(); // Only need to save mapped position

                // Add possible container covering the remaining free space to the right
                possibleContainers.push_front(QRect(
                    QPoint(iSmallestContainer->x() + iSmallestBox->width(),
                           iSmallestContainer->y()),
                    QSize(iSmallestContainer->width() - iSmallestBox->width(),
                          iSmallestContainer->height())
                ));

                // Add possible container covering the remaining free space below
                possibleContainers.push_front(QRect(
                    QPoint(iSmallestContainer->x(),
                           iSmallestContainer->y() + iSmallestBox->height()),
                    QSize(iSmallestContainer->width(),
                          iSmallestContainer->height() - iSmallestBox->height())
                ));

                // Remove consumed container
                possibleContainers.erase(iSmallestContainer);

                // Split each possible container into smaller containers surrounding the inserted box
                // now that it's consuming part of the original space that the container encompassed
                std::list<QRect>::iterator iContainer = possibleContainers.begin();
                while(iContainer != possibleContainers.end())
                {
                    // Only act if the box is at least partially within the original container
                    if(mappedBox.intersects(*iContainer))
                    {
                        // Box is within left edge of container
                        if(mappedBox.left() > iContainer->left())
                        {
                            possibleContainers.push_front(QRect(
                                QPoint(iContainer->left(),
                                       iContainer->top()),
                                QSize(mappedBox.left() - iContainer->left(),
                                      iContainer->height())
                            ));
                        }

                        // Box is within right edge of container
                        if(mappedBox.right() < iContainer->right())
                        {
                            possibleContainers.push_front(QRect(
                                QPoint(mappedBox.right() + 1,
                                       iContainer->top()),
                                QSize(iContainer->right() - mappedBox.right(),
                                      iContainer->height())
                            ));
                        }

                        // Box is within top edge of container
                        if(mappedBox.top() > iContainer->top())
                        {
                            possibleContainers.push_front(QRect(
                                QPoint(iContainer->left(),
                                       iContainer->top()),

                                QSize(iContainer->width(),
                                      mappedBox.top() - iContainer->top())
                            ));
                        }

                        // Box is within bottom edge of container
                        if(mappedBox.bottom() < iContainer->bottom())
                        {
                            possibleContainers.push_front(QRect(
                                QPoint(iContainer->left(),
                                       mappedBox.bottom() + 1),
                                QSize(iContainer->width(),
                                      iContainer->bottom() - mappedBox.bottom())
                            ));
                        }

                        // Remove original container
                        iContainer++;
                        possibleContainers.erase(std::prev(iContainer));
                    }
                    else
                        iContainer++;
                }

                // Remove redundant containers that are contained within larger ones
                // Compares each container to every other in the list and checks for encapsulation in both directions
                std::list<QRect>::iterator iContainerA = possibleContainers.begin();
                while(iContainerA != possibleContainers.end())
                {
                    std::list<QRect>::iterator iContainerB = std::next(iContainerA);

                    while(iContainerB != possibleContainers.end())
                    {
                        if(iContainerA->contains(*iContainerB))
                        {
                            iContainerB++;
                            possibleContainers.erase(std::prev(iContainerB));
                        }
                        else if(iContainerB->contains(*iContainerA))
                        {
                            iContainerA++;
                            possibleContainers.erase(std::prev(iContainerA));
                            break;
                        }
                        else
                            iContainerB++;
                    }

                    if(iContainerB == possibleContainers.end())
                        iContainerA++;
                }

//                // Old removal method (only checks in one direction)
//                for(std::list<QRect>::iterator iContainers = possibleContainers.begin(); iContainers != possibleContainers.end(); iContainers++)
//                {
//                    std::list<QRect>::iterator iSubContainers = std::next(iContainers);

//                    while(iSubContainers != possibleContainers.end())
//                    {
//                        if(iContainers->contains(*iSubContainers))
//                        {
//                            iSubContainers++;
//                            possibleContainers.erase(std::prev(iSubContainers));
//                        }

//                        else
//                            iSubContainers++;
//                    }
//                }

                // Remove handled box
                remainingBoxes.erase(iSmallestBox);
            }
        }
    }

    return packedMap;
}

KAtlas KAtlaser::processSingleImage() const
{
    // Get single image
    QString imageName = mNamedImages.firstKey();
    QImage image = mNamedImages.first();

    // Get next power of 2 size
    QSize atlasSize(Qx::ceilPowOfTwo(image.width()),
                    Qx::ceilPowOfTwo(image.height()));

    // Create atlas canvas
    QImage atlasImage(atlasSize.width(), atlasSize.height(), QImage::Format_ARGB32);
    atlasImage.fill(Qt::transparent);

    // Paint image on atlas
    QPainter elementPainter(&atlasImage);
    elementPainter.drawImage(QPoint(0, 0), image);

    // Create element key (correct for atlas being stored buttom up)
    QRect topUpRect = {QPoint(0, 0), image.size()};
    QRect bottomUpRect = {QPoint(topUpRect.x(), (atlasSize.height() - 1) - topUpRect.bottom()), topUpRect.size()};
    QMap<QString, QRect> atlasElements = {{imageName, bottomUpRect}};

    // Flip image to be stored bottom up
    QImage atlasMirrored = atlasImage.mirrored();

    return KAtlas{atlasMirrored, atlasElements};
}

KAtlas KAtlaser::processMultiImage() const
{
    // Element Image Maps
    QMap<QString, QSize> elementBoundingBoxes;

    // Determine total area of all images
    uint totalArea = 0;

    // Generate bounding boxes
    QMap<QString, QImage>::const_iterator i;
    for (i = mNamedImages.constBegin(); i != mNamedImages.constEnd(); i++)
    {
        QSize boundingBox;
        boundingBox.setWidth(i->width());
        boundingBox.setHeight(i->height());

        // Apply safety margin if requested
        if(mUseMargin)
        {
            boundingBox.rwidth()++;
            boundingBox.rheight()++;
        }

        // Add element box
        elementBoundingBoxes[i.key()] = boundingBox;

        totalArea += boundingBox.height() * boundingBox.width();
    }

    // Estimate final atlas size, preferring a rectangular shape
    int longSide = Qx::ceilPowOfTwo(static_cast<int>(std::sqrt(totalArea)));
    int shortSide = Qx::roundPowOfTwo(static_cast<int>(std::sqrt(totalArea)));
    QSize atlasSize(longSide, shortSide);

    // Map element images to final atlas
    QMap<QString, QPoint> packedBoxes = packMap(elementBoundingBoxes, atlasSize);

    // Create atlas canvas
    QImage atlasImage(atlasSize.width(), atlasSize.height(), QImage::Format_ARGB32);
    atlasImage.fill(Qt::transparent);

    // Insert elements into atlas
    QMap<QString, QRect> atlasElements;
    QMap<QString, QPoint>::const_iterator iMappedBoxes;
    for (iMappedBoxes = packedBoxes.constBegin(); iMappedBoxes != packedBoxes.constEnd(); iMappedBoxes++)
    {
        // Get original image of mapped bounding box
        const QImage& image = mNamedImages[iMappedBoxes.key()];
        const QPoint& imagePos = iMappedBoxes.value();

        // Paint image on atlas
        QPainter elementPainter(&atlasImage);
        elementPainter.drawImage(imagePos, image);

        // Correct for atlas being stored bottom up.
        // The new top left corner will be the old bottom left corner after a simulated flip
        QRect topUpRect = {imagePos, image.size()};
        QRect bottomUpRect = {QPoint(topUpRect.x(), (atlasSize.height() - 1) - topUpRect.bottom()), topUpRect.size()};

        // Add to element list, accounting for atlas being stored bottom up
        atlasElements[iMappedBoxes.key()] = bottomUpRect;
    }

    // Flip image to be stored bottom up
    QImage atlasMirrored = atlasImage.mirrored();

    return KAtlas{atlasMirrored, atlasElements};
}

//Public:
KAtlas KAtlaser::process() const
{
    assert(mNamedImages.size() > 0);

    if(mNamedImages.size() == 1)
        return processSingleImage();
    else
        return processMultiImage();
}


//===============================================================================================================
// K_DEATLASER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KDeatlaser::KDeatlaser(const KAtlas& atlas) : mAtlas(atlas) {}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:
QMap<QString, QRect> KDeatlaser::flipElements() const
{
    QMap<QString, QRect> flippedElements;

    QMap<QString, QRect>::const_iterator i;
    for(i = mAtlas.elements.constBegin(); i != mAtlas.elements.constEnd(); i++)
        flippedElements[i.key()] = {QPoint(i->x(), (mAtlas.image.height() - 1) - i->bottom()), i->size()};

    return flippedElements;
}

QMap<QString, QImage> KDeatlaser::extractElements(const QImage& normalizedAtlas, const QMap<QString, QRect> normalizedElements) const
{
    QMap<QString, QImage> namedImages;

    QMap<QString, QRect>::const_iterator i;
    for(i = normalizedElements.constBegin(); i != normalizedElements.constEnd(); i++)
        namedImages[i.key()] = normalizedAtlas.copy(i.value());

    return namedImages;
}

//Public:
QMap<QString, QImage> KDeatlaser::process() const
{
    // Flip atlas
    QImage standardImage = mAtlas.image.mirrored();

    // Convert to "standard" format (not needed, but hey)
    standardImage = standardImage.convertToFormat(QImage::Format_ARGB32);

    // Flip elements
    QMap<QString, QRect> flippedElements = flipElements();

    // Extract
    return extractElements(standardImage, flippedElements);

}
