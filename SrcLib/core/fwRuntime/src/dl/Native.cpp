/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cassert>
#include <string>
#include <limits.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#if defined(_USE_BOOST_REGEX) || defined(__MACOSX__) || defined(WIN32)
#include <boost/regex.hpp>
#endif

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/dl/INameDecorator.hpp"
#include "fwRuntime/dl/Native.hpp"



namespace fwRuntime
{

namespace dl
{



Native::Native( const boost::filesystem::path & modulePath, const ::boost::shared_ptr< INameDecorator > nameDecorator ) throw()
:       m_modulePath            ( modulePath ),
        m_nameDecorator ( nameDecorator ),
        m_bundle                        ( 0 )
{}



Native::~Native() throw()
{}



const boost::filesystem::path Native::getFullPath( const bool _bMustBeFile ) const throw(RuntimeException)
{
        // Pre-condition
        assert( m_bundle != 0 );

        boost::filesystem::path result = m_bundle->getLocation() / getPath();

        // Test that the result path exists.
        if(result.empty() == true)
        {
                throw RuntimeException("Unable to find a native library for the bundle.");
        }
        if(boost::filesystem::exists(result) == false)
        {
                throw RuntimeException("'" + result.native_file_string() + "': invalid native module file name.");
        }
        if(_bMustBeFile == true && boost::filesystem::is_directory(result) == true)
        {
                throw RuntimeException("'" + result.native_file_string() + "': is a file. Perhaps dynamic librairie is missing.");
        }
        return result;
}



const boost::filesystem::path Native::getPath() const throw(RuntimeException)
{
        // Pre-condition
        assert( m_bundle != 0 );

        boost::filesystem::path                 result;

#if defined(_USE_BOOST_REGEX) || defined(__MACOSX__) || defined(WIN32)
        const boost::filesystem::path   fullModulePath( m_bundle->getLocation() / m_modulePath );
        const boost::regex              nativeFileRegex( m_nameDecorator->getNativeName(fullModulePath.leaf()) );

        // Walk through the module directory, seeking for a matching file.
        boost::filesystem::directory_iterator curDirEntry(fullModulePath.branch_path());
        boost::filesystem::directory_iterator endDirEntry;
        for(; curDirEntry != endDirEntry; ++curDirEntry)
        {
                boost::filesystem::path curEntryPath( *curDirEntry );
                if( boost::regex_match( curEntryPath.leaf(), nativeFileRegex ) == true )
                {
                        result =  m_modulePath.branch_path() / curEntryPath.leaf();
                        break;
                }
        }

#else
        const boost::filesystem::path   fullModulePath(m_bundle->getLocation() / m_modulePath);

        // Adapts the file name to a posix dynamic library name
        // and build te regular expression.
        const std::string       searchPrefix("lib" + fullModulePath.leaf());
        const std::string       searchSuffix("_posix_gcc.so");

        // Walk through the module directory, seeking for a matching file.
        boost::filesystem::directory_iterator curDirEntry(fullModulePath.branch_path());
        boost::filesystem::directory_iterator endDirEntry;
        for(; curDirEntry != endDirEntry; ++curDirEntry)
        {
                boost::filesystem::path curEntryPath(*curDirEntry);
                std::string curEntryName(curEntryPath.leaf());
                if(curEntryName.find(searchPrefix) != std::string::npos && curEntryName.find(searchSuffix) != std::string::npos)
                {
                        result = m_modulePath.branch_path() / curEntryPath.leaf();
                        break;
                }
        }
#endif

        return result;
}



void Native::setBundle( const Bundle * bundle ) throw()
{
        // Pre-condition
        assert( m_bundle == 0 );

        m_bundle = bundle;

        // Post-condition
        assert( m_bundle == bundle );
}



void Native::operator=(const Native&) throw()
{}



} // namespace dl


} // namespace fwRuntime
