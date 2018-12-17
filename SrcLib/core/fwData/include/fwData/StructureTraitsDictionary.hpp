/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWDATA_STRUCTURETRAITSDICTIONARY_HPP__
#define __FWDATA_STRUCTURETRAITSDICTIONARY_HPP__

#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"
#include "fwData/StructureTraits.hpp"

fwCampAutoDeclareDataMacro((fwData)(StructureTraitsDictionary), FWDATA_API);

namespace fwData
{
/**
 * @brief This class defines a dictionary of structure traits.
 *
 * @note The dictionary cannot contain two structure with the same type.
 * @see StructureTraits
 */
class FWDATA_CLASS_API StructureTraitsDictionary : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (StructureTraitsDictionary)(::fwData::Object), (()),
                                            ::fwData::factory::New< StructureTraitsDictionary >);
    fwCampMakeFriendDataMacro((fwData)(StructureTraitsDictionary));

    typedef std::vector<std::string> StructureTypeNameContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API StructureTraitsDictionary(::fwData::Object::Key key);

    /// Destructor. Does nothing.
    FWDATA_API virtual ~StructureTraitsDictionary();

    /**
     * @brief Add a structure in dictionary
     * @param [in] structureTraits a StructureTraits correctly initialized.
     * @pre A structure with the same type cannot exist in dictionary.
     * @pre Only structure of class 'Lesion' or 'Functional' can have attachment.
     * @pre Structure attachment must be of class 'Organ' and must exist in dictionary.
     * @pre Structure type cannot contain space.
     */
    FWDATA_API void addStructure(StructureTraits::sptr structureTraits);

    /**
     * @brief Return the structure traits for given type
     * @pre given 'type' must exist in dictionary
     */
    FWDATA_API StructureTraits::sptr getStructure(std::string type);

    /// Return all array names stock in the structureTraits-map
    FWDATA_API StructureTypeNameContainer getStructureTypeNames() const;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    typedef std::map< std::string, ::fwData::StructureTraits::sptr > StructureTraitsMapType;

    /// map to register structure traits. Map key is structure type.
    StructureTraitsMapType m_structureTraitsMap;

};

} // namespace fwData

#endif // __FWDATA_STRUCTURETRAITSDICTIONARY_HPP__

