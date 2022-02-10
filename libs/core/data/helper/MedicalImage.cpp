/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "data/helper/MedicalImage.hpp"

#include "data/helper/Composite.hpp"
#include "data/helper/Field.hpp"

#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>
#include <data/TransferFunction.hpp>
#include <data/Vector.hpp>

#include <numeric>
#include <utility> // std::pair

namespace sight::data
{

namespace helper
{

namespace id
{

// Note: keeping old name to preserve compatibility, should be harmonized in the future.
static constexpr std::string_view axial_slice_index    = "Axial Slice Index";
static constexpr std::string_view frontal_slice_index  = "Frontal Slice Index";
static constexpr std::string_view sagittal_slice_index = "Sagittal Slice Index";
static constexpr std::string_view landmarks            = "m_imageLandmarksId";
static constexpr std::string_view distances            = "m_imageDistancesId";
static constexpr std::string_view distance_visibility  = "ShowDistances";
static constexpr std::string_view transferFunction     = "m_transferFunctionCompositeId";
static constexpr std::string_view landmarks_visibility = "ShowLandmarks";

}

namespace MedicalImage
{

//------------------------------------------------------------------------------

bool checkImageValidity(data::Image::csptr _pImg)
{
    return _pImg ? checkImageValidity(*_pImg) : false;
}

//------------------------------------------------------------------------------

bool checkImageValidity(const data::Image& _image)
{
    // Test if the image is allocated
    bool dataImageIsAllocated = (_image.getAllocatedSizeInBytes() > 0);

    if(dataImageIsAllocated)
    {
        std::size_t nbDim = _image.numDimensions();
        dataImageIsAllocated &= nbDim > 1;

        for(std::size_t k = 0 ; dataImageIsAllocated && k < nbDim ; ++k)
        {
            dataImageIsAllocated = dataImageIsAllocated && (_image.getSize()[k] >= 1);
        }
    }

    return dataImageIsAllocated;
}

//------------------------------------------------------------------------------

bool checkImageSliceIndex(data::Image::sptr _pImg)
{
    SIGHT_ASSERT("_pImg pointer null", _pImg);

    bool fieldIsModified = false;

    const data::Image::Size& imageSize = _pImg->getSize();

    auto axialIdx    = getSliceIndex(*_pImg, orientation_t::AXIAL);
    auto frontalIdx  = getSliceIndex(*_pImg, orientation_t::FRONTAL);
    auto sagittalIdx = getSliceIndex(*_pImg, orientation_t::SAGITTAL);

    // Check if values are out of bounds
    if((axialIdx < 0 || imageSize[2] < static_cast<std::size_t>(axialIdx)) && imageSize[2] > 0)
    {
        axialIdx        = static_cast<std::int64_t>(imageSize[2] / 2);
        fieldIsModified = true;
    }

    if((frontalIdx < 0 || imageSize[1] < static_cast<std::size_t>(frontalIdx)) && imageSize[1] > 0)
    {
        frontalIdx      = static_cast<std::int64_t>(imageSize[1] / 2);
        fieldIsModified = true;
    }

    if((sagittalIdx < 0 || imageSize[0] < static_cast<std::size_t>(sagittalIdx)) && imageSize[0] > 0)
    {
        sagittalIdx     = static_cast<std::int64_t>(imageSize[0] / 2);
        fieldIsModified = true;
    }

    // Update or create fields.
    if(fieldIsModified)
    {
        setSliceIndex(*_pImg, orientation_t::AXIAL, axialIdx);
        setSliceIndex(*_pImg, orientation_t::FRONTAL, frontalIdx);
        setSliceIndex(*_pImg, orientation_t::SAGITTAL, sagittalIdx);
    }

    return fieldIsModified;
}

//------------------------------------------------------------------------------

bool isBufNull(const data::Image::BufferType* buf, const unsigned int len)
{
    bool isNull;
    const data::Image::BufferType* buffer = static_cast<const data::Image::BufferType*>(buf);
    isNull = 0 == std::accumulate(
        buffer,
        buffer + len,
        0,
        std::bit_or<data::Image::BufferType>()
    );
    return isNull;
}

//------------------------------------------------------------------------------

bool checkTransferFunctionPool(const data::Image::sptr& image)
{
    bool fieldIsCreated = false;
    const std::string poolFieldName(id::transferFunction);
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
        else if(checkImageValidity(image))
        {
            double min, max;
            getMinMax(image, min, max);
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

std::int64_t getSliceIndex(
    const data::Image& _image,
    const orientation_t& _orientation
)
{
    // Initialize index to -1
    std::int64_t index = -1;
    std::string orientation_index;
    switch(_orientation)
    {
        case orientation_t::AXIAL:
            orientation_index = std::string(id::axial_slice_index);
            break;

        case orientation_t::SAGITTAL:
            orientation_index = std::string(id::sagittal_slice_index);
            break;

        case orientation_t::FRONTAL:
            orientation_index = std::string(id::frontal_slice_index);
            break;

        default:
            SIGHT_THROW_EXCEPTION(data::Exception("Wrong orientation type."));
            break;
    }

    // Test if field exists
    const auto field = _image.getField(orientation_index);
    if(field)
    {
        // Test if the type is data::Integer.
        const auto field_int = _image.getField<data::Integer>(orientation_index);
        if(field_int)
        {
            // Get value.
            index = field_int->value();
        }
    }

    return index;
}

//------------------------------------------------------------------------------

void setSliceIndex(
    data::Image& _image,
    const orientation_t& _orientation,
    std::int64_t _sliceIdx
)
{
    data::Integer::sptr value = data::Integer::New();
    value->setValue(_sliceIdx);

    std::string orientation_index;
    switch(_orientation)
    {
        case orientation_t::AXIAL:
            orientation_index = std::string(id::axial_slice_index);
            break;

        case orientation_t::SAGITTAL:
            orientation_index = std::string(id::sagittal_slice_index);
            break;

        case orientation_t::FRONTAL:
            orientation_index = std::string(id::frontal_slice_index);
            break;

        default:
            SIGHT_THROW_EXCEPTION(data::Exception("Wrong orientation type."));
            break;
    }

    _image.setField(orientation_index, value);
}

//------------------------------------------------------------------------------

data::PointList::sptr getLandmarks(const data::Image& _image)
{
    return _image.getField<data::PointList>(std::string(id::landmarks));
}

//------------------------------------------------------------------------------

void setLandmarks(data::Image& _image, const data::PointList::sptr& _landmarks)
{
    if(_landmarks)
    {
        _image.setField(std::string(id::landmarks), _landmarks);
    }
    else
    {
        SIGHT_THROW_EXCEPTION(data::Exception("Trying to set nullptr as landmark field."));
    }
}

//------------------------------------------------------------------------------

data::Vector::sptr getDistances(const data::Image& _image)
{
    return _image.getField<data::Vector>(std::string(id::distances));
}

//------------------------------------------------------------------------------

void setDistances(data::Image& _image, const data::Vector::sptr& _distances)
{
    if(_distances)
    {
        _image.setField(std::string(id::distances), _distances);
    }
    else
    {
        SIGHT_ERROR("Trying to set nullptr to distances field.");
    }
}

//------------------------------------------------------------------------------

bool getDistanceVisibility(const data::Image& _image)
{
    const auto visibility = _image.getField<Boolean>(std::string(id::distance_visibility));

    if(visibility)
    {
        return visibility->value();
    }

    // default value is true.
    return true;
}

//------------------------------------------------------------------------------

void setDistanceVisibility(data::Image& _image, bool _visibility)
{
    _image.setField(std::string(id::distance_visibility), data::Boolean::New(_visibility));
}

//------------------------------------------------------------------------------

bool getLandmarksVisibility(const data::Image& _image)
{
    const auto visibility = _image.getField<Boolean>(std::string(id::landmarks_visibility));

    if(visibility)
    {
        return visibility->value();
    }

    // default value is true.
    return true;
}

//------------------------------------------------------------------------------

void setLandmarksVisibility(data::Image& _image, bool _visibility)
{
    _image.setField(std::string(id::landmarks_visibility), data::Boolean::New(_visibility));
}

//------------------------------------------------------------------------------

data::Composite::sptr getTransferFunction(const data::Image& _image)
{
    const auto field = _image.getField(std::string(id::transferFunction));
    if(field)
    {
        const auto composite = _image.getField<data::Composite>(std::string(id::transferFunction));
        if(composite)
        {
            return composite;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void setTransferFunction(data::Image& _image, const data::Composite::sptr& _cmp)
{
    _image.setField(std::string(id::transferFunction), _cmp);
}

//------------------------------------------------------------------------------

} //namespace MedicalImage

} //namespace helper

} //namespace sight::data
