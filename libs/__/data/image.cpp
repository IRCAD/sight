/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/image.hpp"

#include "data/exception.hpp"
#include "data/helper/MedicalImage.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>

#include <numeric>

//------------------------------------------------------------------------------

SIGHT_REGISTER_DATA(sight::data::image);

//------------------------------------------------------------------------------

namespace sight::data
{

auto pixelFormatToNumComponents =
    [](image::PixelFormat format)
    {
        static const std::array<std::size_t, image::PixelFormat::_SIZE> s_pixelFormatToNumComponents =
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

const core::com::signals::key_t image::BUFFER_MODIFIED_SIG      = "bufferModified";
const core::com::signals::key_t image::LANDMARK_ADDED_SIG       = "landmarkAdded";
const core::com::signals::key_t image::LANDMARK_REMOVED_SIG     = "landmarkRemoved";
const core::com::signals::key_t image::LANDMARK_DISPLAYED_SIG   = "landmarkDisplayed";
const core::com::signals::key_t image::DISTANCE_ADDED_SIG       = "distanceAdded";
const core::com::signals::key_t image::DISTANCE_MODIFIED_SIG    = "distanceModified";
const core::com::signals::key_t image::DISTANCE_REMOVED_SIG     = "distanceRemoved";
const core::com::signals::key_t image::DISTANCE_DISPLAYED_SIG   = "distanceDisplayed";
const core::com::signals::key_t image::SLICE_INDEX_MODIFIED_SIG = "sliceIndexModified";
const core::com::signals::key_t image::SLICE_TYPE_MODIFIED_SIG  = "sliceTypeModified";

//------------------------------------------------------------------------------

image::image() :
    m_dataArray(std::make_shared<data::array>())
{
    new_signal<BufferModifiedSignalType>(BUFFER_MODIFIED_SIG);
    new_signal<LandmarkAddedSignalType>(LANDMARK_ADDED_SIG);
    new_signal<LandmarkRemovedSignalType>(LANDMARK_REMOVED_SIG);
    new_signal<LandmarkDisplayedSignalType>(LANDMARK_DISPLAYED_SIG);
    new_signal<DistanceDisplayedSignalType>(DISTANCE_DISPLAYED_SIG);
    new_signal<DistanceAddedSignalType>(DISTANCE_ADDED_SIG);
    new_signal<DistanceModifiedSignalType>(DISTANCE_MODIFIED_SIG);
    new_signal<DistanceRemovedSignalType>(DISTANCE_REMOVED_SIG);
    new_signal<SliceIndexModifiedSignalType>(SLICE_INDEX_MODIFIED_SIG);
    new_signal<SliceTypeModifiedSignalType>(SLICE_TYPE_MODIFIED_SIG);

    auto pl = std::make_shared<data::point_list>();
    data::helper::MedicalImage::setLandmarks(*this, pl);
}

//-----------------------------------------------------------------------------

void image::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const image>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    // Assign
    copyInformation(other);

    m_dataArray = other->m_dataArray;

    base_class::shallow_copy(other);
}

//-----------------------------------------------------------------------------

void image::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const image>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    // Assign
    this->copyInformation(other);

