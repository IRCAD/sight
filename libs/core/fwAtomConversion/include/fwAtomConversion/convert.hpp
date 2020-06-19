/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWATOMCONVERSION_CONVERT_HPP__
#define __FWATOMCONVERSION_CONVERT_HPP__

#include "fwAtomConversion/config.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomVisitor.hpp"

namespace fwAtoms
{
class Object;
}
namespace fwData
{
class Object;
}

namespace fwAtomConversion
{

/**
 * @brief Helper to convert a ::fwData::Object to a ::fwAtoms::Object
 *
 * @param data data object to convert
 * @return Converted ::fwAtoms::Object
 *
 * @throw ::camp::ClassNotFound if data class is not found in camp world during visit
 * @throw ::fwAtomConversion::exception::ConversionNotManaged if data has attribute type not managed by algorithm
 *
 * @note All data must be camped or mapped by a specific ::fwAtomConversion::mapper::Base
 * @note fwData camp property names are used like key to store attributes in fwAtoms::Object
 * @note sub objects multi-referenced in the fwData structure are represented by the same fwAtoms::Object
 * ( also multi-referenced in fwAtoms structure )
 * @note null ::fwData::Object::sptr are kept and are converted in null ::fwAtoms::Base::sptr
 * @note Only std::vector of type fwData::Object::sptr, enum, bool, string, int or real are managed
 * @note Only std::map with key value : string, int or real are managed
 * @note Only std::map with type value : fwData::Object::sptr, bool, enum, string, int or real are managed
 */
FWATOMCONVERSION_API SPTR(::fwAtoms::Object) convert( const SPTR(::fwData::Object) &data );

/**
 * @brief Converts a ::fwData::Object to a ::fwAtoms::Object.
 * @param dataObj data to convert
 * @param cache   cache to register already converted data, used when a data is referenced multiple times.
 * @return Returns the converted atom.
 * @throw ::camp::ClassNotFound if data class is not found in camp world during visit
 */
FWATOMCONVERSION_API SPTR(::fwAtoms::Object) convert( const SPTR(::fwData::Object) &dataObj,
                                                      DataVisitor::AtomCacheType & cache );

/**
 * @brief Helper to convert a ::fwAtoms::Object to a ::fwData::Object
 *
 * @param atom atom to convert
 * @param uuidPolicy AtomVisitor policy
 * @return Converted ::fwData::Object
 *
 * @throw ::camp::ClassNotFound if data class is not found in camp world during visit
 * @throw ::fwAtomConversion::exception::DataFactoryNotFound if the data class is not found in ::fwData::factory
 * @throw ::fwAtomConversion::exception::DuplicatedDataUUID if the data uuid already exists in the system.
 * @throw ::fwAtomConversion::exception::ConversionNotManaged if atom is not data compliant
 *
 * @note see notes of convert(::fwData::Object::sptr data) method
 */
FWATOMCONVERSION_API SPTR(::fwData::Object) convert(
    const SPTR(::fwAtoms::Object) &atom,
    const AtomVisitor::IReadPolicy &uuidPolicy = AtomVisitor::ChangePolicy()
    );

/**
 * @brief Converts a ::fwAtoms::Object to a ::fwData::Object
 *
 * @param atomObj atom to convert
 * @param cache  cache to register the atoms already converted, used when an atom is referenced multiple times
 * @param uuidPolicy AtomVisitor policy
 * @return Converted ::fwData::Object.
 *
 * @throw ::fwAtomConversion::exception::DataFactoryNotFound if the data class is not found in ::fwData::factory
 * @throw ::fwAtomConversion::exception::DuplicatedDataUUID if the data uuid already exists in the system.
 */
FWATOMCONVERSION_API SPTR(::fwData::Object) convert( const SPTR(::fwAtoms::Object) &atomObj,
                                                     AtomVisitor::DataCacheType & cache,
                                                     const AtomVisitor::IReadPolicy &uuidPolicy );

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_CONVERT_HPP__
