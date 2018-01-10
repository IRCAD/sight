/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRUNTIME_OPERATIONS_HPP__
#define __FWRUNTIME_OPERATIONS_HPP__

#include "fwRuntime/config.hpp"
#include "fwRuntime/ConfigurationElement.hpp"
#include "fwRuntime/Executable.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/ExtensionPoint.hpp"
#include "fwRuntime/Runtime.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"

#include <fwCore/base.hpp>

#include <boost/filesystem/path.hpp>

#include <iterator>
#include <memory>
#include <string>

namespace fwRuntime
{
struct Bundle;

namespace profile
{
class Profile;
}
}

namespace fwRuntime
{

/**
 * @brief       Retrieves the extension point having the specified identifier.
 *
 * @param[in]   identifier  a string containing an extension point identifier
 *
 * @return      a pointer to the found extension point or null if none
 */
FWRUNTIME_API std::shared_ptr<ExtensionPoint> findExtensionPoint( const std::string& identifier );

/**
 * @brief       Creates an executable instance for the specified configuration element.
 *
 * @param[in]   extension   a shared to an extension containing a configuration element
 *                          to use for the executable instance creation
 * @param[in]   element     a string containing the name of a configuration element
 * @param[in]   attribute   a string containing an element attribute name to use as
 *                          the executable type
 *
 * @return      a shared pointer to the built executable object
 */
template<typename T>
T* createExecutableInstance(
    const std::shared_ptr<Extension>    extension,
    const std::string& element = "executable",
    const std::string& attribute = "class" )
{
    // Retrieves the right configuration element.
    std::shared_ptr<ConfigurationElement> elt( extension->findConfigurationElement(element) );
    if( elt == 0 )
    {
        throw RuntimeException(element + ": configuration element not found in extension.");
    }

    // Creates the executable instance from the found configuration element.
    return createExecutableInstance<T>( elt, attribute );
}

/**
 * @brief       Creates an executable instance for the specifed configuration element.
 *
 * @param[in]   element     a shared pointer to a configuration element
 * @param[in]   attribute   a string containing an element attribute name to use as
 *                      the executable type
 *
 * @return      a shared pointer to the built executable object
 */
template<typename T>
T* createExecutableInstance(
    const std::shared_ptr<ConfigurationElement> element,
    const std::string& attribute = "class" )
{

    // Retrieves the executable type.
    if( element->hasAttribute( attribute ) == false )
    {
        throw RuntimeException( "Configuration element has no attribute '" + attribute + "'." );
    }
    const std::string type( element->getExistingAttributeValue(attribute) );

    // Creates the executable instance.
    Runtime* rntm( Runtime::getDefault()                         );
    std::unique_ptr< IExecutable > executable( rntm->createExecutableInstance(type, element) );

    // Converts the executable instance to the right type.
    T* result = dynamic_cast<T*>( executable.get() );
    if( result == 0 )
    {
        throw RuntimeException( "Executable creation failed. Bad cast" );
    }
    executable.release();

    // That's all folks !
    return result;
}

/**
 * @brief   Retrieves all extensions for the point having the specified
 *          identifier.
 *
 * @param   identifier  a string containing an extension point identifier
 * @param   output      an insert iterator used to store each found extension
 */
template<typename OutputIterator>
void getAllExtensionsForPoint(
    const std::string& identifier,
    OutputIterator output
    )
{
    std::shared_ptr< ExtensionPoint >  point = findExtensionPoint(identifier);

    if( !point )
    {
        throw RuntimeException( identifier + ": invalid extension point identifier." );
    }
    point->getAllExtensions( output );
}

/**
 * @brief   Retrieves all configuration elements for the point having the
 *          specified identifier.
 *
 * @param   identifier  a string containing an extension point identifier
 * @param   output      an insert iterator used to store found configuration
 *                      elements
 */
template<typename OutputIterator>
void getAllConfigurationElementsForPoint(
    const std::string& identifier,
    OutputIterator output
    )
{
    std::shared_ptr< ExtensionPoint >  point = findExtensionPoint(identifier);

    OSLM_TRACE("getAllConfigurationElementsForPoint(" << identifier << "Bundle" <<
               point->getBundle()->getIdentifier() );

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
        OSLM_DEBUG( "IGNORING getAllConfigurationElementsForPoint(" << identifier << ") extension point disabled");
    }

}

