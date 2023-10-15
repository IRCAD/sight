/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include "data/factory/new.hpp"
#include "data/object.hpp"
#include "data/structure_traits.hpp"

namespace sight::data
{

/**
 * @brief This class defines a dictionary of structure traits.
 *
 * @note The dictionary cannot contain two structure with the same type.
 * @see structure_traits
 */
class DATA_CLASS_API structure_traits_dictionary : public object
{
public:

    SIGHT_DECLARE_CLASS(structure_traits_dictionary, object);

    typedef std::vector<std::string> StructureTypeNameContainer;
    typedef std::map<std::string, structure_traits::sptr> StructureTraitsMapType;

    /// Destructor. Does nothing.
    DATA_API ~structure_traits_dictionary() noexcept override = default;

    /**
     * @brief Add a structure in dictionary
     * @param [in] structureTraits a structure_traits correctly initialized.
     * @pre A structure with the same type cannot exist in dictionary.
     * @pre Only structure of class 'Lesion' or 'Functional' can have attachment.
     * @pre Structure attachment must be of class 'Organ' and must exist in dictionary.
     * @pre Structure type cannot contain space.
     */
    DATA_API void addStructure(structure_traits::sptr structureTraits);

    /**
     * @brief Return the structure traits for given type
     * @pre given 'type' must exist in dictionary
     */
    DATA_API structure_traits::sptr getStructure(std::string type);
    DATA_API structure_traits::csptr getStructure(std::string type) const;

    /// Return all array names stock in the structureTraits-map
    DATA_API StructureTypeNameContainer getStructureTypeNames() const;

    /// sets the entire structures map
    DATA_API void setStructureTraitsMap(const StructureTraitsMapType& structureTraitsMap);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const structure_traits_dictionary& other) const noexcept;
    DATA_API bool operator!=(const structure_traits_dictionary& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& source,
        const std::unique_ptr<deep_copy_cache_t>& cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    /// map to register structure traits. Map key is structure type.
    StructureTraitsMapType m_structureTraitsMap;
};

} // namespace sight::data
