/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
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

#include <fwCore/base.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwTools/Failed.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
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
 * @author	IRCAD (Research and Development Team).
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
	 * @brief Container type for fwServices
	 */
	typedef std::vector< ::fwServices::IService::sptr > SContainer ;
	/**
	 * @brief Container type for object-fwServices association : to each weak_ptr on object is associated a vector of fwServices
	 * @note Enables to registration of an object without fwServices, this being convenient for monitoring for instance
	 */
	typedef std::map< ::fwTools::Object::wptr , SContainer > OSContainer ;

	//@}

	/// Return the unique Instance, create it if required at first access
	FWSERVICES_API static ObjectServiceRegistry::sptr getDefault();
	/// Destructor
	FWSERVICES_API virtual ~ObjectServiceRegistry();

    /**
     * @name Management of the software top root object
     */

    //@{

	/**
	 * @brief Define the root object class name
	 */
	FWSERVICES_API static void setRootObjectClassName(std::string name) ;

	/**
	 * @brief Set the root object
	 * @note Care must be taken as external affectation may lead to conflict with internal one, through initializeRootObject() method invocation.
	 */
	FWSERVICES_API static void setRootObject(::fwTools::Object::sptr obj) ;

	/**
	 * @brief Define the root object XML like configuration name, to be later retrieved from component graph analysis
	 */
	FWSERVICES_API static void setRootObjectConfigurationName(std::string name) ;

	FWSERVICES_API static void setRootObjectConfigurationFile(std::string _rootObjectConfigurationFile) ;

	/**
	 * @brief Return true if the proposed XML like configuration is valid.
	 *
	 * Validity is checked through component graph analysis, class registry requests and XSD based schema analysis
	 */
	FWSERVICES_API static bool isRootObjectConfigurationValid() ;

	/**
	 * @brief Return true if the root object is initialized (i.e. instanciated with attached services, appropriately configured and started and updated)
	 */
	FWSERVICES_API static bool isRootObjectInitialized() ;
	/**
	 * @brief Trigger root object instanciation, configuration, service attachment, starting and updating
	 */
	FWSERVICES_API static void initializeRootObject() ;

	/**
	 * @brief Reset root object
	 * This involves the stopping of services reported in the configuration and then the stopping of the other. Finally,
	 * all services are eliminated as well as objects composing the root object. The object service registry should become empty.
	 */
	FWSERVICES_API static void uninitializeRootObject() ;

	/**
	 * @brief Return the root object
	 */
	FWSERVICES_API static ::fwTools::Object::sptr getRootObject() ;

	//@}

    /**
     * @name Management of registrations
     */

    //@{

	/**
	 * @brief Register an object with no services
	 * @author IRCAD (Research and Development Team).
	 */
	FWSERVICES_API static void registerObject(  ::fwTools::Object::sptr obj );

	/**
	 * @brief Register the service (service) for the object (obj)
	 * It also updates IService::m_associatedObject of service to point to obj
	 * It also affects to obj an instance of DefaultObjectDeleter (invoking updateObjectDeleter(  ::fwTools::Object::sptr ) ), to further enables obj's services automatic
	 * removal at obj destruction.
	 */
	FWSERVICES_API static void registerService(  ::fwTools::Object::sptr obj, fwServices::IService::sptr service);
	/**
	 * @brief Similar to registerService(  ::fwTools::Object::sptr , fwServices::IService::sptr )
	 * @deprecated Traditional pointers should not be used
	 */
	FWSERVICES_API static void registerService(  fwTools::Object * obj, ::fwServices::IService::sptr);

	//@}

    /**
     * @name Management of unregistrations
     */

    //@{

	/**
	 * @brief Unregister all services attached to the object (obj)
	 * It invokes service stop method first as well as related observations removal.
	 * It also clear objects decovered as expired when parsing the m_container
	 */
	FWSERVICES_API static void unregisterServices( fwTools::Object::sptr  obj );

	/**
	 * @brief Remove the service (_service) from the m_container
	 * It invokes service stop method first as well as related observations removal.
	 * It also clear objects decovered as expired when parsing the m_container
	 * @todo Code should be shared with the one written in unregisterServices( fwTools::Object::sptr )
	 */
	FWSERVICES_API static void unregisterService(  ::fwServices::IService::sptr _service );

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
	 * @brief Return a container with all objects associated with a service of type SERVICE in m_container
	 */
	 template<class SERVICE>
	 static std::vector<  ::fwTools::Object::sptr > getObjects();

	/**
	 * @brief Return a container with all objects of type OBJECT associated with a service of type SERVICE in m_container
	 */
	 template< class OBJECT, class SERVICE >
	 static std::vector< SPTR(OBJECT) > getObjects();


	/**
	 * @brief Return a container with all objects registered in m_container
	 */
	 FWSERVICES_API static std::vector<  ::fwTools::Object::sptr > getObjects();

	 /**
	 * @brief Return a reference on m_container
	 */
	 FWSERVICES_API static const OSContainer & getOSContainer();

	//@}

    /**
     * @name Misc.
     */

    //@{

	 /**
	 * @brief Move service (_service) from object objSrc to object objDst in the m_container
	 * @author	IRCAD (Research and Development Team).
	 */
	FWSERVICES_API static void swapService( fwTools::Object::sptr  objSrc, fwTools::Object::sptr  objDst, ::fwServices::IService::sptr _service );

	//@}

    /**
     * @name DEPRECATED
     */

    //@{

	/**
	 * @brief Return a strong reference on a object passed as traditional pointer
	 * @deprecated Traditional pointers should not be used
	 * @return empty smart pointer if not found
	 */
	 FWSERVICES_API static ::fwTools::Object::sptr shared_from( fwTools::Object * );

	 /**
	 * @brief Return a strong reference on a servce passed as traditional pointer
	 * @deprecated Traditional pointers should not be used
	 * @return empty smart pointer if not found
	 */
	 FWSERVICES_API static ::fwServices::IService::sptr shared_from( fwServices::IService *_service );

	 /**
	 * @brief Return true if the service (_service) is attached to an object
	 * @deprecated Traditional pointers should not be used
	 * @deprecated IService already provides this information
	 */
	 FWSERVICES_API static bool hasObject(IService * _service);

	//@}

