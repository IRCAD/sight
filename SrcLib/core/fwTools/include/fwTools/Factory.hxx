/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_FACTORY_HXX_
#define _FWTOOLS_FACTORY_HXX_

#include <string>
#include <list>
#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include <fwCore/base.hpp>

#include "fwTools/Failed.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/ClassFactoryRegistry.hpp"

namespace fwTools
{

template< class CLASSNAME >
::boost::shared_ptr< CLASSNAME > Factory::New()
{
    ::boost::shared_ptr< CLASSNAME > newObject( new CLASSNAME ) ;
    OSLM_ASSERT("Factory::New unable to build class with key " << ::fwTools::getString(typeid(CLASSNAME)), newObject);
    if ( newObject == 0 )
    {
        std::string mes = "Factory::New unable to build class with key ";
        mes += ::fwTools::getString(typeid(CLASSNAME));
        throw ::fwTools::Failed(mes);
    }

    BOOST_FOREACH( Initializer::sptr initializer , m_initializers )
    {
        initializer->init( newObject );
    }

    return newObject;
}

//------------------------------------------------------------------------------

template< class BASECLASS, class KEYTYPE >
::boost::shared_ptr< BASECLASS > Factory::New(const KEYTYPE &key)
{
    ::boost::shared_ptr< BASECLASS > newObject = ::fwTools::ClassFactoryRegistry::create< BASECLASS >( key );
    OSLM_ASSERT("Factory::New unable to build class with key " << ::fwTools::getString(typeid(BASECLASS)), newObject);
    if ( newObject == 0 )
    {
        std::string mes = "Factory::New unable to build class with key ";
        mes += ::fwTools::getString(typeid(BASECLASS));
        throw ::fwTools::Failed(mes);
    }

    BOOST_FOREACH( Initializer::sptr initializer , m_initializers )
    {
        initializer->init( newObject );
    }

    return newObject;
}

//------------------------------------------------------------------------------

template<class KEYTYPE>
::fwTools::Object::sptr Factory::New(const KEYTYPE &key)
{
    BOOST_STATIC_ASSERT( ::boost::is_pointer<KEYTYPE>::value == false );

    ::fwTools::Object::sptr newObject = ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >( key );
    OSLM_ASSERT("Factory::New unable to build class with key " << ::fwTools::getString(key), newObject);
    if ( newObject == 0 )
    {
        std::string mes = "Factory::New unable to build class with key ";
        mes += ::fwTools::getString(key);
        throw ::fwTools::Failed(mes);
    }

    BOOST_FOREACH( Initializer::sptr initializer , m_initializers )
    {
        initializer->init( newObject );
    }

    return newObject;
}

} // namespace fwTools

#endif //_FWTOOLS_FACTORY_HXX_
