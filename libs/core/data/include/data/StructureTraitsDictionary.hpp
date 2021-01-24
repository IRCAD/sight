/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
#include "data/Object.hpp"
#include "data/StructureTraits.hpp"

fwCampAutoDeclareDataMacro((sight)(data)(StructureTraitsDictionary));

namespace sight::data
{
/**
 * @brief This class defines a dictionary of structure traits.
 *
 * @note The dictionary cannot contain two structure with the same type.
 * @see StructureTraits
 */
class DATA_CLASS_API StructureTraitsDictionary : public data::Object
{
public:
    fwCoreClassMacro(StructureTraitsDictionary, data::Object, data::factory::New< StructureTraitsDictionary >)
    fwCampMakeFriendDataMacro((sight)(data)(StructureTraitsDictionary));

    typedef std::vector<std::string> StructureTypeNameContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API StructureTraitsDictionary(data::Object::Key key);

    /// Destructor. Does nothing.
    DATA_API virtual ~StructureTraitsDictionary();

    /**
     * @brief Add a structure in dictionary
     * @param [in] structureTraits a StructureTraits correctly initialized.
     * @pre A structure with the same type cannot exist in dictionary.
     * @pre Only structure of class 'Lesion' or 'Functional' can have attachment.
     * @pre Structure attachment must be of class 'Organ' and must exist in dictionary.
     * @pre Structure type cannot contain space.
     */
    DATA_API void addStructure(StructureTraits::sptr structureTraits);

    /**
     * @brief Return the structure traits for given type
     * @pre given 'type' must exist in dictionary
     */
    DATA_API StructureTraits::sptr getStructure(std::string type);

    /// Return all array names stock in the structureTraits-map
    DATA_API StructureTypeNameContainer getStructureTypeNames() const;

    /// Defines shallow copy
    DATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    typedef std::map< std::string, data::StructureTraits::sptr > StructureTraitsMapType;

    /// map to register structure traits. Map key is structure type.
    StructureTraitsMapType m_structureTraitsMap;

};

} // namespace sight::data
