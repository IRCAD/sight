/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_BUNDLE_HPP
#define _FWRUNTIME_BUNDLE_HPP

#include <map>
#include <set>
#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem/path.hpp>

#include "fwCore/base.hpp"

#include "fwRuntime/config.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"
#include "fwRuntime/dl/Library.hpp"

namespace fwRuntime
{
    struct Extension;
    struct ExtensionPoint;
    struct ExtensionRegistry;
    struct IExecutable;
    struct ExecutableFactory;
    struct IPlugin;
    struct Runtime;

    namespace io
    {
        struct BundleDescriptorReader;
    }
}



namespace fwRuntime
{



/**
 * @brief   Defines the bundle class.
 * @struct  Bundle
 * @date    2004-2009
 * @author  IRCAD (Research and Development Team).
 */
struct Bundle : public ::boost::enable_shared_from_this< Bundle >
{
    friend struct ::fwRuntime::io::BundleDescriptorReader;


    /**
     * @name    Type definitions
     */
    //@{
    typedef std::set< SPTR( ExecutableFactory ) >      ExecutableFactoryContainer;     ///< Defines the executable factory container type.
    typedef ExecutableFactoryContainer::const_iterator ExecutableFactoryConstIterator; ///< Defines the executable factory container constant iterator type.

    typedef std::set< SPTR( Extension ) >      ExtensionContainer;             ///< Defines the extension container type.
    typedef ExtensionContainer::const_iterator ExtensionConstIterator;         ///< Defines the extension container constant iterator type.

    typedef std::set< SPTR( ExtensionPoint ) >      ExtensionPointContainer;        ///< Defines the extension point container type.
    typedef ExtensionPointContainer::const_iterator ExtensionPointConstIterator;    ///< Defines the extension point container constant iterator type.

    typedef std::set< SPTR(dl::Library) >     LibraryContainer;               ///< Defines the dynamic library container type.
    typedef LibraryContainer::const_iterator  LibraryConstIterator;           ///< Defines the dynamic library container constant iterator type.
    //@}


    /**
     * @brief   Retrieves the pointer to the bundle that is currently loading its dynamic libraries
     */
    static SPTR( Bundle ) getLoadingBundle();


    /**
     * @name    Dynamic Libraries
     */
    //@{
    /**
     * @brief       Adds the specified library to the bundle.
     *
     * @param[in]   library a shared pointer to the library to add
     */
    FWRUNTIME_API void addLibrary( SPTR( dl::Library ) library );

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed dynamic library collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API LibraryConstIterator librariesBegin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed dynamic library collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API LibraryConstIterator librariesEnd() const;
    //@}


    /**
     * @name    Executable Factories & Dynamic Objects Instantiation
     */
    //@{
    /**
     * @brief       Adds an executable factory instance to the bundle.
     *
     * @param[in]   factory a shared pointer to the executable factory instance to add
     */
    FWRUNTIME_API void addExecutableFactory( SPTR( ExecutableFactory ) factory );

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * @param   type    a string containing an executable type
     *
     * @return  a pointer to the created executable instance
     *
     * @todo    not implemented and not used !!!!!!!
     */
    FWRUNTIME_API IExecutable * createExecutableInstance( const std::string & type ) throw( RuntimeException );

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed executable factory collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExecutableFactoryConstIterator executableFactoriesBegin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed executable factory collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExecutableFactoryConstIterator executableFactoriesEnd() const;

    /**
     * @brief       Retrieves the executable factory instance for the specified type name.
     *
     * @param[in]   type    a string containing a type name
     *
     * @return      a shared pointer to the found executable factory instance or null if none
     */
    FWRUNTIME_API SPTR( ExecutableFactory ) findExecutableFactory( const std::string & type ) const;
    //@}


    /**
     * @name    Extensions
     */
    //@{
    /**
     * @brief       Adds the specified extension to the bundle.
     *
     * @param[in]   extension   a shared pointer to the extension to add
     */
    void addExtension( SPTR( Extension ) extension );

