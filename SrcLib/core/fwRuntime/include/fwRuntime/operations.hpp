/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_OPERATIONS_HPP_
#define _FWRUNTIME_OPERATIONS_HPP_

#include <iterator>
#include <memory>
#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwCore/base.hpp>

#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/Executable.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"

namespace fwRuntime
{
        struct Bundle;

        namespace profile
        {
                struct Profile;
        }
}



namespace fwRuntime
{


/**
 * @brief               Retrieves the extension point having the specified identifier.
 *
 * @param[in]   identifier      a string containing an extension point identifier
 *
 * @return              a pointer to the found extension point or null if none
 */
FWRUNTIME_API ::boost::shared_ptr<ExtensionPoint> findExtensionPoint( const std::string & identifier );



/**
 * @brief               Creates an executable instance for the specified configuration element.
 *
 * @param[in]   extension       a shared to an extension containing a configuration element
 *                                                      to use for the executable instance creation
 * @param[in]   element         a string containing the name of a configuration element
 * @param[in]   attribute       a string containing an element attribute name to use as
 *                                                      the executable type
 *
 * @return              a shared pointer to the built executable object
 */
template<typename T>
T* createExecutableInstance(
                const ::boost::shared_ptr<Extension>    extension,
                const std::string                       & element = "executable",
                const std::string                       & attribute = "class" ) throw(RuntimeException)
{
        // Retrieves the right configuration element.
         ::boost::shared_ptr<ConfigurationElement> elt( extension->findConfigurationElement(element) );
        if( elt == 0 )
        {
                throw RuntimeException(element + ": configuration element not found in extension.");
        }

        // Creates the executable instance from the found configuration element.
        return createExecutableInstance<T>( elt, attribute );
}


/**
 * @brief               Creates an executable instance for the specifed configuration element.
 *
 * @param[in]   element         a shared pointer to a configuration element
 * @param[in]   attribute       a string containing an element attribute name to use as
 *                                              the executable type
 *
 * @return              a shared pointer to the built executable object
 */
template<typename T>
T * createExecutableInstance(
                const ::boost::shared_ptr<ConfigurationElement> element,
                const std::string                                               & attribute = "class" ) throw(RuntimeException)
{

        // Retrieves the executable type.
        if( element->hasAttribute( attribute ) == false )
        {
                throw RuntimeException( "Configuration element has no attribute '" + attribute + "'." );
        }
        const std::string type( element->getExistingAttributeValue(attribute) );

        // Creates the executable instance.
        Runtime                                                 * rntm          ( Runtime::getDefault()                                                 );
        std::auto_ptr< IExecutable >    executable      ( rntm->createExecutableInstance(type, element) );

        // Converts the executable instance to the right type.
        T * result = dynamic_cast<T*>( executable.get() );
        if( result == 0 )
        {
                throw RuntimeException( "Executable creation failed. Bad cast" );
        }
        executable.release();

        // That's all folks !
        return result;
}



/**
 * @brief       Retrieves all extensions for the point having the specified
 *                      identifier.
 *
 * @param       identifier      a string containing an extension point identifier
 * @param       output          an insert iterator used to store each found extension
 */
template<typename OutputIterator>
void getAllExtensionsForPoint(
                const std::string       & identifier,
                OutputIterator            output
        ) throw(RuntimeException)
{
         ::boost::shared_ptr< ExtensionPoint >  point = findExtensionPoint(identifier);

        if( !point )
        {
                throw RuntimeException( identifier + ": invalid extension point identifier." );
        }
        point->getAllExtensions( output );
}



/**
 * @brief       Retrieves all configuration elements for the point having the
 *                      specified identifier.
 *
 * @param       identifier      a string containing an extension point identifier
 * @param       output          an insert iterator used to store found configuration
 *                                              elements
 */
template<typename OutputIterator>
void getAllConfigurationElementsForPoint(
                const std::string       & identifier,
                OutputIterator            output
        ) throw(RuntimeException)
{
         ::boost::shared_ptr< ExtensionPoint >  point = findExtensionPoint(identifier);

        OSLM_TRACE("getAllConfigurationElementsForPoint(" << identifier << "Bundle" << point->getBundle()->getIdentifier() );

        if( !point )
        {
                throw RuntimeException( identifier + ": invalid extension point identifier." );
        }

        // VAG test if ExtensionPoint is enable
        if ( point->isEnable() )
        {
                point->getAllConfigurationElements( output );
        }
        else
        {
                OSLM_DEBUG ( "IGNORING getAllConfigurationElementsForPoint(" << identifier << ") extension point disabled");
        }

}


/**
 * @brief       Retrieves all configuration elements for the point having the
 *                      specified identifier.
 *
 * This method use the container type specified by the template parameter. The
 * type of the elements of the container must be ::boost::shared_ptr< ConfigurationElement >
 * or the compilation will fail.
 *
 * @return      a container containing shared pointers to all found configuration
 *                      elements
 */
template<typename Container>
const Container getAllConfigurationElementsForPoint(const std::string & identifier) throw(RuntimeException)
{
        // Defines an insert iterator type for the container.
        typedef std::back_insert_iterator< Container > Inserter;

        // Collects all contributed configuratoin elements.
        Container       elements;
        Inserter        inserter(elements);
        getAllConfigurationElementsForPoint(identifier, inserter);

        // The job is done!
        return elements;
}


/**
 * @brief       Retrieves all executable objects for the point having the specified identifier
 *
 * @param       identifier      a string containing the extension point identifier
 * @param       attribute       a string containing the name of the element attribute containing
 *                                              the executable identifier (default is "class")
 *
 * This method use the container type specified by the template parameter. The
 * type of the elements of the container must be ::boost::shared_ptr< T >
 * or the compilation will fail (where T is the type of the executable you want to create).
 *
 * @return      a container containing shared pointers to all created executable instances
 */
template< typename Container, typename T >
const Container getAllExecutableForPoint( const std::string & identifier, const std::string & attribute = "class" ) throw(RuntimeException)
{
        // Defines the element container
        typedef std::vector< ::boost::shared_ptr< ConfigurationElement > > ConfigurationElementContainer;

        // Retrieves all configuration elements.
        ConfigurationElementContainer   elements( getAllConfigurationElementsForPoint< ConfigurationElementContainer >(identifier) );

        // Defines an insert iterator type for the executable container.
        typedef std::back_insert_iterator< Container > Inserter;

        // Walks through collected configuration elements and create desired executable instances
        Container                                                               result;
        ConfigurationElementContainer::iterator iElement;
        Inserter                                                                iInserter( result );
        for( iElement = elements.begin(); iElement != elements.end(); ++iElement, ++iInserter )
        {
                 ::boost::shared_ptr< ConfigurationElement >    element( *iElement );
                 ::boost::shared_ptr< T >                                               executable( createExecutableInstance< T >(element, attribute) );

                iInserter = executable;
        }
        return result;
}


/**
 * @brief       Retrieves the configuation element with the given identifier for the
 *                      given extension point
 *
 * @param       identifier                      a string containing the configuration element identifier
 * @param       pointIdentifier         a string containing the extension point identifier
 *
 * @return      a shared pointer to the found configuration element or null if none
 */
FWRUNTIME_API ::boost::shared_ptr< ConfigurationElement > findConfigurationElement( const std::string & identifier, const std::string & pointIdentifier );



/**
 * @brief       Retrieves the extension having the specified identifier.
 *
 * @param       identifier      a string containing an extension identifier
 *
 * @return      a shared pointer to the found extension or null if none
 */
FWRUNTIME_API ::boost::shared_ptr<Extension> findExtension( const std::string & identifier );


/**
 * @brief       Retrieve a filesystem valid path for a path relative to the bundle having the specified identifier.
 *
 * @param       bundleIdentifier        a string containing a bundle identifier
 * @param       path                            a bundle relative path
 *
 * @return      a system valid path
 */
FWRUNTIME_API const boost::filesystem::path getBundleResourcePath(const std::string& bundleIdentifier, const boost::filesystem::path &path) throw();


/**
 * @brief       Retreives a filesystem valid path for a path relative to the specified bundle.
 *
 * @param       bundle  a pointer to a bundle instance
 * @param       path    a path relative to the bundle
 *
 * @return      a system valid path
 */
FWRUNTIME_API const boost::filesystem::path getBundleResourcePath( ::boost::shared_ptr<Bundle> bundle, const boost::filesystem::path &path) throw();


/**
 * @brief       Retreives a filesystem valid path for a path relative to the bundle of the specified configuration element.
 *
 * @param       element a shared pointer to a configuration element instance
 * @param       path    a path relative to the bundle
 *
 * @return      a system valid path
 */
FWRUNTIME_API const boost::filesystem::path getBundleResourcePath( ::boost::shared_ptr<ConfigurationElement> element, const boost::filesystem::path &path) throw();


/**
 * @brief       Retreives a filesystem valid path for a path relative to the specified executable instance.
 *
 * @param       executable      a pointer to an executable instance
 * @param       path            a path relative to the bundle
 *
 * @return      a system valid path
 */
FWRUNTIME_API const boost::filesystem::path getBundleResourcePath(const IExecutable *executable, const boost::filesystem::path &path) throw();


/**
 * @brief       Loads all bundles that can be found in the specified directory.
 *
 * @param       directory       a path to the directory to explore for bundles
 */
FWRUNTIME_API void addBundles( const boost::filesystem::path & directory ) throw(RuntimeException);


/**
 * @brief       Starts the given bundle set profile.
 *
 * @param       path    a path to an xml profile file
 *
 * @return      a shared pointer to the started profile
 */
FWRUNTIME_API ::boost::shared_ptr< ::fwRuntime::profile::Profile > startProfile( const boost::filesystem::path & path );



/**
 * @brief       Retrieves the bundle with the given identifier and version
 *
 * @param       identifier      a string containing a bunle identifier
 * @param       version         a version (none by default)
 *
 * @return      a shared pointer to the found bundle, or empty when none
 */
FWRUNTIME_API ::boost::shared_ptr<Bundle> findBundle( const std::string & identifier, const Version & version = Version() );



/**
 * @brief       Starts the bundle specified by the given identifier.
 *
 * @param       identifier      a string containing a bundle identifier
 */
FWRUNTIME_API void startBundle(const std::string &identifier) throw(RuntimeException);


} // namespace fwRuntime


#endif // #define _FWRUNTIME_OPERATIONS_HPP_
