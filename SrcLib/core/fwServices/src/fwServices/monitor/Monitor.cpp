/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/monitor/Monitor.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/helper.hpp"
#include <fwCore/base.hpp>
#include <set>
#include <map>
#include <fwTools/Object.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>

namespace fwServices
{

namespace monitor
{
::fwServices::monitor::map_object_servicesNames Monitor::scannerObjectServicesRegistry()
{
    ::fwServices::monitor::map_object_servicesNames map_string ;

    std::vector< ::fwTools::Object::sptr > objects = fwServices::ObjectServiceRegistry::getDefault()->getObjects();
    std::vector< ::fwTools::Object::sptr >::iterator    iter = objects.begin() ;
    for( iter = objects.begin() ; iter != objects.end() ; ++iter )
    {
        map_string[ *iter ] = std::list< std::string > (0) ;

        std::vector< ::fwServices::IService::sptr > lservices = fwServices::getRegisteredServices( *iter ) ;
        std::vector< ::fwServices::IService::sptr >::iterator serviceIter ;
        for( serviceIter = lservices.begin() ; serviceIter != lservices.end() ; ++serviceIter )
        {
            std::stringstream valueStream ;
            valueStream << *serviceIter << " " << *((*serviceIter));
            map_string[*iter ].push_back(valueStream.str());
        }
    }

    return map_string ;
}


::fwServices::monitor::string_map Monitor::scannerObjectServices()
{
    std::stringstream myStream ;
    ::fwServices::monitor::string_map map_string ;

    typedef std::set< std::string > setType ;
    typedef std::map< std::string , setType > mapType;
    mapType mapObjectNameFactoryName ;
    setType setObjectNames;
    std::list< ::fwTools::Object::sptr > myObjectList = fwTools::ClassFactoryRegistry::subClasses< fwTools::Object >() ;
    for ( std::list< ::fwTools::Object::sptr >::iterator iter = myObjectList.begin() ; iter != myObjectList.end() ; ++iter )
    {
        setType registeredFactoriesClassNames;
        std::list< ::fwServices::IService::sptr >       myList = fwTools::ClassFactoryRegistry::subClasses<fwServices::IService>(typeid(*(*iter))) ;
        std::list< ::fwServices::IService::sptr >::iterator myIter ;
        for ( myIter = myList.begin() ; myIter != myList.end() ; ++myIter )
        {
            std::stringstream localInfoStream ;
            localInfoStream << *(*myIter) ;
            registeredFactoriesClassNames.insert( localInfoStream.str() ) ;
        }
        mapObjectNameFactoryName[(*iter)->className()] = registeredFactoriesClassNames ;
    }
    //////////////////////////////////
    /// Display
    //////////////////////////////////
    for( mapType::iterator iter = mapObjectNameFactoryName.begin() ; iter != mapObjectNameFactoryName.end() ; ++iter )
    {
        myStream.str("");
        myStream << iter->first ; //pere
        setType::const_iterator lIter ;
        for( lIter = iter->second.begin() ; lIter != iter->second.end() ; ++lIter )
        {
            std::stringstream myStream2;
            myStream2 << *lIter ; //fils
            map_string[myStream.str()].push_back(myStream2.str()) ;
        }
    }
    return map_string ;
}



}
}
