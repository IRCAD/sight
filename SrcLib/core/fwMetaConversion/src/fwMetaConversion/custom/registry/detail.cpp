/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/util/LazyInstantiator.hpp>

#include "fwMetaConversion/custom/registry/detail.hpp"


namespace fwMetaConversion
{
namespace custom
{
namespace registry
{

struct FwMetaConversionRegistryInstantiatorTag {} ;

SPTR(Type) get()
{
    typedef ::fwCore::util::LazyInstantiator< Type, FwMetaConversionRegistryInstantiatorTag > InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace registry
} // namespace custom
} // namespace fwMetaConversion


