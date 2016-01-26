/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_REGISTRY_MACROS_HPP__
#define __FWATOMS_REGISTRY_MACROS_HPP__

#include "fwAtoms/registry/detail.hpp"

namespace fwAtoms
{
namespace registry
{

#define fwAtomsRegisterMacro( classname )                                  \
    static ::fwAtoms::Base::Registrar< classname > s__factory__record__ ## __LINE__;

} // end namespace registry
} // end namespace fwAtoms

#endif /*__FWATOMS_REGISTRY_MACROS_HPP__*/

