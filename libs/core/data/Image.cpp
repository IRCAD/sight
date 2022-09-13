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

#include "data/helper/MedicalImage.hpp"
#include "data/Image.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>

#include <numeric>

//------------------------------------------------------------------------------

SIGHT_REGISTER_DATA(sight::data::Image);

//------------------------------------------------------------------------------

namespace sight::data
{

auto pixelFormatToNumComponents =
    [](Image::PixelFormat format)
    {
        static const std::array<std::size_t, Image::PixelFormat::_SIZE> s_pixelFormatToNumComponents =
        {
            ~0UL,
            3,
            4,
            3,
            4,
            1,
            2
        };
        return s_pixelFormatToNumComponents[format];
    };

const core::com::Signals::SignalKeyType Image::s_BUFFER_MODIFIED_SIG      = "bufferModified";
const core::com::Signals::SignalKeyType Image::s_LANDMARK_ADDED_SIG       = "landmarkAdded";
const core::com::Signals::SignalKeyType Image::s_LANDMARK_REMOVED_SIG     = "landmarkRemoved";
const core::com::Signals::SignalKeyType Image::s_LANDMARK_DISPLAYED_SIG   = "landmarkDisplayed";
const core::com::Signals::SignalKeyType Image::s_DISTANCE_ADDED_SIG       = "distanceAdded";
const core::com::Signals::SignalKeyType Image::s_DISTANCE_REMOVED_SIG     = "distanceRemoved";
const core::com::Signals::SignalKeyType Image::s_DISTANCE_DISPLAYED_SIG   = "distanceDisplayed";
const core::com::Signals::SignalKeyType Image::s_SLICE_INDEX_MODIFIED_SIG = "sliceIndexModified";
const core::com::Signals::SignalKeyType Image::s_SLICE_TYPE_MODIFIED_SIG  = "sliceTypeModified";

//------------------------------------------------------------------------------

Image::Image(data::Object::Key /*unused*/) :
    m_dataArray(data::Array::New())
{
    newSignal<BufferModifiedSignalType>(s_BUFFER_MODIFIED_SIG);
    newSignal<LandmarkAddedSignalType>(s_LANDMARK_ADDED_SIG);
    newSignal<LandmarkRemovedSignalType>(s_LANDMARK_REMOVED_SIG);
    newSignal<LandmarkDisplayedSignalType>(s_LANDMARK_DISPLAYED_SIG);
    newSignal<DistanceDisplayedSignalType>(s_DISTANCE_DISPLAYED_SIG);
    newSignal<DistanceAddedSignalType>(s_DISTANCE_ADDED_SIG);
    newSignal<DistanceRemovedSignalType>(s_DISTANCE_REMOVED_SIG);
    newSignal<SliceIndexModifiedSignalType>(s_SLICE_INDEX_MODIFIED_SIG);
    newSignal<SliceTypeModifiedSignalType>(s_SLICE_TYPE_MODIFIED_SIG);

    auto pl = data::PointList::New();
    data::helper::MedicalImage::setLandmarks(*this, pl);

    sight::data::helper::MedicalImage::updateDefaultTransferFunction(*this);
}

//-----------------------------------------------------------------------------

void Image::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    // Assign
    copyInformation(other);

    m_dataArray = other->m_dataArray;

    BaseClass::shallowCopy(other);
}

//-----------------------------------------------------------------------------

void Image::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !other
    );

    // Assign
    this->copyInformation(other);

    if(other->m_dataArray)
    {
        m_dataArray = Object::copy(other->m_dataArray, cache);
    }

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

std::size_t Image::resize(const Size& size, const core::Type& type, PixelFormat format)
{
    return this->_resize(size, type, format, true);
}

//------------------------------------------------------------------------------

std::size_t Image::_resize(const Size& size, const core::Type& type, PixelFormat format, bool realloc)
{
    m_size          = size;
    m_type          = type;
    m_pixelFormat   = format;
    m_numComponents = pixelFormatToNumComponents(format);

    SIGHT_ASSERT("Number of components must be > 0", m_numComponents > 0);
    SIGHT_ASSERT("Number of components must be <= 4", m_numComponents <= 4);

    const std::size_t imageDims = this->numDimensions();
    data::Array::SizeType arraySize(imageDims);

    std::size_t count = 0;
    if(m_numComponents > 1)
    {
        arraySize.resize(imageDims + 1);
        arraySize[0] = m_numComponents;
        count        = 1;
    }

    for(std::size_t i = 0 ; i < imageDims ; ++i)
    {
        arraySize[count] = m_size[i];
        ++count;
    }

    return m_dataArray->resize(arraySize, m_type, realloc);
}

//------------------------------------------------------------------------------

core::Type Image::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

void Image::copyInformation(Image::csptr _source)
{
    m_size          = _source->m_size;
    m_spacing       = _source->m_spacing;
    m_origin        = _source->m_origin;
    m_type          = _source->m_type;
    m_windowCenter  = _source->m_windowCenter;
    m_windowWidth   = _source->m_windowWidth;
    m_numComponents = _source->m_numComponents;
    m_pixelFormat   = _source->m_pixelFormat;
}

//------------------------------------------------------------------------------

std::size_t Image::numDimensions() const
{
    std::size_t dims = 0;

    for(const auto& val : m_size)
    {
        if(val > 0)
        {
            ++dims;
        }
        else
        {
            break;
        }
    }

    return dims;
}

