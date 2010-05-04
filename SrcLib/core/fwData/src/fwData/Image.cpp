/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <numeric>
#include <functional>
#include <algorithm>
#include <sstream>

#include <fwCore/base.hpp>
#include <boost/lexical_cast.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>

#include "fwData/StandardBuffer.hpp"
#include "fwData/Image.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Image, ::fwData::Image );
namespace fwData
{

//------------------------------------------------------------------------------

boost::int32_t  imageSizeInBytes( const ::fwData::Image &image )
{
    SLM_ASSERT("Image size must be specified", !image.getCRefSize().empty() );
    SLM_ASSERT("Image must have a valid PixelType", image.getPixelType() != ::fwTools::DynamicType() );

    ::boost::int32_t  size = std::accumulate( image.getCRefSize().begin() ,  image.getCRefSize().end(), 1, std::multiplies<boost::int32_t> () );
    size  *= image.getPixelType().sizeOf();
    return size;
 }

/**
 * @brief return the pixel value of the image in given coordinates
 */
template < class T >
T getPixel( ::fwData::Image::csptr image, unsigned int x, unsigned int y, unsigned int z )
{
    std::vector<boost::int32_t> size = image->getSize();
    int offset = x + size[0]*y + z*size[0]*size[1];

    T * buffer = static_cast < T* > (image->getBuffer());

    return *(buffer+offset);
}

/**
 * @class StringGetter
 * @brief This class is use to convert a pixel value of an image in a string value
 */
class StringGetter
{

    public:

        /**
         * @class Parameter
         * @brief This class defines the parameter use to get a pixel in an image
         */
        class Parameter
        {
            public:
                Parameter(::fwData::Image::csptr _image, unsigned int _x, unsigned int _y, unsigned int _z)
                    : image(_image), x(_x), y(_y), z(_z) {};

                ::fwData::Image::csptr image;
                unsigned int x;
                unsigned int y;
                unsigned int z;
                std::string output;
        };

        template < class T >
            void operator()(Parameter &param)
            {

                T value = getPixel< T >(param.image, param.x, param.y, param.z);
                param.output = ::boost::lexical_cast < std::string , double> (value);
            }
};

std::string  getPixelAsString( ::fwData::Image::csptr image, unsigned int x, unsigned int y, unsigned int z )
{
    StringGetter::Parameter param(image, x,y,z);

    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, StringGetter>::invoke(image->getPixelType(), param);
    return param.output;
}

//------------------------------------------------------------------------------

Image::Image() :
m_ui8Dimension(3),
m_dtPixelType(),
//m_bManagesBuff(true),
m_vSpacing(m_ui8Dimension,1),
m_vOrigin(m_ui8Dimension,0),
m_vSize(m_ui8Dimension,0),
m_fsFilename(""),
m_dWindowCenter(0),
m_dWindowWidth(0),
m_dRescaleIntercept(0),
m_bufferDelegate( new StandardBuffer )
{
}

//------------------------------------------------------------------------------

Image::~Image() throw()
{
    SLM_TRACE("Image::~Image()");
//  if( m_bManagesBuff && getBuffer() != NULL )
//  {
//      char *buff=static_cast<char *>(getBuffer());
//      if( buff )
//      {
//          delete[] buff;
//      }
//      setBuffer ( NULL );
//  }
}

//-----------------------------------------------------------------------------

void Image::shallowCopy( Image::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );

    // Assign
    this->m_ui8Dimension        = _source->m_ui8Dimension;
    this->m_dtPixelType         = _source->m_dtPixelType;
    this->m_vSpacing            = _source->m_vSpacing;
    this->m_vOrigin             = _source->m_vOrigin;
    this->m_vSize               = _source->m_vSize;
    this->m_fsFilename          = _source->m_fsFilename;
    this->m_dWindowCenter       = _source->m_dWindowCenter;
    this->m_dWindowWidth        = _source->m_dWindowWidth;
    this->m_dRescaleIntercept   = _source->m_dRescaleIntercept;

    this->m_bufferDelegate  = _source->m_bufferDelegate;
}

//-----------------------------------------------------------------------------

