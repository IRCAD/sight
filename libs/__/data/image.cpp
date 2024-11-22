/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
    [](image::pixel_format_t _format)
    {
        static const std::array<std::size_t, image::pixel_format_t::count> s_PIXEL_FORMAT_TO_NUM_COMPONENTS =
        {
            ~0UL,
            3,
            4,
            3,
            4,
            1,
            2
        };
        return s_PIXEL_FORMAT_TO_NUM_COMPONENTS[_format];
    };

const core::com::signals::key_t image::BUFFER_MODIFIED_SIG      = "buffer_modified";
const core::com::signals::key_t image::LANDMARK_ADDED_SIG       = "landmarkAdded";
const core::com::signals::key_t image::LANDMARK_REMOVED_SIG     = "landmarkRemoved";
const core::com::signals::key_t image::LANDMARK_DISPLAYED_SIG   = "landmarkDisplayed";
const core::com::signals::key_t image::SLICE_INDEX_MODIFIED_SIG = "sliceIndexModified";
const core::com::signals::key_t image::SLICE_TYPE_MODIFIED_SIG  = "sliceTypeModified";
const core::com::signals::key_t image::RULER_MODIFIED_SIG       = "ruler_modified";
const core::com::signals::key_t image::FIDUCIAL_REMOVED_SIG     = "fiducial_removed";

//------------------------------------------------------------------------------

