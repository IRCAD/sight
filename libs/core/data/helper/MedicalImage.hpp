/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include "data/config.hpp"

#include <core/tools/Dispatcher.hpp>
#include <core/tools/NumericRoundCast.hxx>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <data/Composite.hpp>
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/PointList.hpp>
#include <data/Vector.hpp>
namespace sight::data
{

namespace helper
{

/**
 * @brief   Helpers for medical image.
 */
namespace MedicalImage
{

typedef enum
{
    /// Directions.
    X_AXIS = 0,
    Y_AXIS,
    Z_AXIS,
    /// Planar definitions.
    SAGITTAL = X_AXIS,
    FRONTAL  = Y_AXIS,
    AXIAL    = Z_AXIS
} orientation_t;

/**
 * @brief       Check if the image has a landmark field.
 *
 * If the image hasn't landmark field, it is added and the method return true.
 *
 * @param[in]   _pImg   image which must contains the field.
 * @return      true if the field is added.
 */
DATA_API bool checkLandmarks(data::Image::sptr _pImg);

/**
 * @brief       Check if the image is valid.
 *
 * The image size must be > 1 for each dimension.
 *
 * @param[in]   _pImg   image checked.
 * @return      true if the image is valid.
 */
DATA_API bool checkImageValidity(data::Image::csptr _pImg);

/**
 * @brief       Check if the image is valid.
 *
 * The image size must be > 1 for each dimension.
 *
 * @param[in]   _pImg   image checked.
 * @return      true if the image is valid.
 */
DATA_API bool checkImageValidity(const data::Image& _image);

/**
 * @brief       Check if the image has slice index fields.
 *
 * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
 *
 * @param       _pImg   image which must contains the field.
 * @return      true if the fields are added.
 */
DATA_API bool checkImageSliceIndex(data::Image::sptr _pImg);

/**
 * @brief Checks if buf contains zero, from begin to begin + len
 *
 * @param buf data::Image::BufferType* begin of the buffer.
 * @param len unsigned int length, as begin+len.
 * @return boolean, true if null, false otherwise.
 */
DATA_API bool isBufNull(const data::Image::BufferType* buf, const unsigned int len);

/**
 * @brief Return a buffer of image type's size, containing 'value' casted to image data type
 * @param[in] image : reference image
 * @param[in] value : value to map
 */
template<typename T>
SPTR(data::Image::BufferType) getPixelInImageSpace(data::Image::sptr image, T & value);

/**
 * @brief Return minimum and maximum values contained in image. If image
 * min or max value is out of MINMAXTYPE range, they are clamped to
 * MINMAXTYPE capacity
 * @param[in] _img : image
 * @param[out] _min : minimum value
 * @param[out] _max : maximum value
 */
template<typename MINMAXTYPE>
void getMinMax(const data::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max);

/**
 * @brief Check if the image has a transfer function pool
 *
 * Create transfer function pool if the pool is not present.
 *
 * If the image has not transfer functions, the pool is
 * created and a grey level transfer function is added.
 */
DATA_API bool checkTransferFunctionPool(const data::Image::sptr& _img);

// Getter/Setter for specific image fields

/**
 * @brief Helper function to get slice count of an medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference.
 * @param _orientation : desired orientation @see orientation enum.
 * @return std::int64_t slice count.
 */
DATA_API std::int64_t getSliceCount(const data::Image& _image, const orientation_t& _orientation);

/**
 * @brief Helper function to set slice count of an medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference.
 * @param _orientation : desired orientation @see orientation enum.
 * @param _sliceCount : the required slice cound as std::int64_t.
 */
DATA_API void setSliceCount(
    data::Image& _image,
    const orientation_t& _orientation,
    std::int64_t _sliceCount
);

/**
 * @brief Helper function to get current slice index on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation
 * @return the current index as a std::int64_t.
 */
DATA_API std::int64_t getSliceIndex(const data::Image& _image, const orientation_t& _orientation);

/**
 * @brief Helper function to set current slice index on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation.
 * @param _sliceCount : current slice index to set as std::int64_t.
 */
DATA_API void setSliceIndex(
    data::Image& _image,
    const orientation_t& _orientation,
    std::int64_t _sliceCount
);

/**
 * @brief Helper function to get landmarks of a medical image.
 *
 * @param _image : input image reference.
 * @return a data::PointList::sptr containing landmarks, pointer can be null.
 */
DATA_API data::PointList::sptr getLandmarks(const data::Image& _image);

/**
 * @brief Helper function to set landmarks in a medical image.
 *
 * @param _image : input image reference.
 * @param _landmarks : landmarks pointer as data::PointList::sptr.
 */
DATA_API void setLandmarks(data::Image& _image, const data::PointList::sptr& _landmarks);

/**
 * @brief Helper function to get distances vector of a medical Image.
 *
 * @param _image : input image reference.
 * @return a data::Vector::sptr containing distances, pointer can be null.
 */
DATA_API data::Vector::sptr getDistances(const data::Image& _image);

/**
 * @brief Helper function to set distances in a medical image as a data::Vector::sptr.
 *
 * @param _image : input image reference.
 * @param _distances : data::Vector::sptr containing distances.
 */
DATA_API void setDistances(data::Image& _image, const data::Vector::sptr& _distances);

/**
 * @brief Helper function to get distance visibility on a medical image.
 *
 * @param _image : input image reference.
 * @return boolean value (visible or not).
 */
DATA_API bool getDistanceVisibility(const data::Image& _image);

/**
 * @brief Helper function to set distance visibility on a medical image.
 *
 * @param _image : input image reference.
 * @param _visibility : boolean to represents if distance is visible or not.
 */
DATA_API void setDistanceVisibility(data::Image& _image, bool _visibility);

/**
 * @brief Helper function to get landmarks visibility on a medical image.
 *
 * @param _image : input image reference.
 * @return boolean value (visible or not).
 */
DATA_API bool getLandmarksVisibility(const data::Image& _image);

/**
 * @brief Helper function to set landmarks visibility on a medical image.
 *
 * @param _image : input image reference.
 * @param _visibility : boolean to represents if landmarks are visible or not.
 */
DATA_API void setLandmarksVisibility(data::Image& _image, bool _visibility);

/**
 * @brief Helper to get comment on medical image.
 *
 * @param _image : input image reference.
 * @return std::string the comment, can be empty.
 */
DATA_API std::string getComment(const data::Image& _image);

/**
 * @brief Helper function to set comment on medical image.
 *
 * @param _image : input image reference.
 * @param _comment : std::string comment to add.
 */
DATA_API void setComment(data::Image& _image, const std::string& _comment);

/**
 * @brief Helper function to get label on a medical image.
 *
 * @param _image : input image reference.
 * @return std::string the label, can be empty.
 */
DATA_API std::string getLabel(const data::Image& _image);

/**
 * @brief Helper function to set label on medical image.
 *
 * @param _image : input image reference.
 * @param _label : std::string the label.
 */
DATA_API void setLabel(data::Image& _image, const std::string& _label);

/**
 * @brief Helper function to get transfer function from medical image.
 *
 * @param _image : input image reference.
 * @return data::Composite::sptr containing transfer function, can be null.
 */
DATA_API data::Composite::sptr getTransferFunction(const data::Image& _image);

/**
 * @brief Helper function to set transfer function on medical image.
 *
 * @param _image : input image reference.
 * @param _cmp : data::Composite::sptr containing transfer function.
 * @return DATA_API
 */
DATA_API void setTransferFunction(data::Image& _image, const data::Composite::sptr& _cmp);

// ------------------------------------------------------------------------------

template<typename VALUE>
class PixelCastAndSetFunctor
{
public:

