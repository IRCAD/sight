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

structure_traits::sptr structure_traits_dictionary::getStructure(std::string type)
{
    SIGHT_ASSERT(
        "Structure of type '" + type + "' not found",
        m_structureTraitsMap.find(type) != m_structureTraitsMap.end()
    );
    return m_structureTraitsMap[type];
}

//------------------------------------------------------------------------------

structure_traits::csptr structure_traits_dictionary::getStructure(std::string type) const
{
    SIGHT_ASSERT(
        "Structure of type '" + type + "' not found",
        m_structureTraitsMap.find(type) != m_structureTraitsMap.end()
    );
    return m_structureTraitsMap.at(type);
}

//------------------------------------------------------------------------------

void structure_traits_dictionary::addStructure(structure_traits::sptr structureTraits)
{
    std::string type                             = structureTraits->getType();
    structure_traits::StructureClass structClass = structureTraits->getClass();
    std::string attachment                       = structureTraits->getAttachmentType();

    SIGHT_THROW_IF(
        "Structure of type '" << type << "' already exist",
        m_structureTraitsMap.find(type) != m_structureTraitsMap.end()
    );

    SIGHT_THROW_IF(
        "Structure of class '" << structClass << "' can not have attachment",
        !(attachment.empty() || structClass == structure_traits::LESION || structClass
          == structure_traits::FUNCTIONAL)
    );

    SIGHT_THROW_IF(
        "Structure attachment '" << attachment << "' not found in dictionary",
        !(attachment.empty() || m_structureTraitsMap.find(attachment) != m_structureTraitsMap.end())
    );

    SIGHT_THROW_IF(
        "Structure attachment '" << attachment << "' must be of class ORGAN",
        !(attachment.empty() || m_structureTraitsMap[attachment]->getClass() == structure_traits::ORGAN)
    );

    SIGHT_THROW_IF(
        "Structure must have at least one category",
        structureTraits->getCategories().empty()
    );

    SIGHT_THROW_IF(
        "Wrong structure type '" << type << "', a type cannot contain space",
        structureTraits->getType().find(' ') != std::string::npos
    );

    m_structureTraitsMap[type] = structureTraits;
}

//------------------------------------------------------------------------------

structure_traits_dictionary::StructureTypeNameContainer structure_traits_dictionary::getStructureTypeNames() const
{
    StructureTypeNameContainer vectNames;
    std::transform(
        m_structureTraitsMap.begin(),
        m_structureTraitsMap.end(),
        std::back_inserter(vectNames),
        [](const auto& e){return e.first;});
    return vectNames;
}

//------------------------------------------------------------------------------

DATA_API void structure_traits_dictionary::setStructureTraitsMap(const StructureTraitsMapType& structureTraitsMap)
{
    m_structureTraitsMap = structureTraitsMap;
}

//------------------------------------------------------------------------------

void structure_traits_dictionary::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const structure_traits_dictionary>(source);

    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_structureTraitsMap = other->m_structureTraitsMap;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void structure_traits_dictionary::deep_copy(
    const object::csptr& source,
    const std::unique_ptr<deep_copy_cache_t>& cache
)
{
    const auto& other = std::dynamic_pointer_cast<const structure_traits_dictionary>(source);

    SIGHT_THROW_EXCEPTION_IF(
        data::exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_structureTraitsMap.clear();
    for(const StructureTraitsMapType::value_type& elt : other->m_structureTraitsMap)
    {
        m_structureTraitsMap[elt.first] = data::object::copy(elt.second, cache);
    }

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool structure_traits_dictionary::operator==(const structure_traits_dictionary& other) const noexcept
{
    if(!core::tools::is_equal(m_structureTraitsMap, other.m_structureTraitsMap))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool structure_traits_dictionary::operator!=(const structure_traits_dictionary& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