//------------------------------------------------------------------------------

std::size_t Image::getSizeInBytes() const
{
    std::size_t size       = 0;
    const std::size_t dims = this->numDimensions();
    if(dims > 0)
    {
        size = std::accumulate(
            m_size.begin(),
            m_size.begin() + dims,
            static_cast<std::size_t>(m_type.size()) * m_numComponents,
            std::multiplies<>()
        );
    }

    return size;
}

//------------------------------------------------------------------------------

std::size_t Image::getAllocatedSizeInBytes() const
{
    std::size_t size = 0;
    if(m_dataArray)
    {
        size = m_dataArray->getSizeInBytes();
    }

    return size;
}

//------------------------------------------------------------------------------

void Image::dump_lock_impl(std::vector<core::memory::BufferObject::Lock>& locks) const
{
    const auto& array_locks = m_dataArray->dump_lock();
    locks.insert(locks.end(), array_locks.cbegin(), array_locks.cend());
}

//-----------------------------------------------------------------------------

void* Image::getBuffer()
{
    return m_dataArray->getBuffer();
}

//-----------------------------------------------------------------------------

const void* Image::getBuffer() const
{
    return m_dataArray->getBuffer();
}

//------------------------------------------------------------------------------

void* Image::getPixel(IndexType index)
{
    const std::size_t imagePixelSize = m_type.size() * m_numComponents;
    auto* buf                        = static_cast<BufferType*>(this->getBuffer());
    const IndexType bufIndex         = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

const void* Image::getPixel(IndexType index) const
{
    const std::size_t imagePixelSize = m_type.size() * m_numComponents;
    const auto* buf                  = static_cast<const BufferType*>(this->getBuffer());
    const IndexType bufIndex         = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

void Image::setPixel(IndexType index, const Image::BufferType* pixBuf)
{
    const std::size_t imagePixelSize = m_type.size() * m_numComponents;
    auto* buf                        = static_cast<BufferType*>(this->getPixel(index));

    std::copy(pixBuf, pixBuf + imagePixelSize, buf);
}

//------------------------------------------------------------------------------

std::string Image::getPixelAsString(
    IndexType x,
    IndexType y,
    IndexType z
) const
{
    const IndexType offset = x + m_size[0] * y + z * m_size[0] * m_size[1];
    return m_type.toString(this->getPixel(offset));
}

//------------------------------------------------------------------------------

Image::iterator<char> Image::begin()
{
    return {static_cast<char*>(getBuffer())};
}

//------------------------------------------------------------------------------

Image::iterator<char> Image::end()
{
    auto itr = begin<char>();
    itr += static_cast<typename iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

Image::const_iterator<char> Image::begin() const
{
    return {static_cast<const char*>(getBuffer())};
}

//------------------------------------------------------------------------------

Image::const_iterator<char> Image::end() const
{
    auto itr = begin<char>();
    itr += static_cast<typename iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

std::size_t Image::numElements() const
{
    std::size_t nbElts = 0;
    if(m_size[0] > 0)
    {
        nbElts = m_numComponents;
        for(const auto& val : m_size)
        {
            if(val > 0)
            {
                nbElts *= val;
            }
            else
            {
                break;
            }
        }
    }

    return nbElts;
}

//------------------------------------------------------------------------------

void Image::setBuffer(
    void* buf,
    bool takeOwnership,
    const core::Type& type,
    const data::Image::Size& size,
    PixelFormat format,
    core::memory::BufferAllocationPolicy::sptr policy
)
{
    this->_resize(size, type, format, false);
    this->setBuffer(buf, takeOwnership, policy);
}

//------------------------------------------------------------------------------

void Image::setBuffer(void* buf, bool takeOwnership, core::memory::BufferAllocationPolicy::sptr policy)
{
    if(m_dataArray->getIsBufferOwner())
    {
        if(!m_dataArray->getBufferObject()->isEmpty())
        {
            m_dataArray->getBufferObject()->destroy();
        }
    }
    else
    {
        core::memory::BufferObject::sptr newBufferObject = core::memory::BufferObject::New();
        core::memory::BufferObject::sptr oldBufferObject = m_dataArray->getBufferObject();
        oldBufferObject->swap(newBufferObject);
    }

    m_dataArray->getBufferObject()->setBuffer(buf, (buf == nullptr) ? 0 : m_dataArray->getSizeInBytes(), policy);
    m_dataArray->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

core::memory::BufferObject::sptr Image::getBufferObject()
{
    return m_dataArray->getBufferObject();
}

//------------------------------------------------------------------------------

core::memory::BufferObject::csptr Image::getBufferObject() const
{
    return m_dataArray->getBufferObject();
}

//------------------------------------------------------------------------------

bool Image::operator==(const Image& other) const noexcept
{
    if(!core::tools::is_equal(m_size, other.m_size)
       || !core::tools::is_equal(m_spacing, other.m_spacing)
       || !core::tools::is_equal(m_origin, other.m_origin)
       || m_type != other.m_type
       || !core::tools::is_equal(m_windowCenter, other.m_windowCenter)
       || !core::tools::is_equal(m_windowWidth, other.m_windowWidth)
       || m_numComponents != other.m_numComponents
       || m_pixelFormat != other.m_pixelFormat
       || !core::tools::is_equal(m_dataArray, other.m_dataArray))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Image::operator!=(const Image& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
