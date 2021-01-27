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

#include <core/reflection/camp/ExtendedClassVisitor.hpp>

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
 * @brief This visitor visits data object to fill it with associated atom object attributes.
 * This class is used by AtomVisitor to convert an data::Object.
 */
class ATOMS_CLASS_API AtomToDataMappingVisitor : public ::camp::ExtendedClassVisitor
{

public:

    /// Constructor. Initializes visitor.
    ATOMS_API AtomToDataMappingVisitor(
        SPTR(data::Object)dataObj,
        SPTR(atoms::Object)atomObj,
        AtomVisitor::DataCacheType& cache,
        const AtomVisitor::IReadPolicy& uuidPolicy
        );

    /// Destructor. Does nothing.
    ATOMS_API virtual ~AtomToDataMappingVisitor();

    /// Visits data object simple property and fill it with associated atom attribute.
    ATOMS_API void visit(const camp::SimpleProperty& property);

    /// Visits data object enum property and fill it with associated atom attribute.
    ATOMS_API void visit(const camp::EnumProperty& property);

    /**
     * @brief Visits data object user property and fill it with associated atom attribute.
     * Manages null atoms::Base::sptr by inserting a null data::Object::sptr.
     */
    ATOMS_API void visit(const camp::UserProperty& property);

    /**
     * @brief Visits data object array property and fill it with associated atom attribute.
     * Manages null atoms::Base::sptr by inserting a null data::Object::sptr.
     *
     * Only array that contains atoms::Base::BOOLEAN, atoms::Base::NUMERIC, atoms::Base::STRING and
     * atoms::Base::OBJECT are managed.
     */
    ATOMS_API void visit(const camp::ArrayProperty& property);

    /**
     * @brief Visits data object map property and fill it with associated atom attribute.
     * Manages null atoms::Base::sptr by inserting a null data::Object::sptr.
     *
     * Only map that contains atoms::Base::BOOLEAN, atoms::Base::NUMERIC, atoms::Base::STRING and
     * atoms::Base::OBJECT are managed.
     */
    ATOMS_API void visit(const camp::MapProperty& property);

private:

    /// Converted data object
    SPTR(data::Object) m_dataObj;

    /// Reflection in camp world of m_dataObj
    ::camp::UserObject m_campDataObj;

    /// Atom object to convert
    SPTR(atoms::Object) m_atomObj;

    /// Cache to register the atoms already converted, used when an atom is referenced multiple times.
    AtomVisitor::DataCacheType& m_cache;

    /// Atom visitor uuids policy
    const AtomVisitor::IReadPolicy& m_uuidPolicy;
};

} // end namespace sight::atoms::conversion
