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

#include "io/atoms/config.hpp"
#include "io/atoms/patch/conditions/Abstract.hpp"

#include <atoms/Object.hpp>

namespace sight::io::atoms::patch
{

namespace helper
{

namespace conditions = io::atoms::patch::conditions;

/**
 * @brief Object helper
 */
class IO_ATOMS_CLASS_API Object
{
public:

    /// Constructor
    IO_ATOMS_API Object(sight::atoms::Object::sptr metaObject);

    /// Destructor
    IO_ATOMS_API ~Object();

    /// Adds a new attribute in the current object
    IO_ATOMS_API void addAttribute(
        const std::string& name,
        sight::atoms::Base::sptr value,
        conditions::Abstract::sptr condition = conditions::Abstract::New()
    );

    /// Adds or replaces an attribute in the current object
    IO_ATOMS_API void addOrReplaceAttribute(
        const std::string& name,
        sight::atoms::Base::sptr value,
        conditions::Abstract::sptr condition = conditions::Abstract::New()
    );

    /// Removes an attribute
    IO_ATOMS_API void removeAttribute(
        const std::string& name,
        conditions::Abstract::sptr condition = conditions::Abstract::New()
    );

    /// Replaces an attribute
    IO_ATOMS_API void replaceAttribute(
        const std::string& name,
        sight::atoms::Base::sptr newValue,
        conditions::Abstract::sptr condition = conditions::Abstract::New()
    );

    /// Renames an attribute.
    IO_ATOMS_API void renameAttribute(
        const std::string& name,
        const std::string& newName,
        conditions::Abstract::sptr condition = conditions::Abstract::New()
    );

    /// Atom::Object getter
    IO_ATOMS_API sight::atoms::Object::sptr getObject() const;

private:

    /// Object
    sight::atoms::Object::sptr m_object;
};

} //helper

} //fwAtomHelper
