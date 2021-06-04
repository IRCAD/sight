/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "atoms/config.hpp"
#include "atoms/conversion/AtomVisitor.hpp"
#include "atoms/conversion/DataVisitor.hpp"

namespace sight::atoms
{

class Object;

}
namespace sight::data
{

class Object;

}

namespace sight::atoms::conversion
{

/**
 * @brief Helper to convert a data::Object to a atoms::Object
 *
 * @param data data object to convert
 * @return Converted atoms::Object
 *
 * @throw ::camp::ClassNotFound if data class is not found in camp world during visit
 * @throw atoms::conversion::exception::ConversionNotManaged if data has attribute type not managed by algorithm
 *
 * @note All data must be camped or mapped by a specific atoms::conversion::mapper::Base
 * @note fwData camp property names are used like key to store attributes in atoms::Object
 * @note sub objects multi-referenced in the fwData structure are represented by the same atoms::Object
 * ( also multi-referenced in fwAtoms structure )
 * @note null data::Object::sptr are kept and are converted in null atoms::Base::sptr
 * @note Only std::vector of type data::Object::sptr, enum, bool, string, int or real are managed
 * @note Only std::map with key value : string, int or real are managed
 * @note Only std::map with type value : data::Object::sptr, bool, enum, string, int or real are managed
 */
ATOMS_API SPTR(atoms::Object) convert(const SPTR(data::Object) & data);

/**
 * @brief Converts a data::Object to a atoms::Object.
 * @param dataObj data to convert
 * @param cache   cache to register already converted data, used when a data is referenced multiple times.
 * @return Returns the converted atom.
 * @throw ::camp::ClassNotFound if data class is not found in camp world during visit
 */
ATOMS_API SPTR(atoms::Object) convert(
    const SPTR(data::Object) & dataObj,
    DataVisitor::AtomCacheType& cache
);

/**
 * @brief Helper to convert a atoms::Object to a data::Object
 *
 * @param atom atom to convert
 * @param uuidPolicy AtomVisitor policy
 * @return Converted data::Object
 *
 * @throw ::camp::ClassNotFound if data class is not found in camp world during visit
 * @throw atoms::conversion::exception::DataFactoryNotFound if the data class is not found in data::factory
 * @throw atoms::conversion::exception::DuplicatedDataUUID if the data uuid already exists in the system.
 * @throw atoms::conversion::exception::ConversionNotManaged if atom is not data compliant
 *
 * @note see notes of convert(data::Object::sptr data) method
 */
ATOMS_API SPTR(data::Object) convert(
    const SPTR(atoms::Object) & atom,
    const AtomVisitor::IReadPolicy& uuidPolicy = AtomVisitor::ChangePolicy()
);

/**
 * @brief Converts a atoms::Object to a data::Object
 *
 * @param atomObj atom to convert
 * @param cache  cache to register the atoms already converted, used when an atom is referenced multiple times
 * @param uuidPolicy AtomVisitor policy
 * @return Converted data::Object.
 *
 * @throw atoms::conversion::exception::DataFactoryNotFound if the data class is not found in data::factory
 * @throw atoms::conversion::exception::DuplicatedDataUUID if the data uuid already exists in the system.
 */
ATOMS_API SPTR(data::Object) convert(
    const SPTR(atoms::Object) & atomObj,
    AtomVisitor::DataCacheType& cache,
    const AtomVisitor::IReadPolicy& uuidPolicy
);

} // end namespace sight::atoms::conversion
