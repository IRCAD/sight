/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_FIELDHELPER_MEDICALIMAGEHELPERS_HPP_
#define _FWCOMED_FIELDHELPER_MEDICALIMAGEHELPERS_HPP_

#include <utility> // std::pair
#include <numeric>


#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Point.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/NumericRoundCast.hxx>
#include <fwServices/IService.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{
namespace fieldHelper
{


template <class T> struct bitwise_or : std::binary_function <T,T,T>
{
  T operator() (const T& x, const T& y) const
    {return x|y;}
};


/**
 * @class   MedicalImageHelpers
 * @brief   This class contains helpers for medical image fields.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class FWCOMED_CLASS_API MedicalImageHelpers
{
public :

    /**
     * @brief       Check if the image has a landmark field.
     *
     * If the image hasn't landmark field, it is added and the method return true.
     *
     * @param[in]   _pImg   image which must contains the field.
     * @return      true if the field is added.
     */
    FWCOMED_API static bool checkLandmarks( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image is valid.
     *
     * The image size must be > 1 for each dimension.
     *
     * @param[in]   _pImg   image checked.
     * @return      true if the image is valid.
     */
    FWCOMED_API static bool checkImageValidity( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image has slice index fields.
     *
     * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
     *
     * @param       _pImg   image which must contains the field.
     * @return      true if the fields are added.
     */
    FWCOMED_API static bool checkImageSliceIndex( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Get the X,Y,Z slice position.
     * @param[in]   _pImg   current image.
     * @return      X,Y,Z  slice position of the current image if exist field exist
     *              else install field and return (imgSizeX/2,imgSizeY/2,imgSizeZ/2).
     */
    FWCOMED_API static ::fwData::Point::sptr   getImageSliceIndices( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Check if the image has a comment field.
     *
     * If the image has no comment field, this is added with default name ("Original image").
     *
     * @param[in]   _pImg image which contains the comment field.
     * @return      true if the comment field is added.
     */
    FWCOMED_API static bool checkComment( ::fwData::Image::sptr _pImg );

    /**
     * @brief       Set a label in a field of the image.
     *
     * For each image in the patient the label is incremented (I0, I1, I2,...).
     *
     * @param[in]   pPatient    patient which contains the image.
     * @param[in]   pImage      image to set the label.
     */
    FWCOMED_API static void setImageLabel( ::fwData::Patient::sptr pPatient, ::fwData::Image::sptr pImage);

    /**
     * @brief       Initialized an image to 0 from image source (same field, pixel type, buffer size...).
     *
     *
     * @param[in]   imgSrc             image which gives information (field, pixel type, buffer size...).
     * @param[in]   imgToInitialize    image to initialized (if not null).
     *
     * @return      Returns initialized image.
     */
    FWCOMED_API static ::fwData::Image::sptr initialize( ::fwData::Image::sptr imgSrc, ::fwData::Image::sptr imgToInitialize = ::fwData::Image::sptr());

    /**
     * @brief  Merge PatientDB:
     *         Check if firstname, name, sex, ... are the same
     *
     * @param[in]    _patientDBFrom    PatientDB source.
     * @param[inout] _patientDBTo      PatientDB destination (data are merged in this PatientDB).
     * @param[in]    _msgSender        Service source to notify modifications
     */
    FWCOMED_API static void mergePatientDBInfo( ::fwData::PatientDB::sptr _patientDBFrom, ::fwData::PatientDB::sptr _patientDBTo, ::fwServices::IService::sptr _msgSender );

    /**
     * @brief       Return true if the pixel value is null.
     * @param[in] image : image containing the pixel
     * @param[in] point : the coordinate of the pixel in the image.
     */
    template < typename INT_INDEX>
    static bool isPixelNull(::fwData::Image::sptr image, INT_INDEX &point);

    FWCOMED_API static bool isBufNull(const ::fwData::Image::BufferType *buf, const unsigned int len);


    /**
     * @brief Set a pixel value.
     * @param[in] image : image containing the pixel
     * @param[in] point : coordinate of the pixel
     * @param[in] value : the pixel value
     */
    template < typename T , typename INT_INDEX>
    static void setPixel(::fwData::Image::sptr image, INT_INDEX &point, T &value);

    /**
     * @brief Set a pixel value.
     * @param[in] pImage : image containing the pixel
     * @param[in] point : coordinate of the pixel
     * @param[in] value : the pixel value
     */
    template < typename T >
    static void setPixel(::fwData::Image::sptr pImage, ::fwData::Point::sptr point, T &value);


    /**
     * @brief Return a buffer of image type's size, containing 'value' casted to image data type
     * @param[in] pImage : reference image
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
    static void getMinMax(const ::fwData::Image::sptr _img, MINMAXTYPE &_min, MINMAXTYPE &_max);

protected:

    /**
     * @brief Merge data from two identical patients.
     */
    FWCOMED_API static void mergeInformation(::fwData::Patient::sptr currentPatient, ::fwData::Patient::sptr importedPatient);

};


//------------------------------------------------------------------------------

template < typename VALUE >
class PixelCastAndSetFunctor
{
public:
    class Param
    {
        public:
        typedef VALUE ValueType;
        typedef SPTR( ::fwData::Image::BufferType ) BufferTypeSptr;

        Param(ValueType &v): value (v)
        {};

        const ValueType &value;
        BufferTypeSptr res;
    };

    template < typename IMAGE >
    void operator()( Param &param )
    {
        unsigned char imageTypeSize = sizeof(IMAGE);

        IMAGE val = ::fwTools::numericRoundCast<IMAGE>(param.value);

        ::fwData::Image::BufferType *buf = reinterpret_cast< ::fwData::Image::BufferType* > (&val);

        SPTR( ::fwData::Image::BufferType ) res ( new ::fwData::Image::BufferType[imageTypeSize] );
        std::copy(buf, buf+imageTypeSize, res.get());
        param.res = res;
    }

};


//------------------------------------------------------------------------------

template < typename VALUE, typename INT_INDEX >
class CastAndSetFunctor
{
public:
    class Param
    {
        public:
        typedef VALUE ValueType;
        typedef INT_INDEX PointType;

            Param(PointType &p, ValueType &v):
                value (v), point(p)
            {};

        ::fwData::Image::sptr image;
        const ValueType &value;
        const PointType &point;
    };

    template < typename IMAGE >
    void operator()( Param &param )
    {
        IMAGE * buffer = static_cast < IMAGE* > (param.image->getBuffer());
        const INT_INDEX &p = param.point;
        const ::fwData::Image::SizeType &size = param.image->getSize();
        const int &sx = size[0];
        const int &sy = size[1];
        const int &offset = p[0] + sx*p[1] + p[2]*sx*sy;
        *(buffer+offset) = ::fwTools::numericRoundCast<IMAGE>(param.value);
    }

};



//------------------------------------------------------------------------------

template < typename T >
void MedicalImageHelpers::setPixel(::fwData::Image::sptr image, ::fwData::Point::sptr point, T &value)
{
    setPixel(image, point->getCoord(), value);
}


//------------------------------------------------------------------------------

template < typename T , typename INT_INDEX>
void MedicalImageHelpers::setPixel(::fwData::Image::sptr image, INT_INDEX &point, T &value)
{
    typename CastAndSetFunctor<T,INT_INDEX>::Param param(point, value);
    param.image = image;

    ::fwTools::DynamicType type = image->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , CastAndSetFunctor<T, INT_INDEX> >::invoke( type, param );
}



//------------------------------------------------------------------------------

template < typename T >
SPTR( ::fwData::Image::BufferType ) MedicalImageHelpers::getPixelBufferInImageSpace(::fwData::Image::sptr image, T &value)
{
    typename PixelCastAndSetFunctor<T>::Param param(value);

    ::fwTools::DynamicType type = image->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , PixelCastAndSetFunctor<T> >::invoke( type, param );
    return param.res;
}



//------------------------------------------------------------------------------

template < typename INT_INDEX >
class CastAndCheckFunctor
{
public:
    class Param
    {
        public:
        typedef INT_INDEX PointType;

        Param(PointType &p, bool &b):
            point(p), isNull(b)
        {};

        ::fwData::Image::sptr image;
        const PointType &point;
        bool &isNull;
    };

    template < typename IMAGE >
    void operator()( Param &param )
    {
        IMAGE * buffer = static_cast < IMAGE* > (param.image->getBuffer());
        const INT_INDEX &p = param.point;
        const std::vector<boost::int32_t> &size = param.image->getCRefSize();
        const int &sx = size[0];
        const int &sy = size[1];
        const int &offset = p[0] + sx*p[1] + p[2]*sx*sy;
        param.isNull = (*(buffer+offset) == 0);
    }

};

//------------------------------------------------------------------------------

template < typename INT_INDEX>
bool MedicalImageHelpers::isPixelNull(::fwData::Image::sptr image, INT_INDEX &point)
{
    const unsigned char imageTypeSize = image->getPixelType().sizeOf();
    ::fwData::Image::BufferType *buf = static_cast< ::fwData::Image::BufferType *> (image->getPixelBuffer(point[0], point[1], point[2]));

    return isBufNull(buf, imageTypeSize);
}

//------------------------------------------------------------------------------

template < typename T >
class MinMaxFunctor
{
public:
    class Param
    {
        public:

        Param(::fwData::Image::sptr _img, T &_min, T &_max):
            image(_img), min(_min), max(_max)
        {};

        ::fwData::Image::sptr image;
        T &min;
        T &max;
    };

    template < typename IMAGE >
    void operator()( Param &param )
    {
        IMAGE * buffer = static_cast < IMAGE* > (param.image->getBuffer());
        const ::fwData::Image::SizeType &size = param.image->getSize();
        ::fwData::Image::SizeType::value_type len = size[0]*size[1]*size[2];

        T &min = param.min;
        T &max = param.max;

        typedef std::numeric_limits<IMAGE> ImgLimits;
        IMAGE imin = ImgLimits::max();
        IMAGE imax = (ImgLimits::is_integer) ? ImgLimits::min() : - ImgLimits::max();

        IMAGE * bufEnd = buffer + len;
        IMAGE currentVoxel;

        for (IMAGE * voxel = buffer; voxel < bufEnd; ++voxel )
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
        T minT =  (TLimits::is_integer) ? TLimits::min() : - TLimits::max();
        T maxT = TLimits::max();

        min = ( imin < minT ) ? minT : static_cast< T > (imin) ;
        max = ( imax > maxT ) ? maxT : static_cast< T > (imax) ;

    }

};


template < typename MINMAXTYPE >
void MedicalImageHelpers::getMinMax(const ::fwData::Image::sptr _img, MINMAXTYPE &_min, MINMAXTYPE &_max)
{
    typename MinMaxFunctor<MINMAXTYPE>::Param param(_img, _min, _max);

    ::fwTools::DynamicType type = _img->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , MinMaxFunctor<MINMAXTYPE> >::invoke( type, param );
}



} // fieldHelper
} // fwComEd


#endif // _FWCOMED_FIELDHELPER_MEDICALIMAGEHELPERS_HPP_

