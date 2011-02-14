/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_INFO_HPP_
#define _FWSERVICES_INFO_HPP_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @name Methods checking whether an object has or not a given service
 */

//@{

/**
 * @brief return true is obj has at least one service of type serviceId
 * @author IRCAD (Research and Development Team).

 */
FWSERVICES_API bool has( ::fwTools::Object::sptr obj , std::string serviceId) throw();

/**
 * @brief return true is the system has an object associated with uuid
 * @author IRCAD (Research and Development Team).

 */
FWSERVICES_API bool has( ::std::string uuid ) throw();

/**
 * @brief return true is obj has at least one service of type SERVICE
 * @author IRCAD (Research and Development Team).

 */
template<class SERVICE>
bool has( ::fwTools::Object::sptr obj) throw();

//@}

}

#include "fwServices/op/Info.hxx"

#endif /*_FWSERVICES_INFO_HPP_*/
