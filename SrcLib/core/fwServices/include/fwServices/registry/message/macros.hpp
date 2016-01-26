/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_MESSAGE_MACROS_HPP__
#define __FWSERVICES_REGISTRY_MESSAGE_MACROS_HPP__

#include "fwServices/registry/message/detail.hpp"

namespace fwServices
{
namespace registry
{
namespace message
{

#define fwServicesMessageRegisterMacro( classname )                                  \
    static ::fwServices::ObjectMsg::Registrar< classname > s__factory__record__ ## __LINE__;

} // end namespace message
} // end namespace registry
} // end namespace fwServices

#endif /*__FWSERVICES_REGISTRY_MESSAGE_MACROS_HPP__*/
