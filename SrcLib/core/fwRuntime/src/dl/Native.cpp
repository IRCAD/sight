/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cassert>
#include <string>
#include <limits.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/dl/INameDecorator.hpp"
#include "fwRuntime/dl/Native.hpp"
#include "fwRuntime/Runtime.hpp"



namespace fwRuntime
{

namespace dl
{

//------------------------------------------------------------------------------

Native::Native( const ::boost::filesystem::path & modulePath,
                const std::shared_ptr< INameDecorator > nameDecorator ) throw()
    :   m_modulePath        ( modulePath ),
      m_nameDecorator ( nameDecorator ),
      m_bundle            ( 0 )
{
}

//------------------------------------------------------------------------------

Native::~Native() throw()
{
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path Native::getFullPath( const bool _bMustBeFile ) const throw(RuntimeException)
{
    // Pre-condition
    SLM_ASSERT("bundle not initialized", m_bundle != 0 );

    ::boost::filesystem::path result;
#ifdef ANDROID
    std::string libName = "lib"+m_bundle->getIdentifier()+"_"+m_bundle->getVersion().string()+".so";
    result = ::fwRuntime::Runtime::getDefault()->getWorkingPath()/"lib"/libName;
#else
    result = m_bundle->getLocation()/ getPath();
#endif

    // Test that the result path exists.
    if(result.empty())
    {
        throw RuntimeException("Unable to find a native library for the bundle.");
    }
    if( !::boost::filesystem::exists(result) )
    {
        throw RuntimeException("'" + result.string() + "': invalid native module file name.");
    }
    if(_bMustBeFile && ::boost::filesystem::is_directory(result) )
    {
        throw RuntimeException("'" + result.string() + "': is a file. Perhaps dynamic librairie is missing.");
    }
    return result;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path Native::getPath() const throw(RuntimeException)
{
    // Pre-condition
    SLM_ASSERT("bundle not initialized", m_bundle != 0 );

    ::boost::filesystem::path result;

    const ::boost::filesystem::path fullModulePath( m_bundle->getLocation() / m_modulePath );
    const ::boost::regex nativeFileRegex( m_nameDecorator->getNativeName(fullModulePath.filename().string()) );

    // Walk through the module directory, seeking for a matching file.
    ::boost::filesystem::directory_iterator curDirEntry(fullModulePath.parent_path());
    ::boost::filesystem::directory_iterator endDirEntry;
    for(; curDirEntry != endDirEntry; ++curDirEntry)
    {
        ::boost::filesystem::path curEntryPath( *curDirEntry );
        if( ::boost::regex_match( curEntryPath.filename().string(), nativeFileRegex ) )
        {
            result = m_modulePath.parent_path() / curEntryPath.filename();
            break;
        }
    }

    return result;
}

//------------------------------------------------------------------------------

void Native::setBundle( const Bundle * bundle ) throw()
{
    // Pre-condition
    SLM_ASSERT("bundle already initialized", m_bundle == 0 );
    m_bundle = bundle;
    // Post-condition
    SLM_ASSERT("bundle not correctly attached", m_bundle == bundle );
}

//------------------------------------------------------------------------------

void Native::operator=(const Native&) throw()
{
}

//------------------------------------------------------------------------------

} // namespace dl


} // namespace fwRuntime
