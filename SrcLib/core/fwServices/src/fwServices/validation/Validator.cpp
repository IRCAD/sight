/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/Factory.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwRuntime/Extension.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/io/Validator.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>


#include "fwServices/Factory.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/helper.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/validation/Validator.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"

namespace fwServices
{
namespace validation
{

bool checkObject( ::fwRuntime::ConfigurationElement::sptr _elt , std::string objType )
{
    SLM_ASSERT("Missing id attribute", _elt->hasAttribute("id")) ;
    bool validity = true;
    // To store check log
    ::std::stringstream stream ;

    // Initialize objectType variable
    std::string objectType ;
    if(  !objType.empty()  )
    {
        objectType = objType ;
    }
    if( _elt->hasAttribute("type") )
    {
        objectType = _elt->getExistingAttributeValue("type") ;
        assert(  objectType == _elt->getExistingAttributeValue("type") ) ;
    }
    assert( !objectType.empty() ) ;
    // Object id
    ::boost::shared_ptr< ::fwRuntime::Extension > ext = ::fwRuntime::findExtension( objectType ) ;
    if( ext )
    {
        assert( ext );
        assert( ext->getPoint() == ::fwCore::getClassname< ::fwTools::Object >() );
        ext->getBundle()->start(); // start dll to retrieve proxy and register object
    }

    ::fwTools::Object::sptr obj = ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >(objectType) ;
    OSLM_ASSERT("root object " << objectType << " not created", obj);

    stream << "Object type = " << objectType << (_elt->hasAttribute("id") ? " (id = " + _elt->getExistingAttributeValue("id") + ")" : " No id" ) ;
    // Object uid
    stream << (_elt->hasAttribute("uid") ? " (uid = " + _elt->getExistingAttributeValue("uid") + ")" : " No uid" ) ;

    // Validation of the overall structure : object/service/start/update + object specificity
    ::boost::shared_ptr< ::fwRuntime::ExtensionPoint > extPt = ::fwRuntime::findExtensionPoint( objectType ) ;
    if( extPt )
    {
      ::boost::shared_ptr< ::fwRuntime::io::Validator > validator = extPt->getExtensionValidator() ;
       stream << " : " ;
       if( validator )
       {
           assert( validator );
           std::pair< bool , std::string > validationResult = ::fwRuntime::validateConfigurationElement( validator , _elt ) ;
           if( validationResult.first )
           {
               stream << "XSD validation OK" ;
           }
           else
           {
               stream << "XSD validation NOK (log = " << validationResult.second << ")";
           }
           validity = validationResult.first ;
       }
       else
       {
           stream << "No XSD validator" ;
       }
   }
   stream << std::endl;
   // Validation of service elements
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = _elt->begin() ; iter != _elt->end() ; ++iter)
    {
        stream << "------> " ;
        // ELEMENT "SERVICE"
        if ((*iter)->getName() == "service")
        {
            stream << "Add service " ;
            stream << ((*iter)->hasAttribute("uid") ? " UUID = " + (*iter)->getExistingAttributeValue("uid") + " - " : " No UUID - " ) ;
            if( (*iter)->hasAttribute("type") )
            {
                std::string serviceType = (*iter)->getExistingAttributeValue("type") ;
                // Test : does support
                stream << " type = " << serviceType << ( ::fwServices::support(obj,serviceType) ? " (supported)" : " (no supported)") ;
                // SERVICE IMPLEMENTATION : Check implementation details
                stream << " --> Implementation = " ;
                if( (*iter)->hasAttribute("implementation"))
                {
                    std::string implementationId = (*iter)->getExistingAttributeValue("implementation") ;
                    stream << implementationId ;
                    // CASE INLINE
                    if( !(*iter)->hasAttribute("config") )
                    {
                        stream << " (Inline) : " ;
                        checkService( implementationId , (*iter) , stream ) ;
                    }
                    // CASE OFFLINE
                    else
                    {
                        std::string configId = (*iter)->getExistingAttributeValue("config") ;
                        stream << " (Offline=" << configId << ") : " ;
                         ::fwRuntime::ConfigurationElement::sptr cfg = ::fwServices::bundle::findConfigurationForPoint(configId , implementationId ) ;
                        if( !cfg )
                        {
                            stream << "offline configuration not found" ;
                        }
                        else
                        {
                            checkService( implementationId , cfg , stream ) ;
                        }
                    }
                }
                else
                {
                    stream << "none specified (use default = " << ::fwServices::getDefaultImplementationIds(obj,serviceType) << ")" ;
                }
            }
            else
            {
                stream << " NONE!!!"  ;
            }
        }
        // ELEMENT "UPDATE/START"
        else if (  (*iter)->getName() == "update" || (*iter)->getName() == "start" || (*iter)->getName() == "stop")
        {
            std::string startOrUpdateOrStop;
            if( (*iter)->getName() == "update" ) startOrUpdateOrStop = "Update" ;
            if( (*iter)->getName() == "start" ) startOrUpdateOrStop = "Start" ;
            if( (*iter)->getName() == "stop" ) startOrUpdateOrStop = "Stop" ;

            stream << startOrUpdateOrStop << " service " ;
            if( (*iter)->hasAttribute("type") )
            {
                std::string serviceType = (*iter)->getExistingAttributeValue("type") ;
                stream << " Type = " << serviceType ;
                // Check that service to be started/updated is specified in the configuration
                bool isFound = false ;
                for (::fwRuntime::ConfigurationElementContainer::Iterator iter2 = _elt->begin() ; iter2 != _elt->end() ; ++iter2)
                {
                    if ((*iter2)->getName() == "service")
                    {
                        if( (*iter2)->hasAttribute("type") )
                        {
                            if( (*iter2)->getExistingAttributeValue("type") == serviceType )
                            {
                                isFound = true ;
                            }
                        }
                    }
                }
                stream << (isFound ? " (specified in configuration)" : " (NOT specified in configuration)" ) ;
            }
            else if( (*iter)->hasAttribute("uid") )
            {
                stream << " UUID = " << (*iter)->getExistingAttributeValue("uid") << " (existence not checked)" ;
            }
            else
            {
                stream << " has no flag TYPE or UUID: warning!!!" ;
            }
        }
        // ELEMENT "UNKNOWN"
        else
        {
            stream << "Unknown element " << (*iter)->getName() ;
        }
        stream << std::endl;
    }
    // Validation of start/update elements
    OSLM_INFO( std::endl << stream.str() ) ;
    return validity ;
}

bool checkService( std::string implementationId , ::fwRuntime::ConfigurationElement::sptr _elt , ::std::stringstream &stream)
{
    bool validity = true;
   // Typical use of validation
  ::boost::shared_ptr< ::fwRuntime::ExtensionPoint > extPt = ::fwRuntime::findExtensionPoint( implementationId ) ;
   if( !extPt )
   {
       stream << " No extension point declared" ;
   }
   else
   {
      ::boost::shared_ptr< ::fwRuntime::io::Validator > validator = extPt->getExtensionValidator() ;
       if( validator )
       {
           assert( validator );
           std::pair< bool , std::string > validationResult = ::fwRuntime::validateConfigurationElement( validator , _elt ) ;
           if( validationResult.first )
           {
               stream << " XSD validation OK" ;
           }
           else
           {
               stream << " XSD validation NOK (log = " << validationResult.second << ")";
           }
           validity = validationResult.first ;
       }
       else
       {
           OSLM_WARN("NO XSD VALIDATOR FOR " << implementationId << " : WRITE IT!!!") ;
           stream << " No XSD validator" ;
       }
   }


    return validity ;
}



}
}
