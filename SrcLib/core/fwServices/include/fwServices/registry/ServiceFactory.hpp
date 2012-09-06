/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_REGISTRY_SERVICEFACTORY_HPP__
#define __FWSERVICES_REGISTRY_SERVICEFACTORY_HPP__

#include <map>

#ifdef linux
#include <boost/unordered_map.hpp>
#else
#include <boost/tr1/unordered_map.hpp>
#endif

#include <boost/tuple/tuple.hpp>

#include <fwCore/mt/types.hpp>

#include "fwServices/config.hpp"

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

class IService;

namespace registry
{

/**
 * @class ServiceInfo
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ServiceInfo : public ::fwTools::Object
{
    public :
        typedef ::boost::function< SPTR(::fwServices::IService)() > FactoryType;

        fwCoreClassDefinitionsWithFactoryMacro( (ServiceInfo)(::fwTools::Object), (()), ::boost::make_shared< ServiceInfo > );

        std::string serviceType;
        std::string objectImpl;
        /// service description.
        std::string desc;

        ::boost::shared_ptr< ::fwRuntime::Bundle > bundle;

        FactoryType factory;
};


/**
 * @class ServiceFactory
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ServiceFactory : public ::fwCore::BaseObject
{

public:

    typedef std::string KeyType;
    typedef std::vector<KeyType> KeyVectorType;
    typedef std::pair<std::string, std::string> StringPair;

#ifdef linux
    typedef ::boost::unordered_map< StringPair, bool > SupportMapType;
#else
    typedef std::tr1::unordered_map< StringPair, bool > SupportMapType;
#endif

    fwCoreClassDefinitionsWithFactoryMacro( (ServiceFactory)(::fwCore::BaseObject), (()), new ServiceFactory) ;

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static ServiceFactory::sptr getDefault();

    /// Parse bundle information to retrieve service declaration
    FWSERVICES_API void parseBundleInformation( );

    FWSERVICES_API void addFactory( ServiceInfo::FactoryType _factory,
                                    const std::string & simpl,
                                    const std::string & stype,
                                    const std::string & oimpl);

    FWSERVICES_API SPTR(IService) create( const std::string & _srvImpl ) const;

    FWSERVICES_API SPTR(IService) create( const std::string & _srvType, const std::string & _srvImpl ) const;

    FWSERVICES_API void clearFactory();

    /// return a vector of service implementation
    FWSERVICES_API  std::vector< std::string > getImplementationIdFromObjectAndType(const std::string& object, const std::string& type) const;

    /// return the default service implementation for an object
    FWSERVICES_API std::string getDefaultImplementationIdFromObjectAndType( const std::string& object, const std::string& type ) const;

    /// return the associated object implementation.
    FWSERVICES_API  std::string getObjectImplementation(const std::string& srvImpl) const;

    /// return the service description.
    FWSERVICES_API  std::string getServiceDescription(const std::string& srvImpl) const;

    /// Check if the service with given object and implementation is valid
    FWSERVICES_API  bool checkServiceValidity(const std::string & object, const std::string & srvImpl) const;

    /**
     * @brief Check whether an object (object) supports service of type srvType
     * @return true if service type supported
     */
    FWSERVICES_API bool support(const std::string & object, const std::string & srvType);

    /**
     * @brief Check whether an object (object) supports service of type srvType and implementation srvImpl
     * @return true if service type supported
     */
    FWSERVICES_API bool support(const std::string & object, const std::string & srvType, const std::string & srvImpl) const;

    /**
     * @brief returns the registered factory keys.
     */
    FWSERVICES_API virtual KeyVectorType getFactoryKeys() const;

protected :

    typedef std::map< KeyType, ServiceInfo::sptr > SrvRegContainer;

    /// Container of service information
    SrvRegContainer m_srvImplTosrvInfo;
    SupportMapType  m_supportMap;

    mutable ::fwCore::mt::ReadWriteMutex m_srvImplTosrvInfoMutex;
    mutable ::fwCore::mt::ReadWriteMutex m_supportMapMutex;

private :

    /**
     * @brief print services informations
     * @warning not thread-safe
     */
    void printInfoMap( const SrvRegContainer & src ) const;

    /**
     * @brief Trace services not declared in plugin.xml
     * @warning not thread-safe
     */
    void checkServicesNotDeclaredInPluginXml() const;
};

} // namespace registry
} // namespace fwServices

#endif // __FWSERVICES_REGISTRY_SERVICEFACTORY_HPP__


