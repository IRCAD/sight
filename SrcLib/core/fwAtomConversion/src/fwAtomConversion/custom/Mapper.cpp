/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/AtomVisitor.hpp>

#include "fwAtomConversion/custom/Mapper.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/policy/Data.hpp"

namespace fwAtomConversion
{
namespace custom
{



::fwAtoms::Object::sptr
Mapper::computeMinimalInfos(::fwData::Object::sptr object,
                            ::fwAtomConversion::AtomHelper& metaHelper)
{
    const camp::Class& metaclass = camp::classByName(object->getClassname());
    ::fwAtomConversion::DataVisitor visitor(object, metaHelper);

    metaclass.visit(visitor);

    return visitor.getAtomObject();
}

::fwData::Object::sptr
Mapper::computeMinimalInfos(::fwAtoms::Object::sptr object,
                            ::fwAtomConversion::AtomHelper& metaHelper)
{
    ::fwAtomConversion::policy::Data policy(metaHelper);
    ::fwAtoms::AtomVisitor visitor(policy, object);
    visitor.visit();

    return policy.getObject();
}

}
}
