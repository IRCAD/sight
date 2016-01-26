/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWACTIVITIES_BUILDER_REGISTRY_DETAIL_HPP__
#define __FWACTIVITIES_BUILDER_REGISTRY_DETAIL_HPP__

#include <string>

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include "fwActivities/config.hpp"

namespace fwActivities
{

class IBuilder;

namespace builder
{
namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwActivities::IBuilder) (), KeyType > Type;

FWACTIVITIES_API SPTR(Type) get();

} // namespace registry
} // namespace builder
} // namespace fwActivities

#endif /* __FWACTIVITIES_BUILDER_REGISTRY_DETAIL_HPP__ */