    /**
     * @brief       Tells if an specific extension exists.
     *
     * @param[in]   identifier  the extension identifier
     *
     * @return      true or false
     */
    FWRUNTIME_API const bool hasExtension(const std::string & identifier) const;

    /**
     * @brief       Search a specific extension in the bundle to enable or
     *              disable it
     *
     * @remark      if the extension is not found, the method do nothing.
     *
     * @param[in]   identifier  the extension identifier
     * @param[in]   enable      enable or disable this extension
     */
    FWRUNTIME_API void setEnableExtension(const std::string & identifier, const bool enable);

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed extension collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExtensionConstIterator extensionsBegin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed extension collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExtensionConstIterator extensionsEnd() const;
    //@}


    /**
     * @name    Extension Points
     */
    //@{
    /**
     * @brief       Adds the specified extension point to the bundle.
     *
     * @param[in]   extension   a shared pointer to the extension point to add
     */
    void addExtensionPoint( SPTR( ExtensionPoint ) extension );

    /**
     * @brief       Retrieves the extension point for the given identifier.
     *
     * @param[in]   identifier  a string containing the extension point identifier
     *
     * @return      a shared pointer to the found extensoin point, may be empty if none
     */
    FWRUNTIME_API SPTR( ExtensionPoint ) findExtensionPoint( const std::string & identifier ) const;

    /**
     * @brief       Tells if a specific extension point exists.
     *
     * @param[in]   identifier  the extension point identifier
     *
     * @return      true or false
     */
    FWRUNTIME_API const bool hasExtensionPoint(const std::string & identifier) const;

    /**
     * @brief       Search a specific extension point in the bundle to enable or
     *              disable it
     *
     * @remark      if the extension point is not found, the method do nothing.
     *
     * @param[in]   identifier  the extension point identifier
     * @param[in]   enable      enable or disable this extension point
     */
    FWRUNTIME_API void setEnableExtensionPoint(const std::string & identifier, const bool enable);

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed extension point collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExtensionPointConstIterator extensionPointsBegin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed extension point collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExtensionPointConstIterator extensionPointsEnd() const;
    //@}


    /**
     * @name    Misc
     */
    //@{
    /**
     * @brief       Adds a requirement to the bundle.
     *
     * @param[in]   requirement a string containing a bundle identifier that is required
     */
    FWRUNTIME_API void addRequirement( const std::string & requirement );

    /**
     * @brief   Retrieves the bundle identifier.
     *
     * @return  a string containing the bundle identifier
     */
    FWRUNTIME_API const std::string & getIdentifier() const;

    /**
     * @brief   Retrieves the bundle location.
     *
     * @return  a path representing the bundle location
     */
    FWRUNTIME_API const boost::filesystem::path & getLocation() const;

    /**
     * @brief   Retrieves the class representing the bundle executable part.
     *
     * @return  a string containing the bundle's plugin class
     */
    FWRUNTIME_API const std::string getClass() const;

    /**
     * @brief   Retrieves the version of the bundle.
     *
     * @return  the bundle version
     */
    FWRUNTIME_API const Version & getVersion() const;

    /**
     * @brief   Retrieves the plugin instance for the specified bundle identifier.
     *
     * @return  a shared pointer to a plugin instance or null if the bundle has not been started.
     */
    FWRUNTIME_API SPTR( IPlugin ) getPlugin() const;
    //@}


    /**
     * @name        State Management
     */
    //@{
    /**
     * @brief   Tells if the bundle is enable.
     */
    FWRUNTIME_API const bool isEnable() const;

    /**
     * @brief   Changes the enable state of the bundle.
     *
     * @remark  It is possible to disable a started bundle but this
     *          will have no effect.
     */
    FWRUNTIME_API void setEnable( const bool state );

    /**
     * @brief   Starts the bundle.
     *
     * @remark  The bundle must be enabled to be able to start.
     */
    FWRUNTIME_API void start() throw(RuntimeException);

    FWRUNTIME_API void stop() throw(RuntimeException);
    //@}


