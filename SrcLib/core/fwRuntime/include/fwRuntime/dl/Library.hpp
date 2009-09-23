/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_DL_MODULE_HPP
#define _FWRUNTIME_DL_MODULE_HPP

#include <boost/filesystem/path.hpp>

#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/dl/Posix.hpp"
#include "fwRuntime/dl/Win32.hpp"
#include "fwRuntime/dl/Darwin.hpp"


namespace fwRuntime
{


struct Bundle;


namespace dl
{


/**
 * @brief	Defines the module class.
 * @struct 	LibraryBridge
 * @date	2004-2009
 *
 * This class is only a bridge to a native module implementor.
 *
 * @author 	IRCAD (Research and Development Team).
 */
template< typename Implementor >
struct LibraryBridge
{

	/**
	 * @brief		Constructor.
	 *
	 * @param[in]	modulePath		a path pointing the module to load without any extension
	 * 							information
	 */
	LibraryBridge( const boost::filesystem::path & modulePath ) throw()
	: m_implementor( modulePath )
	{}

	/**
	 * @brief	Destructor : does nothing.
	 */
	~LibraryBridge() throw()
	{}


	/**
	 * @brief	Tells if the module is loaded.
	 *
	 * @return	true or false
	 */
	const bool isLoaded() const throw()
	{
		return m_implementor.isLoaded();
	}

	/**
	 * @brief	Retrieves the file path of the library including the owning bundle's path.
	 *
	 * @return	a file path
	 *
	 * @see		getPath
	 */
	const boost::filesystem::path getFullPath() const throw(RuntimeException)
	{
		return m_implementor.getFullPath();
	}

	/**
	 * @brief	Retrieves the file path of the library.
	 *
	 * @remark	This path is relative to the owning bundle's path.
	 *
	 * @return	a file path
	 *
	 * @see		getFullPath
	 */
	const boost::filesystem::path getPath() const throw(RuntimeException)
	{
		return m_implementor.getPath();
	}

	/**
	 * @brief		Retrieves the address of a symbol specified by its name.
	 *
	 * @param[in]	name	a string containing the symbol name.
	 *
	 * @return		a pointer to the found symbol or null if none has been found
	 */
	void* getSymbol(const std::string& name) const throw(RuntimeException)
	{
		return m_implementor.getSymbol(name);
	}

	/**
	 * @brief	Loads the module.
	 */
	void load() throw(RuntimeException)
	{
		m_implementor.load();
	}

	/**
	 * @brief		Sets the bundle the library is attached to.
	 *
	 * @param[in]	bundle	a pointer to a bundle instance
	 */
	void setBundle(const ::fwRuntime::Bundle* bundle) throw()
	{
		m_implementor.setBundle(bundle);
	}

	/**
	 * @brief	Undloads the module.
	 */
	void unload() throw(RuntimeException)
	{
		m_implementor.unload();
	}


private:

	/**
	 * @brief	The native module implementator.
	 */
	Implementor m_implementor;


	/**
	 * @brief	Assignment operator.
	 *
	 * @remark	Assignment is fobidden for this class.
	 */
	void operator=(const LibraryBridge&) throw()
	{}
};


#ifdef linux
	typedef struct LibraryBridge< Posix > Library;
#else
#	ifdef WIN32
	typedef struct LibraryBridge< Win32 > Library;
#	else
#		ifdef __MACOSX__
			typedef struct LibraryBridge< Darwin > Library;
#		endif
#	endif
#endif


} // namespace dl

} // namespace fwRuntime


#endif // #ifdef _FWRUNTIME_DL_MODULE_HPP
