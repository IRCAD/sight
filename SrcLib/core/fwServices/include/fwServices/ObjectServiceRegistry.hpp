/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_MANAGER_HPP_
#define _FWSERVICES_MANAGER_HPP_

#include <map>
#include <vector>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/multiset_of.hpp>

#include <fwCore/base.hpp>
#include <fwCore/LogicStamp.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

namespace fwServices
{

/**
 * @class ObjectServiceRegistry
 *
 * Singleton maintaining the correspondence between objects and attached IService.
 *
 * This singleton also maintains a strong reference on the root object of a software/application instance.
 *
 * Except method related to root object, the API of the singleton should not be directly invoked, invocation of fwServices methods (i.e. add, get,...) should be preferred
 *
 * @note Weak references are considered for objects instead of strong ones to facilitate implicit destruction
 * @author  IRCAD (Research and Development Team).
 */
class FWSERVICES_CLASS_API ObjectServiceRegistry: public ::fwCore::BaseObject
{
public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ObjectServiceRegistry)(::fwCore::BaseObject) ) ;

    /**
     * @name Definition of service status
     */

    //@{

    /**
     * @brief Container type for key-fwServices association : for each LogicStampType there are an associated set of fwServices
     */
    typedef ::boost::bimaps::bimap<
            ::boost::bimaps::multiset_of< ::fwCore::LogicStamp::LogicStampType >,
            ::boost::bimaps::unordered_set_of< ::fwServices::IService::sptr >
            > KSContainer;
    //@}

    /// Return the unique Instance, create it if required at first access
    FWSERVICES_API static ObjectServiceRegistry::sptr getDefault();

    /// Destructor
    FWSERVICES_API virtual ~ObjectServiceRegistry();

    /// Return some informations contain in the registry
    FWSERVICES_API static std::string getRegistryInformation();

    /**
     * @name Management of registrations
     */

    //@{

    /**
     * @brief Register the service (service) for the object (obj)
     * It also updates IService::m_associatedObject of service to point to obj
     * removal at obj destruction.
     */
    FWSERVICES_API static void registerService(  ::fwTools::Object::sptr obj, fwServices::IService::sptr service);

    //@}

    /**
     * @name Management of unregistrations
     */

    //@{

    /**
     * @brief Remove the service (_service) from the m_container
     * It invokes service stop method first as well as related observations removal.
     */
    FWSERVICES_API static void unregisterService(  ::fwServices::IService::sptr _service );

    /**
     * @brief container manipulator (Helper)
     * remove service from ObjectServiceRegistry containers, do not perform any "semantic action" like
     * stop service, remove comm channel
     * @note (internal use) use with care
     */
    static void  removeFromContainer( ::fwServices::IService::sptr _service );

    //@}

    /**
     * @name Some useful getters
     */

    //@{

    /**
     * @brief Return a container with all services of type SERVICE registered in m_container
     * @note Services may be associated to different object
     */
    template<class SERVICE>
    static std::vector< SPTR(SERVICE) > getServices() ;

    /**
     * @brief Return a container with all services of type SERVICE associated to obj in m_container
     */
    template<class SERVICE>
    static std::vector< SPTR(SERVICE) > getServices(::fwTools::Object::sptr obj) ;

    /**
     * @brief Return a container of all services of type serviceType registered in the system (i.e. attached to the different objects of the system)
     * @note Should be optimized
     * @note Invoke getServices( ::fwTools::Object::sptr , std::string ) for each registered object
     * @author IRCAD (Research and Development Team).
     */
    FWSERVICES_API static std::vector< ::fwServices::IService::sptr > getServices( std::string serviceType ) ;

    /**
     * @brief Return a container of services of type serviceType which are attached to obj
     * @author IRCAD (Research and Development Team).
     */
    FWSERVICES_API static std::vector< ::fwServices::IService::sptr > getServices( ::fwTools::Object::sptr obj , std::string serviceType ) ;

    /**
     * @brief return a vector containing all services associated with the object _obj
     * @author IRCAD (Research and Development Team).
     */
    FWSERVICES_API static std::vector< ::fwServices::IService::sptr > getServices( ::fwTools::Object::sptr _obj );

    /**
     * @brief Return a container with all objects associated with a service of type SERVICE in m_container
     */
     template<class SERVICE>
     static std::vector<  ::fwTools::Object::sptr > getObjects();


    /**
     * @brief Return a container with all objects registered in m_container
     */
     FWSERVICES_API static std::vector<  ::fwTools::Object::sptr > getObjects();

     /**
     * @brief Return a reference on m_container
     */
     FWSERVICES_API static const KSContainer & getKSContainer();

    //@}

     /**
      * @name Some useful getters
      */

     //@{
     /**
      * @brief return true is obj has at least one service of type srvType
      * @author IRCAD (Research and Development Team).
      */
     FWSERVICES_API static bool has( ::fwTools::Object::sptr obj , const std::string & srvType);

     /**
      * @brief return true if key is still present in OSR
      */
     FWSERVICES_API static bool hasKey( ::fwCore::LogicStamp::csptr key );

     //@}


    /**
     * @name Misc.
     */

    //@{

     /**
     * @brief Move service (_service) from object objSrc to object objDst in the m_container
     * @author  IRCAD (Research and Development Team).
     */
    FWSERVICES_API static void swapService( fwTools::Object::sptr  objSrc, fwTools::Object::sptr  objDst, ::fwServices::IService::sptr _service );

    //@}

protected :

    /**
     * @brief Object to service associations container
     * @note An object can be registered without services
     * @warning Do not use smart pointers for fwTools::Object, otherwise they will never destroy
     */
    KSContainer m_container ;

    /**
     * @brief Constructor, protected to ensure unique instance (singleton pattern)
     */
    ObjectServiceRegistry();

private :


    /**
     * @brief Strong reference on the unique ObjectServiceRegistry instance
     */
    static ObjectServiceRegistry::sptr m_instance;

    /// Defines the string used to defined in bundles extensions that describes configurations
    static const std::string CONFIG_EXTENSION_POINT;

};


typedef ObjectServiceRegistry OSR ;



} // namespace fwServices

#include "fwServices/ObjectServiceRegistry.hxx"

#endif // _FWSERVICES_MANAGER_HPP_


