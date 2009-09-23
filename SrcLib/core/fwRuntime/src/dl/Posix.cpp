/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifdef linux


#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/dl/Posix.hpp"
#include "fwRuntime/dl/PosixNameDecorator.hpp"


namespace fwRuntime
{

namespace dl
{


Posix::Posix( const boost::filesystem::path & modulePath ) throw()
: Native		( modulePath, ::boost::shared_ptr< INameDecorator >(new PosixNameDecorator()) ),
  m_handle	( 0 )
{}


Posix::~Posix() throw()
{}


const bool Posix::isLoaded() const throw()
{
	return m_handle != 0;
}


void * Posix::getSymbol( const std::string & name ) const throw(RuntimeException)
{
	void* result = 0;
	if(isLoaded() == true)
	{
		dlerror(); /* Clear existing error */
		result = dlsym(m_handle, name.c_str());
		if(result == 0) /* Check for possible errors */
		{
			std::string message(dlerror());
			if(message.empty() == false)
			{
				throw RuntimeException("Symbol retrieval failed. " + message);
			}
		}
	}
	return result;
}


void Posix::load() throw(RuntimeException)
{
	if(m_handle == 0)
	{
		// Opens the dynamic library.
		m_handle = dlopen(getFullPath(true).native_file_string().c_str(), RTLD_LAZY|RTLD_GLOBAL);
		if(m_handle == 0)
		{
			std::string message(dlerror());
			throw RuntimeException("Module load failed. " + message);
		}
	}
}


void Posix::unload() throw(RuntimeException)
{
	if(m_handle != 0)
	{
		int result;
		result = dlclose(m_handle);
		if(result != 0)
		{
			std::string message(dlerror());
			throw RuntimeException("Module unload failed. " + message);
		}
		m_handle = 0;
	}
}


} // namespace dl

} // namespace fwRuntime


#endif // #ifdef linux