    if(other->m_dataArray)
    {
        m_dataArray = object::copy(other->m_dataArray, cache);
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

std::size_t image::resize(const Size& size, const core::type& type, PixelFormat format)
{
    return this->_resize(size, type, format, true);
}

//------------------------------------------------------------------------------

std::size_t image::_resize(const Size& size, const core::type& type, PixelFormat format, bool realloc)
{
    m_size          = size;
    m_type          = type;
    m_pixelFormat   = format;
    m_numComponents = pixelFormatToNumComponents(format);

    SIGHT_ASSERT("Number of components must be > 0", m_numComponents > 0);
    SIGHT_ASSERT("Number of components must be <= 4", m_numComponents <= 4);

    const std::size_t imageDims = this->numDimensions();
    data::array::SizeType arraySize(imageDims);

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

core::type image::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

void image::copyInformation(image::csptr _source)
{
    m_size          = _source->m_size;
    m_spacing       = _source->m_spacing;
    m_origin        = _source->m_origin;
    m_type          = _source->m_type;
    m_windowCenters = _source->m_windowCenters;
    m_windowWidths  = _source->m_windowWidths;
    m_numComponents = _source->m_numComponents;
    m_pixelFormat   = _source->m_pixelFormat;
}

//------------------------------------------------------------------------------

std::size_t image::numDimensions() const
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

std::size_t image::getSizeInBytes() const
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

std::size_t image::getAllocatedSizeInBytes() const
{
    std::size_t size = 0;
    if(m_dataArray)
    {
        size = m_dataArray->getSizeInBytes();
    }

    return size;
}

//------------------------------------------------------------------------------

void image::dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& locks) const
{
    const auto& array_locks = m_dataArray->dump_lock();
    locks.insert(locks.end(), array_locks.cbegin(), array_locks.cend());
}

//-----------------------------------------------------------------------------

void* image::buffer()
{
    return m_dataArray->buffer();
}

//-----------------------------------------------------------------------------

const void* image::buffer() const
{
    return m_dataArray->buffer();
}

//------------------------------------------------------------------------------

void* image::getPixel(IndexType index)
{
    const std::size_t imagePixelSize = m_type.size() * m_numComponents;
    auto* buf                        = static_cast<BufferType*>(this->buffer());
    const IndexType bufIndex         = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

const void* image::getPixel(IndexType index) const
{
    const std::size_t imagePixelSize = m_type.size() * m_numComponents;
    const auto* buf                  = static_cast<const BufferType*>(this->buffer());
    const IndexType bufIndex         = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

void image::setPixel(IndexType index, const image::BufferType* pixBuf)
{
    const std::size_t imagePixelSize = m_type.size() * m_numComponents;
    auto* buf                        = static_cast<BufferType*>(this->getPixel(index));

    std::copy(pixBuf, pixBuf + imagePixelSize, buf);
}

//------------------------------------------------------------------------------

std::string image::getPixelAsString(
    IndexType x,
    IndexType y,
    IndexType z
) const
{
    const IndexType offset = x + m_size[0] * y + z * m_size[0] * m_size[1];
    return m_type.to_string(this->getPixel(offset));
}

//------------------------------------------------------------------------------

image::iterator<char> image::begin()
{
    return {static_cast<char*>(buffer())};
}

//------------------------------------------------------------------------------

image::iterator<char> image::end()
{
    auto itr = begin<char>();
    itr += static_cast<typename iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

image::const_iterator<char> image::begin() const
{
    return {static_cast<const char*>(buffer())};
}

//------------------------------------------------------------------------------

image::const_iterator<char> image::end() const
{
    auto itr = begin<char>();
    itr += static_cast<typename iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

std::size_t image::numElements() const
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

void image::setBuffer(
    void* buf,
    bool takeOwnership,
    const core::type& type,
    const data::image::Size& size,
    PixelFormat format,
    core::memory::buffer_allocation_policy::sptr policy
)
{
    this->_resize(size, type, format, false);
    this->setBuffer(buf, takeOwnership, policy);
}

//------------------------------------------------------------------------------

void image::setBuffer(void* buf, bool takeOwnership, core::memory::buffer_allocation_policy::sptr policy)
{
    if(m_dataArray->getIsBufferOwner())
    {
        if(!m_dataArray->get_buffer_object()->is_empty())
        {
            m_dataArray->get_buffer_object()->destroy();
        }
    }
    else
    {
        core::memory::buffer_object::sptr newBufferObject = std::make_shared<core::memory::buffer_object>();
        core::memory::buffer_object::sptr oldBufferObject = m_dataArray->get_buffer_object();
        oldBufferObject->swap(newBufferObject);
    }

    m_dataArray->get_buffer_object()->set_buffer(buf, (buf == nullptr) ? 0 : m_dataArray->getSizeInBytes(), policy);
    m_dataArray->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

core::memory::buffer_object::sptr image::get_buffer_object()
{
    return m_dataArray->get_buffer_object();
}

//------------------------------------------------------------------------------

core::memory::buffer_object::csptr image::get_buffer_object() const
{
    return m_dataArray->get_buffer_object();
}

//------------------------------------------------------------------------------

bool image::operator==(const image& other) const noexcept
{
    if(!core::tools::is_equal(m_size, other.m_size)
       || !core::tools::is_equal(m_spacing, other.m_spacing)
       || !core::tools::is_equal(m_origin, other.m_origin)
       || m_type != other.m_type
       || !core::tools::is_equal(m_windowCenters, other.m_windowCenters)
       || !core::tools::is_equal(m_windowWidths, other.m_windowWidths)
       || m_numComponents != other.m_numComponents
       || m_pixelFormat != other.m_pixelFormat
       || !core::tools::is_equal(m_dataArray, other.m_dataArray))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool image::operator!=(const image& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