protected :

	/**
	 * @brief Affect, if not already done, a ::fwService::DefaultDeleter to object
	 */
	FWSERVICES_API void  updateObjectDeleter(  ::fwTools::Object::sptr object);


	/**
	 * @brief Object to service associations container
	 * @note An object can be registered without services
	 * @warning Do not use smart pointers for fwTools::Object, otherwise they will never destroy
	 */
	OSContainer	m_container ;

	/**
	 * @brief Constructor, protected to ensure unique instance (singleton pattern)
	 */
	ObjectServiceRegistry();

private :


	/**
	 * @brief Strong reference on the unique ObjectServiceRegistry instance
	 */
    static ObjectServiceRegistry::sptr m_instance;

	/**
	 * @brief Class name of the root object.
	 * @note The boolean is false if name undefined as at m_instance initialization.
	 */
    std::pair< bool , std::string > m_rootObjectClassName ;
	/**
	 * @brief Name of the configuration for the root object
	 * @note The boolean is false if name undefined as at m_instance initialization.
	 */
	std::pair< bool , std::string > m_rootObjectConfigurationName ;

	std::pair< bool , std::string > m_rootObjectConfigurationFile ;

	/**
	 * @brief True if root object initialized
	 * @note Should be useless as m_rootObject can provide this information
	 */
	bool m_isRootInitialized ;

	/**
	 * @brief Strong reference of the root object
	 */
    ::fwTools::Object::sptr m_rootObject ;

	/**
	 * @brief Strong reference of the root object configuration
	 */
    ::fwRuntime::ConfigurationElement::sptr m_rootObjectConfiguration ;


};


typedef ObjectServiceRegistry OSR ;



} // namespace fwServices

#include "fwServices/ObjectServiceRegistry.hxx"

#endif // _FWSERVICES_MANAGER_HPP_


