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
#include "fwComEd/helper/Composite.hpp"
#include "fwComEd/Dictionary.hpp"
#include "fwComEd/fieldHelper/MedicalImageHelpers.hpp"

namespace fwComEd
{
namespace helper
{


//-----------------------------------------------------------------------------

Image::Image( ::fwData::Image::sptr image )
    : m_image(image),
      m_imageMsg ( new ::fwComEd::ImageMsg() )
{
    if ( image )
    {
        m_lock = image->getDataArray()->getBufferObject()->lock();
    }
}

//-----------------------------------------------------------------------------

Image::~Image()
{}

//-----------------------------------------------------------------------------

void Image::notify( ::fwServices::IService::sptr _serviceSource )
{
    if ( m_imageMsg->getEventIds().size() > 0 )
    {
        ::fwServices::IEditionService::notify( _serviceSource, m_image, m_imageMsg );
    }
    SLM_INFO_IF("Sorry, this helper cannot notify his message because the message is empty.", m_imageMsg->getEventIds().empty());
}

//------------------------------------------------------------------------------

bool Image::createLandmarks()
{
    bool fieldIsCreated = false;

    // Manage image landmarks
    if ( ! m_image->getField( ::fwComEd::Dictionary::m_imageLandmarksId ) )
    {
        ::fwData::PointList::NewSptr pl;
        m_image->setField( ::fwComEd::Dictionary::m_imageLandmarksId, pl );
        fieldIsCreated = true;
    }

    return fieldIsCreated;
}


//------------------------------------------------------------------------------

bool Image::createTransferFunctionPool(::fwServices::IService::sptr serviceSource)
{
    bool fieldIsCreated = false;
    const std::string poolFieldName = ::fwComEd::Dictionary::m_transfertFunctionCompositeId;
    ::fwData::Composite::sptr tfPool;

    tfPool = m_image->getField< ::fwData::Composite >(poolFieldName);
    // Transfer functions
    if ( ! tfPool )
    {
        tfPool = ::fwData::Composite::New();

        // Set in selected image
        ::fwComEd::helper::Field fieldHelper(m_image);
        fieldHelper.setField(poolFieldName, tfPool);
        if(serviceSource)
        {
            fieldHelper.notify(serviceSource);
        }
        // TF pool is modified
        fieldIsCreated = true;
    }

    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    if(tfPool->find(defaultTFName) == tfPool->end())
    {
        ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::createDefaultTF();
        tf->setWindow( m_image->getWindowWidth() );
        tf->setLevel( m_image->getWindowCenter() );
        if(::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(m_image))
        {
            double min, max;
            ::fwComEd::fieldHelper::MedicalImageHelpers::getMinMax(m_image, min, max);
            ::fwData::TransferFunction::TFValuePairType wlMinMax(min, max);
            tf->setWLMinMax(wlMinMax);
        }
        // Set in TFPool
        ::fwComEd::helper::Composite compositeHelper(tfPool);
        compositeHelper.add(defaultTFName, tf);
        if(serviceSource)
        {
            compositeHelper.notify(serviceSource);
        }
    }

    return fieldIsCreated;
}


//------------------------------------------------------------------------------

bool Image::createImageSliceIndex()
{
    bool fieldIsCreated = false;

    const ::fwData::Image::SizeType &imageSize = m_image->getSize();

    ::fwData::Integer::sptr axialIdx    = m_image->getField< ::fwData::Integer >( ::fwComEd::Dictionary::m_axialSliceIndexId );
    ::fwData::Integer::sptr frontalIdx  = m_image->getField< ::fwData::Integer >( ::fwComEd::Dictionary::m_frontalSliceIndexId);
    ::fwData::Integer::sptr sagittalIdx = m_image->getField< ::fwData::Integer >( ::fwComEd::Dictionary::m_sagittalSliceIndexId );

    // Manage image slice index
    if ( ! (axialIdx && frontalIdx && sagittalIdx) )
    {
        // Set value
        axialIdx = ::fwData::Integer::New(-1);
        m_image->setField( ::fwComEd::Dictionary::m_axialSliceIndexId, axialIdx );

        frontalIdx = ::fwData::Integer::New(-1);
        m_image->setField( ::fwComEd::Dictionary::m_frontalSliceIndexId, frontalIdx );

        sagittalIdx = ::fwData::Integer::New(-1);
        m_image->setField( ::fwComEd::Dictionary::m_sagittalSliceIndexId, sagittalIdx );

        fieldIsCreated = true;
    }


    SLM_ASSERT (
            "Information on image slice index is not correct, miss one of these fields : "
            "m_axialSliceIndexId, m_frontalSliceIndexId, m_sagittalSliceIndexId.",
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

void * Image::getBuffer()
{
    return m_lock.getBuffer();
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( SizeType::value_type x, SizeType::value_type y, SizeType::value_type z )
{
    SizeType size = m_image->getSize();
    IndexType offset = x + size[0]*y + z*size[0]*size[1];
    return getPixelBuffer(offset);
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( IndexType index )
{
    ::boost::uint8_t imagePixelSize = m_image->getType().sizeOf();
    BufferType * buf = static_cast < BufferType * > (this->getBuffer());
    BufferIndexType bufIndex = index * imagePixelSize;
    return buf + bufIndex;
}
} // helper
} // fwComEd
