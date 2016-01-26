/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_REGISTRY_DETAIL_HPP__
#define __FWATOMS_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwAtoms/config.hpp"

namespace fwAtoms
{

class Base;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwAtoms::Base) (), KeyType > Type;

FWATOMS_API SPTR(Type) get();

} // namespace registry

} // namespace fwAtoms

#endif /* __FWATOMS_REGISTRY_DETAIL_HPP__ */


