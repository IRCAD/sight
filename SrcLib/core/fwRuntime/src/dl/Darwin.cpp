/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifdef __MACOSX__

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/dl/Darwin.hpp"
#include "fwRuntime/dl/DarwinNameDecorator.hpp"

namespace fwRuntime
{

namespace dl
{

Darwin::Darwin( const boost::filesystem::path & modulePath ) throw()
: Native                ( modulePath, ::boost::shared_ptr< INameDecorator >(new DarwinNameDecorator()) ),
  m_handle      ( 0 )
{}

/*
Darwin::Darwin(const boost::filesystem::path& modulePath, const bool partialName) throw()
: Native(modulePath, partialName),
  m_handle(0)
{}
*/

Darwin::~Darwin() throw()
{}


const bool Darwin::isLoaded() const throw()
{
    return m_handle != 0;
}


const std::string Darwin::getNativeFilePrefix() const throw()
{
    return std::string("lib");
}


const std::string Darwin::getNativeFileSuffix() const throw()
{
    return std::string(".dylib");
}


void* Darwin::getSymbol(const std::string& name) const throw(RuntimeException)
{
    void* result = 0;
    if(isLoaded() == true)
    {
        dlerror(); // Clear existing error
        result = dlsym(m_handle, name.c_str());
        if(result == 0) // Check for possible errors
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


void Darwin::load() throw(RuntimeException)
{
    if(m_handle == 0)
    {
        // Opens the dynamic library.
#if BOOST_FILESYSTEM_VERSION > 2
        m_handle = dlopen(getFullPath(true).string().c_str(), RTLD_LAZY|RTLD_GLOBAL);
#else
        m_handle = dlopen(getFullPath(true).native_file_string().c_str(), RTLD_LAZY|RTLD_GLOBAL);
#endif
        if(m_handle == 0)
        {
            std::string message(dlerror());
            throw RuntimeException("Module load failed. " + message);
        }
        else
        {
        /*  void (*pFunc)(void);
            *(void **) (&pFunc) = dlsym(m_handle, "DylibInit");
            // *(void **) (&pFunc) = dlsym(m_handle, "__initialize_Cplusplus");
            if(pFunc == 0)// Check for possible errors
            {
                std::string message(dlerror());
                if(message.empty() == false)
                {
                    throw RuntimeException("DylibInit : Symbol retrieval failed. " + message);
                }
            }
            (*pFunc)();
        */}
    }
}


void Darwin::unload() throw(RuntimeException)
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


#endif // #ifdef __MACOSX__

