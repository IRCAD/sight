/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/structure_traits_dictionary.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

#include <functional>

SIGHT_REGISTER_DATA(sight::data::structure_traits_dictionary);

namespace sight::data
{

//------------------------------------------------------------------------------

structure_traits::sptr structure_traits_dictionary::get_structure(std::string _type)
{
    SIGHT_ASSERT(
        "Structure of type '" + _type + "' not found",
        m_structure_traits_map.find(_type) != m_structure_traits_map.end()
    );
    return m_structure_traits_map[_type];
}

//------------------------------------------------------------------------------

structure_traits::csptr structure_traits_dictionary::get_structure(std::string _type) const
{
    SIGHT_ASSERT(
        "Structure of type '" + _type + "' not found",
        m_structure_traits_map.find(_type) != m_structure_traits_map.end()
    );
    return m_structure_traits_map.at(_type);
}

//------------------------------------------------------------------------------

void structure_traits_dictionary::add_structure(structure_traits::sptr _structure_traits)
{
    std::string type                               = _structure_traits->type();
    structure_traits::structure_class struct_class = _structure_traits->get_class();
    std::string attachment                         = _structure_traits->get_attachment_type();

    SIGHT_THROW_IF(
        "Structure of type '" << type << "' already exist",
        m_structure_traits_map.find(type) != m_structure_traits_map.end()
    );

    SIGHT_THROW_IF(
        "Structure of class '" << struct_class << "' can not have attachment",
        !(attachment.empty() || struct_class == structure_traits::lesion || struct_class
          == structure_traits::functional)
    );

    SIGHT_THROW_IF(
        "Structure attachment '" << attachment << "' not found in dictionary",
        !(attachment.empty() || m_structure_traits_map.find(attachment) != m_structure_traits_map.end())
    );

    SIGHT_THROW_IF(
        "Structure attachment '" << attachment << "' must be of class ORGAN",
        !(attachment.empty() || m_structure_traits_map[attachment]->get_class() == structure_traits::organ)
    );

    SIGHT_THROW_IF(
        "Structure must have at least one category",
        _structure_traits->get_categories().empty()
    );

    SIGHT_THROW_IF(
        "Wrong structure type '" << type << "', a type cannot contain space",
        _structure_traits->type().find(' ') != std::string::npos
    );

    m_structure_traits_map[type] = _structure_traits;
}

//------------------------------------------------------------------------------

structure_traits_dictionary::StructureTypeNameContainer structure_traits_dictionary::get_structure_type_names() const
{
    StructureTypeNameContainer vect_names;
    std::transform(
        m_structure_traits_map.begin(),
        m_structure_traits_map.end(),
        std::back_inserter(vect_names),
        [](const auto& _e){return _e.first;});
    return vect_names;
}

//------------------------------------------------------------------------------

DATA_API void structure_traits_dictionary::set_structure_traits_map(const structure_traits_map_t& _structure_traits_map)
{
    m_structure_traits_map = _structure_traits_map;
}

//------------------------------------------------------------------------------

void structure_traits_dictionary::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const structure_traits_dictionary>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_structure_traits_map = other->m_structure_traits_map;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void structure_traits_dictionary::deep_copy(
    const object::csptr& _source,
    const std::unique_ptr<deep_copy_cache_t>& _cache
)
{
    const auto& other = std::dynamic_pointer_cast<const structure_traits_dictionary>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_structure_traits_map.clear();
    for(const auto& elt : other->m_structure_traits_map)
    {
        m_structure_traits_map[elt.first] = data::object::copy(elt.second, _cache);
    }

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool structure_traits_dictionary::operator==(const structure_traits_dictionary& _other) const noexcept
{
    if(!core::tools::is_equal(m_structure_traits_map, _other.m_structure_traits_map))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool structure_traits_dictionary::operator!=(const structure_traits_dictionary& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
