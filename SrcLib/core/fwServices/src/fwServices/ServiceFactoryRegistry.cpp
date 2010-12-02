/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>

#include "fwTools/IClassFactory.hpp"
#include "fwTools/TBKClassFactory.hpp"

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/ServiceFactoryRegistry.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

ServiceFactoryRegistry::sptr ServiceFactoryRegistry::getDefault()
{
    SLM_TRACE_FUNC();
    static ServiceFactoryRegistry::sptr m_instance = ServiceFactoryRegistry::New();
    return m_instance;
}

//-----------------------------------------------------------------------------

ServiceFactoryRegistry::~ServiceFactoryRegistry()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void printXml( std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > > elems )
{
    for (   std::vector< ::boost::shared_ptr< ::fwRuntime::ConfigurationElement > >::iterator iter = elems.begin();
            iter != elems.end();
            ++iter )
    {
        OSLM_WARN( "\nxml =  " << (**iter) );
    }
}

//-----------------------------------------------------------------------------

void ServiceFactoryRegistry::parseBundleInformationForObject( SrvRegContainer & _bundleInfoMap )
{
    typedef std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > ExtensionContainer;
    typedef std::back_insert_iterator< ExtensionContainer > Inserter;

    ExtensionContainer  extElements;
    Inserter            extInserter(extElements);


    // Retrieve appropriate extensions
    ::fwRuntime::getAllExtensionsForPoint ( "::fwTools::Object" , extInserter );
    for( ExtensionContainer::iterator iter = extElements.begin() ; iter != extElements.end() ; ++iter )
    {
        std::string id = (*iter)->getIdentifier();
        if ( id != "::fwServices::IService" )
        {
            SrvRegContainer::iterator impl = _bundleInfoMap.find( id );

            if ( impl != _bundleInfoMap.end() )
            {
                impl->second->objectImpl = "::fwTools::Object";
            }
            else
            {

                OSLM_WARN( "Object classname =  " << id );

                ExtensionContainer  extElements2;
                Inserter            extInserter2(extElements2);

                ::fwRuntime::getAllExtensionsForPoint ( id , extInserter2 );
                for(    ExtensionContainer::iterator iter2 = extElements2.begin() ;
                        iter2 != extElements2.end() ;
                        ++iter2 )
                {
                    std::string id2 = (*iter2)->getIdentifier();

                    impl = _bundleInfoMap.find( id2 );

                    if( impl != _bundleInfoMap.end())
                    {
                        SLM_ASSERT( "Pas normal => " << id2, impl != _bundleInfoMap.end() );
                        impl->second->objectImpl = (*iter)->getIdentifier();
                    }
                }
            }
        }


    }

}

//-----------------------------------------------------------------------------

void ServiceFactoryRegistry::parseBundleInformation()
{

    SrvRegContainer bundleInfoMap;

    typedef std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > ExtensionContainer;
    typedef std::back_insert_iterator< ExtensionContainer > Inserter;

    ExtensionContainer  extElements;
    Inserter            extInserter(extElements);


    // Retrieve appropriate extensions
    ::fwRuntime::getAllExtensionsForPoint ( "::fwServices::IService" , extInserter );
    for( ExtensionContainer::iterator iter = extElements.begin() ; iter != extElements.end() ; ++iter )
    {
        ExtensionContainer  extElements2;
        Inserter            extInserter2(extElements2);

        ::fwRuntime::getAllExtensionsForPoint ( (*iter)->getIdentifier() , extInserter2 );
        for(    ExtensionContainer::iterator iter2 = extElements2.begin() ;
                iter2 != extElements2.end() ;
                ++iter2 )
        {
            ServiceFactoryInfo::NewSptr info;
            info->serviceType = (*iter)->getIdentifier();
            info->bundle = (*iter2)->getBundle();
            bundleInfoMap[ (*iter2)->getIdentifier() ] = info;
        }
    }


    parseBundleInformationForObject( bundleInfoMap );

    // Verify object
    for ( SrvRegContainer::iterator iter = bundleInfoMap.begin();
            iter != bundleInfoMap.end();
            ++iter )
    {
        if ( iter->second->objectImpl.empty() )
        {
            OSLM_WARN("Service " << iter->first << " of type  " << iter->second->serviceType << " in bundle " << iter->second->bundle->getIdentifier() << " has not object defined in plugin.xml ( declaration missing in dataReg ? ). Else is a service of service (sorry but not managed in this version )");

        }
    }

    //Print information
    //printInfoMap( bundleInfoMap );
    //printInfoMap( m_srvImplTosrvInfo )

    // Merge data info
    for ( SrvRegContainer::iterator iterBundle = bundleInfoMap.begin();
            iterBundle != bundleInfoMap.end();
            ++iterBundle )
    {
        SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( iterBundle->first );

        if ( iter != m_srvImplTosrvInfo.end() )
        {
            OSLM_DEBUG("Have already information (from register maccro) about this service ( " << iterBundle->first << " )." );
            ServiceFactoryInfo::sptr info = iter->second;
            ServiceFactoryInfo::sptr infoBundle = iterBundle->second;
            SLM_ASSERT("Try to add bundle, but bundle exists.", ! info->bundle );
            SLM_ASSERT("Try to add bundle, but this srv is already register and has not the same srv type.", infoBundle->serviceType == info->serviceType );
            SLM_ASSERT("Try to add bundle, but this srv (" << iterBundle->first << ")is already register and has not the same oimpl. ( "<< infoBundle->objectImpl <<" != "<< info->objectImpl <<" )", infoBundle->objectImpl == info->objectImpl );
            info->bundle =  infoBundle->bundle;
        }
        else
        {
            m_srvImplTosrvInfo[ iterBundle->first ] = iterBundle->second;
        }

    }

    //Print information
    printInfoMap( m_srvImplTosrvInfo );
    checkServicesNotDeclaredInPluginXml();
}

