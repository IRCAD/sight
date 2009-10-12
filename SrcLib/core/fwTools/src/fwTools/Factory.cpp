/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include "fwTools/Failed.hpp"
#include "fwTools/ClassRegistrar.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/Factory.hpp"

namespace fwTools
{

//------------------------------------------------------------------------------

std::list< ::boost::shared_ptr< ::fwTools::Factory::Initializer > > Factory::m_initializers ;

//------------------------------------------------------------------------------

void Factory::Initializer::init_from_sp( ::boost::shared_ptr< ::fwTools::Object > _obj )
{
 ::boost::weak_ptr< ::fwTools::Object > wp( _obj );
  this->init( wp );
};

//------------------------------------------------------------------------------

Factory::Factory()
{
}

//------------------------------------------------------------------------------

Factory::~Factory()
{
}

//------------------------------------------------------------------------------

void Factory::addInitializer( ::boost::shared_ptr< ::fwTools::Factory::Initializer > _init )
{
	m_initializers.push_back( _init ) ;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr Factory::buildData(const std::string &className)
{
	::fwTools::Object::sptr newObject ( ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >( className ) );
	if ( newObject == 0 )
	{
		OSLM_FATAL( "Factory::buildData unables to build class : " <<  className; )
		assert(false);
		std::string mes = "Factory::buildData unables to build class : " + className;
		throw ::fwTools::Failed(mes);
	}
	for( std::list< ::boost::shared_ptr< Initializer > >::iterator iter = m_initializers.begin() ; iter != m_initializers.end() ; ++iter  )
//	BOOST_FOREACH( ::boost::shared_ptr< ::fwTools::Factory::Initializer > initializer , m_initializers )
	{
//		initializer->init( weakVersion );
		(*iter)->init( newObject );
	}

//	BOOST_FOREACH( ::boost::shared_ptr< fwTools::Factory::Initializer > initializer , m_initializers )
//	{
//		initializer->init( newObject ) ;
//	}

	return newObject;
}

//------------------------------------------------------------------------------

} // namespace fwTools
