/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_FWRUNTIME_HPP
#define _FWRUNTIME_FWRUNTIME_HPP

#include <boost/shared_ptr.hpp>
#include <boost/filesystem/path.hpp>
#include <set>
#include <vector>

#include "fwRuntime/config.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"

namespace fwRuntime
{
	struct Bundle;
	struct ConfigurationElement;
	struct Extension;
	struct ExtensionPoint;
	struct IExecutable;
	struct ExecutableFactory;
	struct IPlugin;
}


/**
 * @namespace 	runtime
 * @brief 		Namespace runtime
 * @date		2004-2009
 * @author 		IRCAD (Research and Development Team).
 */
namespace fwRuntime
{

/**
 * @brief	Defines the runtime class.
 * @struct 	Runtime
 * @date	2004-2009
 * @author 	IRCAD (Research and Development Team).
 */
struct Runtime
{
	/**
	 * @name	Type Definitions
	 */
	//@{
	typedef std::set< ::boost::shared_ptr<Bundle> >		BundleContainer;	///< Defines the bundle container type.
	typedef BundleContainer::iterator					BundleIterator;		///< Defines the bundle container iterator type.

	typedef std::set< ::boost::shared_ptr<Extension> >	ExtensionContainer;	///< Defines the extension container type.
	typedef ExtensionContainer::iterator				ExtensionIterator;	///< Defines the extension container type.
	//@}


	/**
	 * @brief	Retrieves the default runtime instance.
	 */
	FWRUNTIME_API static Runtime* getDefault();


	/**
	 * @brief	Destructor : does nothing.
	 */
	~Runtime();


	/**
	 * @name	Bundles
	 *
	 * @{
	 */

	/**
	 * @brief		Adds a new bundle instance to the runtime system.
	 *
	 * @remark		The given bundle state will be altered according to the current configuration rules.
	 * @param[in]	bundle	a shared pointer to the bundle instance to add
	 */
	FWRUNTIME_API void addBundle( ::boost::shared_ptr< Bundle > bundle ) throw( RuntimeException );

	/**
	 * @brief		Adds all bundle found in t he given path.
	 *
	 * @param[in]	repository	a path that may containing bundles
	 */
	FWRUNTIME_API void addBundles( const ::boost::filesystem::path & repository ) throw( RuntimeException );

	/**
	 * @brief	Retrieves the iterator on the begining of the bundle collection.
	 *
	 * @return	an iterator
	 */
	FWRUNTIME_API BundleIterator bundlesBegin();

	/**
	 * @brief	Retrieves the iterator on the end of the bundle collection.
	 *
	 * @return	an iterator
	 */
	FWRUNTIME_API BundleIterator bundlesEnd();

	/**
	 * @brief		Retrieves the bundle for the specified idenfier.
	 *
	 * @param[in]	identifier	a string containing a bundle identifier
	 * @param[in]	version		the version of the bundle (undefined by default)
	 *
	 * @return		a shared pointer to the found bundle or null if none
	 */
	FWRUNTIME_API ::boost::shared_ptr< Bundle > findBundle( const std::string & identifier, const Version & version = Version() ) const;

	//@}


	/**
	 * @name	Executable, Executable Factories & Dynamic Executable Instanciation
	 *
	 * @{
	 */

	/**
	 * @brief		Adds a new executable factory instance to the runtime system.
	 *
	 * @param[in]	factory	a shared pointer to an executable factory
	 */
	FWRUNTIME_API void addExecutableFactory( ::boost::shared_ptr< ExecutableFactory > factory ) throw(RuntimeException);

	/**
	 * @brief	Create an instance of the given executable object type.
	 *
	 * An attempt is made to retrieve a registered executable factory. If none
	 * is found, the creation will fail.
	 *
	 * @remark		This method will not try to load any bundle.
	 *
	 * @param[in]	type	a string containing an executable type
	 *
	 * @return		a pointer to the created executable instance
	 */
	FWRUNTIME_API IExecutable * createExecutableInstance( const std::string & type ) throw( RuntimeException );

	/**
	 * @brief	Create an instance of the given executable object type and configuration element.
	 *
	 * An attempt is made to find a registered executable factory. If none
	 * is found, the bundle of the given configuration element is started in the
	 * hope it will register a executable factory for the given type. Then an
	 * executable factory for the given type is searched once again and the
	 * instantiation procedure goes further.
	 *
	 * @param[in]	type					a string containing an executable type
	 * @param[in]	configurationElement	a shared pointer to the configuration element to use for the executable initialization
	 *
	 * @return	a pointer to the created executable instance
	 */
	FWRUNTIME_API IExecutable * createExecutableInstance( const std::string & type, ::boost::shared_ptr< ConfigurationElement > configurationElement ) throw( RuntimeException );

