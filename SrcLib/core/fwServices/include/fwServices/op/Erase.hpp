/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_ERASE_HPP_
#define _FWSERVICES_ERASE_HPP_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @name Methods for erasing services
 */
//@{

/**
 * @brief Erase service instance from the system
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API void erase( ::fwServices::IService::sptr service ) ;

/**
 * @brief Erase all services of type serviceId which are attached to obj
 * @note the template method can be used, where the template parameter SERVICE is related to serviceId
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API void eraseServices( ::fwTools::Object::sptr obj , std::string serviceId ) ;

template<class SERVICE>
void eraseServices( ::fwTools::Object::sptr obj ) ;


//@}


}

#include "fwServices/op/Erase.hxx"

#endif /*_FWSERVICES_ERASE_HPP_*/
