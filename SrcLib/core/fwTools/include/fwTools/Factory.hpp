/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_FACTORY_HPP_
#define _FWTOOLS_FACTORY_HPP_

#include <string>
#include <list>
//#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include "fwTools/Failed.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/ClassFactoryRegistry.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{

//class Object;

/**
 * @brief 	The purpose of this class is to provide a dataObjectFactory
 * @class 	Factory
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo	Complete comments
 *
 * End-user can simply register is new Object using REGISTER_BINDING( fwTools::Object, fwTools::MyObject, "MyObject" );\n
 * String key must be the same as fwTools::MyObject->className();\n
 * All data of this lib are already registred in Factory source file
 */
class FWTOOLS_CLASS_API Factory
{
public :

	/// API for Initializer : must implement ::boost::weak_ptr< fwTools::Object >
	FWTOOLS_CLASS_API class  Initializer
	{
	public :
		FWTOOLS_API Initializer() {} ;
		FWTOOLS_API ~Initializer() {} ;
		FWTOOLS_API void init_from_sp( ::boost::shared_ptr< ::fwTools::Object > _obj );
		FWTOOLS_API virtual void init( ::boost::weak_ptr< ::fwTools::Object > _obj ) = 0;
	};


	/**
	 * @brief 	Return a smart pointer on the specialized data required
	 * @return 	Empty smart pointer if not found
	 */
	FWTOOLS_API static ::boost::shared_ptr< ::fwTools::Object > buildData(const std::string &className);

	template<class CLASSNAME >
	static ::boost::shared_ptr< CLASSNAME > New();

	// reference on KEYTYPE mandatory to also enable non copiable key for exemple typeid
	template<class KEYTYPE>
	static ::boost::shared_ptr< ::fwTools::Object > New(const KEYTYPE &key);

	FWTOOLS_API static void addInitializer( ::boost::shared_ptr< Initializer > _init ) ;

	FWTOOLS_API virtual ~Factory();

protected:

	FWTOOLS_API Factory();

	FWTOOLS_API static std::list< ::boost::shared_ptr< Initializer > > m_initializers;
};



template< class CLASSNAME >
::boost::shared_ptr< CLASSNAME > Factory::New()
{

	::boost::shared_ptr< CLASSNAME > newObject( new CLASSNAME ) ;
	if ( newObject == 0 )
	{
		std::string mes = "Factory::buildData unables to build class from key=";
		mes += ::fwTools::getString(typeid(CLASSNAME));
		throw ::fwTools::Failed(mes);
	}

	for( std::list< ::boost::shared_ptr< Initializer > >::iterator iter = m_initializers.begin() ; iter != m_initializers.end() ; ++iter  )
	{
		// Fatal error compiler with boost 1.37
		// Replace these lines
		// ::boost::weak_ptr< CLASSNAME > weakVersion = newObject;
		// (*iter)->init (weakVersion);
		// by
		(*iter)->init_from_sp( newObject );
	}

	return newObject;
}



template<class KEYTYPE>
::boost::shared_ptr<fwTools::Object > Factory::New(const KEYTYPE &key)
{
	BOOST_STATIC_ASSERT( ::boost::is_pointer<KEYTYPE>::value == false );

	::boost::shared_ptr<fwTools::Object > newObject ( ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >( key ) );
	if ( newObject == 0 )
	{
		std::string mes = "Factory::buildData unables to build class from key=";
		mes += ::fwTools::getString(key);
		throw ::fwTools::Failed(mes);
	}

	for( std::list< ::boost::shared_ptr< Initializer > >::iterator iter = m_initializers.begin() ; iter != m_initializers.end() ; ++iter  )
//	BOOST_FOREACH( ::boost::shared_ptr< ::fwTools::Factory::Initializer > initializer , m_initializers )
	{
		//::boost::weak_ptr< ::fwTools::Object  > weakVersion (newObject);
//		initializer->init( weakVersion );
		// (*iter)->init( weakVersion );
		(*iter)->init( newObject );
	}

	//	BOOST_FOREACH( ::boost::shared_ptr< ::fwTools::Factory::Initializer > initializer , m_initializers )
//	{
//		::boost::weak_ptr< ::fwTools::Object > weakVersion = newObject;
//		initializer->init( weakVersion );
//	}

	return newObject;
}

} // namespace fwTools

#endif //_FWDATA_FACTORY_HPP_