image::image() :
    m_data_array(std::make_shared<data::array>())
{
    new_signal<buffer_modified_signal_t>(BUFFER_MODIFIED_SIG);
    new_signal<landmark_added_signal_t>(LANDMARK_ADDED_SIG);
    new_signal<landmark_removed_signal_t>(LANDMARK_REMOVED_SIG);
    new_signal<landmark_displayed_signal_t>(LANDMARK_DISPLAYED_SIG);
    new_signal<slice_index_modified_signal_t>(SLICE_INDEX_MODIFIED_SIG);
    new_signal<slice_type_modified_signal_t>(SLICE_TYPE_MODIFIED_SIG);
    new_signal<ruler_modified_signal_t>(RULER_MODIFIED_SIG);
    new_signal<fiducial_removed_signal_t>(FIDUCIAL_REMOVED_SIG);

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
    copy_information(other);

    m_data_array = other->m_data_array;

    base_class_t::shallow_copy(other);
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
    this->copy_information(other);

    if(other->m_data_array)
    {
        m_data_array->deep_copy(other->m_data_array, _cache);
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

std::size_t image::resize(const size_t& _size, const core::type& _type, enum pixel_format_t _format)
{
    return this->resize(_size, _type, _format, true);
}

//------------------------------------------------------------------------------

std::size_t image::resize(const size_t& _size, const core::type& _type, enum pixel_format_t _format, bool _realloc)
{
    m_size           = _size;
    m_type           = _type;
    m_pixel_format   = _format;
    m_num_components = pixel_format_to_num_components(_format);
    m_stride         = m_type.size() * m_num_components;

    SIGHT_ASSERT("Number of components must be > 0", m_num_components > 0);
    SIGHT_ASSERT("Number of components must be <= 4", m_num_components <= 4);

    const std::size_t image_dims = this->num_dimensions();
    data::array::size_t array_size(image_dims);

    std::size_t count = 0;
    if(m_num_components > 1)
    {
        array_size.resize(image_dims + 1);
        array_size[0] = m_num_components;
        count         = 1;
    }

    for(std::size_t i = 0 ; i < image_dims ; ++i)
    {
        array_size[count] = m_size[i];
        ++count;
    }

    return m_data_array->resize(array_size, m_type, _realloc);
}

//------------------------------------------------------------------------------

core::type image::type() const
{
    return m_type;
}

//------------------------------------------------------------------------------

void image::copy_information(image::csptr _source)
{
    m_size           = _source->m_size;
    m_spacing        = _source->m_spacing;
    m_origin         = _source->m_origin;
    m_orientation    = _source->m_orientation;
    m_stride         = _source->m_stride;
    m_type           = _source->m_type;
    m_window_centers = _source->m_window_centers;
    m_window_widths  = _source->m_window_widths;
    m_num_components = _source->m_num_components;
    m_pixel_format   = _source->m_pixel_format;
}

//------------------------------------------------------------------------------

std::size_t image::num_dimensions() const
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

std::size_t image::size_in_bytes() const
{
    std::size_t size       = 0;
    const std::size_t dims = this->num_dimensions();
    if(dims > 0)
    {
        size = std::accumulate(
            m_size.begin(),
            m_size.begin() + dims,
            m_stride,
            std::multiplies<>()
        );
    }

    return size;
}

//------------------------------------------------------------------------------

std::size_t image::allocated_size_in_bytes() const
{
    std::size_t size = 0;
    if(m_data_array)
    {
        size = m_data_array->size_in_bytes();
    }

    return size;
}

//------------------------------------------------------------------------------

void image::dump_lock_impl(std::vector<core::memory::buffer_object::lock_t>& _locks) const
{
    const auto& array_locks = m_data_array->dump_lock();
    _locks.insert(_locks.end(), array_locks.cbegin(), array_locks.cend());
}

//-----------------------------------------------------------------------------

void* image::buffer()
{
    return m_data_array->buffer();
}

//-----------------------------------------------------------------------------

const void* image::buffer() const
{
    return m_data_array->buffer();
}

//------------------------------------------------------------------------------

void* image::get_pixel(index_t _index)
{
    auto* buf               = static_cast<buffer_t*>(this->buffer());
    const index_t buf_index = _index * m_stride;
    return buf + buf_index;
}

//------------------------------------------------------------------------------

const void* image::get_pixel(index_t _index) const
{
    const auto* buf         = static_cast<const buffer_t*>(this->buffer());
    const index_t buf_index = _index * m_stride;
    return buf + buf_index;
}

//------------------------------------------------------------------------------

void image::set_pixel(index_t _index, const image::buffer_t* _pix_buf)
{
    auto* buf               = static_cast<buffer_t*>(m_data_array->buffer());
    const index_t buf_index = _index * m_stride;

    std::copy(_pix_buf, _pix_buf + m_stride, buf + buf_index);
}

//------------------------------------------------------------------------------

std::string image::get_pixel_as_string(
    index_t _x,
    index_t _y,
    index_t _z
) const
{
    const index_t offset = _x + m_size[0] * _y + _z * m_size[0] * m_size[1];
    return m_type.to_string(this->get_pixel(offset));
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
    itr += static_cast<typename iterator<char>::difference_type>(this->size_in_bytes());
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
    itr += static_cast<typename iterator<char>::difference_type>(this->size_in_bytes());
    return itr;
}

//------------------------------------------------------------------------------

std::size_t image::num_elements() const
{
    std::size_t nb_elts = 0;
    if(m_size[0] > 0)
    {
        nb_elts = m_num_components;
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

void image::set_buffer(
    void* _buf,
    bool _take_ownership,
    const core::type& _type,
    const data::image::size_t& _size,
    enum pixel_format_t _format,
    core::memory::buffer_allocation_policy::sptr _policy
)
{
    this->resize(_size, _type, _format, false);
    this->set_buffer(_buf, _take_ownership, _policy);
}

//------------------------------------------------------------------------------

void image::set_buffer(void* _buf, bool _take_ownership, core::memory::buffer_allocation_policy::sptr _policy)
{
    if(m_data_array->get_is_buffer_owner())
    {
        if(!m_data_array->get_buffer_object()->is_empty())
        {
            m_data_array->get_buffer_object()->destroy();
        }
    }
    else
    {
        core::memory::buffer_object::sptr new_buffer_object = std::make_shared<core::memory::buffer_object>();
        core::memory::buffer_object::sptr old_buffer_object = m_data_array->get_buffer_object();
        old_buffer_object->swap(new_buffer_object);
    }

    m_data_array->get_buffer_object()->set_buffer(_buf, (_buf == nullptr) ? 0 : m_data_array->size_in_bytes(), _policy);
    m_data_array->set_is_buffer_owner(_take_ownership);
}

//------------------------------------------------------------------------------

core::memory::buffer_object::sptr image::get_buffer_object()
{
    return m_data_array->get_buffer_object();
}

//------------------------------------------------------------------------------

core::memory::buffer_object::csptr image::get_buffer_object() const
{
    return m_data_array->get_buffer_object();
}

//------------------------------------------------------------------------------

bool image::operator==(const image& _other) const noexcept
{
    if(!core::is_equal(m_size, _other.m_size)
       || !core::is_equal(m_spacing, _other.m_spacing)
       || !core::is_equal(m_origin, _other.m_origin)
       || !core::is_equal(m_orientation, _other.m_orientation)
       || m_type != _other.m_type
       || !core::is_equal(m_window_centers, _other.m_window_centers)
       || !core::is_equal(m_window_widths, _other.m_window_widths)
       || m_num_components != _other.m_num_components
       || m_pixel_format != _other.m_pixel_format
       || !core::is_equal(m_data_array, _other.m_data_array))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool image::operator!=(const image& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
