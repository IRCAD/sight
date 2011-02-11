///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#include <vector>
//
//#include <fwTools/TypeInfo.hpp>
//
//#include <fwCore/Demangler.hpp>
//
//#include <fwRuntime/ConfigurationElement.hpp>
//#include <fwRuntime/Runtime.hpp>
//#include <fwRuntime/helper.hpp>
//
//#include "fwServices/bundle/runtime.hpp"
//
//namespace fwServices
//{
//
//namespace bundle
//{
//
//std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > findExtensions(std::string _identifier )
//{
//    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > extensions;
//    ::fwRuntime::Runtime::ExtensionIterator iter ;
//    for( iter = ::fwRuntime::Runtime::getDefault()->extensionsBegin() ; iter != ::fwRuntime::Runtime::getDefault()->extensionsEnd() ; ++iter )
//    {
//        if( (*iter)->getIdentifier() == _identifier )
//        {
//            extensions.push_back( *iter ) ;
//        }
//    }
//    return extensions ;
//}
//
//std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > findExtensionsForPoint(std::string _identifier )
//{
//    typedef std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > ExtensionContainer;
//    typedef std::back_insert_iterator< ExtensionContainer > Inserter;
//    ExtensionContainer  extElements;
//    Inserter            extInserter(extElements);
//    // Retrieve appropriate extensions
//    ::fwRuntime::getAllExtensionsForPoint (_identifier, extInserter);
//    return extElements ;
//}
//
//std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > findConfigurationExtensionsForPoint(std::string _identifier )
//{
//    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >    extElements = findExtensionsForPoint(_identifier);
//    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >    filteredExtElements;
//    for( std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >::iterator iter = extElements.begin() ; iter != extElements.end() ; ++iter )
//    {
//        // If only one configuration element, one considers that extension is a configuration
//        if( (*iter)->begin() != (*iter)->end() )
//        {
//            filteredExtElements.push_back( *iter ) ;
//        }
//    }
//    return filteredExtElements ;
//}
//
//::fwRuntime::ConfigurationElement::sptr findConfigurationForPoint(std::string _cfgId , std::string _pointId )
//{
//    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > cfgLikeExtension = ::fwServices::bundle::findConfigurationExtensionsForPoint(_pointId) ;
//    OSLM_ASSERT("Configuration for "<< _pointId << " not found.", !cfgLikeExtension.empty() );
//    std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >::iterator iter ;
//
//    BOOST_FOREACH(::boost::shared_ptr< ::fwRuntime::Extension > extension, cfgLikeExtension)
//    {
//        if( extension->getIdentifier() == _cfgId)
//        {
//            OSLM_ASSERT("ConfigurationElement "<< _cfgId << " for "<<_pointId<<" must have 1 element (size = " <<  extension->getElements().size() << " )",
//                    extension->getElements().size() == 1 );
//            return *( extension->begin() ) ;
//        }
//    }
//
//    return ::fwRuntime::ConfigurationElement::sptr() ;
//}
//
//std::vector< std::string > getValidExtensionIdsForObjectAndService( ::fwTools::Object::sptr object, std::string serviceId, unsigned int mode)
//{
//    // Local variables
//    std::vector< std::string > allExtIdContributingToServiceAndObject ;
//    typedef std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > ExtensionContainer;
//    typedef std::back_insert_iterator< ExtensionContainer > Inserter;
//    ExtensionContainer  extElements;
//    Inserter            extInserter(extElements);
//
//    // Check that SERVICECLASS is associated to an extension point
//    if( ! ::fwRuntime::findExtensionPoint( serviceId ) )
//    {
//        return allExtIdContributingToServiceAndObject ;
//    }
//
//    // Retrieve appropriate extensions
//     //::fwTools::Object::NewSptr genericObject;
//    ::fwRuntime::getAllExtensionsForPoint (serviceId, extInserter);
//    for( ExtensionContainer::iterator iter = extElements.begin() ; iter != extElements.end() ; ++iter )
//    {
//        // Find all extension having the current id and contributing to object.className()
//        std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > extensions = findExtensions( (*iter)->getIdentifier() ) ;
//        for( std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > >::iterator iter2 = extensions.begin() ; iter2 != extensions.end() ; ++iter2 )
//        {
//            switch( mode )
//            {
//                // Generic and specific
//                case 0 :
//                {
//                    if( (*iter2)->getPoint() == object->className() || (*iter2)->getPoint() == ::fwCore::TypeDemangler< ::fwTools::Object >().getClassname() )
//                    {
//                        allExtIdContributingToServiceAndObject.push_back((*iter2)->getIdentifier() ) ;
//                    }
//                    break;
//                }
//
//                // Specific
//                case 1 :
//                {
//                    if( (*iter2)->getPoint() == object->className() )
//                    {
//                        allExtIdContributingToServiceAndObject.push_back((*iter2)->getIdentifier() ) ;
//                    }
//                    break;
//                }
//
//                // Generic
//                case 2 :
//                {
//                    if( (*iter2)->getPoint() == ::fwCore::TypeDemangler< ::fwTools::Object >().getClassname() )
//                    {
//                        allExtIdContributingToServiceAndObject.push_back((*iter2)->getIdentifier() ) ;
//                    }
//                    break;
//                }
//
//                default:
//                {
//                    OSLM_FATAL("Unknown mode "<<mode);
//                    break;
//                }
//            }
//
//        }
//    }
//
//    return allExtIdContributingToServiceAndObject ;
//}
//
//::boost::shared_ptr< ::fwRuntime::Extension > getRuntimeExtension( std::string extId , std::string serviceId )
//{
//    // Local variables
//    typedef std::vector< ::boost::shared_ptr< ::fwRuntime::Extension > > ExtensionContainer;
//    typedef std::back_insert_iterator< ExtensionContainer > Inserter;
//    ExtensionContainer  extElements;
//    Inserter            extInserter(extElements);
//
//    // Check that SERVICECLASS is associated to an extension point
//    if( ! ::fwRuntime::findExtensionPoint(serviceId) )
//    {
//        return ::boost::shared_ptr< ::fwRuntime::Extension >() ;
//    }
//
//    // Retrieve appropriate extensions
//    ::fwRuntime::getAllExtensionsForPoint ( serviceId , extInserter );
//    for( ExtensionContainer::iterator iter = extElements.begin() ; iter != extElements.end() ; ++iter )
//    {
//        if( (*iter)->getIdentifier() == extId )
//        {
//            return *iter ;
//        }
//    }
//    return ::boost::shared_ptr< ::fwRuntime::Extension >() ;
//}
//
//
//}
//
//}
