/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERICES_MONITOR_MONITOR_HPP_
#define _FWSERICES_MONITOR_MONITOR_HPP_

#include <sstream>
#include <map>
#include <list>

#include <fwTools/Object.hpp>

#include "fwServices/config.hpp"

namespace fwServices
{

/**
 * @brief       The namespace fwServices::monitor contains classes for monitoring purposes.
 * @namespace   fwServices::monitor
 * @author      IRCAD (Research and Development Team).
 * @date        2009-2010.
 *
 */

namespace monitor
{

    typedef std::map<std::string, std::list<std::string> > string_map ;
    typedef std::map< ::fwTools::Object::sptr , std::list<std::string> > map_object_servicesNames ;


/**
 * @brief helper for monitoring purposes
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API Monitor
{
public:

    /**
     * @brief return in a std::map< < Object::sptr > , std::list<std::string> >,
     *  a list of services names (map value) associated with instanciated Objects (map key).
     *  The services list can be empty.
     */
    FWSERVICES_API static ::fwServices::monitor::map_object_servicesNames scannerObjectServicesRegistry() ;

    /**
     * @brief return in a string all object TYPES and services (does not concerns object/service registry)
     * @author IRCAD (Research and Development Team).
     */
    FWSERVICES_API static ::fwServices::monitor::string_map scannerObjectServices() ;

    ~Monitor() ;

protected:

    Monitor() ;

};

} // namespace monitor

} // namespace fwServices

#endif // _FWSERICES_MONITOR_MONITOR_HPP_
