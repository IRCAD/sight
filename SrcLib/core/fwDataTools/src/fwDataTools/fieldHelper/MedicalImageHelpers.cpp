/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwDataTools/fieldHelper/MedicalImageHelpers.hpp"

#include "fwDataTools/fieldHelper/Image.hpp"
#include "fwDataTools/helper/Composite.hpp"
#include "fwDataTools/helper/Field.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/PointList.hpp>
#include <fwData/ResectionDB.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwMath/MeshFunctions.hpp>

namespace fwDataTools
{

namespace fieldHelper
{

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkLandmarks( ::fwData::Image::sptr _pImg )
{
    bool fieldIsModified = false;

    // Manage image landmarks
    if ( !_pImg->getField( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId ) )
    {
        ::fwData::PointList::sptr pl = ::fwData::PointList::New();
        _pImg->setField( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId, pl );
        fieldIsModified = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageValidity( ::fwData::Image::csptr _pImg )
{
    SLM_TRACE_FUNC();

    // Test if the image is allocated
    bool dataImageIsAllocated = (_pImg != ::fwData::Image::sptr());

    if (dataImageIsAllocated)
    {
        size_t nbDim = _pImg->getNumberOfDimensions();
        dataImageIsAllocated &= nbDim > 1;

        for ( size_t k = 0; dataImageIsAllocated && k < nbDim; ++k )
        {
            dataImageIsAllocated = dataImageIsAllocated && ( _pImg->getSize()[k] >= 1 );
        }
    }

    return dataImageIsAllocated;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageSliceIndex( ::fwData::Image::sptr _pImg )
{
    SLM_ASSERT("_pImg pointer null", _pImg);

    bool fieldIsModified = false;

    const ::fwData::Image::SizeType& imageSize = _pImg->getSize();

    ::fwData::Integer::sptr axialIdx = _pImg->getField< ::fwData::Integer >(
        ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId );
    ::fwData::Integer::sptr frontalIdx = _pImg->getField< ::fwData::Integer >(
        ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId);
    ::fwData::Integer::sptr sagittalIdx = _pImg->getField< ::fwData::Integer >(
        ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId );

    // Manage image landmarks
    if ( !(axialIdx && frontalIdx && sagittalIdx) )
    {
        // Set value
        axialIdx = ::fwData::Integer::New(-1);
        _pImg->setField( ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, axialIdx );

        frontalIdx = ::fwData::Integer::New(-1);
        _pImg->setField( ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, frontalIdx );

        sagittalIdx = ::fwData::Integer::New(-1);
        _pImg->setField( ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, sagittalIdx );

        fieldIsModified = true;
    }

    SLM_ASSERT(
        "Information on image slice index is not correct, miss one of these fields : m_axialSliceIndexId, m_frontalSliceIndexId, m_sagittalSliceIndexId.",
        axialIdx && frontalIdx && sagittalIdx
        );

    // Get value
    if( axialIdx->value() < 0 ||  imageSize[2] < axialIdx->value() )
    {
        axialIdx->value() = static_cast< ::fwData::Integer::ValueType >(imageSize[2] / 2);
        fieldIsModified   = true;
    }

    if( frontalIdx->value() < 0 ||  imageSize[1] < frontalIdx->value() )
    {
        frontalIdx->value() = static_cast< ::fwData::Integer::ValueType >(imageSize[1] / 2);
        fieldIsModified     = true;
    }

    if( sagittalIdx->value() < 0 ||  imageSize[0] < sagittalIdx->value() )
    {
        sagittalIdx->value() = static_cast< ::fwData::Integer::ValueType >(imageSize[0] / 2);
        fieldIsModified      = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

::fwData::Point::sptr MedicalImageHelpers::getImageSliceIndices( ::fwData::Image::sptr _pImg )
{
    SLM_ASSERT("_pImg pointer null", _pImg);

    ::fwData::Point::sptr point = ::fwData::Point::New();

    MedicalImageHelpers::checkImageSliceIndex(_pImg);

    point->getCoord()[0] =
        _pImg->getField< ::fwData::Integer >( ::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId )->value();
    point->getCoord()[1] =
        _pImg->getField< ::fwData::Integer >( ::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId  )->value();
    point->getCoord()[2] =
        _pImg->getField< ::fwData::Integer >( ::fwDataTools::fieldHelper::Image::m_axialSliceIndexId    )->value();

    return point;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkComment( ::fwData::Image::sptr _pImg )
{
    SLM_ASSERT("_pImg pointer null", _pImg);

    bool fieldIsModified = false;

    if ( !_pImg->getField( ::fwDataTools::fieldHelper::Image::m_commentId ) )
    {
        // Set value
        ::fwData::String::sptr param = ::fwData::String::New("Original image");
        _pImg->setField( ::fwDataTools::fieldHelper::Image::m_commentId, param );
        fieldIsModified = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

::fwData::Image::sptr MedicalImageHelpers::initialize( ::fwData::Image::sptr imgSrc,
                                                       ::fwData::Image::sptr imgToInitialize)
{
    SLM_ASSERT("Image source must be initialized", imgSrc);
    SLM_ASSERT("Image source must be valid", MedicalImageHelpers::checkImageValidity(imgSrc));

    if(!imgToInitialize)
    {
        imgToInitialize = ::fwData::Image::New();
    }
    ::fwData::Array::sptr imgData = imgSrc->getDataArray();
    imgSrc->setDataArray(::fwData::Array::sptr(), false);

    imgToInitialize = ::fwData::Object::copy(imgSrc);

    imgSrc->setDataArray(imgData, false);

    imgToInitialize->allocate();

    return imgToInitialize;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::isBufNull(const ::fwData::Image::BufferType* buf, const unsigned int len)
{
    bool isNull;
    const ::fwData::Image::BufferType* ucbuf = static_cast< const ::fwData::Image::BufferType*> (buf);
    isNull = 0 == std::accumulate(
        ucbuf,
        ucbuf+len,
        0,
        std::bit_or< ::fwData::Image::BufferType>()
        );
    return isNull;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkTransferFunctionPool(const ::fwData::Image::sptr& image)
{
    bool fieldIsCreated             = false;
    const std::string poolFieldName = ::fwDataTools::fieldHelper::Image::m_transferFunctionCompositeId;
    ::fwData::Composite::sptr tfPool;

    tfPool = image->getField< ::fwData::Composite >(poolFieldName);
    // Transfer functions
    if ( !tfPool )
    {
        tfPool = ::fwData::Composite::New();

        // Set in selected image
        ::fwDataTools::helper::Field fieldHelper(image);
        fieldHelper.setField(poolFieldName, tfPool);
        fieldHelper.notify();

        // TF pool is modified
        fieldIsCreated = true;
    }

    const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
    if(tfPool->find(defaultTFName) == tfPool->end())
    {
        ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::createDefaultTF();
        if (image->getWindowWidth() != 0. )
        {
            tf->setWindow( image->getWindowWidth() );
            tf->setLevel( image->getWindowCenter() );
        }
        else if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
        {
            double min, max;
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            ::fwData::TransferFunction::TFValuePairType wlMinMax(min, max);
            tf->setWLMinMax(wlMinMax);
        }
        // Set in TFPool
        ::fwDataTools::helper::Composite compositeHelper(tfPool);
        compositeHelper.add(defaultTFName, tf);
        compositeHelper.notify();
    }

    return fieldIsCreated;
}

//------------------------------------------------------------------------------

} // fieldHelper
} // fwDataTools
