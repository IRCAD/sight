/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwAtomsPatch/conditions/Abstract.hpp"

namespace fwAtomsPatch
{
namespace conditions
{

Abstract::Abstract(::fwAtoms::Base::sptr defaultValue)
    : m_defaultValue(defaultValue)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Base::sptr Abstract::getDefaultValue()
{
    return m_defaultValue;
}

// ----------------------------------------------------------------------------

bool Abstract::test(::fwAtoms::Base::sptr old, ::fwAtoms::Base::sptr newValue)
{
    return true;
}

// ----------------------------------------------------------------------------

bool Abstract::test(::fwAtoms::Base::sptr value)
{
    return true;
}

} //conditions

} //fwAtomsPatch


