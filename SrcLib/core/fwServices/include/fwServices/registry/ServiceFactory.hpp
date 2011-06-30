/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_REGISTRY_SERVICEFACTORY_HPP_
#define _FWSERVICES_REGISTRY_SERVICEFACTORY_HPP_

#include <map>
#include <boost/tr1/unordered_map.hpp>
#include <boost/tuple/tuple.hpp>

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

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

namespace fwServices
{

namespace registry
{

/**
 * @class ServiceFactoryInfo
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ServiceFactoryInfo : public ::fwTools::Object
{
    public :

        fwCoreClassDefinitionsWithFactoryMacro( (ServiceFactoryInfo)(::fwTools::Object), (()), ::fwTools::Factory::New< ServiceFactoryInfo > );

        /// Constructor, do nothing.
        FWSERVICES_API ServiceFactoryInfo(){}

        /// Destructor, do nothing.
        FWSERVICES_API virtual ~ServiceFactoryInfo(){}

        std::string serviceType;
        std::string objectImpl;
        /// service description.
        std::string desc;

        ::boost::shared_ptr< ::fwRuntime::Bundle > bundle;
        ::boost::shared_ptr< ::fwTools::TBKClassFactory< ::fwServices::IService, std::string > > factory;
};


/**
 * @class ServiceFactory
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ServiceFactory : public ::fwCore::BaseObject
{

public:

    typedef std::pair<std::string, std::string> StringPair;
    typedef std::tr1::unordered_map< StringPair, bool > SupportMapType;

    fwCoreClassDefinitionsWithFactoryMacro( (ServiceFactory)(::fwCore::BaseObject), (()), new ServiceFactory) ;

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static ServiceFactory::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~ServiceFactory();

    /// Parse bundle information to retreive service declaration
    FWSERVICES_API void parseBundleInformation( );

    FWSERVICES_API void addFactory
    (   ::boost::shared_ptr< ::fwTools::IClassFactory > _factory,
        const std::string & simpl,
        const std::string & stype,
        const std::string & oimpl);

    FWSERVICES_API IService::sptr create( const std::string & _srvType, const std::string & _srvImpl );

    FWSERVICES_API void clearFactory();

    /// return a vector of service implementation
    FWSERVICES_API  std::vector< std::string > getImplementationIdFromObjectAndType(std::string object, std::string type);

    /// return the default service implementation for an object
    FWSERVICES_API std::string getDefaultImplementationIdFromObjectAndType( std::string object, std::string type );

    /// return the service description.
    FWSERVICES_API  std::string getServiceDescription(std::string srvImpl);

    /// Check if the service with given object and implementation is valid
    FWSERVICES_API  bool checkServiceValidity(const std::string & object, const std::string & srvImpl);

    /**
     * @brief Check whether an object (object) supports service of type srvType
     * @return true if service type supported
     */
    FWSERVICES_API bool support(const std::string & object, const std::string & srvType);

    /**
     * @brief Check whether an object (object) supports service of type srvType and implementation srvImpl
     * @return true if service type supported
     */
    FWSERVICES_API bool support(const std::string & object, const std::string & srvType, const std::string & srvImpl);

protected :

    typedef std::map< std::string, ServiceFactoryInfo::sptr > SrvRegContainer;

    /// Container of service information
    SrvRegContainer m_srvImplTosrvInfo;
    SupportMapType  m_supportMap;

    /// Constructor, protected to ensure unique instance (singleton pattern)
    FWSERVICES_API ServiceFactory();

private :

    /// Helper to print some informations
    void printInfoMap( const SrvRegContainer & src );

    /// Helper to trace services not declared in plugin.xml
    void checkServicesNotDeclaredInPluginXml();
};

} // namespace registry
} // namespace fwServices

#endif // _FWSERVICES_REGISTRY_SERVICEFACTORY_HPP_


