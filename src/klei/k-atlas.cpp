#include "k-atlas.h"
#include "qx.h"

#include <QImageReader>
#include <QPainter>

//===============================================================================================================
// K_ATLASER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
KAtlaser::KAtlaser(const QMap<QString, QImage>& namedImages) : mNamedImages(namedImages) {}

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

//Public:
KAtlas KAtlaser::process() const
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
        boundingBox.setWidth(i->width() + 1);
        boundingBox.setHeight(i->height() + 1);

        elementBoundingBoxes[i.key()] = boundingBox;

        totalArea += boundingBox.height() * boundingBox.width();
    }

    // Estimate final atlas size, preferring a rectangular shape
    int longSide = Qx::Number::ceilPowOfTwo(static_cast<int>(sqrt(totalArea)));
    int shortSide = Qx::Number::roundPowOfTwo(static_cast<int>(sqrt(totalArea)));
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

        // Add to element list
        atlasElements[iMappedBoxes.key()] = {imagePos, image.size()};
    }

    return KAtlas{atlasImage, atlasElements};
}