    class Param
    {
    public:

        typedef VALUE ValueType;
        typedef SPTR(data::Image::BufferType) BufferTypeSptr;

        Param(ValueType& v) :
            value(v)
        {
        }

        const ValueType& value;
        BufferTypeSptr res;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(Param& param)
    {
        unsigned char imageTypeSize = sizeof(IMAGE);

        IMAGE val = core::tools::numericRoundCast<IMAGE>(param.value);

        data::Image::BufferType* buf = reinterpret_cast<data::Image::BufferType*>(&val);

        SPTR(data::Image::BufferType) res(new data::Image::BufferType[imageTypeSize]);
        std::copy(buf, buf + imageTypeSize, res.get());
        param.res = res;
    }
};

// ------------------------------------------------------------------------------

template<typename VALUE, typename INT_INDEX>
class CastAndSetFunctor
{
public:

    class Param
    {
    public:

        typedef VALUE ValueType;
        typedef INT_INDEX PointType;

        Param(PointType& p, ValueType& v) :
            value(v),
            point(p)
        {
        }

        data::Image::sptr image;
        const ValueType& value;
        const PointType& point;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(Param& param)
    {
        IMAGE* buffer                 = static_cast<IMAGE*>(param.image->getBuffer());
        const INT_INDEX& p            = param.point;
        const data::Image::Size& size = param.image->getSize();
        const int& sx                 = size[0];
        const int& sy                 = size[1];
        const int& offset             = p[0] + sx * p[1] + p[2] * sx * sy;
        *(buffer + offset) = core::tools::numericRoundCast<IMAGE>(param.value);
    }
};

// ------------------------------------------------------------------------------

template<typename T>
SPTR(data::Image::BufferType) getPixelInImageSpace(
    data::Image::sptr image,
    T & value
)
{
    typename PixelCastAndSetFunctor<T>::Param param(value);

    core::tools::Type type = image->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, PixelCastAndSetFunctor<T> >::invoke(type, param);
    return param.res;
}

// ------------------------------------------------------------------------------

template<typename INT_INDEX>
class [[deprecated("sight 22.0")]] CastAndCheckFunctor
{
public:

    class Param
    {
    public:

        typedef INT_INDEX PointType;

        Param(PointType& p, bool& b) :
            point(p),
            isNull(b)
        {
        }

        data::Image::sptr image;
        const PointType& point;
        bool& isNull;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(Param& param)
    {
        const auto dumpLock = param.image->lock();
        IMAGE* buffer       = static_cast<IMAGE*>(param.image->getBuffer());
        const INT_INDEX& p  = param.point;
        const auto& size    = param.image->getSize();
        const int& sx       = size[0];
        const int& sy       = size[1];
        const int& offset   = p[0] + sx * p[1] + p[2] * sx * sy;
        param.isNull = (*(buffer + offset) == 0);
    }
};

// ------------------------------------------------------------------------------

template<typename T>
class MinMaxFunctor
{
public:

    class Param
    {
    public:

        Param(data::Image::csptr _img, T& _min, T& _max) :
            image(_img),
            min(_min),
            max(_max)
        {
        }

        data::Image::csptr image;
        T& min;
        T& max;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(Param& param)
    {
        const data::Image::csptr image = param.image;
        const auto dumpLock            = image->lock();

        auto itr       = image->begin<IMAGE>();
        const auto end = image->end<IMAGE>();

        T& min = param.min;
        T& max = param.max;

        typedef std::numeric_limits<IMAGE> ImgLimits;
        IMAGE imin = ImgLimits::max();
        IMAGE imax = ImgLimits::lowest();

        IMAGE currentVoxel;

        for( ; itr != end ; ++itr)
        {
            currentVoxel = *itr;

            if(currentVoxel < imin)
            {
                imin = currentVoxel;
            }
            else if(currentVoxel > imax)
            {
                imax = currentVoxel;
            }
        }

        typedef std::numeric_limits<T> TLimits;
        T minT = TLimits::lowest();
        T maxT = TLimits::max();

        min = (static_cast<T>(imin) < minT) ? minT : static_cast<T>(imin);
        max = (static_cast<T>(imax) > maxT) ? maxT : static_cast<T>(imax);
    }
};

// ------------------------------------------------------------------------------

template<typename MINMAXTYPE>
void getMinMax(const data::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max)
{
    typename MinMaxFunctor<MINMAXTYPE>::Param param(_img, _min, _max);

    core::tools::Type type = _img->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, MinMaxFunctor<MINMAXTYPE> >::invoke(type, param);
}

} // namespace MedicalImage

} // namespace helper

} // namespace sight::data
