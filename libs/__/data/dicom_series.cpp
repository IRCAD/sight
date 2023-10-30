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

#include "data/dicom_series.hpp"

#include <core/memory/stream/in/raw.hpp>

#include <data/exception.hpp>
#include <data/registry/macros.hpp>

#include <filesystem>

SIGHT_REGISTER_DATA(sight::data::dicom_series)

namespace sight::data
{

//------------------------------------------------------------------------------

void dicom_series::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const dicom_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_number_of_instances   = other->m_number_of_instances;
    m_dicom_container       = other->m_dicom_container;
    m_sop_class_ui_ds       = other->m_sop_class_ui_ds;
    m_computed_tag_values   = other->m_computed_tag_values;
    m_first_instance_number = other->m_first_instance_number;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void dicom_series::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const dicom_series>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_number_of_instances   = other->m_number_of_instances;
    m_sop_class_ui_ds       = other->m_sop_class_ui_ds;
    m_computed_tag_values   = other->m_computed_tag_values;
    m_first_instance_number = other->m_first_instance_number;

    m_dicom_container.clear();
    for(const auto& elt : other->m_dicom_container)
    {
        const core::memory::buffer_object::sptr& buffer_src = elt.second;
        core::memory::buffer_object::lock_t locker_source(buffer_src);

        if(!buffer_src->is_empty())
        {
            auto buffer_dest = std::make_shared<core::memory::buffer_object>(true);
            core::memory::buffer_object::lock_t locker_dest(buffer_dest);

            buffer_dest->allocate(buffer_src->size());

            char* buff_dest = static_cast<char*>(locker_dest.buffer());
            char* buff_src  = static_cast<char*>(locker_source.buffer());
            std::copy(buff_src, buff_src + buffer_src->size(), buff_dest);

            m_dicom_container[elt.first] = buffer_dest;
        }
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

void dicom_series::add_dicom_path(std::size_t _instance_index, const std::filesystem::path& _path)
{
    SIGHT_THROW_EXCEPTION_IF(
        std::filesystem::filesystem_error(
            std::string("Dicom path can not be found: "),
            _path,
            std::make_error_code(
                std::errc::no_such_file_or_directory
            )
        ),
        !std::filesystem::exists(_path)
    );

    auto buffer          = std::make_shared<core::memory::buffer_object>(true);
    const auto buff_size = std::filesystem::file_size(_path);
    buffer->set_istream_factory(
        std::make_shared<core::memory::stream::in::raw>(_path),
        static_cast<core::memory::buffer_object::size_t>(buff_size),
        _path,
        core::memory::raw
    );
    m_dicom_container[_instance_index] = buffer;
}

//------------------------------------------------------------------------------

void dicom_series::add_binary(std::size_t _instance_index, const core::memory::buffer_object::sptr& _buffer)
{
    m_dicom_container[_instance_index] = _buffer;
}

//------------------------------------------------------------------------------

bool dicom_series::is_instance_available(std::size_t _instance_index) const
{
    const auto& dicom_container_iter = m_dicom_container.find(_instance_index);
    return dicom_container_iter != m_dicom_container.end();
}

//------------------------------------------------------------------------------

void dicom_series::addsop_class_uid(const std::string& _sop_class_uid)
{
    m_sop_class_ui_ds.insert(_sop_class_uid);
}

//------------------------------------------------------------------------------

void dicom_series::add_computed_tag_value(const std::string& _tag_name, const std::string& _value)
{
    m_computed_tag_values[_tag_name] = _value;
}

//------------------------------------------------------------------------------

bool dicom_series::has_computed_values(const std::string& _tag_name) const
{
    return m_computed_tag_values.find(_tag_name) != m_computed_tag_values.end();
}

//------------------------------------------------------------------------------

bool dicom_series::operator==(const dicom_series& _other) const noexcept
{
    if(m_number_of_instances != _other.m_number_of_instances
       || m_sop_class_ui_ds != _other.m_sop_class_ui_ds
       || m_computed_tag_values != _other.m_computed_tag_values
       || m_first_instance_number != _other.m_first_instance_number
       || !core::tools::is_equal(m_dicom_container, _other.m_dicom_container))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool dicom_series::operator!=(const dicom_series& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