void Image::deepCopy( Image::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    // Assign
    this->m_ui8Dimension        = _source->m_ui8Dimension;
    this->m_dtPixelType         = _source->m_dtPixelType;
    this->m_vSpacing            = _source->m_vSpacing;
    this->m_vOrigin             = _source->m_vOrigin;
    this->m_vSize               = _source->m_vSize;
    this->m_fsFilename          = _source->m_fsFilename;
    this->m_dWindowCenter       = _source->m_dWindowCenter;
    this->m_dWindowWidth        = _source->m_dWindowWidth;
    this->m_dRescaleIntercept   = _source->m_dRescaleIntercept;

    if(this->getPixelType() != ::fwTools::DynamicType() )
    {
        char * src = static_cast<char *>( _source->getBuffer() );
        ::boost::int32_t size = imageSizeInBytes( *_source );
        char * dest = new char[size];
        ::std::copy( src, src + size , dest );
        this->setBuffer( dest );
    }
    else
    {
        this->setBuffer( NULL );
    }
}

//------------------------------------------------------------------------------

//void Image::copy( ::fwData::Image::sptr _img )
//{
//  SLM_TRACE("copy()");
//
//  if( m_bManagesBuff && this->getBuffer() != NULL )
//  {
//      char * buff= static_cast< char * >( this->getBuffer() );
//      if( buff )
//      {
//          delete[] buff;
//      }
//      this->setBuffer( NULL );
//  }
//
//  this->m_ui8Dimension        = _img->m_ui8Dimension;
//  this->m_dtPixelType         = _img->m_dtPixelType;
//  this->m_vSpacing            = _img->m_vSpacing;
//  this->m_vOrigin             = _img->m_vOrigin;
//  this->m_vSize               = _img->m_vSize;
//  this->m_fsFilename          = _img->m_fsFilename;
//  this->m_dWindowCenter       = _img->m_dWindowCenter;
//  this->m_dWindowWidth        = _img->m_dWindowWidth;
//  this->m_dRescaleIntercept   = _img->m_dRescaleIntercept;
//
//
//  // this->setBuffer( _img->getBuffer() );
//  // this->m_bManagesBuff  = _img->m_bManagesBuff;
//  this->m_bManagesBuff = true;
//  char * src = static_cast<char *>( _img->getBuffer() );
//  ::boost::int32_t size = imageSizeInBytes( *_img );
//  char * dest = new char[size];
//  ::std::copy( src, src + size , dest );
//
//  this->setBuffer( dest );
//}

//------------------------------------------------------------------------------

//Image::sptr Image::clone() const
//{
//    SLM_TRACE("clone()");
//    ::fwData::Image::NewSptr pNewImage;
//
//    pNewImage->m_ui8Dimension       = this->m_ui8Dimension;
//    pNewImage->m_dtPixelType        = this->m_dtPixelType;
//    pNewImage->m_vSpacing           = this->m_vSpacing;
//    pNewImage->m_vOrigin            = this->m_vOrigin;
//    pNewImage->m_vSize              = this->m_vSize;
//    pNewImage->m_fsFilename         = this->m_fsFilename;
//    pNewImage->m_dWindowCenter      = this->m_dWindowCenter;
//    pNewImage->m_dWindowWidth       = this->m_dWindowWidth;
//    pNewImage->m_dRescaleIntercept  = this->m_dRescaleIntercept;
//
//    // pNewImage->setBuffthis->is->getBuffer() );
//    // pNewImage->m_bManagesBuff  = this->m_bManagesBuff;
//    // pNewImage->m_bManagesBuff = true;
//
//    char * src = static_cast<char *>( this->getBuffer() );
//    ::boost::int32_t size = imageSizeInBytes( *this );
//    char * dest = new char[size];
//    ::std::copy( src, src + size , dest );
//    pNewImage->setBuffer( dest );
//
//    return pNewImage;
//}

//------------------------------------------------------------------------------

