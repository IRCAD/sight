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

#include "data/StructureTraits.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::StructureTraits);

namespace sight::data
{

//------------------------------------------------------------------------------

StructureTraits::StructureTraits() :
    m_color(std::make_shared<data::Color>())
{
}

//------------------------------------------------------------------------------

void StructureTraits::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const StructureTraits>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
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

void StructureTraits::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const StructureTraits>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_type               = other->m_type;
    m_categories         = other->m_categories;
    m_color              = data::Object::copy(other->m_color, cache);
    m_class              = other->m_class;
    m_nativeExp          = other->m_nativeExp;
    m_nativeGeometricExp = other->m_nativeGeometricExp;
    m_attachmentType     = other->m_attachmentType;
    m_anatomicRegion     = other->m_anatomicRegion;
    m_propertyCategory   = other->m_propertyCategory;
    m_propertyType       = other->m_propertyType;

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool StructureTraits::operator==(const StructureTraits& other) const noexcept
{
    if(m_type != other.m_type
       || m_class != other.m_class
       || m_nativeExp != other.m_nativeExp
       || m_nativeGeometricExp != other.m_nativeGeometricExp
       || m_attachmentType != other.m_attachmentType
       || m_anatomicRegion != other.m_anatomicRegion
       || m_propertyCategory != other.m_propertyCategory
       || m_propertyType != other.m_propertyType
       || !core::tools::is_equal(m_categories, other.m_categories)
       || !core::tools::is_equal(m_color, other.m_color))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool StructureTraits::operator!=(const StructureTraits& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
