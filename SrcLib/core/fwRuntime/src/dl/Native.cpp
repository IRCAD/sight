/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRuntime/dl/Native.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/Runtime.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <limits.h>

#include <string>

namespace fwRuntime
{

namespace dl
{

//------------------------------------------------------------------------------

Native::Native( const ::boost::filesystem::path& modulePath ) noexcept :
    m_modulePath( modulePath ),
    m_bundle( 0 )
{
}

//------------------------------------------------------------------------------

Native::~Native() noexcept
{
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path Native::getBundleLocation() const
{
    return m_bundle->getLibraryLocation();
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path Native::getFullPath( const bool _bMustBeFile ) const
{
    // Pre-condition
    SLM_ASSERT("bundle not initialized", m_bundle != 0 );

    ::boost::filesystem::path result;

    result = this->getBundleLocation() / this->getPath();

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
        throw RuntimeException("'" + result.string() + "': is a directory. Perhaps dynamic library is missing.");
    }
    return result;
}

//------------------------------------------------------------------------------

const ::boost::regex Native::getNativeName() const
{
    const ::boost::filesystem::path fullModulePath( this->getBundleLocation() / m_modulePath );
    ::boost::regex nativeName;

#if defined(linux) || defined(__linux)
    nativeName = ::boost::regex(
        "lib" + fullModulePath.filename().string() + "\\.so" +
        "[0-9\\.]*" );
#elif defined(WIN32)
    nativeName = ::boost::regex(
        fullModulePath.filename().string() + "\\.dll");
#elif defined (__APPLE__)
    nativeName = ::boost::regex(
        "lib" + fullModulePath.filename().string() + "[0-9\\.]*\\.dylib" );
#endif

    return nativeName;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path Native::getPath() const
{
    // Pre-condition
    SLM_ASSERT("bundle not initialized", m_bundle != 0 );

    ::boost::filesystem::path result;

    const ::boost::filesystem::path fullModulePath( this->getBundleLocation() / m_modulePath );
    const ::boost::regex nativeFileRegex( this->getNativeName() );

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

void Native::setBundle( const Bundle* bundle ) noexcept
{
    // Pre-condition
    SLM_ASSERT("bundle already initialized", m_bundle == 0 );
    m_bundle = bundle;
    // Post-condition
    SLM_ASSERT("bundle not correctly attached", m_bundle == bundle );
}

//------------------------------------------------------------------------------

void Native::operator=(const Native&) noexcept
{
}

//------------------------------------------------------------------------------

} // namespace dl

} // namespace fwRuntime
