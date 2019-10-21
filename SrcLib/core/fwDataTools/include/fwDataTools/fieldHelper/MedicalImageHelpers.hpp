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

#pragma once

#include "fwDataTools/config.hpp"
#include "fwDataTools/helper/Image.hpp"
#include "fwDataTools/helper/ImageGetter.hpp"

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/NumericRoundCast.hxx>

#include <numeric>
#include <utility> // std::pair

namespace fwDataTools
{
namespace fieldHelper
{

/**
 * @brief   This class contains helpers for medical image fields.
 */
class FWDATATOOLS_CLASS_API MedicalImageHelpers
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
    FWDATATOOLS_API static bool checkLandmarks( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image is valid.
     *
     * The image size must be > 1 for each dimension.
     *
     * @param[in]   _pImg   image checked.
     * @return      true if the image is valid.
     */
    FWDATATOOLS_API static bool checkImageValidity( ::fwData::Image::csptr _pImg );

    /**
     * @brief       Check if the image has slice index fields.
     *
     * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
     *
     * @param       _pImg   image which must contains the field.
     * @return      true if the fields are added.
     */
    FWDATATOOLS_API static bool checkImageSliceIndex( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Get the X,Y,Z slice position.
     * @param[in]   _pImg   current image.
     * @return      X,Y,Z  slice position of the current image if exist field exist
     *              else install field and return (imgSizeX/2,imgSizeY/2,imgSizeZ/2).
     */
    FWDATATOOLS_API static ::fwData::Point::sptr   getImageSliceIndices( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image has a comment field.
     *
     * If the image has no comment field, this is added with default name ("Original image").
     *
     * @param[in]   _pImg image which contains the comment field.
     * @return      true if the comment field is added.
     */
    FWDATATOOLS_API static bool checkComment( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Initialized an image to 0 from image source (same field, pixel type, buffer size...).
     *
     *
     * @param[in]   imgSrc             image which gives information (field, pixel type, buffer size...).
     * @param[in]   imgToInitialize    image to initialized (if not null).
     *
     * @return      Returns initialized image.
     * @deprecated  This method is not longer supported
     */
    FWDATATOOLS_API static ::fwData::Image::sptr initialize( ::fwData::Image::sptr imgSrc,
                                                             ::fwData::Image::sptr imgToInitialize =
                                                                 ::fwData::Image::sptr());

    /**
     * @brief       Return true if the pixel value is null.
     * @param[in] image : image containing the pixel
     * @param[in] point : the coordinate of the pixel in the image.
     */
    template < typename INT_INDEX>
    static bool isPixelNull(::fwData::Image::sptr image, INT_INDEX& point);

    FWDATATOOLS_API static bool isBufNull(const ::fwData::Image::BufferType* buf, const unsigned int len);

    /**
     * @brief Set a pixel value.
     * @param[in] image : image containing the pixel
     * @param[in] point : coordinate of the pixel
     * @param[in] value : the pixel value
     */
    template < typename T, typename INT_INDEX>
    static void setPixel(::fwData::Image::sptr image, INT_INDEX& point, T& value);

    /**
     * @brief Set a pixel value.
     * @param[in] pImage : image containing the pixel
     * @param[in] point : coordinate of the pixel
     * @param[in] value : the pixel value
     */
    template < typename T >
    static void setPixel(::fwData::Image::sptr pImage, ::fwData::Point::sptr point, T& value);

    /**
     * @brief Return a buffer of image type's size, containing 'value' casted to image data type
     * @param[in] image : reference image
     * @param[in] value : value to map
     */
    template < typename T >
    static SPTR( ::fwData::Image::BufferType ) getPixelBufferInImageSpace(::fwData::Image::sptr image, T &value);

    /**
     * @brief Return minimum and maximum values contained in image. If image
     * min or max value is out of MINMAXTYPE range, they are clamped to
     * MINMAXTYPE capacity
     * @param[in] _img : image
     * @param[out] _min : minimum value
     * @param[out] _max : maximum value
     */
    template < typename MINMAXTYPE >
    static void getMinMax(const ::fwData::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max);

    /**
     * @brief Check if the image has a transfer function pool
     *
     * Create transfer function pool if the pool is not present.
     *
     * If the image has not transfer functions, the pool is
     * created and a grey level transfer function is added.
     */
    FWDATATOOLS_API bool checkTransferFunctionPool(const ::fwData::Image::sptr& _img);
};

// ------------------------------------------------------------------------------

template < typename VALUE >
class PixelCastAndSetFunctor
{
public:
    class Param
    {
    public:
        typedef VALUE ValueType;
        typedef SPTR ( ::fwData::Image::BufferType ) BufferTypeSptr;

        Param(ValueType& v) :
            value(v)
        {
        }

        const ValueType& value;
        BufferTypeSptr res;
    };

    // ------------------------------------------------------------------------------

    template < typename IMAGE >
    void operator()( Param& param )
    {
        unsigned char imageTypeSize = sizeof(IMAGE);

        IMAGE val = ::fwTools::numericRoundCast<IMAGE>(param.value);

        ::fwData::Image::BufferType* buf = reinterpret_cast< ::fwData::Image::BufferType* > (&val);

        SPTR( ::fwData::Image::BufferType ) res( new ::fwData::Image::BufferType[imageTypeSize] );
        std::copy(buf, buf+imageTypeSize, res.get());
        param.res = res;
    }

};

// ------------------------------------------------------------------------------

template < typename VALUE, typename INT_INDEX >
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

        ::fwData::Image::sptr image;
        const ValueType& value;
        const PointType& point;
    };

    // ------------------------------------------------------------------------------

    template < typename IMAGE >
    void operator()( Param& param )
    {
        ::fwDataTools::helper::Image imagehelper(param.image);
        IMAGE* buffer                         = static_cast < IMAGE* > (imagehelper.getBuffer());
        const INT_INDEX& p                    = param.point;
        const ::fwData::Image::SizeType& size = param.image->getSize();
        const int& sx                         = size[0];
        const int& sy                         = size[1];
        const int& offset                     = p[0] + sx*p[1] + p[2]*sx*sy;
        *(buffer+offset) = ::fwTools::numericRoundCast<IMAGE>(param.value);
    }

};

// ------------------------------------------------------------------------------

template < typename T >
void MedicalImageHelpers::setPixel(::fwData::Image::sptr image, ::fwData::Point::sptr point, T& value)
{
    setPixel(image, point->getCoord(), value);
}

// ------------------------------------------------------------------------------

template < typename T, typename INT_INDEX>
void MedicalImageHelpers::setPixel(::fwData::Image::sptr image, INT_INDEX& point, T& value)
{
    typename CastAndSetFunctor<T, INT_INDEX>::Param param(point, value);
    param.image = image;

    ::fwTools::DynamicType type = image->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, CastAndSetFunctor<T, INT_INDEX> >::invoke( type, param );
}

// ------------------------------------------------------------------------------

template < typename T >
SPTR( ::fwData::Image::BufferType ) MedicalImageHelpers::getPixelBufferInImageSpace(::fwData::Image::sptr image,
                                                                                    T &value)
{
    typename PixelCastAndSetFunctor<T>::Param param(value);

    ::fwTools::DynamicType type = image->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, PixelCastAndSetFunctor<T> >::invoke( type, param );
    return param.res;
}

// ------------------------------------------------------------------------------

template < typename INT_INDEX >
class CastAndCheckFunctor
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

        ::fwData::Image::sptr image;
        const PointType& point;
        bool& isNull;
    };

