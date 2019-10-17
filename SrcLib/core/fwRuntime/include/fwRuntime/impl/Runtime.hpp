/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwRuntime/config.hpp"
#include "fwRuntime/Runtime.hpp"

namespace fwRuntime
{

namespace impl
{

class ExtensionPoint;
class Bundle;

/**
 * @brief   Defines the runtime class.
 *
 */
class Runtime : public ::fwRuntime::Runtime
{
public:
    /// Defines the bundle container type.
    typedef std::set< std::shared_ptr<Bundle> >     BundleContainer;

    /**
     * @brief   Retrieves the default runtime instance.
     */
    static Runtime* getDefault();
    static Runtime& get();

    /**
     * @brief   Constructor.
     */
    Runtime();
    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~Runtime();

    /**
     * @name    Public API implementation
     * @{
     */

    /**
     * @brief       Adds all bundle found in the given path.
     *
     * @param[in]   repository  a path that may containing bundles
     */
    virtual void addBundles( const std::filesystem::path& repository );

    /**
     * @brief       Adds all bundle found at the default location.
     *
     * @remark      The given bundle state will be altered according to the current configuration rules.
     */
    virtual void addDefaultBundles();

    /**
     * @brief       Retrieves the bundle for the specified idenfier.
     *
     * @param[in]   identifier  a string containing a bundle identifier
     * @param[in]   version     the version of the bundle (undefined by default)
     *
     * @return      a shared pointer to the found bundle or null if none
     */
    virtual SPTR( ::fwRuntime::Bundle ) findBundle( const std::string& identifier,
                                                    const Version& version = Version() ) const final;

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * An attempt is made to retrieve a registered executable factory. If none
     * is found, the creation will fail.
     *
     * @remark      This method will not try to load any bundle.
     *
     * @param[in]   type    a string containing an executable type
     *
     * @return      a pointer to the created executable instance
     */
    virtual IExecutable* createExecutableInstance( const std::string& type );

    /**
     * @brief   Create an instance of the given executable object type and configuration element.
     *
     * An attempt is made to find a registered executable factory. If none
     * is found, the bundle of the given configuration element is started in the
     * hope it will register a executable factory for the given type. Then an
     * executable factory for the given type is searched once again and the
     * instantiation procedure goes further.
     *
     * @param[in]   type                    a string containing an executable type
     * @param[in]   configurationElement    a shared pointer to the configuration element to use for the executable
     * initialization
     *
     * @return  a pointer to the created executable instance
     */
    virtual IExecutable* createExecutableInstance( const std::string& type,
                                                   SPTR(ConfigurationElement) configurationElement );

    /**
     * @brief   Retrieves the iterator on the end of the extension collection.
     * @return  an iterator
     */
    virtual ExtensionIterator extensionsEnd();

    /**
     * @brief       Retrieves the extension instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension identifier
     *
     * @return      a shared pointer to the found extension instance or null if none
     */
    virtual std::shared_ptr< Extension > findExtension( const std::string& identifier ) const;

    /// @copydoc ::fwRuntime::Runtime::getBundles
    virtual ::fwRuntime::Runtime::BundleContainer getBundles() final;
    //@}

    /**
     * @name    Bundles
     *
     * @{
     */

    /**
     * @brief       Adds a new bundle instance to the runtime system.
     *
     * @remark      The given bundle state will be altered according to the current configuration rules.
     * @param[in]   bundle  a shared pointer to the bundle instance to add
     */
    void addBundle( std::shared_ptr< impl::Bundle > bundle );

    /**
     * @brief       Unregister a bundle instance to the runtime system.
     *
     * @param[in]   bundle  a shared pointer to the bundle instance to unregister
     */
    void unregisterBundle( std::shared_ptr< impl::Bundle > bundle );

    /**
     * @brief       Retrieves the enabled bundle for the specified idenfier.
     *
     * @param[in]   identifier  a string containing a bundle identifier
     * @param[in]   version     the version of the bundle (undefined by default)
     *
     * @return      a shared pointer to the found bundle or null if none
     */
    std::shared_ptr< Bundle > findEnabledBundle( const std::string& identifier,
                                                 const Version& version = Version() ) const;

    //@}

    /**
     * @name    Executable, Executable Factories & Dynamic Executable Instanciation
     *
     * @{
     */

    /**
     * @brief       Adds a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    void addExecutableFactory( std::shared_ptr< ExecutableFactory > factory );

    /**
     * @brief       Unregister a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    void unregisterExecutableFactory( std::shared_ptr< ExecutableFactory > factory );

    /**
     * @brief       Retrieves the executable factory for the given identifier.
     *
     * @param[in]   type    a string containing a type identifier
     *
     * @return      a sgared pointer to the found executable factory or null of none
     */
    std::shared_ptr< ExecutableFactory > findExecutableFactory( const std::string& type ) const;

    /**
     * @brief       Retrieves the plugin instance for the specified bundle.
     *
     * @param[in]   bundle  a shared pointer to a bundle instance
     *
     * @return      a shared pointer to a plugin instance or null if none
     */
    std::shared_ptr< IPlugin > getPlugin( const std::shared_ptr< Bundle > bundle ) const;

    //@}

    /**
     * @name    Extensions
     *
     * @{
     */

    /**
     * @brief       Registers a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    void addExtension( std::shared_ptr<Extension> extension);

    /**
     * @brief       Unregister a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    void unregisterExtension( std::shared_ptr<Extension> extension);
    //@}

    /**
     * @name    Extension Points
     */
    //@{
    /**
     * @brief       Registers a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    void addExtensionPoint( std::shared_ptr<ExtensionPoint> point);

    /**
     * @brief       Unregister a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    void unregisterExtensionPoint( std::shared_ptr<ExtensionPoint> point);

    /**
     * @brief       Retrieves the extension point instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension point identifier
     *
     * @return      a shared pointer to the found extension point instance or null if none
     */
    std::shared_ptr< ExtensionPoint > findExtensionPoint( const std::string& identifier ) const;

    /**
     * @brief   Retrieves the extension collection.
     * @return  the extension collection.
     */
    ExtensionContainer getExtensions();

    /**
     * @brief   Retrieves the iterator on the beginning of the extension collection.
     * @return  an iterator
     */
    ExtensionIterator extensionsBegin();
    //@}

private:

    ///< Defines the executable factory container type.
    typedef std::set< std::shared_ptr< ExecutableFactory > > ExecutableFactoryContainer;

    ///< Defines the extension point container type.
    typedef std::set< std::shared_ptr<ExtensionPoint> > ExtensionPointContainer;

    ///< Defines the plugin container type.
    typedef std::vector< std::shared_ptr<IPlugin> > PluginContainer;

    static std::shared_ptr<Runtime> m_instance;     ///< The runtime instance.

    ExecutableFactoryContainer m_executableFactories;     ///< Contains all executable factories.
    ExtensionContainer m_extensions;                      ///< Contains all registered extensions.
    ExtensionPointContainer m_extensionPoints;            ///< Contains all registered extension points.
    BundleContainer m_bundles;                            ///< Contains all bundles.
    PluginContainer m_plugins;                            ///< Contains all plugins.

    std::filesystem::path m_workingPath;     ///< Path where Bundles and share folder are located.
};

} // namespace impl

} // namespace fwRuntime
