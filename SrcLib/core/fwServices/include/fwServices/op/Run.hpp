/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_RUN_HPP_
#define _FWSERVICES_RUN_HPP_

#include <fwTools/Object.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/ComChannelService.hpp"

namespace fwServices
{

/**
 * @name Methods to start, stop or update of set of service
 */

//@{

/**
 * @brief start services specified in the XML like structure (_elt) and already attached to objects
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API void start( ::fwRuntime::ConfigurationElement::sptr _elt) ;

/**
 * @brief update services specified in the XML like structure (_elt) and already attached to objects
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API void update( ::fwRuntime::ConfigurationElement::sptr _elt) ;

/**
 * @brief stop services specified in the XML like structure (_elt) and already attached to objects
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API void stop( ::fwRuntime::ConfigurationElement::sptr _elt) ;

/**
 * @brief stop services specified in the XML like structure (_elt) and unregister CommChannel
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API void stopAndUnregister( ::fwRuntime::ConfigurationElement::sptr _elt);

/**
 * @brief start all obj services
 * @author IRCAD (Research and Development Team).
 */
FWSERVICES_API  void startServices( ::fwTools::Object::sptr obj ) ;

//@}


}

#endif /*HELPER_HPP_*/