    // ------------------------------------------------------------------------------

    template < typename IMAGE >
    void operator()( Param& param )
    {
        ::fwDataTools::helper::Image imagehelper(param.image);
        IMAGE* buffer                         = static_cast < IMAGE* > (imagehelper.getBuffer());
        const INT_INDEX& p                    = param.point;
        const std::vector<std::int32_t>& size = param.image->getSize();
        const int& sx                         = size[0];
        const int& sy                         = size[1];
        const int& offset                     = p[0] + sx*p[1] + p[2]*sx*sy;
        param.isNull = (*(buffer+offset) == 0);
    }

};

// ------------------------------------------------------------------------------

template < typename INT_INDEX>
bool MedicalImageHelpers::isPixelNull(::fwData::Image::sptr image, INT_INDEX& point)
{
    ::fwDataTools::helper::Image imageLock( image );
    const unsigned char imageTypeSize = image->getPixelType().sizeOf();
    ::fwData::Image::BufferType* buf =
        static_cast< ::fwData::Image::BufferType*> (imageLock.getPixelBuffer(point[0], point[1], point[2]));

    return isBufNull(buf, imageTypeSize);
}

// ------------------------------------------------------------------------------

template < typename T >
class MinMaxFunctor
{
public:
    class Param
    {
    public:

        Param(::fwData::Image::csptr _img, T& _min, T& _max) :
            image(_img),
            min(_min),
            max(_max)
        {
        }

        ::fwData::Image::csptr image;
        T& min;
        T& max;
    };

    // ------------------------------------------------------------------------------

    template < typename IMAGE >
    void operator()( Param& param )
    {
        ::fwDataTools::helper::ImageGetter imageLock( param.image );
        IMAGE* buffer                         = static_cast < IMAGE* > (imageLock.getBuffer());
        const ::fwData::Image::SizeType& size = param.image->getSize();
        ::fwData::Image::SizeType::value_type len = static_cast< ::fwData::Image::SizeType::value_type >(
            std::accumulate(size.begin(), size.end(), 1, std::multiplies< ::fwData::Image::SizeType::value_type >()));
        T& min = param.min;
        T& max = param.max;

        typedef std::numeric_limits<IMAGE> ImgLimits;
        IMAGE imin = ImgLimits::max();
        IMAGE imax = ImgLimits::lowest();

        IMAGE* bufEnd = buffer + len;
        IMAGE currentVoxel;

        for (IMAGE* voxel = buffer; voxel < bufEnd; ++voxel )
        {
            currentVoxel = *voxel;

            if ( currentVoxel < imin )
            {
                imin = currentVoxel;
            }
            else if (currentVoxel > imax)
            {
                imax = currentVoxel;
            }
        }

        typedef std::numeric_limits<T> TLimits;
        T minT = TLimits::lowest();
        T maxT = TLimits::max();

        min = ( imin < minT ) ? minT : static_cast< T > (imin);
        max = ( imax > maxT ) ? maxT : static_cast< T > (imax);

    }

};

// ------------------------------------------------------------------------------

template < typename MINMAXTYPE >
void MedicalImageHelpers::getMinMax(const ::fwData::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max)
{
    typename MinMaxFunctor<MINMAXTYPE>::Param param(_img, _min, _max);

    ::fwTools::DynamicType type = _img->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, MinMaxFunctor<MINMAXTYPE> >::invoke( type, param );
}

} // fieldHelper
} // fwDataTools
