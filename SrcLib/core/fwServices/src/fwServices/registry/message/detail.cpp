/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/util/LazyInstantiator.hpp>

#include "fwServices/registry/message/detail.hpp"

template class  ::fwCore::util::LazyInstantiator< ::fwServices::registry::message::Type >;

namespace fwServices
{

namespace registry
{

namespace message
{


struct FwServicesRegistryMessageLazyInstantiatorTag {} ;

SPTR( Type ) get()
{
    typedef ::fwCore::util::LazyInstantiator< Type, FwServicesRegistryMessageLazyInstantiatorTag > InstantiatorType;
    return InstantiatorType::getInstance();
}

} // namespace message

} // namespace registry

} // namespace fwServices


