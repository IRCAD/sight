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

#include "data/helper/medical_image.hpp"
#include "data/image.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>

#include <numeric>

//------------------------------------------------------------------------------

SIGHT_REGISTER_DATA(sight::data::image);

//------------------------------------------------------------------------------

namespace sight::data
{

auto pixel_format_to_num_components =
    [](image::PixelFormat _format)
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
        return s_pixelFormatToNumComponents[_format];
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
    new_signal<buffer_modified_signal_t>(BUFFER_MODIFIED_SIG);
    new_signal<landmark_added_signal_t>(LANDMARK_ADDED_SIG);
    new_signal<landmark_removed_signal_t>(LANDMARK_REMOVED_SIG);
    new_signal<landmark_displayed_signal_t>(LANDMARK_DISPLAYED_SIG);
    new_signal<distance_displayed_signal_t>(DISTANCE_DISPLAYED_SIG);
    new_signal<distance_added_signal_t>(DISTANCE_ADDED_SIG);
    new_signal<distance_modified_signal_t>(DISTANCE_MODIFIED_SIG);
    new_signal<distance_removed_signal_t>(DISTANCE_REMOVED_SIG);
    new_signal<SliceIndexModifiedSignalType>(SLICE_INDEX_MODIFIED_SIG);
    new_signal<SliceTypeModifiedSignalType>(SLICE_TYPE_MODIFIED_SIG);

    auto pl = std::make_shared<data::point_list>();
    data::helper::medical_image::set_landmarks(*this, pl);
}

//-----------------------------------------------------------------------------

void image::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const image>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
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

void image::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const image>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !other
    );

    // Assign
    this->copyInformation(other);

    if(other->m_dataArray)
    {
        m_dataArray = object::copy(other->m_dataArray, _cache);
    }

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

std::size_t image::resize(const Size& _size, const core::type& _type, PixelFormat _format)
{
    return this->resize(_size, _type, _format, true);
}

//------------------------------------------------------------------------------

std::size_t image::resize(const Size& _size, const core::type& _type, PixelFormat _format, bool _realloc)
{
    m_size          = _size;
    m_type          = _type;
    m_pixelFormat   = _format;
    m_numComponents = pixel_format_to_num_components(_format);

    SIGHT_ASSERT("Number of components must be > 0", m_numComponents > 0);
    SIGHT_ASSERT("Number of components must be <= 4", m_numComponents <= 4);

    const std::size_t image_dims = this->numDimensions();
    data::array::size_t array_size(image_dims);

    std::size_t count = 0;
    if(m_numComponents > 1)
    {
        array_size.resize(image_dims + 1);
        array_size[0] = m_numComponents;
        count         = 1;
    }

    for(std::size_t i = 0 ; i < image_dims ; ++i)
    {
        array_size[count] = m_size[i];
        ++count;
    }

    return m_dataArray->resize(array_size, m_type, _realloc);
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

void image::dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const
{
    const auto& array_locks = m_dataArray->dump_lock();
    _locks.insert(_locks.end(), array_locks.cbegin(), array_locks.cend());
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

void* image::getPixel(index_t _index)
{
    const std::size_t image_pixel_size = m_type.size() * m_numComponents;
    auto* buf                          = static_cast<buffer_t*>(this->buffer());
    const index_t buf_index            = _index * image_pixel_size;
    return buf + buf_index;
}

//------------------------------------------------------------------------------

const void* image::getPixel(index_t _index) const
{
    const std::size_t image_pixel_size = m_type.size() * m_numComponents;
    const auto* buf                    = static_cast<const buffer_t*>(this->buffer());
    const index_t buf_index            = _index * image_pixel_size;
    return buf + buf_index;
}

//------------------------------------------------------------------------------

void image::setPixel(index_t _index, const image::buffer_t* _pix_buf)
{
    const std::size_t image_pixel_size = m_type.size() * m_numComponents;
    auto* buf                          = static_cast<buffer_t*>(this->getPixel(_index));

    std::copy(_pix_buf, _pix_buf + image_pixel_size, buf);
}

//------------------------------------------------------------------------------

std::string image::getPixelAsString(
    index_t _x,
    index_t _y,
    index_t _z
) const
{
    const index_t offset = _x + m_size[0] * _y + _z * m_size[0] * m_size[1];
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
    std::size_t nb_elts = 0;
    if(m_size[0] > 0)
    {
        nb_elts = m_numComponents;
        for(const auto& val : m_size)
        {
            if(val > 0)
            {
                nb_elts *= val;
            }
            else
            {
                break;
            }
        }
    }

    return nb_elts;
}

//------------------------------------------------------------------------------

void image::setBuffer(
    void* _buf,
    bool _take_ownership,
    const core::type& _type,
    const data::image::Size& _size,
    PixelFormat _format,
    core::memory::buffer_allocation_policy::sptr _policy
)
{
    this->resize(_size, _type, _format, false);
    this->setBuffer(_buf, _take_ownership, _policy);
}

//------------------------------------------------------------------------------

void image::setBuffer(void* _buf, bool _take_ownership, core::memory::buffer_allocation_policy::sptr _policy)
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
        core::memory::buffer_object::sptr new_buffer_object = std::make_shared<core::memory::buffer_object>();
        core::memory::buffer_object::sptr old_buffer_object = m_dataArray->get_buffer_object();
        old_buffer_object->swap(new_buffer_object);
    }

    m_dataArray->get_buffer_object()->set_buffer(_buf, (_buf == nullptr) ? 0 : m_dataArray->getSizeInBytes(), _policy);
    m_dataArray->setIsBufferOwner(_take_ownership);
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

bool image::operator==(const image& _other) const noexcept
{
    if(!core::tools::is_equal(m_size, _other.m_size)
       || !core::tools::is_equal(m_spacing, _other.m_spacing)
       || !core::tools::is_equal(m_origin, _other.m_origin)
       || m_type != _other.m_type
       || !core::tools::is_equal(m_windowCenters, _other.m_windowCenters)
       || !core::tools::is_equal(m_windowWidths, _other.m_windowWidths)
       || m_numComponents != _other.m_numComponents
       || m_pixelFormat != _other.m_pixelFormat
       || !core::tools::is_equal(m_dataArray, _other.m_dataArray))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool image::operator!=(const image& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
