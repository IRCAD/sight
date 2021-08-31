/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/fieldHelper/MedicalImageHelpers.hpp"

#include "data/fieldHelper/Image.hpp"
#include "data/helper/Composite.hpp"
#include "data/helper/Field.hpp"
#include <data/Composite.hpp>
#include <data/Integer.hpp>
#include <data/PointList.hpp>
#include <data/ResectionDB.hpp>
#include <data/String.hpp>
#include <data/TransferFunction.hpp>

namespace sight::data
{

namespace fieldHelper
{

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkLandmarks(data::Image::sptr _pImg)
{
    bool fieldIsModified = false;

    // Manage image landmarks
    if(!_pImg->getField(data::fieldHelper::Image::m_imageLandmarksId))
    {
        data::PointList::sptr pl = data::PointList::New();
        _pImg->setField(data::fieldHelper::Image::m_imageLandmarksId, pl);
        fieldIsModified = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageValidity(data::Image::csptr _pImg)
{
    return _pImg ? checkImageValidity(*_pImg) : false;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageValidity(const data::Image& _image)
{
    // Test if the image is allocated
    bool dataImageIsAllocated = (_image.getAllocatedSizeInBytes() > 0);

    if(dataImageIsAllocated)
    {
        size_t nbDim = _image.getNumberOfDimensions();
        dataImageIsAllocated &= nbDim > 1;

        for(size_t k = 0 ; dataImageIsAllocated && k < nbDim ; ++k)
        {
            dataImageIsAllocated = dataImageIsAllocated && (_image.getSize2()[k] >= 1);
        }
    }

    return dataImageIsAllocated;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkImageSliceIndex(data::Image::sptr _pImg)
{
    SIGHT_ASSERT("_pImg pointer null", _pImg);

    bool fieldIsModified = false;

    const data::Image::Size& imageSize = _pImg->getSize2();

    data::Integer::sptr axialIdx = _pImg->getField<data::Integer>(
        data::fieldHelper::Image::m_axialSliceIndexId
    );
    data::Integer::sptr frontalIdx = _pImg->getField<data::Integer>(
        data::fieldHelper::Image::m_frontalSliceIndexId
    );
    data::Integer::sptr sagittalIdx = _pImg->getField<data::Integer>(
        data::fieldHelper::Image::m_sagittalSliceIndexId
    );

    // Manage image landmarks
    if(!(axialIdx && frontalIdx && sagittalIdx))
    {
        // Set value
        axialIdx = data::Integer::New(-1);
        _pImg->setField(data::fieldHelper::Image::m_axialSliceIndexId, axialIdx);

        frontalIdx = data::Integer::New(-1);
        _pImg->setField(data::fieldHelper::Image::m_frontalSliceIndexId, frontalIdx);

        sagittalIdx = data::Integer::New(-1);
        _pImg->setField(data::fieldHelper::Image::m_sagittalSliceIndexId, sagittalIdx);

        fieldIsModified = true;
    }

    SIGHT_ASSERT(
        "Information on image slice index is not correct, miss one of these fields : m_axialSliceIndexId, m_frontalSliceIndexId, m_sagittalSliceIndexId.",
        axialIdx && frontalIdx && sagittalIdx
    );

    // Get value
    if(axialIdx->value() < 0 || imageSize[2] < static_cast<size_t>(axialIdx->value()))
    {
        axialIdx->value() = static_cast<data::Integer::ValueType>(imageSize[2] / 2);
        fieldIsModified   = true;
    }

    if(frontalIdx->value() < 0 || imageSize[1] < static_cast<size_t>(frontalIdx->value()))
    {
        frontalIdx->value() = static_cast<data::Integer::ValueType>(imageSize[1] / 2);
        fieldIsModified     = true;
    }

    if(sagittalIdx->value() < 0 || imageSize[0] < static_cast<size_t>(sagittalIdx->value()))
    {
        sagittalIdx->value() = static_cast<data::Integer::ValueType>(imageSize[0] / 2);
        fieldIsModified      = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

data::Point::sptr MedicalImageHelpers::getImageSliceIndices(data::Image::sptr _pImg)
{
    SIGHT_ASSERT("_pImg pointer null", _pImg);

    data::Point::sptr point = data::Point::New();

    MedicalImageHelpers::checkImageSliceIndex(_pImg);

    point->getCoord()[0] =
        static_cast<double>(_pImg->getField<data::Integer>(data::fieldHelper::Image::m_sagittalSliceIndexId)
                            ->value());
    point->getCoord()[1] =
        static_cast<double>(_pImg->getField<data::Integer>(data::fieldHelper::Image::m_frontalSliceIndexId)
                            ->value());
    point->getCoord()[2] =
        static_cast<double>(_pImg->getField<data::Integer>(data::fieldHelper::Image::m_axialSliceIndexId)
                            ->value());

    return point;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkComment(data::Image::sptr _pImg)
{
    SIGHT_ASSERT("_pImg pointer null", _pImg);

    bool fieldIsModified = false;

    if(!_pImg->getField(data::fieldHelper::Image::m_commentId))
    {
        // Set value
        data::String::sptr param = data::String::New("Original image");
        _pImg->setField(data::fieldHelper::Image::m_commentId, param);
        fieldIsModified = true;
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

data::Image::sptr MedicalImageHelpers::initialize(
    data::Image::sptr imgSrc,
    data::Image::sptr imgToInitialize
)
{
    FW_DEPRECATED_MSG("This method is no longer supported", "22.0")
    SIGHT_ASSERT("Image source must be initialized", imgSrc);
    SIGHT_ASSERT("Image source must be valid", MedicalImageHelpers::checkImageValidity(imgSrc));

    if(!imgToInitialize)
    {
        imgToInitialize = data::Image::New();
    }

    data::Array::sptr imgData = imgSrc->getDataArray();
    imgSrc->setDataArray(data::Array::sptr(), false);

    imgToInitialize = data::Object::copy(imgSrc);

    imgSrc->setDataArray(imgData, false);

    imgToInitialize->resize();

    return imgToInitialize;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::isBufNull(const data::Image::BufferType* buf, const unsigned int len)
{
    bool isNull;
    const data::Image::BufferType* ucbuf = static_cast<const data::Image::BufferType*>(buf);
    isNull = 0 == std::accumulate(
        ucbuf,
        ucbuf + len,
        0,
        std::bit_or<data::Image::BufferType>()
    );
    return isNull;
}

//------------------------------------------------------------------------------

bool MedicalImageHelpers::checkTransferFunctionPool(const data::Image::sptr& image)
{
    bool fieldIsCreated             = false;
    const std::string poolFieldName = data::fieldHelper::Image::m_transferFunctionCompositeId;
    data::Composite::sptr tfPool;

    tfPool = image->getField<data::Composite>(poolFieldName);
    // Transfer functions
    if(!tfPool)
    {
        tfPool = data::Composite::New();

        // Set in selected image
        data::helper::Field fieldHelper(image);
        fieldHelper.setField(poolFieldName, tfPool);
        fieldHelper.notify();

        // TF pool is modified
        fieldIsCreated = true;
    }

    const std::string defaultTFName = data::TransferFunction::s_DEFAULT_TF_NAME;
    if(tfPool->find(defaultTFName) == tfPool->end())
    {
        data::TransferFunction::sptr tf = data::TransferFunction::createDefaultTF();
        if(image->getWindowWidth() != 0.)
        {
            tf->setWindow(image->getWindowWidth());
            tf->setLevel(image->getWindowCenter());
        }
        else if(data::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
        {
            double min, max;
            data::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            data::TransferFunction::TFValuePairType wlMinMax(min, max);
            tf->setWLMinMax(wlMinMax);
        }

        // Set in TFPool
        data::helper::Composite compositeHelper(tfPool);
        compositeHelper.add(defaultTFName, tf);
        compositeHelper.notify();
    }

    return fieldIsCreated;
}

//------------------------------------------------------------------------------

} // fieldHelper

} // fwDataTools
