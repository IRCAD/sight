/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/StructureTraitsDictionary.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

#include <functional>

SIGHT_REGISTER_DATA(sight::data::StructureTraitsDictionary);

namespace sight::data
{

//------------------------------------------------------------------------------

StructureTraitsDictionary::StructureTraitsDictionary(data::Object::Key)
{
}

//------------------------------------------------------------------------------

StructureTraitsDictionary::~StructureTraitsDictionary()
{
}

//------------------------------------------------------------------------------

StructureTraits::sptr StructureTraitsDictionary::getStructure(std::string type)
{
    SIGHT_ASSERT(
        "Structure of type '" + type + "' not found",
        m_structureTraitsMap.find(type) != m_structureTraitsMap.end()
    );
    return m_structureTraitsMap[type];
}

//------------------------------------------------------------------------------

StructureTraits::csptr StructureTraitsDictionary::getStructure(std::string type) const
{
    SIGHT_ASSERT(
        "Structure of type '" + type + "' not found",
        m_structureTraitsMap.find(type) != m_structureTraitsMap.end()
    );
    return m_structureTraitsMap.at(type);
}

//------------------------------------------------------------------------------

void StructureTraitsDictionary::addStructure(StructureTraits::sptr structureTraits)
{
    std::string type                            = structureTraits->getType();
    StructureTraits::StructureClass structClass = structureTraits->getClass();
    std::string attachment                      = structureTraits->getAttachmentType();

    SIGHT_THROW_IF(
        "Structure of type '" << type << "' already exist",
        m_structureTraitsMap.find(type) != m_structureTraitsMap.end()
    );

    SIGHT_THROW_IF(
        "Structure of class '" << structClass << "' can not have attachment",
        !(attachment.empty() || structClass == StructureTraits::LESION || structClass
          == StructureTraits::FUNCTIONAL)
    );

    SIGHT_THROW_IF(
        "Structure attachment '" << attachment << "' not found in dictionary",
        !(attachment.empty() || m_structureTraitsMap.find(attachment) != m_structureTraitsMap.end())
    );

    SIGHT_THROW_IF(
        "Structure attachment '" << attachment << "' must be of class ORGAN",
        !(attachment.empty() || m_structureTraitsMap[attachment]->getClass() == StructureTraits::ORGAN)
    );

    SIGHT_THROW_IF(
        "Structure must have at least one category",
        structureTraits->getCategories().empty()
    );

    SIGHT_THROW_IF(
        "Wrong structure type '" << type << "', a type cannot contain space",
        structureTraits->getType().find(" ") != std::string::npos
    );

    m_structureTraitsMap[type] = structureTraits;
}

//------------------------------------------------------------------------------

StructureTraitsDictionary::StructureTypeNameContainer StructureTraitsDictionary::getStructureTypeNames() const
{
    StructureTypeNameContainer vectNames;
    std::transform(
        m_structureTraitsMap.begin(),
        m_structureTraitsMap.end(),
        std::back_inserter(vectNames),
        std::bind(&StructureTraitsMapType::value_type::first, std::placeholders::_1)
    );
    return vectNames;
}

//------------------------------------------------------------------------------

DATA_API void StructureTraitsDictionary::setStructureTraitsMap(const StructureTraitsMapType& structureTraitsMap)
{
    m_structureTraitsMap = structureTraitsMap;
}

//------------------------------------------------------------------------------

void StructureTraitsDictionary::shallowCopy(const Object::csptr& source)
{
    StructureTraitsDictionary::csptr other = StructureTraitsDictionary::dynamicConstCast(source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(source);
    m_structureTraitsMap = other->m_structureTraitsMap;
}

//------------------------------------------------------------------------------

void StructureTraitsDictionary::cachedDeepCopy(const Object::csptr& source, DeepCopyCacheType& cache)
{
    StructureTraitsDictionary::csptr other = StructureTraitsDictionary::dynamicConstCast(source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(source, cache);
    m_structureTraitsMap.clear();
    for(const StructureTraitsMapType::value_type& elt : other->m_structureTraitsMap)
    {
        m_structureTraitsMap[elt.first] = data::Object::copy(elt.second, cache);
    }
}

//------------------------------------------------------------------------------

bool StructureTraitsDictionary::operator==(const StructureTraitsDictionary& other) const noexcept
{
    if(!core::tools::is_equal(m_structureTraitsMap, other.m_structureTraitsMap))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool StructureTraitsDictionary::operator!=(const StructureTraitsDictionary& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
