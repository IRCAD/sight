/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_POLICY_REGISTRY_MACROS_HPP__
#define __FWMEMORY_POLICY_REGISTRY_MACROS_HPP__

#include <boost/preprocessor/cat.hpp>

#include "fwMemory/IPolicy.hpp"
#include "fwMemory/policy/registry/detail.hpp"

namespace fwMemory
{
namespace policy
{
namespace registry
{

#define fwMemoryPolicyRegisterMacro( classname )                                  \
    static ::fwMemory::IPolicy::Registrar< classname > s__factory__record__ ## __LINE__;

} // namespace registry
} // namespace policy
} // namespace fwMemory

#endif /*__FWMEMORY_POLICY_REGISTRY_MACROS_HPP__*/