//-----------------------------------------------------------------------------

IService::sptr ServiceFactoryRegistry::create( const std::string & _srvType, const std::string & _srvImpl )
{
    SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( _srvImpl );
    OSLM_ASSERT("Sorry don't find in ServiceFactoryRegistry the service called " << _srvImpl , iter != m_srvImplTosrvInfo.end() )
    ServiceFactoryInfo::sptr info = iter->second;

    OSLM_DEBUG("SR create a new service ( classname = " << _srvImpl << " )");
    OSLM_ASSERT("Sorry, type of service must correspond. "<< _srvType << " != " << info->serviceType, _srvType == info->serviceType);
    if ( info->factory )
    {
        return info->factory->create();
    }
    else
    {
        SLM_ASSERT( "Sorry bundle must exist if a factory missing.", info->bundle );
        info->bundle->start();
        SLM_ASSERT( "Sorry after bundle loading ( " << info->bundle->getIdentifier() << " ) , factory must exist.", info->factory );
        return info->factory->create();
    }

    checkServicesNotDeclaredInPluginXml();
}

//-----------------------------------------------------------------------------

void ServiceFactoryRegistry::addFactory
( ::boost::shared_ptr< ::fwTools::IClassFactory > _factory,
        const std::string & simpl,
        const std::string & stype,
        const std::string & oimpl)
{
    OSLM_DEBUG( "New service registring : "
            << " simpl =" << simpl
            << " stype=" << stype
            << " oimpl=" << oimpl
    );

    SrvRegContainer::iterator iter = m_srvImplTosrvInfo.find( simpl );

    if ( iter != m_srvImplTosrvInfo.end() )
    {
        OSLM_DEBUG("Have already information about this service ( " << simpl << " )." );
        ServiceFactoryInfo::sptr info = iter->second;
        OSLM_ASSERT("Try to add factory, but this srv ( " << simpl << " ) has already a registered factory.", ! info->factory );
        OSLM_ASSERT("Try to add factory, but this srv ( " << simpl << " ) is already register and has not the same srv type. ( " << stype << " != " << info->serviceType <<" )", stype == info->serviceType );
        OSLM_ASSERT("Try to add factory, but this srv ( " << simpl << " ) is already register and has not the same oimpl. ( " << oimpl << " != " << info->objectImpl <<" )", oimpl == info->objectImpl );
        info->factory =  ::boost::dynamic_pointer_cast< ::fwTools::TBKClassFactory< ::fwServices::IService, std::pair< std::string, std::string > > >( _factory );
    }
    else
    {
        OSLM_DEBUG("Add new service factory in registry ( " << simpl << " )." );
        ServiceFactoryInfo::NewSptr info;
        info->serviceType = stype;
        info->objectImpl = oimpl;
        info->factory =  ::boost::dynamic_pointer_cast< ::fwTools::TBKClassFactory< ::fwServices::IService, std::pair< std::string, std::string > > >( _factory );
        m_srvImplTosrvInfo[simpl] = info;
    }
}

//-----------------------------------------------------------------------------

ServiceFactoryRegistry::ServiceFactoryRegistry()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void ServiceFactoryRegistry::printInfoMap( const SrvRegContainer & src )
{
    //Print information
    for (   SrvRegContainer::const_iterator iter = src.begin();
            iter != src.end();
            ++iter )
    {
        OSLM_DEBUG(" Service name = " << iter->first );
        OSLM_DEBUG("  - type   = " << iter->second->serviceType );
        OSLM_DEBUG("  - object = " << iter->second->objectImpl);

        OSLM_DEBUG_IF("  - bundle = " <<  iter->second->bundle->getIdentifier(), iter->second->bundle );
        OSLM_DEBUG_IF("  - bundle = ( no bundle registered )", ! iter->second->bundle );

        OSLM_DEBUG_IF("  - name after creation = " <<  iter->second->factory->create()->getClassname(), iter->second->factory );
        OSLM_DEBUG_IF("  - name after creation = ( no factory registered )", ! iter->second->factory );
    }
}

//-----------------------------------------------------------------------------

void ServiceFactoryRegistry::checkServicesNotDeclaredInPluginXml()
{
    //Print information
    for (   SrvRegContainer::const_iterator iter = m_srvImplTosrvInfo.begin();
            iter != m_srvImplTosrvInfo.end();
            ++iter )
    {
        if ( ! iter->second->bundle )
        {
            OSLM_WARN("Service " << iter->first << " is not declared/found in a plugin.xml." );
        }
    }
}

//-----------------------------------------------------------------------------

void ServiceFactoryRegistry::clearFactory()
{
    m_srvImplTosrvInfo.clear();
}

//-----------------------------------------------------------------------------

} // namespace fwServices