//Image &Image::operator=(const Image &_img)
//{
//    SLM_INFO(" Image::operator= : the image buffer is not duplicate copy only the pointer.");
//
//    // Delete buffer if m_bManagesBuff
////  if( m_bManagesBuff && this->getBuffer() != NULL )
////  {
////      char * buff= static_cast< char * >( this->getBuffer() );
////      if( buff )
////      {
////          delete[] buff;
////      }
////      this->setBuffer( NULL );
////  }
//
//    // Assign
//    this->m_ui8Dimension        = _img.m_ui8Dimension;
//    this->m_dtPixelType         = _img.m_dtPixelType;
////  this->m_bManagesBuff        = _img.m_bManagesBuff;
//    this->m_vSpacing            = _img.m_vSpacing;
//    this->m_vOrigin             = _img.m_vOrigin;
//    this->m_vSize               = _img.m_vSize;
//    this->m_fsFilename          = _img.m_fsFilename;
//    this->m_dWindowCenter       = _img.m_dWindowCenter;
//    this->m_dWindowWidth        = _img.m_dWindowWidth;
//    this->m_dRescaleIntercept   = _img.m_dRescaleIntercept;
//
//    // this->setBuffer( _img.getBuffer() );
//    this->m_bufferDelegate  = _img.m_bufferDelegate;
//
//    return (*this);
//}

//------------------------------------------------------------------------------

void * Image::getBuffer() const
{
    return m_bufferDelegate->getBuffer();
}

//------------------------------------------------------------------------------

void Image::setBuffer(void *_buffer)
{
    m_bufferDelegate->setBuffer(_buffer);
}

//------------------------------------------------------------------------------

void Image::setBufferDelegate(IBufferDelegate::sptr bufferDelegate)
{
    m_bufferDelegate = bufferDelegate;
}

//------------------------------------------------------------------------------

IBufferDelegate::sptr  Image::getBufferDelegate()
{
    return m_bufferDelegate;
}


//------------------------------------------------------------------------------
//
//void Image::setDimension( const boost::uint8_t  _ui8Dimension )
//{
//  m_ui8Dimension = _ui8Dimension;
//}
//
////------------------------------------------------------------------------------
//
//void Image::setCRefDimension( const boost::uint8_t  & _ui8Dimension )
//{
//  m_ui8Dimension = _ui8Dimension;
//}
//
////------------------------------------------------------------------------------
//
//const boost::uint8_t  Image::getDimension() const
//{
//  return m_ui8Dimension;
//}
//
////------------------------------------------------------------------------------
//
//const boost::uint8_t  & Image::getCRefDimension() const
//{
//  return m_ui8Dimension;
//}
//
////------------------------------------------------------------------------------
//
//boost::uint8_t  & Image::getRefDimension()
//{
//  return m_ui8Dimension;
//}
//
////------------------------------------------------------------------------------
//
//void Image::setPixelType( const fwTools::DynamicType _dtPixelType )
//{
//  m_dtPixelType = _dtPixelType;
//}
//
////------------------------------------------------------------------------------
//
//void Image::setCRefPixelType( const fwTools::DynamicType & _dtPixelType )
//{
//  m_dtPixelType = _dtPixelType;
//}
//
////------------------------------------------------------------------------------
//
//const fwTools::DynamicType Image::getPixelType() const
//{
//  return m_dtPixelType;
//}
//
////------------------------------------------------------------------------------
//
//const fwTools::DynamicType & Image::getCRefPixelType() const
//{
//  return m_dtPixelType;
//}
//
////------------------------------------------------------------------------------
//
//fwTools::DynamicType & Image::getRefPixelType()
//{
//  return m_dtPixelType;
//}

//------------------------------------------------------------------------------

void Image::setManagesBuff( const bool _bManagesBuff )
{
//  m_bManagesBuff = _bManagesBuff;
    this->m_bufferDelegate->setManagesBuff( _bManagesBuff ) ;
}

//------------------------------------------------------------------------------

void Image::setCRefManagesBuff( const bool & _bManagesBuff )
{
//  m_bManagesBuff = _bManagesBuff;
    this->m_bufferDelegate->setCRefManagesBuff( _bManagesBuff ) ;
}

//------------------------------------------------------------------------------

const bool Image::getManagesBuff() const
{
//  return m_bManagesBuff;
    return this->m_bufferDelegate->getManagesBuff() ;
}

//------------------------------------------------------------------------------

const bool & Image::getCRefManagesBuff() const
{
//  return m_bManagesBuff;
    return this->m_bufferDelegate->getCRefManagesBuff() ;
}

//------------------------------------------------------------------------------

bool & Image::getRefManagesBuff()
{
//  return m_bManagesBuff;
    return this->m_bufferDelegate->getRefManagesBuff() ;
}

//------------------------------------------------------------------------------

} // namespace fwData
