#include "conversion.h"

//===============================================================================================================
// TO_TEX_CONVERTER
//===============================================================================================================

//-Constructor-------------------------------------------------------------------------------------------------
ToTexConverter::ToTexConverter(const QImage& sourceImage, const Options& options) :
    mSourceImage(sourceImage),
    mOptions(options)
{}

//-Instance Functions--------------------------------------------------------------------------------------------
//Private:


//Public:
KTex ToTexConverter::convert()
{
    return KTex();
}
