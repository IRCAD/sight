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

::boost::int32_t  imageSizeInBytes( const ::fwData::Image &image )
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("Image size must be specified", !image.getCRefSize().empty() );
    SLM_ASSERT("Image must have a valid PixelType", image.getPixelType() != ::fwTools::DynamicType() );

    ::boost::int32_t  size = std::accumulate( image.getCRefSize().begin() ,  image.getCRefSize().end(), 1, std::multiplies< ::boost::int32_t > () );
    size  *= image.getPixelType().sizeOf();
    return size;
 }

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

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
        m_vSpacing(m_ui8Dimension,1),
        m_vOrigin(m_ui8Dimension,0),
        m_vSize(m_ui8Dimension,0),
        m_fsFilename(""),
        m_dWindowCenter(0),
        m_dWindowWidth(0),
        m_dRescaleIntercept(0),
        m_bufferDelegate( StandardBuffer::New() )
{}

//------------------------------------------------------------------------------

Image::~Image() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Image::shallowCopy( Image::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );

    // Assign
    getInformation(_source );

    this->m_bufferDelegate  = _source->m_bufferDelegate;
}

//-----------------------------------------------------------------------------

void Image::deepCopy( Image::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    // Assign
    getInformation(_source );

    if(this->getPixelType() != ::fwTools::DynamicType() && _source->getBufferDelegate() )
    {
        char * src = static_cast<char *>( _source->getBuffer() );
        SLM_ASSERT("Image source has not buffer", src);
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

void Image::setManagesBuff( const bool _bManagesBuff )
{
    this->m_bufferDelegate->setManagesBuff( _bManagesBuff ) ;
}

//------------------------------------------------------------------------------

void Image::setCRefManagesBuff( const bool & _bManagesBuff )
{
    this->m_bufferDelegate->setCRefManagesBuff( _bManagesBuff ) ;
}

//------------------------------------------------------------------------------

const bool Image::getManagesBuff() const
{
    return this->m_bufferDelegate->getManagesBuff() ;
}

//------------------------------------------------------------------------------

const bool & Image::getCRefManagesBuff() const
{
    return this->m_bufferDelegate->getCRefManagesBuff() ;
}

//------------------------------------------------------------------------------

bool & Image::getRefManagesBuff()
{
    return this->m_bufferDelegate->getRefManagesBuff() ;
}


void Image::getInformation( Image::csptr _source )
{
    this->m_ui8Dimension        = _source->m_ui8Dimension;
    this->m_dtPixelType         = _source->m_dtPixelType;
    this->m_vSpacing            = _source->m_vSpacing;
    this->m_vOrigin             = _source->m_vOrigin;
    this->m_vSize               = _source->m_vSize;
    this->m_fsFilename          = _source->m_fsFilename;
    this->m_dWindowCenter       = _source->m_dWindowCenter;
    this->m_dWindowWidth        = _source->m_dWindowWidth;
    this->m_dRescaleIntercept   = _source->m_dRescaleIntercept;
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( ::boost::int32_t x, ::boost::int32_t y, ::boost::int32_t z )
{
    std::vector<boost::int32_t> size = this->getSize();
    int offset = x + size[0]*y + z*size[0]*size[1];
    return getPixelBuffer(offset);
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( VoxelIndexType index )
{
    unsigned char imagePixelSize = this->getPixelType().sizeOf();
    BufferType * buf = static_cast < BufferType * > (this->getBuffer());
    BufferIndexType bufIndex = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------
::boost::shared_ptr< Image::BufferType > Image::getPixelBufferCopy( ::boost::int32_t x, ::boost::int32_t y, ::boost::int32_t z )
{
    std::vector<boost::int32_t> size = this->getSize();
    int offset = x + size[0]*y + z*size[0]*size[1];
    return getPixelBufferCopy(offset);
}

//------------------------------------------------------------------------------

::boost::shared_ptr< Image::BufferType > Image::getPixelBufferCopy( VoxelIndexType index )
{
    unsigned char imagePixelSize = this->getPixelType().sizeOf();
    BufferType * buf = static_cast < BufferType * > (this->getPixelBuffer(index));
    ::boost::shared_ptr< BufferType > res(new BufferType(imagePixelSize));
    std::copy(buf, buf+imagePixelSize, res.get());
    return res;
}


//------------------------------------------------------------------------------

void Image::setPixelBuffer( VoxelIndexType index , Image::BufferType * pixBuf)
{
    unsigned char imagePixelSize = this->getPixelType().sizeOf();
    BufferType * buf = static_cast < BufferType * > (this->getPixelBuffer(index));

    std::copy(pixBuf, pixBuf+imagePixelSize, buf);
}


} // namespace fwData
