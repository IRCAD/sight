/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
