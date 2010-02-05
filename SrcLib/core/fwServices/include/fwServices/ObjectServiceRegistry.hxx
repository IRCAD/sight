/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <sstream>
#include <iterator>

#include <fwCore/base.hpp>
#include <fwTools/ClassFactoryRegistry.hpp>

#include <fwServices/IService.hpp>

namespace fwServices
{

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectServiceRegistry::getServices()
{
        std::vector< SPTR(SERVICE) >  lfwServices;

        for( OSContainer::iterator iter = getDefault()->m_container.begin() ; iter != getDefault()->m_container.end() ; ++iter )
        {
                if( !iter->first.expired()) // To avoid loops with getObject( shared on service) when object has expired
                {
                        for( SContainer::iterator lIter = iter->second.begin() ; lIter != iter->second.end() ; ++lIter )
                        {
                                if( (*lIter).use_count() != 0 )
                                {
                                        SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( (*lIter) );
                                        if ( service.get() )
                                        {
                                                lfwServices.push_back( service ) ;
                                        }
                                }
                        }
                }
        }
#ifdef _DEBUG
        // Error/Warning management
        if( lfwServices.empty() )
        {
                SLM_DEBUG("No service registered");
        }
#endif
        return lfwServices;
}

template<class SERVICE>
std::vector< SPTR(SERVICE) > ObjectServiceRegistry::getServices( fwTools::Object::sptr obj)
{
        std::vector< SPTR(SERVICE) >  lfwServices;

        for( OSContainer::iterator iter = getDefault()->m_container.begin() ; iter != getDefault()->m_container.end() ; ++iter )
        {
                if( iter->first.lock().get() == obj.get() )
                {
                        for( SContainer::iterator lIter = iter->second.begin() ; lIter != iter->second.end() ; ++lIter )
                        {
                                 SPTR(SERVICE) service = ::boost::dynamic_pointer_cast< SERVICE >( (*lIter) );
                                if ( service.get() )
                                {
                                        lfwServices.push_back( service ) ;
                                }
                        }
                }
        }
        return lfwServices;
}

template<class SERVICE>
std::vector< ::fwTools::Object::sptr > ObjectServiceRegistry::getObjects()
{
        std::vector< ::fwTools::Object::sptr >   lobjects;
        for ( OSContainer::iterator pos = getDefault()->m_container.begin(); pos!= getDefault()->m_container.end() ; ++pos )
        {
                for( SContainer::iterator lIter = pos->second.begin() ; lIter != pos->second.end() ; ++lIter )
                {
                        if ( dynamic_cast< SERVICE *>( (*lIter).get() ) )// attached service is castable in SERVICE
                        {
                                assert( !pos->first.expired() ) ;
                                bool alreadyPushed = false ;
                                for( std::vector< ::fwTools::Object::sptr >::iterator iter = lobjects.begin() ; iter != lobjects.end() ; ++iter )
                                {
                                        if( (*iter).get() == pos->first.lock().get() )
                                        {
                                                alreadyPushed = true ;
                                        }
                                }
                                if( !alreadyPushed )
                                {
                                        lobjects.push_back( pos->first.lock() );
                                }
                        }
                }
        }

        // Error/Warning management
        if( lobjects.empty() )
        {
                std::stringstream mssg;
                mssg << "No object registered for the requested type of service" << std::endl;
                OSLM_WARN( mssg.str() );
        }

        return lobjects;
}


template<class OBJECT,class SERVICE>
std::vector< SPTR(OBJECT) > ObjectServiceRegistry::getObjects()
{
        std::vector< ::fwTools::Object::sptr >   lobjects = getObjects< SERVICE >();
        std::vector< SPTR(OBJECT) >   castedObjects ;

        for( std::vector< ::fwTools::Object::sptr >::iterator iter = lobjects.begin() ; iter != lobjects.end() ; ++iter )
        {
                if( ::boost::dynamic_pointer_cast< OBJECT >( *iter ) )
                {
                        castedObjects.push_back( ::boost::dynamic_pointer_cast< OBJECT >( *iter ) ) ;
                }
        }

        // Error/Warning management
        if( lobjects.empty() )
        {
                std::stringstream mssg;
                mssg << "No object registered for the requested type of service" << std::endl;
                OSLM_WARN( mssg.str() );
        }

        return castedObjects ;

}

} // end namespace
