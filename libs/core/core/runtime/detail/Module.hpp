/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "core/runtime/detail/dl/Library.hpp"
#include "core/runtime/Module.hpp"

#include <core/base.hpp>

#include <filesystem>
#include <map>
#include <set>
#include <string>

namespace sight::core::runtime
{

namespace detail
{

class ExtensionPoint;

/**
 * @brief   Defines the module class.
 *
 */
class Module :  public core::runtime::Module,
                public std::enable_shared_from_this< Module >
{
public:
    /**
     * @name    Type definitions
     */
    //@{
    typedef std::set< SPTR( ExecutableFactory ) >      ExecutableFactoryContainer;     ///< Defines the executable
                                                                                       // factory container type.
    typedef ExecutableFactoryContainer::const_iterator ExecutableFactoryConstIterator; ///< Defines the executable
                                                                                       // factory container constant
                                                                                       // iterator type.

    typedef std::set< SPTR( ExtensionPoint ) >      ExtensionPointContainer;        ///< Defines the extension point
                                                                                    // container type.
    typedef ExtensionPointContainer::const_iterator ExtensionPointConstIterator;    ///< Defines the extension point
                                                                                    // container constant iterator type.

    ///< Defines the extension container constant iterator type.
    typedef ExtensionContainer::const_iterator ExtensionConstIterator;
    //@}

    /**
     * @brief       Constructor.
     *
     * @param[in]   location        a path to the directory containing the module
     * @param[in]   id              a string containing the module identifier
     * @param[in]   version         a string containing the moduler version
     * @param[in]   pluginClass     a string containing the module's pugin class name
     *
     * @todo        test parameters validity
     *
     */
    Module( const std::filesystem::path& location,
            const std::string& id,
            const std::string& pluginClass = "");

    /**
     * @name    Public API implementation
     * @{
     */

    /**
     * @brief   Starts the module.
     * @remark  The module must be enabled to be able to start.
     */
    void start() final;
    void stop() final;

    /**
     * @brief   Retrieves the module identifier.
     *
     * @return  a string containing the module identifier
     */
    const std::string& getIdentifier() const final;

    /**
     * @brief   Retrieves the library name if it exists.
     *
     * @return  a path representing the module location, can be empty if no library is set
     */
    const std::string getLibraryName() const final;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    const std::filesystem::path& getLibraryLocation() const final;

    /**
     * @brief   Retrieves the module location.
     *
     * @return  a path representing the module location
     */
    const std::filesystem::path& getResourcesLocation() const final;

    /**
     * @brief   Retrieves the class representing the module executable part.
     *
     * @return  a string containing the module's plugin class
     */
    const std::string getClass() const final;

    /**
     * @brief   Retrieves the plugin instance for the specified module identifier.
     *
     * @return  a shared pointer to a plugin instance or null if the module has not been started.
     */
    SPTR( IPlugin ) getPlugin() const final;

    /** @copydoc core::runtime::Module */
    const std::string getParameterValue( const std::string& identifier ) const final;

    /**
     * @brief   Tells if a parameter exists.
     * @return  true or false
     */
    bool hasParameter( const std::string& name ) const final;

    /// @copydoc core::runtime::Module::getExtensions
    ExtensionContainer getExtensions( ) const final;

    /// @copydoc core::runtime::Module::isEnabled
    bool isEnabled() const final;
    //@}

    /**
     * @brief   Retrieves the pointer to the module that is currently loading its dynamic libraries
     */
    static SPTR( Module ) getLoadingModule();

    /**
     * @name    Dynamic Libraries
     */
    //@{
    /**
     * @brief       Sets the specified library to the module.
     *
     * @param[in]   library a shared pointer to the library to add
     */
    void setLibrary( SPTR( dl::Library ) library );
    //@}

    /**
     * @name    Executable Factories & Dynamic Objects Instantiation
     */
    //@{
    /**
     * @brief       Adds an executable factory instance to the module.
     *
     * @param[in]   factory a shared pointer to the executable factory instance to add
     */
    void addExecutableFactory( SPTR( ExecutableFactory ) factory );

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * @param   type    a string containing an executable type
     *
     * @return  a pointer to the created executable instance
     *
     * @todo    not implemented and not used !!!!!!!
     */
    IExecutable* createExecutableInstance( const std::string& type );

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed executable factory collection.
     *
     * @return  an iterator
     */
    ExecutableFactoryConstIterator executableFactoriesBegin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed executable factory collection.
     *
     * @return  an iterator
     */
    ExecutableFactoryConstIterator executableFactoriesEnd() const;

    /**
     * @brief       Retrieves the executable factory instance for the specified type name.
     *
     * @param[in]   type    a string containing a type name
     *
     * @return      a shared pointer to the found executable factory instance or null if none
     */
    SPTR( ExecutableFactory ) findExecutableFactory( const std::string& type ) const;
    //@}

    /**
     * @name    Extensions
     */
    //@{
    /**
     * @brief       Adds the specified extension to the module.
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
    bool hasExtension(const std::string& identifier) const;

    /**
     * @brief       Search a specific extension in the module to enable or
     *              disable it
     *
     * @remark      if the extension is not found, the method do nothing.
     *
     * @param[in]   identifier  the extension identifier
     * @param[in]   enable      enable or disable this extension
     */
    void setEnableExtension(const std::string& identifier, const bool enable);

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed extension collection.
     *
     * @return  an iterator
     */
    ExtensionConstIterator extensionsBegin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed extension collection.
     *
     * @return  an iterator
     */
    ExtensionConstIterator extensionsEnd() const;
    //@}

    /**
     * @name    Extension Points
     */
    //@{
    /**
     * @brief       Adds the specified extension point to the module.
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
    SPTR( ExtensionPoint ) findExtensionPoint( const std::string& identifier ) const;

    /**
     * @brief       Tells if a specific extension point exists.
     *
     * @param[in]   identifier  the extension point identifier
     *
     * @return      true or false
     */
    bool hasExtensionPoint(const std::string& identifier) const;

    /**
     * @brief       Search a specific extension point in the module to enable or
     *              disable it
     *
     * @remark      if the extension point is not found, the method do nothing.
     *
     * @param[in]   identifier  the extension point identifier
     * @param[in]   enable      enable or disable this extension point
     */
    void setEnableExtensionPoint(const std::string& identifier, const bool enable);

    /**
     * @brief   Retrieves the iterator on the first item
     *          in the managed extension point collection.
     *
     * @return  an iterator
     */
    ExtensionPointConstIterator extensionPointsBegin() const;

    /**
     * @brief   Retrieves the iterator on the ending item
     *          in the managed extension point collection.
     *
     * @return  an iterator
     */
    ExtensionPointConstIterator extensionPointsEnd() const;
    //@}

    /**
     * @name    Misc
     */
    //@{
    /**
     * @brief       Adds a requirement to the module.
     *
     * @param[in]   requirement a string containing a module identifier that is required
     */
    void addRequirement( const std::string& requirement );
    //@}

    /**
     * @name        State Management
     */
    //@{
    /**
     * @brief   Changes the enable state of the module.
     *
     * @remark  It is possible to disable a started module but this
     *          will have no effect.
     */
    void setEnable( const bool state );
    //@}

    /**
     * @brief   Initialize the module.
     *
     * @remark  The module and it's own dependencies must be started to be able to be initialized.
     */
    void initialize();

    void uninitialize();
    //@}

    /**
     * @name    Parameters Management
     */
    //@{
    /**
     * @brief       Adds a parameter to the module.
     *
     * @param[in]   identifier  a string containing the parameter identifier
     * @param[in]   value       a string containing the parameter value
     */
    void addParameter( const std::string& identifier, const std::string& value );
    //@}

    bool isStarted()
    {
        return m_started;
    }

    //------------------------------------------------------------------------------

    bool isInitialized()
    {
        return m_initialized;
    }

    static std::string getModuleStr(const std::string& identifier);

private:

    typedef std::set< std::string >                 RequirementContainer;    ///< Defines the requirement container
                                                                             // type.
    typedef std::map< std::string, std::string >    ParameterContainer;     ///< defines the parameter container type

    static SPTR( Module ) m_loadingModule;              ///< a pointer to the module that is currently loading its
                                                        // dynamic libraries

    std::filesystem::path m_libraryLocation;            ///< the path to the module libraries
    const std::filesystem::path m_resourcesLocation;    ///< the path to the module resources
    const std::string m_identifier;                     ///< a string containing the module identifier
    const std::string m_class;                          ///< a string containing the module's plugin class name
    ExtensionContainer m_extensions;                    ///< all extensions
    ExtensionPointContainer m_extensionPoints;          ///< all extension points
    ExecutableFactoryContainer m_executableFactories;   ///< all executable factories
    RequirementContainer m_requirements;                ///< all requirements of the module
    SPTR( IPlugin )  m_plugin;                          ///< a shared pointer to the plugin instance
    SPTR( dl::Library ) m_library;                      ///< library that is part of the module
    ParameterContainer m_parameters;                    ///< all parameters

    bool m_enabled { true };                           ///< a boolean telling if the module is enabled or not
    bool m_started { false };
    bool m_initialized { false };

    /**
     * @brief   Assignement operator.
     *
     * @remark  Assignement is forbidden for this class.
     */
    void operator= (const Module& );

    /**
     * @brief   Load module's library in the current process.
     */
    void loadLibraries();

    /**
     * @brief   load all requirement needed by the module to work
     */
    void loadRequirements();

    /**
     * @brief   Starts the plugin associated to the module.
     */
    void startPlugin();
};

} // namespace detail

} // namespace sight::core::runtime