	/**
	 * @brief		Retrieves the executable factory for the given identifier.
	 *
	 * @param[in]	type	a string containing a type identifier
	 *
	 * @return		a sgared pointer to the found executable factory or null of none
	 */
	FWRUNTIME_API ::boost::shared_ptr< ExecutableFactory > findExecutableFactory( const std::string & type ) const;

	/**
	 * @brief		Retrieves the plugin instance for the specified bundle.
	 *
	 * @param[in]	bundle	a shared pointer to a bundle instance
	 *
	 * @return		a shared pointer to a plugin instance or null if none
	 */
	FWRUNTIME_API ::boost::shared_ptr< IPlugin > getPlugin( const ::boost::shared_ptr< Bundle > bundle ) const;

	//@}


	/**
	 * @name	Extensions
	 *
	 * @{
	 */

	/**
	 * @brief		Registers a new extension.
	 *
	 * @param[in]	extension	a shared pointer to the extension to register
	 */
	FWRUNTIME_API void addExtension( ::boost::shared_ptr<Extension> extension) throw(RuntimeException);

	/**
	 * @brief	Retrieves the iterator on the beginning of the extension collection.
	 *
	 * @return	an iterator
	 */
	FWRUNTIME_API ExtensionIterator extensionsBegin();

	/**
	 * @brief	Retrieves the iterator on the end of the extension collection.
	 *
	 * @return	an iterator
	 */
	FWRUNTIME_API ExtensionIterator extensionsEnd();

	/**
	 * @brief		Retrieves the extension instance matching the specified identifier.
	 *
	 * @param[in]	identifier	a string containing an extension identifier
	 *
	 * @return		a shared pointer to the found extension instance or null if none
	 */
	FWRUNTIME_API ::boost::shared_ptr< Extension > findExtension( const std::string & identifier ) const;

	//@}


	/**
	 * @name	Extension Points
	 */
	//@{
	/**
	 * @brief		Registers a new extension point.
	 *
	 * @param[in]	point	a pointer to the extension point to register
	 */
	FWRUNTIME_API void addExtensionPoint( ::boost::shared_ptr<ExtensionPoint> point) throw(RuntimeException);

	/**
	 * @brief		Retrieves the extension point instance matching the specified identifier.
	 *
	 * @param[in]	identifier	a string containing an extension point identifier
	 *
	 * @return		a shared pointer to the found extension point instance or null if none
	 */
	FWRUNTIME_API ::boost::shared_ptr< ExtensionPoint > findExtensionPoint( const std::string & identifier ) const;
	//@}

#ifdef _USE_FW_THREADS
	// all the threads currently alive - as soon as the thread terminates, it's
	// removed from the array
	wxArrayThread m_threads;

	// crit section protects access to all of the arrays below
	wxCriticalSection m_critsect;

	// semaphore used to wait for the threads to exit, see MyFrame::OnQuit()
	wxSemaphore m_semAllDone;

	// the last exiting thread should post to m_semAllDone if this is true
	// (protected by the same m_critsect)
	bool m_waitingUntilAllDone;
#endif


private:

	typedef std::set< ::boost::shared_ptr< ExecutableFactory > > ExecutableFactoryContainer;	///< Defines the executable factory container type.
	typedef std::set< ::boost::shared_ptr<ExtensionPoint> > ExtensionPointContainer;			///< Defines the extension point container type.
	typedef std::vector< ::boost::shared_ptr<IPlugin> > PluginContainer;						///< Defines the plugin container type.

	static ::boost::shared_ptr<Runtime> m_instance;	///< The runtime instance.

	ExecutableFactoryContainer					m_executableFactories;	///< Contains all executable factories.
	ExtensionContainer							m_extensions;			///< Contains all registered extensions.
	ExtensionPointContainer						m_extensionPoints;		///< Contains all registered extension points.
	BundleContainer								m_bundles;				///< Contains all bundles.
	PluginContainer								m_plugins;				///< Contains all plugins.


	/**
	 * @brief	Constructor.
	 */
	Runtime();

};


} // namespace fwRuntime


#endif // #define _FWRUNTIME_FWRUNTIME_HPP