    /**
     * @brief   Initialize the bundle.
     *
     * @remark  The bundle and it's own dependencies must be started to be able to be initialized.
     */
    FWRUNTIME_API void initialize() throw(RuntimeException);

    FWRUNTIME_API void uninitialize() throw(RuntimeException);
    //@}


    /**
     * @name    Parameters Management
     */
    //@{
    /**
     * @brief       Adds a parameter to the bundle.
     *
     * @param[in]   identifier  a string containing the parameter identifier
     * @param[in]   value       a string containing the parameter value
     */
    FWRUNTIME_API void addParameter( const std::string & identifier, const std::string & value );

    /**
     * @brief       Retrieves the value of the given parameter
     *
     * @remark      When no such parameter has been found, an empty string is returned.
     *
     * @see         hasParameter
     *
     * @param[in]   identifier  a string containing a parameter identifier
     *
     * @return      a string containing the parameter value
     */
    FWRUNTIME_API const std::string getParameterValue( const std::string & identifier ) const;

    /**
     * @brief   Tells if a parameter exists.
     *
     * @return  true or false
     */
    FWRUNTIME_API const bool hasParameter( const std::string & name ) const;
    //@}


    FWRUNTIME_API bool isStarted()     {return m_started;};
    FWRUNTIME_API bool isInitialized() {return m_initialized;};

protected:

    /**
     * @brief       Constructor.
     *
     * @param[in]   location    a path to the directory containing the bundle
     * @param[in]   id          a string containing the bundle identifier
     * @param[in]   version     a string containing the bundle version
     *
     * @todo        test parameters validity
     *
     */
    Bundle( const boost::filesystem::path   & location,
            const std::string               & id,
            const std::string               & version );

    /**
     * @brief       Constructor.
     *
     * @param[in]   location        a path to the directory containing the bundle
     * @param[in]   id              a string containing the bundle identifier
     * @param[in]   version         a string containing the bundler version
     * @param[in]   pluginClass     a string containing the bundle's pugin class name
     *
     * @todo        test parameters validity
     *
     */
    Bundle( const boost::filesystem::path   & location,
            const std::string               & id,
            const std::string               & version,
            const std::string               & pluginClass );


private:

    typedef std::set< std::string >                 RequirementContainer;   ///< Defines the requirement container type.
    typedef std::map< std::string, std::string >    ParameterContainer;     ///< defines the parameter container type

    static SPTR( Bundle )           m_loadingBundle;        ///< a pointer to the bundle that is currently loading its dynamic libaries

    const boost::filesystem::path   m_location;             ///< the path to the bundle location
    const std::string               m_identifier;           ///< a string containing the bundle identifier
    const Version                   m_version;              ///< defines the version of the bundle
    const std::string               m_class;                ///< a string containing the bundle's plugin class name
    bool                            m_enable;               ///< a boolean telling if the bundle is enabled or not
    ExtensionContainer              m_extensions;           ///< all extensions
    ExtensionPointContainer         m_extensionPoints;      ///< all extension points
    ExecutableFactoryContainer      m_executableFactories;  ///< all executable factories
    LibraryContainer                m_libraries;            ///< all libaries that are part of the bundle
    RequirementContainer            m_requirements;         ///< all requirements of the bundle
    SPTR( IPlugin )                 m_plugin;               ///< a shared pointer to the plugin instance
    ParameterContainer              m_parameters;           ///< all parameters

    bool m_started;
    bool m_initialized;

    /**
     * @brief   Assignement operator.
     *
     * @remark  Assignement is forbidden for this class.
     */
    void operator= (const Bundle & );

    /**
     * @brief   Load bundle's library in the current process.
     */
    void loadLibraries() throw(RuntimeException);

    /**
     * @brief   load all requirement needed by the bundle to work
     */
    void loadRequirements() throw(RuntimeException);

    /**
     * @brief   Starts the plugin associated to the bundle.
     */
    void startPlugin() throw(RuntimeException);

};



} // namespace fwRuntime


#endif // #define _FWRUNTIME_BUNDLE_HPP
