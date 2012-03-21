/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/PointList.hpp>
#include <fwData/TransferFunction.hpp>
#include <fwData/Composite.hpp>

#include <fwServices/IEditionService.hpp>

#include "fwComEd/helper/Image.hpp"
#include "fwComEd/helper/Field.hpp"
#include "fwComEd/Dictionary.hpp"

namespace fwComEd
{
namespace helper
{


//-----------------------------------------------------------------------------

Image::Image( ::fwData::Image::wptr image )
    : m_image(image),
      m_imageMsg ( new ::fwComEd::ImageMsg() )
{}

//-----------------------------------------------------------------------------

Image::~Image()
{}

//-----------------------------------------------------------------------------

void Image::notify( ::fwServices::IService::sptr _serviceSource )
{
    if ( m_imageMsg->getEventIds().size() > 0 )
    {
        ::fwServices::IEditionService::notify( _serviceSource, m_image.lock(), m_imageMsg );
    }
    SLM_INFO_IF("Sorry, this helper cannot notify his message because the message is empty.", m_imageMsg->getEventIds().empty());
}

//------------------------------------------------------------------------------

bool Image::createLandmarks()
{
    ::fwData::Image::sptr img = m_image.lock();

    bool fieldIsCreated = false;

    // Manage image landmarks
    if ( ! img->getField( ::fwComEd::Dictionary::m_imageLandmarksId ) )
    {
        ::fwData::PointList::NewSptr pl;
        img->setField( ::fwComEd::Dictionary::m_imageLandmarksId, pl );
        fieldIsCreated = true;
    }

    return fieldIsCreated;
}


//------------------------------------------------------------------------------

bool Image::createTransferFunctionPool(::fwServices::IService::sptr serviceSource)
{
    ::fwData::Image::sptr img = m_image.lock();

    bool fieldIsCreated = false;

    // Transfer functions
    if ( ! img->getField( ::fwComEd::Dictionary::m_transfertFunctionCompositeId ) )
    {
        ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::createDefaultTF();
        tf->setWindow( img->getWindowWidth() );
        tf->setLevel( img->getWindowCenter() );

        ::fwData::Composite::NewSptr cTF;
        cTF->getRefMap()[ tf->getName() ] = tf;

        // Set in selected image
        ::fwComEd::helper::Field fieldHelper(img);
        fieldHelper.setField( ::fwComEd::Dictionary::m_transfertFunctionCompositeId, cTF);
        if(serviceSource)
        {
            fieldHelper.notify(serviceSource);
        }

        // TF is modified
        fieldIsCreated = true;
    }

    return fieldIsCreated;
}


//------------------------------------------------------------------------------

bool Image::createImageSliceIndex()
{
    ::fwData::Image::sptr img = m_image.lock();

    bool fieldIsCreated = false;

    const ::fwData::Image::SizeType &imageSize = img->getSize();

    ::fwData::Integer::sptr axialIdx    = img->getField< ::fwData::Integer >( ::fwComEd::Dictionary::m_axialSliceIndexId );
    ::fwData::Integer::sptr frontalIdx  = img->getField< ::fwData::Integer >( ::fwComEd::Dictionary::m_frontalSliceIndexId);
    ::fwData::Integer::sptr sagittalIdx = img->getField< ::fwData::Integer >( ::fwComEd::Dictionary::m_sagittalSliceIndexId );

    // Manage image slice index
    if ( ! (axialIdx && frontalIdx && sagittalIdx) )
    {
        // Set value
        axialIdx = ::fwData::Integer::New(-1);
        img->setField( ::fwComEd::Dictionary::m_axialSliceIndexId, axialIdx );

        frontalIdx = ::fwData::Integer::New(-1);
        img->setField( ::fwComEd::Dictionary::m_frontalSliceIndexId, frontalIdx );

        sagittalIdx = ::fwData::Integer::New(-1);
        img->setField( ::fwComEd::Dictionary::m_sagittalSliceIndexId, sagittalIdx );

        fieldIsCreated = true;
    }


    SLM_ASSERT (
            "Information on image slice index is not correct, miss one of these fields : m_axialSliceIndexId, m_frontalSliceIndexId, m_sagittalSliceIndexId.",
            axialIdx && frontalIdx && sagittalIdx
            );

    // Get value
    if( axialIdx->value() < 0 ||  imageSize[2] < axialIdx->value() )
    {
        axialIdx->value() = imageSize[2] / 2;
        fieldIsCreated = true;
    }

    if( frontalIdx->value() < 0 ||  imageSize[1] < frontalIdx->value() )
    {
        frontalIdx->value() = imageSize[1] / 2;
        fieldIsCreated = true;
    }

    if( sagittalIdx->value() < 0 ||  imageSize[0] < sagittalIdx->value() )
    {
        sagittalIdx->value() = imageSize[0] / 2;
        fieldIsCreated = true;
    }

    return fieldIsCreated;
}

//-----------------------------------------------------------------------------

} // helper
} // fwComEd
