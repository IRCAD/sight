/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/tr1/unordered_map.hpp>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/helper.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Info.hpp"
#include "fwServices/bundle/Factory.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/library/Factory.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

#if defined(_WIN32) && _MSC_VER > 1499 // Visual C++ 2008 only
#include <boost/functional/hash/hash.hpp>
namespace std
{
namespace tr1
{
template<typename a>
class hash< std::pair<a, a> >
{
public:
   size_t operator()(const std::pair<a, a> &p) const
   {
      return ::boost::hash_value(p);
   }
};
} //namespace tr1
} //namespace std
#endif

typedef std::pair<std::string, std::string> StringPair;
typedef std::tr1::unordered_map< StringPair, bool > SupportMapType;
SupportMapType  supportMap;

namespace fwServices
{

bool has( ::fwTools::Object::sptr obj , std::string serviceId) throw()
{
    //std::vector< std::string > availableImplementations = ::fwServices::getImplementationIds( obj , serviceId ) ;
    std::vector< std::string > availableImplementations =
            ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType( obj->getClassname(), serviceId );
    std::vector< ::fwServices::IService::sptr >  allServices = ::fwServices::getRegisteredServices(obj);
    for(std::vector< ::fwServices::IService::sptr >::iterator iter = allServices.begin() ; iter != allServices.end() ; ++iter )
    {
        std::string className = (*iter)->getClassname() ;
        if( std::find( availableImplementations.begin() , availableImplementations.end() , className ) != availableImplementations.end()  )
        {
            return true ;
        }
    }
    return false ;
}

bool has( ::std::string uuid ) throw()
{
    return ::fwTools::fwID::exist(uuid ) ;
}

bool __support( ::fwTools::Object::sptr obj, std::string serviceId) throw()
{
    // Native library approach
    bool libSupport         = ::fwServices::library::support(obj, serviceId) ;
    // Component approach
    bool componentSupport   = ::fwServices::bundle::support(obj, serviceId) ;

    return ( componentSupport || libSupport ) ;
}


bool support( ::fwTools::Object::sptr obj , std::string serviceId ) throw()
{
    SupportMapType::key_type key(obj->getClassname(), serviceId);
    SupportMapType::iterator iter = supportMap.find( key );
    if (iter != supportMap.end())
    {
      return iter->second;
    }
    bool res = __support(obj, serviceId);
    supportMap[key] = res;
    return res;
}


bool support( ::fwTools::Object::sptr obj, std::string serviceId, std::string implementationId ) throw()
{
    assert( ::fwServices::support(obj,serviceId) ) ;
    //std::vector< std::string > impl = ::fwServices::getImplementationIds( obj , serviceId );
    std::vector< std::string > impl =
                ::fwServices::registry::ServiceFactory::getDefault()->getImplementationIdFromObjectAndType( obj->getClassname(), serviceId );
    bool isImplementationSupported = ( std::find(impl.begin(), impl.end(), implementationId ) != impl.end() ) ;
    return isImplementationSupported ;
}

//std::vector< std::string > getImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId)
//{
//    std::vector< std::string > allImplementationId ;
//
//    // SPECIFIC IMPLEMENTATIONS
//
//    // All implementation already registered in fwTools for specific type
//    ::fwServices::ObjectServiceKeyType myKey(serviceId,obj->className());
//    std::list< ::fwServices::IService::sptr > serviceList = ::fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >(myKey) ;
//    for( std::list< ::fwServices::IService::sptr >::iterator iterLib = serviceList.begin() ; iterLib != serviceList.end() ; ++iterLib )
//    {
//        std::string classname =  (*iterLib)->getClassname() ;
//        if( std::find( allImplementationId.begin() , allImplementationId.end() , classname ) == allImplementationId.end()  )
//        {
//            allImplementationId.push_back( classname ) ;
//        }
//    }
//
//    // All implementations available from bundle analysis
//    std::vector< std::string > extensionIdsSpec = ::fwServices::bundle::getValidExtensionIdsForObjectAndService( obj , serviceId , 1) ;
//    for( std::vector< std::string >::iterator iterBundle = extensionIdsSpec.begin() ; iterBundle != extensionIdsSpec.end() ; ++iterBundle )
//    {
//        if( std::find( allImplementationId.begin() , allImplementationId.end() , *iterBundle ) == allImplementationId.end()  )
//        {
//            allImplementationId.push_back( *iterBundle ) ;
//        }
//    }
//
//    // GENERIC IMPLEMENTATIONS
//
//    // All implementation already registered in fwTools for ::fwTools::Object type
//    // This concerns implementation of type serviceId who are valid for any object type (declared by REGISTER_SERVICE( serviceIdClassType , impl , Object )
//    ::fwServices::ObjectServiceKeyType myRootKey(serviceId,::fwCore::getClassname< ::fwTools::Object >() );
//    std::list< ::fwServices::IService::sptr > rootServiceList = ::fwTools::ClassFactoryRegistry::subClasses< fwServices::IService >(myRootKey) ;
//    for( std::list< ::fwServices::IService::sptr >::iterator iterLib = rootServiceList.begin() ; iterLib != rootServiceList.end() ; ++iterLib )
//    {
//        std::string classname = (*iterLib)->getClassname() ;
//        if( std::find( allImplementationId.begin() , allImplementationId.end() , classname ) == allImplementationId.end()  )
//        {
//            allImplementationId.push_back( classname ) ;
//        }
//    }
//
//    std::vector< std::string > extensionIdsGen = ::fwServices::bundle::getValidExtensionIdsForObjectAndService( obj , serviceId , 2) ;
//    for( std::vector< std::string >::iterator iterBundle = extensionIdsGen.begin() ; iterBundle != extensionIdsGen.end() ; ++iterBundle )
//    {
//        if( std::find( allImplementationId.begin() , allImplementationId.end() , *iterBundle ) == allImplementationId.end()  )
//        {
//            allImplementationId.push_back( *iterBundle ) ;
//        }
//    }
//
//
//    return allImplementationId ;
//}
//std::string getDefaultImplementationIds( ::fwTools::Object::sptr obj , std::string serviceId)
//{
//    std::vector< std::string > implementations = ::fwServices::getImplementationIds( obj , serviceId ) ;
//    OSLM_ASSERT("implementation not found for " << serviceId << " service type", !implementations.empty() ) ;
//    return *implementations.begin() ;
//}

}

