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

#ifndef __FWATOMSPATCH_HELPER_OBJECT_HPP__
#define __FWATOMSPATCH_HELPER_OBJECT_HPP__

#include <fwAtoms/Object.hpp>

#include "fwAtomsPatch/conditions/Abstract.hpp"
#include "fwAtomsPatch/config.hpp"

namespace fwAtomsPatch
{
namespace helper
{

namespace conditions = ::fwAtomsPatch::conditions;

/**
 * @class Object
 * @brief Object helper
 */
class FWATOMSPATCH_CLASS_API Object
{
public:

    /// Constructor
    FWATOMSPATCH_API Object(::fwAtoms::Object::sptr metaObject );

    /// Destructor
    FWATOMSPATCH_API ~Object();

    /// Adds a new attribute in the current object
    FWATOMSPATCH_API void addAttribute(const std::string& name,
                                       ::fwAtoms::Base::sptr value,
                                       conditions::Abstract::sptr condition = conditions::Abstract::New());

    /// Adds or replaces an attribute in the current object
    FWATOMSPATCH_API void addOrReplaceAttribute(const std::string& name,
                                                ::fwAtoms::Base::sptr value,
                                                conditions::Abstract::sptr condition = conditions::Abstract::New());

    /// Removes an attribute
    FWATOMSPATCH_API void removeAttribute(const std::string& name,
                                          conditions::Abstract::sptr condition = conditions::Abstract::New());

    /// Replaces an attribute
    FWATOMSPATCH_API void replaceAttribute(const std::string& name,
                                           ::fwAtoms::Base::sptr newValue,
                                           conditions::Abstract::sptr condition = conditions::Abstract::New());

    /// Renames an attribute.
    FWATOMSPATCH_API void renameAttribute(const std::string& name, const std::string& newName,
                                          conditions::Abstract::sptr condition = conditions::Abstract::New()
                                          );

    /// Atom::Object getter
    FWATOMSPATCH_API ::fwAtoms::Object::sptr getObject() const;

private:

    /// Object
    ::fwAtoms::Object::sptr m_object;
};

} //helper
} //fwAtomHelper

#endif /* __FWATOMSPATCH_HELPER_OBJECT_HPP__ */
