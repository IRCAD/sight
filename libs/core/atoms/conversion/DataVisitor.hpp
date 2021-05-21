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

#include <core/macros.hpp>
#include <core/reflection/camp/ExtendedClassVisitor.hpp>
#include <core/tools/UUID.hpp>

#include <map>

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
 * @brief Visitor used to convert a fwData to a fwAtoms. fwData camp property
 * names are used like key to store attributes in atoms::Object
 * @throw ::camp::ClassNotFound if data class is not found in camp world during visit
 */
class ATOMS_CLASS_API DataVisitor : public ::camp::ExtendedClassVisitor
{
public:

    typedef std::map<std::string, SPTR(atoms::Object)> AtomCacheType;

    typedef std::string ClassnameType;

    /// Key of the meta info to store data object classname
    ATOMS_API static const std::string CLASSNAME_METAINFO;

    /// Key of the meta info to store data object ID
    ATOMS_API static const std::string ID_METAINFO;

    /**
     * @brief Constructor. Initializes atom object and store it in the cache.
     *
     * Creates a new atoms::Object. Sets : ID from dataObj UUID, meta info
     * CLASSNAME_METAINFO from dataObj classname() and add tag information from camp data
     */
    ATOMS_API DataVisitor(SPTR(data::Object)dataObj, AtomCacheType& cache);

    /// Destructor. Does nothing.
    ATOMS_API virtual ~DataVisitor();

    /**
     * @brief Visit simple property
     * @todo Hack : problem with size_t conversion.
     */
    ATOMS_API void visit(const camp::SimpleProperty& property);

    /// Visit enum property. Uses atoms::String to store enum name (and not enum value)
    ATOMS_API void visit(const camp::EnumProperty& property);

    /// Visit user property. Null data::Object::sptr attribute is converted to null atoms::Base::sptr attributes
    ATOMS_API void visit(const camp::UserProperty& property);

    /// Visit array property. Null data::Object::sptr attribute is converted to null atoms::Base::sptr attributes
    ATOMS_API void visit(const camp::ArrayProperty& property);

    /**
     * @brief Visit map property. Null data::Object::sptr attribute is converted to
     * null atoms::Base::sptr attributes
     *
     * Only map with key of type enum, string, real or int are managed ( real and int are
     * converted in string ). In other cases, an assertion is raised.
     */
    ATOMS_API void visit(const camp::MapProperty& property);

    /// Returns the atom object (representation of dataObj in fwAtoms) . Calls this methods after the visit.
    ATOMS_API SPTR(atoms::Object) getAtomObject() const;

private:

    /// Reflection in camp world of m_dataObj
    ::camp::UserObject m_campDataObj;

    /// converted atom object.
    SPTR(atoms::Object) m_atomObj;

    /// cache to register already converted object. Used when a data is referenced several times.
    AtomCacheType& m_cache;
};

} // end namespace sight::atoms::conversion
