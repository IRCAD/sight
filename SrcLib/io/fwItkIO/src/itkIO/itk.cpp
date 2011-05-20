/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include "itkIO/itk.hpp"
#include "itkImageRegionConstIterator.h"

namespace itkIO
{

//------------------------------------------------------------------------------

double computeVolume( ::fwData::Image::sptr imageData )
{
    OSLM_FATAL_IF("only unsigned char image are managed", imageData->getPixelType().isType<unsigned char>()==false );
    typedef  itk::Image<unsigned char, 3>  ItkImage;
    ItkImage::Pointer itkImage = itkImageFactory<ItkImage>(imageData);

    long nbPixel = 0;
    typedef itk::ImageRegionConstIterator< ItkImage > ImageIter;
    ImageIter iter( itkImage , itkImage->GetLargestPossibleRegion() );
    iter.GoToBegin();

    while ( !iter.IsAtEnd() )
    {
        if ( iter.Get() )
        {
            ++nbPixel;
        }
        ++iter;
    }

    float volume = nbPixel*itkImage->GetSpacing()[0]*itkImage->GetSpacing()[1]*itkImage->GetSpacing()[2];
    return volume;
}

//------------------------------------------------------------------------------

} // namespace itkIO



