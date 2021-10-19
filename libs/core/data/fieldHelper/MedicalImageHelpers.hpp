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

#pragma once

#include "data/config.hpp"
#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Point.hpp>

#include <core/tools/Dispatcher.hpp>
#include <core/tools/NumericRoundCast.hxx>
#include <core/tools/TypeKeyTypeMapping.hpp>

#include <numeric>
#include <utility> // std::pair

namespace sight::data
{

namespace fieldHelper
{

/**
 * @brief   This class contains helpers for medical image fields.
 */
class DATA_CLASS_API MedicalImageHelpers
{
public:

    /**
     * @brief       Check if the image has a landmark field.
     *
     * If the image hasn't landmark field, it is added and the method return true.
     *
     * @param[in]   _pImg   image which must contains the field.
     * @return      true if the field is added.
     */
    DATA_API static bool checkLandmarks(data::Image::sptr _pImg);

    /**
     * @brief       Check if the image is valid.
     *
     * The image size must be > 1 for each dimension.
     *
     * @param[in]   _pImg   image checked.
     * @return      true if the image is valid.
     */
    DATA_API static bool checkImageValidity(data::Image::csptr _pImg);

    /**
     * @brief       Check if the image is valid.
     *
     * The image size must be > 1 for each dimension.
     *
     * @param[in]   _pImg   image checked.
     * @return      true if the image is valid.
     */
    DATA_API static bool checkImageValidity(const data::Image& _image);

    /**
     * @brief       Check if the image has slice index fields.
     *
     * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
     *
     * @param       _pImg   image which must contains the field.
     * @return      true if the fields are added.
     */
    DATA_API static bool checkImageSliceIndex(data::Image::sptr _pImg);

    /**
     * @brief       Get the X,Y,Z slice position.
     * @param[in]   _pImg   current image.
     * @return      X,Y,Z  slice position of the current image if exist field exist
     *              else install field and return (imgSizeX/2,imgSizeY/2,imgSizeZ/2).
     */
    DATA_API static data::Point::sptr getImageSliceIndices(data::Image::sptr _pImg);

    /**
     * @brief       Check if the image has a comment field.
     *
     * If the image has no comment field, this is added with default name ("Original image").
     *
     * @param[in]   _pImg image which contains the comment field.
     * @return      true if the comment field is added.
     */
    DATA_API static bool checkComment(data::Image::sptr _pImg);

    /**
     * @brief       Return true if the pixel value is null.
     * @param[in] image : image containing the pixel
     * @param[in] point : the coordinate of the pixel in the image.
     */
    template<typename INT_INDEX>
    static bool isPixelNull(data::Image::sptr image, INT_INDEX& point);

    DATA_API static bool isBufNull(const data::Image::BufferType* buf, const unsigned int len);

    /**
     * @brief Set a pixel value.
     * @param[in] image : image containing the pixel
     * @param[in] point : coordinate of the pixel
     * @param[in] value : the pixel value
     */
    template<typename T, typename INT_INDEX>
    static void setPixel(data::Image::sptr image, INT_INDEX& point, T& value);

    /**
     * @brief Set a pixel value.
     * @param[in] pImage : image containing the pixel
     * @param[in] point : coordinate of the pixel
     * @param[in] value : the pixel value
     */
    template<typename T>
    static void setPixel(data::Image::sptr pImage, data::Point::sptr point, T& value);

    /**
     * @brief Return a buffer of image type's size, containing 'value' casted to image data type
     * @param[in] image : reference image
     * @param[in] value : value to map
     */
    template<typename T>
    static SPTR(data::Image::BufferType) getPixelInImageSpace(data::Image::sptr image, T & value);

    /**
     * @brief Return minimum and maximum values contained in image. If image
     * min or max value is out of MINMAXTYPE range, they are clamped to
     * MINMAXTYPE capacity
     * @param[in] _img : image
     * @param[out] _min : minimum value
     * @param[out] _max : maximum value
     */
    template<typename MINMAXTYPE>
    static void getMinMax(const data::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max);

    /**
     * @brief Check if the image has a transfer function pool
     *
     * Create transfer function pool if the pool is not present.
     *
     * If the image has not transfer functions, the pool is
     * created and a grey level transfer function is added.
     */
    DATA_API static bool checkTransferFunctionPool(const data::Image::sptr& _img);
};

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
void MedicalImageHelpers::setPixel(data::Image::sptr image, data::Point::sptr point, T& value)
{
    setPixel(image, point->getCoord(), value);
}

// ------------------------------------------------------------------------------

template<typename T, typename INT_INDEX>
void MedicalImageHelpers::setPixel(data::Image::sptr image, INT_INDEX& point, T& value)
{
    const auto dumpLock = image->lock();
    typename CastAndSetFunctor<T, INT_INDEX>::Param param(point, value);
    param.image = image;

    core::tools::Type type = image->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, CastAndSetFunctor<T, INT_INDEX> >::invoke(
        type,
        param
    );
}

// ------------------------------------------------------------------------------

template<typename T>
SPTR(data::Image::BufferType) MedicalImageHelpers::getPixelInImageSpace(
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

template<typename INT_INDEX>
bool MedicalImageHelpers::isPixelNull(data::Image::sptr image, INT_INDEX& point)
{
    const auto dumpLock               = image->lock();
    const unsigned char imageTypeSize = image->getType().sizeOf();
    data::Image::BufferType* buf      =
        static_cast<data::Image::BufferType*>(image->getPixel(point[0], point[1], point[2]));

    return isBufNull(buf, imageTypeSize);
}

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
void MedicalImageHelpers::getMinMax(const data::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max)
{
    typename MinMaxFunctor<MINMAXTYPE>::Param param(_img, _min, _max);

    core::tools::Type type = _img->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, MinMaxFunctor<MINMAXTYPE> >::invoke(type, param);
}

} // fieldHelper

} // sight::data
