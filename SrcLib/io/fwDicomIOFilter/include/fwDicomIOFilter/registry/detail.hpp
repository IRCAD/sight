/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_REGISTRY_DETAIL_HPP__
#define __FWDICOMIOFILTER_REGISTRY_DETAIL_HPP__

#include "fwDicomIOFilter/config.hpp"

#include <fwCore/macros.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include <string>

namespace fwDicomIOFilter
{

class IFilter;

namespace registry
{

typedef std::string KeyType;

typedef ::fwCore::util::FactoryRegistry< SPTR(::fwDicomIOFilter::IFilter) (), KeyType > Type;

FWDICOMIOFILTER_API SPTR(Type) get();

} // namespace registry

} // namespace fwDicomIOFilter

#endif /* __FWDICOMIOFILTER_REGISTRY_DETAIL_HPP__ */


