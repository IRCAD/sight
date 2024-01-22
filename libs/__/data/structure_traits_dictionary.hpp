/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

    using StructureTypeNameContainer = std::vector<std::string>;
    using structure_traits_map_t     = std::map<std::string, structure_traits::sptr>;

    /// Destructor. Does nothing.
    DATA_API ~structure_traits_dictionary() noexcept override = default;

    /**
     * @brief Add a structure in dictionary
     * @param [in] _structure_traits a structure_traits correctly initialized.
     * @pre A structure with the same type cannot exist in dictionary.
     * @pre Only structure of class 'Lesion' or 'Functional' can have attachment.
     * @pre Structure attachment must be of class 'Organ' and must exist in dictionary.
     * @pre Structure type cannot contain space.
     */
    DATA_API void add_structure(structure_traits::sptr _structure_traits);

    /**
     * @brief Return the structure traits for given type
     * @pre given 'type' must exist in dictionary
     */
    DATA_API structure_traits::sptr get_structure(std::string _type);
    DATA_API structure_traits::csptr get_structure(std::string _type) const;

    /// Return all array names stock in the structureTraits-map
    DATA_API StructureTypeNameContainer get_structure_type_names() const;

    /// sets the entire structures map
    DATA_API void set_structure_traits_map(const structure_traits_map_t& _structure_traits_map);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const structure_traits_dictionary& _other) const noexcept;
    DATA_API bool operator!=(const structure_traits_dictionary& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

private:

    /// map to register structure traits. Map key is structure type.
    structure_traits_map_t m_structure_traits_map;
};

} // namespace sight::data
