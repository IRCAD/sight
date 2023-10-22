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

#include "data/structure_traits.hpp"

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::structure_traits);

namespace sight::data
{

//------------------------------------------------------------------------------

structure_traits::structure_traits() :
    m_color(std::make_shared<data::color>())
{
}

//------------------------------------------------------------------------------

void structure_traits::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const structure_traits>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_type               = other->m_type;
    m_categories         = other->m_categories;
    m_color              = other->m_color;
    m_class              = other->m_class;
    m_nativeExp          = other->m_nativeExp;
    m_nativeGeometricExp = other->m_nativeGeometricExp;
    m_attachmentType     = other->m_attachmentType;
    m_anatomicRegion     = other->m_anatomicRegion;
    m_propertyCategory   = other->m_propertyCategory;
    m_propertyType       = other->m_propertyType;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void structure_traits::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const structure_traits>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_type               = other->m_type;
    m_categories         = other->m_categories;
    m_color              = data::object::copy(other->m_color, _cache);
    m_class              = other->m_class;
    m_nativeExp          = other->m_nativeExp;
    m_nativeGeometricExp = other->m_nativeGeometricExp;
    m_attachmentType     = other->m_attachmentType;
    m_anatomicRegion     = other->m_anatomicRegion;
    m_propertyCategory   = other->m_propertyCategory;
    m_propertyType       = other->m_propertyType;

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool structure_traits::operator==(const structure_traits& _other) const noexcept
{
    if(m_type != _other.m_type
       || m_class != _other.m_class
       || m_nativeExp != _other.m_nativeExp
       || m_nativeGeometricExp != _other.m_nativeGeometricExp
       || m_attachmentType != _other.m_attachmentType
       || m_anatomicRegion != _other.m_anatomicRegion
       || m_propertyCategory != _other.m_propertyCategory
       || m_propertyType != _other.m_propertyType
       || !core::tools::is_equal(m_categories, _other.m_categories)
       || !core::tools::is_equal(m_color, _other.m_color))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool structure_traits::operator!=(const structure_traits& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