/**
 * @brief   Retrieves all configuration elements for the point having the
 *          specified identifier.
 *
 * This method use the container type specified by the template parameter. The
 * type of the elements of the container must be std::shared_ptr< ConfigurationElement >
 * or the compilation will fail.
 *
 * @return  a container containing shared pointers to all found configuration
 *          elements
 */
template<typename Container>
const Container getAllConfigurationElementsForPoint(const std::string& identifier)
{
    // Defines an insert iterator type for the container.
    typedef std::back_insert_iterator< Container > Inserter;

    // Collects all contributed configuratoin elements.
    Container elements;
    Inserter inserter(elements);
    getAllConfigurationElementsForPoint(identifier, inserter);

    // The job is done!
    return elements;
}

/**
 * @brief   Retrieves all executable objects for the point having the specified identifier
 *
 * @param   identifier  a string containing the extension point identifier
 * @param   attribute   a string containing the name of the element attribute containing
 *                      the executable identifier (default is "class")
 *
 * This method use the container type specified by the template parameter. The
 * type of the elements of the container must be std::shared_ptr< T >
 * or the compilation will fail (where T is the type of the executable you want to create).
 *
 * @return  a container containing shared pointers to all created executable instances
 */
template< typename Container, typename T >
const Container getAllExecutableForPoint( const std::string& identifier,
                                          const std::string& attribute = "class" )
{
    // Defines the element container
    typedef std::vector< std::shared_ptr< ConfigurationElement > > ConfigurationElementContainer;

    // Retrieves all configuration elements.
    ConfigurationElementContainer elements( getAllConfigurationElementsForPoint< ConfigurationElementContainer >(
                                                identifier) );

    // Defines an insert iterator type for the executable container.
    typedef std::back_insert_iterator< Container > Inserter;

    // Walks through collected configuration elements and create desired executable instances
    Container result;
    ConfigurationElementContainer::iterator iElement;
    Inserter iInserter( result );
    for( iElement = elements.begin(); iElement != elements.end(); ++iElement, ++iInserter )
    {
        std::shared_ptr< ConfigurationElement >    element( *iElement );
        std::shared_ptr< T >                       executable( createExecutableInstance< T >(element, attribute) );

        iInserter = executable;
    }
    return result;
}

/**
 * @brief   Retrieve the configuation element with the given identifier for the
 *          given extension point
 *
 * @param   identifier          a string containing the configuration element identifier
 * @param   pointIdentifier     a string containing the extension point identifier
 *
 * @return  a shared pointer to the found configuration element or null if none
 */
FWRUNTIME_API std::shared_ptr< ConfigurationElement > findConfigurationElement( const std::string& identifier,
                                                                                const std::string& pointIdentifier );

/**
 * @brief   Retrieve the extension having the specified identifier.
 *
 * @param   identifier  a string containing an extension identifier
 *
 * @return  a shared pointer to the found extension or null if none
 */
FWRUNTIME_API std::shared_ptr<Extension> findExtension( const std::string& identifier );

/**
 * @brief   Retrieve the filesystem valid path of resources of a bundle.
 *
 * @param   bundleIdentifier    a string containing a bundle identifier
 *
 * @return  a system valid path
 */
FWRUNTIME_API ::boost::filesystem::path getBundleResourcePath(const std::string& bundleIdentifier) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the bundle having the specified identifier.
 *
 * @param   bundleIdentifier    a string containing a bundle identifier
 * @param   path                a bundle relative path
 *
 * @return  a system valid path
 */
