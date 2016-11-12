/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/Image.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwTools/DynamicType.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <boost/assign.hpp>

#include <algorithm>
#include <climits>
#include <functional>
#include <numeric>
#include <sstream>

//------------------------------------------------------------------------------

fwDataRegisterMacro( ::fwData::Image );

//------------------------------------------------------------------------------

namespace fwData
{

const ::fwCom::Signals::SignalKeyType Image::s_BUFFER_MODIFIED_SIG       = "bufferModified";
const ::fwCom::Signals::SignalKeyType Image::s_LANDMARK_ADDED_SIG        = "landmarkAdded";
const ::fwCom::Signals::SignalKeyType Image::s_LANDMARK_REMOVED_SIG      = "landmarkRemoved";
const ::fwCom::Signals::SignalKeyType Image::s_LANDMARK_DISPLAYED_SIG    = "landmarkDisplayed";
const ::fwCom::Signals::SignalKeyType Image::s_DISTANCE_ADDED_SIG        = "distanceAdded";
const ::fwCom::Signals::SignalKeyType Image::s_DISTANCE_REMOVED_SIG      = "distanceRemoved";
const ::fwCom::Signals::SignalKeyType Image::s_DISTANCE_DISPLAYED_SIG    = "distanceDisplayed";
const ::fwCom::Signals::SignalKeyType Image::s_SLICE_INDEX_MODIFIED_SIG  = "sliceIndexModified";
const ::fwCom::Signals::SignalKeyType Image::s_SLICE_TYPE_MODIFIED_SIG   = "sliceTypeModified";
const ::fwCom::Signals::SignalKeyType Image::s_VISIBILITY_MODIFIED_SIG   = "visibilityModified";
const ::fwCom::Signals::SignalKeyType Image::s_TRANSPARENCY_MODIFIED_SIG = "transparencyModified";

//------------------------------------------------------------------------------

Image::Image(::fwData::Object::Key key) :
    m_type(),
    m_windowCenter(0.),
    m_windowWidth(0.),
    m_numberOfComponents(1),
    m_dataArray( ::fwData::Array::New() )
{
    newSignal< BufferModifiedSignalType >(s_BUFFER_MODIFIED_SIG);
    newSignal< LandmarkAddedSignalType >(s_LANDMARK_ADDED_SIG);
    newSignal< LandmarkRemovedSignalType >(s_LANDMARK_REMOVED_SIG);
    newSignal< LandmarkDisplayedSignalType >(s_LANDMARK_DISPLAYED_SIG);
    newSignal< DistanceDisplayedSignalType >(s_DISTANCE_DISPLAYED_SIG);
    newSignal< DistanceAddedSignalType >(s_DISTANCE_ADDED_SIG);
    newSignal< DistanceRemovedSignalType >(s_DISTANCE_REMOVED_SIG);
    newSignal< SliceIndexModifiedSignalType >(s_SLICE_INDEX_MODIFIED_SIG);
    newSignal< SliceTypeModifiedSignalType >(s_SLICE_TYPE_MODIFIED_SIG);
    newSignal< VisibilityModifiedSignalType >(s_VISIBILITY_MODIFIED_SIG);
    newSignal< TransparencyModifiedSignalType >(s_TRANSPARENCY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

Image::~Image() throw()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Image::shallowCopy(const Object::csptr& _source )
{
    Image::csptr other = Image::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    // Assign
    copyInformation( other );

    m_dataArray = other->m_dataArray;
}

//-----------------------------------------------------------------------------

void Image::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Image::csptr other = Image::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    // Assign
    copyInformation( other );

    if( other->m_dataArray )
    {
        m_dataArray = ::fwData::Object::copy(other->m_dataArray, cache);
    }
}

//------------------------------------------------------------------------------

::fwData::Array::sptr Image::getDataArray() const
{
    return m_dataArray;
}

//------------------------------------------------------------------------------

void Image::setDataArray(::fwData::Array::sptr array, bool copyArrayInfo)
{
    if( !array )
    {
        array = ::fwData::Array::New();
    }
    m_dataArray = array;
    if (copyArrayInfo)
    {
        m_size = array->getSize();
        m_type = array->getType();
    }
}

//------------------------------------------------------------------------------

size_t Image::allocate() throw(::fwData::Exception)
{
    if (!m_dataArray)
    {
        m_dataArray = ::fwData::Array::New();
    }

    SLM_ASSERT( "NumberOfComponents must be > 0", m_numberOfComponents > 0 );
    return m_dataArray->resize(m_type, m_size, m_numberOfComponents, true);
}

//------------------------------------------------------------------------------

size_t Image::allocate(SizeType::value_type x, SizeType::value_type y,  SizeType::value_type z,
                       const ::fwTools::Type& type, size_t numberOfComponents) throw(::fwData::Exception)
{
    m_size               = { x, y, z};
    m_type               = type;
    m_numberOfComponents = numberOfComponents;
    return allocate();
}

//------------------------------------------------------------------------------

size_t Image::allocate(const SizeType& size, const ::fwTools::Type& type, size_t numberOfComponents)
throw(::fwData::Exception)
{
    m_size               = size;
    m_type               = type;
    m_numberOfComponents = numberOfComponents;
    return allocate();
}

//------------------------------------------------------------------------------

::fwTools::DynamicType Image::getPixelType() const
{
    typedef std::map<std::string, ::fwTools::DynamicType> DynamicTypeMapType;

    static DynamicTypeMapType dynamicTypeMap = ::boost::assign::map_list_of
                                                   (::fwTools::Type().string(), ::fwTools::DynamicType() )
                                                   ("uint8", ::fwTools::makeDynamicType<std::string>("unsigned char")  )
                                                   ("uint16",
                                                   ::fwTools::makeDynamicType<std::string>("unsigned short") )
                                                   ("uint32",
                                                   ::fwTools::makeDynamicType<std::string>("unsigned int")   )
                                                   ("int8",  ::fwTools::makeDynamicType<std::string>("signed char")    )
                                                   ("int16",
                                                   ::fwTools::makeDynamicType<std::string>("signed short")   )
                                                   ("int32",
                                                   ::fwTools::makeDynamicType<std::string>("signed int")     )
                                                   ("float",
                                                   ::fwTools::makeDynamicType<std::string>("float")          )
                                                   ("double",
                                                   ::fwTools::makeDynamicType<std::string>("double")         )

//special case for dynamic type : 64bits integers was not managed by dynamic type.
#if ( INT_MAX < LONG_MAX )
                                               ("uint64", ::fwTools::makeDynamicType<std::string>("unsigned long")  )
                                                   ("int64",
                                                   ::fwTools::makeDynamicType<std::string>("signed long")    )
#else
                                               ("uint32", ::fwTools::makeDynamicType<std::string>("unsigned long")  )
                                                   ("int32",
                                                   ::fwTools::makeDynamicType<std::string>("signed long")    )
                                                   ("uint64", ::fwTools::DynamicType() )
                                                   ("int64",  ::fwTools::DynamicType() )
#endif
    ;

    ::fwTools::DynamicType dtype = dynamicTypeMap[getType().string()];
    return dtype;
}

//------------------------------------------------------------------------------

::fwTools::Type Image::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

void Image::setType(::fwTools::Type type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

void Image::setType(const std::string& type)
{
    m_type = ::fwTools::Type(type);
}

//------------------------------------------------------------------------------

void Image::copyInformation( Image::csptr _source )
{
    m_size               = _source->m_size;
    m_type               = _source->m_type;
    m_spacing            = _source->m_spacing;
    m_origin             = _source->m_origin;
    m_windowCenter       = _source->m_windowCenter;
    m_windowWidth        = _source->m_windowWidth;
    m_numberOfComponents = _source->m_numberOfComponents;
}

//------------------------------------------------------------------------------

size_t Image::getNumberOfDimensions() const
{
    return m_size.size();
}

//------------------------------------------------------------------------------

const Image::SpacingType& Image::getSpacing() const
{
    return m_spacing;
}

//------------------------------------------------------------------------------

void Image::setSpacing(const SpacingType& spacing)
{
    m_spacing = spacing;
}

//------------------------------------------------------------------------------

const Image::OriginType& Image::getOrigin() const
{
    return m_origin;
}

//------------------------------------------------------------------------------

void Image::setOrigin(const OriginType& origin)
{
    m_origin = origin;
}

//------------------------------------------------------------------------------

const Image::SizeType& Image::getSize() const
{
    return m_size;
}

//------------------------------------------------------------------------------

void Image::setSize(const SizeType& size)
{
    m_size = size;
}

//------------------------------------------------------------------------------

size_t Image::getSizeInBytes() const
{
    SLM_TRACE_FUNC();

    size_t size = std::accumulate(
        m_size.begin(), m_size.end(),
        static_cast<size_t>(m_type.sizeOf()) * m_numberOfComponents,
        std::multiplies< size_t > () );
    return size;
}

//------------------------------------------------------------------------------

size_t Image::getAllocatedSizeInBytes() const
{
    SLM_TRACE_FUNC();
    size_t size = 0;
    if (m_dataArray)
    {
        size = m_dataArray->getSizeInBytes();
    }
    return size;
}

//------------------------------------------------------------------------------

} // namespace fwData
