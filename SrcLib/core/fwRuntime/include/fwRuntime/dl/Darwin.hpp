/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_DL_DARWIN_HPP
#define _FWRUNTIME_DL_DARWIN_HPP


#ifdef __MACOSX__


#include <dlfcn.h>

#include "fwRuntime/dl/Native.hpp"


namespace fwRuntime
{

namespace dl
{


/**
 * @brief	Implements a posix native module.
 * @struct	Darwin
 * @date 	2004-2009
 *
 * @author 	IRCAD (Research and Development Team).
 */
struct Darwin : public Native
{

	/**
	 * @brief	Constructor.
	 *
	 * @param	modulePath	a path to the module to path
	 *
	 * @param	partialName		a boolean telling if the module filename is just
	 * 							a part of the real filename
	 */
	//Darwin(const boost::filesystem::path& modulePath, const bool partialName) throw();
	Darwin( const boost::filesystem::path & modulePath ) throw();

	/**
	 * @brief	Destructor.
	 */
	~Darwin() throw();


	/**
	 * @see	::fwRuntime::dl::Native#isLoaded
	 */
	const bool isLoaded() const throw();

	/**
	 * @brief	Retrieves the address of a symbol specified by its name.
	 *
	 * @param	name	a string containing the symbol name.
	 *
	 * @return	a pointer to the found symbol or null if none has been found
	 */
	void* getSymbol(const std::string& name) const throw(RuntimeException);

	/**
	 * @see	::fwRuntime::dl::Native#load
	 */
	void load() throw(RuntimeException);

	/**
	 * @see	::fwRuntime::dl::Native#unload
	 */
	void unload() throw(RuntimeException);


protected:

	/**
	 * @brief	Retrieves the native module file prefix.
	 *
	 * @return	a string containing the native module file prefix
	 */
	const std::string getNativeFilePrefix() const throw();

	/**
	 * @brief	Retrieves the native module file suffix.
	 *
	 * @return	a string containing the native module file suffix
	 */
	const std::string getNativeFileSuffix() const throw();

private:

	/**
	 * @brief	The handle of the loaded module.
	 */
	void *m_handle;

};


} // namespace dl

} // namespace fwRuntime

#endif // #ifdef __MACOSX__

#endif // _FWRUNTIME_DL_DARWIN_HPP