FWRUNTIME_API ::boost::filesystem::path getBundleResourceFilePath(const std::string& bundleIdentifier,
                                                                  const ::boost::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a bundle identifier.
 * For instance for "bundle-0.2/dir/file.txt", the function returns
 * "/home/login/f4s/build/share/bundle-0.2/dir/file.txt"
 *
 * @param   path                relative path whose first element is a bundle identifier
 *
 * @return  a system valid path
 */
FWRUNTIME_API ::boost::filesystem::path getBundleResourceFilePath(const ::boost::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path whose first element is a library identifier.
 * For instance for "fwLib-0.2/dir/file.txt", the function returns
 * "/home/login/f4s/build/share/fwLib-0.2/dir/file.txt"
 *
 * @param   path                relative path whose first element is a library identifier
 *
 * @return  a system valid path
 */
FWRUNTIME_API ::boost::filesystem::path getLibraryResourceFilePath(const ::boost::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a resource path
 *  whose first element is a library or a bundle identifier.
 *
 * For instance for a library:
 *  - "fwLib-0.2/dir/file.txt"
 * the function returns:
 *  - "/home/login/f4s/build/share/fwLib-0.2/dir/file.txt"
 *
 * For instance for a bundle:
 *  - "bundle-0.2/dir/file.txt"
 *  the function returns:
 * - "/home/login/f4s/build/share/bundle-0.2/dir/file.txt"
 *
 * @param   path   relative path whose first element is a bundle or library identifier
 *
 * @return  a system valid path or an empty path if nothing is found
 */
FWRUNTIME_API ::boost::filesystem::path getResourceFilePath(const ::boost::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified bundle.
 *
 * @param   bundle  a pointer to a bundle instance
 * @param   path    a path relative to the bundle
 *
 * @return  a system valid path
 */
FWRUNTIME_API ::boost::filesystem::path getBundleResourcePath( std::shared_ptr<Bundle> bundle,
                                                               const ::boost::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the bundle of the specified configuration element.
 *
 * @param   element a shared pointer to a configuration element instance
 * @param   path    a path relative to the bundle
 *
 * @return  a system valid path
 */
FWRUNTIME_API ::boost::filesystem::path getBundleResourcePath( std::shared_ptr<ConfigurationElement> element,
                                                               const ::boost::filesystem::path& path) noexcept;

/**
 * @brief   Retrieve a filesystem valid path for a path relative to the specified executable instance.
 *
 * @param   executable  a pointer to an executable instance
 * @param   path        a path relative to the bundle
 *
 * @return  a system valid path
 */
FWRUNTIME_API ::boost::filesystem::path getBundleResourcePath(const IExecutable* executable,
                                                              const ::boost::filesystem::path& path) noexcept;

/**
 * @brief   Loads all bundles that can be found in the specified directory.
 *
 * @param   directory   a path to the directory to explore for bundles
 */
FWRUNTIME_API void addBundles( const boost::filesystem::path& directory );

/**
 * @brief   Starts the given bundle set profile.
 *
 * @param   path    a path to an xml profile file
 *
 * @return  a shared pointer to the started profile
 */
FWRUNTIME_API std::shared_ptr< ::fwRuntime::profile::Profile > startProfile( const boost::filesystem::path& path );

/**
 * @brief   Retrieves the bundle with the given identifier and version
 *
 * @param   identifier  a string containing a bunle identifier
 * @param   version     a version (none by default)
 *
 * @return  a shared pointer to the found bundle, or empty when none
 */
FWRUNTIME_API std::shared_ptr<Bundle> findBundle( const std::string& identifier, const Version& version = Version() );

/**
 * @brief   Starts the bundle specified by the given identifier.
 *
 * @param   identifier  a string containing a bundle identifier
 */
FWRUNTIME_API void startBundle(const std::string& identifier);

} // namespace fwRuntime

#endif // __FWRUNTIME_OPERATIONS_HPP__
