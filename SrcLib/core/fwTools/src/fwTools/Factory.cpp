/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

Factory::Factory()
{}

//------------------------------------------------------------------------------

Factory::~Factory()
{}

//------------------------------------------------------------------------------

void Factory::addInitializer( ::fwTools::Factory::Initializer::sptr _init )
{
    m_initializers.push_back( _init ) ;
}

//------------------------------------------------------------------------------

::fwTools::Object::sptr Factory::buildData(const std::string &className)
{
    ::fwTools::Object::sptr newObject ( ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >( className ) );
    if ( newObject == 0 )
    {
        OSLM_FATAL( "Factory::buildData unable to build class : " <<  className; )
        std::string mes = "Factory::buildData unable to build class : " + className;
        throw ::fwTools::Failed(mes);
    }

    BOOST_FOREACH( ::boost::shared_ptr< ::fwTools::Factory::Initializer > initializer , m_initializers )
    {
        initializer->init( newObject );
    }

    return newObject;
}

//------------------------------------------------------------------------------

void Factory::uninitData(::fwCore::LogicStamp::csptr key )
{
    BOOST_FOREACH( ::fwTools::Factory::Initializer::sptr initializer , m_initializers )
    {
        initializer->uninit( key );
    }
}

//------------------------------------------------------------------------------

} // namespace fwTools
