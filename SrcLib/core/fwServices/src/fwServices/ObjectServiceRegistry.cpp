/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <utility>

#include <boost/filesystem.hpp>

#include <fwTools/ClassFactoryRegistry.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>
#include <fwRuntime/Extension.hpp>


#include "fwServices/Deleter.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/bundle/runtime.hpp"
#include "fwServices/validation/Validator.hpp"
#include "fwServices/op/New.hpp"
#include "fwServices/op/Run.hpp"
#include "fwServices/op/Com.hpp"

namespace fwServices
{



::boost::shared_ptr<ObjectServiceRegistry> ObjectServiceRegistry::m_instance;

ObjectServiceRegistry::ObjectServiceRegistry()
{
	m_rootObjectClassName.first = false  ;
	m_rootObjectConfigurationName.first = false ;
	m_isRootInitialized = false ;
}

ObjectServiceRegistry::~ObjectServiceRegistry()
{
	m_instance.reset() ;
}
void ObjectServiceRegistry::setRootObjectClassName(std::string name)
{
	getDefault()->m_rootObjectClassName = std::pair< bool , std::string >( true , name ) ;
}

void ObjectServiceRegistry::setRootObject(::fwTools::Object::sptr obj)
{
	getDefault()->m_rootObject = obj;
}


void ObjectServiceRegistry::setRootObjectConfigurationName(std::string name)
{
	getDefault()->m_rootObjectConfigurationName = std::pair< bool , std::string >( true , name ) ;
}

void ObjectServiceRegistry::setRootObjectConfigurationFile(std::string _rootObjectConfigurationFile)
{
	getDefault()->m_rootObjectConfigurationFile = std::pair< bool , std::string >( true , _rootObjectConfigurationFile ) ;
}


::boost::shared_ptr< ::fwTools::Object > ObjectServiceRegistry::getRootObject()
{
	return getDefault()->m_rootObject ;
}
bool ObjectServiceRegistry::isRootObjectConfigurationValid()
{
	if( getDefault()->m_rootObjectClassName.first && getDefault()->m_rootObjectConfigurationName.first )
	{
		assert( getDefault()->m_rootObjectClassName.first ) ;
		assert( getDefault()->m_rootObjectConfigurationName.first ) ;
		std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > extensions = ::fwServices::bundle::findExtensionsForPoint( getDefault()->m_rootObjectClassName.second ) ;
	   for( std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >::iterator iter = extensions.begin() ; iter != extensions.end() ; ++iter )
	   {
		   if( (*iter)->getIdentifier() == getDefault()->m_rootObjectConfigurationName.second )
		   {
			   return ::fwServices::validation::checkObject( *((*iter)->begin()) ) ;
		   }
	   }
	}
   return true ;
}
void ObjectServiceRegistry::initializeRootObject()
{
	if( !getDefault()->m_isRootInitialized && getDefault()->m_rootObjectClassName.first && getDefault()->m_rootObjectConfigurationName.first )
	{

		// Load another bundle
		if ( getDefault()->m_rootObjectConfigurationFile.first )
		{
			::boost::filesystem::path filePath ( getDefault()->m_rootObjectConfigurationFile.second );
			::boost::shared_ptr< ::fwRuntime::Bundle > configBundle = ::fwRuntime::io::BundleDescriptorReader::createBundleFromXmlPlugin( filePath );
			::fwRuntime::Runtime::getDefault()->addBundle( configBundle );
			configBundle->setEnable( true );
		}

		assert( getDefault()->isRootObjectConfigurationValid() );
		assert( getDefault()->m_rootObjectClassName.first ) ;
		assert( getDefault()->m_rootObjectConfigurationName.first ) ;

		std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > extensions = ::fwServices::bundle::findExtensionsForPoint( getDefault()->m_rootObjectClassName.second ) ;
		for( std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >::iterator iter = extensions.begin() ; iter != extensions.end() ; ++iter )
		{
			if( (*iter)->getIdentifier() == getDefault()->m_rootObjectConfigurationName.second )
			{
				getDefault()->m_rootObjectConfiguration = *((*iter)->begin()) ;
			}
		}

		getDefault()->m_rootObject = ::fwServices::New(getDefault()->m_rootObjectConfiguration);
		::fwServices::start(getDefault()->m_rootObjectConfiguration) ;
		::fwServices::update(getDefault()->m_rootObjectConfiguration) ;
		getDefault()->m_isRootInitialized = true ;
	}
}
void ObjectServiceRegistry::uninitializeRootObject()
{
	SLM_TRACE_FUNC();

	if( getDefault()->m_isRootInitialized )
	{
		assert( getDefault()->m_rootObjectClassName.first ) ;
		assert( getDefault()->m_rootObjectConfigurationName.first ) ;
		// Stop services reported in m_rootObjectConfiguration before stopping everything
		::fwServices::stopAndUnregister(getDefault()->m_rootObjectConfiguration) ;
		// Unregister root object services
		::fwServices::OSR::unregisterServices(getDefault()->m_rootObject);

		SLM_TRACE("uninitializeRootObject : Reset the last shared_ptr on root object.");
		// Reset the root object: involve complete m_container clearing
		getDefault()->m_rootObject.reset();
		assert( getDefault()->m_rootObject.use_count() == 0 );
		// Setting initialization to false
		getDefault()->m_isRootInitialized = false ;
	}
}

bool ObjectServiceRegistry::isRootObjectInitialized()
{
	return getDefault()->m_isRootInitialized ;
}

void  ObjectServiceRegistry::updateObjectDeleter( ::boost::shared_ptr< fwTools::Object > object)
{
	if ( ::boost::dynamic_pointer_cast< ::fwServices::DefaultObjectDeleter >(object->getDeleter() ) == 0 )
	{
		object->setDeleter( ::boost::shared_ptr< ::fwServices::DefaultObjectDeleter >( new ::fwServices::DefaultObjectDeleter ) ) ;
	}
}

void ObjectServiceRegistry::registerObject( ::boost::shared_ptr< fwTools::Object > obj )
{
	bool isAlreadyRegistered = false ;
	for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
	{
		if( pos->first.lock() ==  obj )
		{
			isAlreadyRegistered = true ;
		}
	}
	if( !isAlreadyRegistered )
	{
		 ::boost::weak_ptr< fwTools::Object > newRef(obj) ;
		 getDefault()->m_container.insert( std::make_pair( newRef , SContainer() ) );
		 getDefault()-> updateObjectDeleter(obj);
	}
}



void ObjectServiceRegistry::registerService(  fwTools::Object * obj, ::boost::shared_ptr<fwServices::IService > service)
{
	OSR::registerService( OSR::shared_from( obj ) , service ) ;
}

::boost::shared_ptr<ObjectServiceRegistry> ObjectServiceRegistry::getDefault()
{
	if ( m_instance==0 )
	{
		m_instance = ::boost::shared_ptr<ObjectServiceRegistry>(new ObjectServiceRegistry() );
	}
	return m_instance;
}



void  ObjectServiceRegistry::registerService( ::boost::shared_ptr< fwTools::Object > object , ::boost::shared_ptr<fwServices::IService > service)
{
	::boost::weak_ptr< fwTools::Object > refKey ;
	for ( OSContainer::iterator iter = getDefault()->m_container.begin(); iter!= getDefault()->m_container.end() ; ++iter )
	{
		::boost::weak_ptr< fwTools::Object > wo = iter->first ;
		assert( !wo.expired() ) ;
		if( (iter->first).lock() == object )
		{
			refKey = iter->first ;
			for ( SContainer::iterator lIter = getDefault()->m_container[refKey].begin() ; lIter!= getDefault()->m_container[refKey].end() ; ++lIter )
			{
				if( (*lIter) == service )
				{
					SLM_WARN( "Registering the same service instance to the same object" ) ;
					return ;
				}
			}
			getDefault()->m_container[refKey].push_back(service) ;
			service->m_associatedObject = object;
			getDefault()->updateObjectDeleter(object);
			OSLM_INFO( "Registering service " << *service ) ;
			return ;
		}
	}

	// If first registration: object is registered as a new object (new reference key) as well as the service
	refKey = object ;
	getDefault()->m_container[refKey].push_back(service) ;

	assert( service->m_associatedObject.use_count()==0 );
	service->m_associatedObject = object;
	getDefault()->updateObjectDeleter(object);
}


void ObjectServiceRegistry::swapService(  fwTools::Object::sptr  _objSrc, fwTools::Object::sptr  _objDst, ::boost::shared_ptr< ::fwServices::IService > _service )
{
		std::vector< ::boost::weak_ptr< fwTools::Object > >   lobjects;
		for ( OSContainer::iterator iter = getDefault()->m_container.begin(); iter != getDefault()->m_container.end() ; ++iter )
		{
			// Usually, unregisterServices is invoked at obj destruction (from its destructor) : its weak_ptr has therefore expired
			// For this reason, for that method only, one unregister all fwServices because expired weak_ptr can (but not sure) correspond to the considered obj
			if( iter->first.expired() )
			{
				OSLM_WARN( "Expired object discovered : swap on expired object !" ) ;

			}
			// Normal case : obj is not expired
			else if( iter->first.lock() == _objSrc )
			{
				SContainer::iterator lIter = iter->second.begin() ;
				bool bFind = false;
				while(  !bFind  && lIter != iter->second.end() )
				{
					if ( (*lIter) == _service )
					{
						iter->second.erase(lIter);
						bFind = true;
					}
					++lIter;
				}
			}
		}
		getDefault()->registerService(_objDst, _service);
}

void ObjectServiceRegistry::unregisterServices(  fwTools::Object::sptr  obj )
{
		std::vector< ::boost::weak_ptr< fwTools::Object > >   lobjects;
		for ( OSContainer::iterator iter = getDefault()->m_container.begin(); iter != getDefault()->m_container.end() ; ++iter )
		{

			// TODO Normally always true (shared_ptr definition)
			SLM_ASSERT("ACH : shared_ptr def ?", ! iter->first.expired() || iter->first.use_count() == 0  );
			SLM_ASSERT("ACH : wptr in the map not initialized ?", iter->first.expired() || iter->first.use_count() != 0 );

			// Usually, unregisterServices is invoked at obj destruction (from its destructor) : its weak_ptr has therefore expired
			// For this reason, for that method only, one unregister all fwServices because expired weak_ptr can (but not sure) correspond to the considered obj
			if( iter->first.expired() )
			{
				::fwTools::Object::wptr tmp1 = iter->first;

				OSLM_WARN( "Expired object discovered : stopping and unregistering all associated fwServices" ) ;
				lobjects.push_back( iter->first ) ;
				// Clean service vector clearing
				OSContainer::iterator _pos = iter ;
				size_t tmpsize = _pos->second.size() ;
				for( SContainer::iterator lIter = _pos->second.begin() ; lIter != _pos->second.end() ; ++lIter )
				{
					assert( (*lIter).use_count() != 0 );
					if (*lIter != NULL )
					{
						( *lIter)->stop();
						fwServices::unregisterComChannels( (*lIter) ) ;
					}
				}
				_pos->second.clear() ;
			}
			// Normal case : obj is not expired
			else if( iter->first.lock() == obj )
			{
				lobjects.push_back( iter->first ) ;
				// Clean service vector clearing
				OSContainer::iterator _pos = iter ;
				for( SContainer::iterator lIter = _pos->second.begin() ; lIter != _pos->second.end() ; ++lIter )
				{
					if (*lIter != NULL)
					{
						( *lIter)->stop();
						fwServices::unregisterComChannels( (*lIter) ) ;
					}
				}
				_pos->second.clear() ;
			}
		}
		// Erase all associated object with the refKey
		for ( std::vector< ::boost::weak_ptr< fwTools::Object > >::iterator iter = lobjects.begin(); iter != lobjects.end() ; ++iter )
		{
			getDefault()->m_container.erase( *iter ) ;
		}
}

void ObjectServiceRegistry::unregisterService( ::boost::shared_ptr< IService > _service )
{
	SLM_TRACE("ObjectServiceRegistry::unregisterService");
	for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
	{
		SContainer::iterator positionToDelete ;
		bool isFound = false ;
		for( SContainer::iterator lIter = pos->second.begin() ; lIter != pos->second.end() ; ++lIter )
		{
			if( ( *lIter) == _service )
			{
				( *lIter)->stop();
				isFound = true ;
				positionToDelete = lIter ;
				/// Logger Information
				std::stringstream msg;
				msg << "Unregistering service  : " << (*_service) << " (" << _service << ") " ;
				if( !pos->first.expired() ) // If service being unregistered is associated with an object having expired
				{
					msg << "Initially attached to object " << pos->first.lock()->className() << " (" << pos->first.lock() << ")";
				}
				SLM_INFO( msg.str() );
			}
		}
		if( isFound )
		{
			fwServices::unregisterComChannels( (*positionToDelete) ) ;
			pos->second.erase( positionToDelete ) ;
		}
	}
}

bool ObjectServiceRegistry::hasObject(IService * _service)
{
	return _service->m_associatedObject.use_count();
}

::boost::shared_ptr< fwTools::Object > ObjectServiceRegistry::shared_from( fwTools::Object *obj )
{
	for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
	{
		assert( !pos->first.expired() ) ;
		if ( (pos->first.lock()).get() == obj )
		{
			return  pos->first.lock() ;
		}
	}
	std::stringstream msg;
	msg << "Requested ObjectServiceRegistry::shared_from( fwTools::Object *obj [(" << obj << "]) for object not registred in  ObjectServiceRegistry";
	OSLM_WARN( msg.str() ); ;

	// avoid compilation warning
	return ::boost::shared_ptr< fwTools::Object >();

}
::boost::shared_ptr< fwServices::IService > ObjectServiceRegistry::shared_from( fwServices::IService *_service )
{
	for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
	{
			assert( !pos->first.expired() ) ;
			for( SContainer::iterator lIter = pos->second.begin() ; lIter != pos->second.end() ; ++lIter )
			{
				if ((*lIter).get() == _service )
				{
					return (*lIter) ;
				}
			}
	}
	std::stringstream msg;
	msg << "Requested ObjectServiceRegistry::shared_from( fwServices::IService *_service [(" << _service << "]) for object not registred in  ObjectServiceRegistry";
	OSLM_WARN( msg.str() ); ;

	// avoid compilation warning
	return ::boost::shared_ptr< fwServices::IService >();
}


std::vector< ::boost::shared_ptr< ::fwTools::Object > > ObjectServiceRegistry::getObjects()
{
	std::vector< ::boost::shared_ptr< ::fwTools::Object > > allObjects ;
	for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
	{
			assert( !pos->first.expired() ) ;
			if( std::find( allObjects.begin() , allObjects.end() , pos->first.lock() ) == allObjects.end() )
			{
				allObjects.push_back( pos->first.lock( ) ) ;
			}
	}

	return allObjects ;
}

const ObjectServiceRegistry::OSContainer  & ObjectServiceRegistry::getOSContainer()
{
	return getDefault()->m_container;
}


}
