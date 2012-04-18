/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>

#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>

// for PID
#ifdef WIN32
#include <wtypes.h>
#include <windows.h>
#include <process.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#endif

#include <fwCore/base.hpp>

#include "fwTools/System.hpp"

#include <boost/functional/hash.hpp>

namespace fwTools
{

//------------------------------------------------------------------------------

System::~System()
{
    if ( ! m_dumpFolder.empty() && ::boost::filesystem::exists(m_dumpFolder) )
    {
        ::boost::filesystem::remove_all(m_dumpFolder);
    }
//    eraseDumpFolderOfZombies();
}

//------------------------------------------------------------------------------

const ::boost::shared_ptr<System> System::getDefault() throw()
{
    /// A shared pointer to the kernel instance
    static ::boost::shared_ptr< System > instance;

    if ( instance.get() == 0 )
    {
        instance = ::boost::shared_ptr<System>(new System());
    }

    return instance;
}

//------------------------------------------------------------------------------

int System::getPID() const
{
    int pid=0;
#ifdef WIN32
    pid = _getpid();
#else
    pid = getpid();
#endif

    return pid;
}

//------------------------------------------------------------------------------

std::string getUserTmp()
{
#ifdef WIN32
    char *userEnv = getenv("USERNAME"); // warning cygwin create USER env. var
#else
    char *userEnv = getenv("USER");
#endif

    SLM_ASSERT("UserEnv not found", userEnv);

    ::boost::hash<std::string> string_hash;
    std::size_t hashValue = string_hash(userEnv);

    std::ostringstream path;
    path << "fwDumpFolder-" << hashValue;

    return path.str();
}

//------------------------------------------------------------------------------

::boost::filesystem::path System::getTemporaryFolder() throw()
{
    static ::boost::filesystem::path tmpdir;

    if ( tmpdir != ::boost::filesystem::path() )
    {
        return tmpdir; // path already setted
    }

#ifdef WIN32
    char *userEnvTmp = getenv("TMP") ;
    char *userEnvTemp = getenv("TEMP") ;

    std::string EnvTmpStr(userEnvTmp);
    std::string EnvTempStr(userEnvTemp);

    if ((EnvTmpStr.find("'")!= std::string::npos) || (EnvTempStr.find("'")!= std::string::npos))
    {
        std::string str = "An apostrophe causes problem to the XML library. One is in the user name, change your TMP environment variable.";
        OSLM_ERROR("PROBLEM : " << str );
    }
    if( userEnvTmp )
    {
        tmpdir = ::boost::filesystem::path(userEnvTmp) / getUserTmp() ;
        OSLM_INFO("Temp Folder : " << tmpdir.string() );
        ::boost::filesystem::create_directories(tmpdir);
        return tmpdir ;
    }

    if( userEnvTemp )
    {
        tmpdir = ::boost::filesystem::path(userEnvTemp) / getUserTmp() ;
        OSLM_INFO("Temp Folder : " << tmpdir.string() );
        ::boost::filesystem::create_directories(tmpdir);
        return tmpdir ;
    }
    // FIXME allow a more configurable choice for DumpFolder
    ::boost::filesystem::path DriveD("D:\\");
    ::boost::filesystem::path DriveC("C:\\");

    if ( ::boost::filesystem::exists( DriveD ) )
    {
        tmpdir = DriveD / getUserTmp();

        try
        {
            ::boost::filesystem::create_directory( tmpdir );
            ::boost::filesystem::create_directory( tmpdir / "writabletest" );
            ::boost::filesystem::remove( tmpdir / "writabletest" );
            return tmpdir;
        }
        catch(...)
        {
            // OK attempt fail use default behavior
        }
    }

    // default behavior
    tmpdir = DriveC / ::boost::filesystem::path(getUserTmp());

#else
    tmpdir = ::boost::filesystem::path("/tmp") /  ::boost::filesystem::path( getUserTmp() );
#endif

    ::boost::filesystem::create_directories(tmpdir);
    return tmpdir;
}

//------------------------------------------------------------------------------

System::System()
{
    int pid=getPID();
    SLM_ASSERT("Process "<< pid << " is not started", isProcessRunning(pid) );
    m_dumpFolder = System::getTemporaryFolder() / boost::lexical_cast<std::string>(pid);

    // create TemporaryFolder if necessary
    if (!::boost::filesystem::exists(System::getTemporaryFolder()))
    {
        ::boost::filesystem::create_directory(System::getTemporaryFolder());
    }

    // create dumpFolder if necessary
    if (!::boost::filesystem::exists(m_dumpFolder))
    {
        ::boost::filesystem::create_directory(m_dumpFolder);
    }

    if (!::boost::filesystem::exists(m_dumpFolder))
    {
        std::string str = "DumpableDataBuffer unable to use";
        str+= m_dumpFolder.string() + " directory to dump Image memory buffer";
        throw std::ios_base::failure(str);
    }
}

//------------------------------------------------------------------------------

::boost::filesystem::path System::getDumpFolder() throw()
{
    return m_dumpFolder;
}

//------------------------------------------------------------------------------

void System::eraseDumpFolderOfZombies() const
{
    namespace fs = ::boost::filesystem;
    fs::directory_iterator end_itr; // default  construction yields past-the-end
    fs::directory_iterator iter_dir( getTemporaryFolder() );
    for ( ; iter_dir != end_itr; ++iter_dir )
    {
#if BOOST_VERSION < 103600
        std::string pidStr = (*iter_dir).leaf();
#else
    #if BOOST_FILESYSTEM_VERSION > 2
        std::string pidStr = (*iter_dir).path().filename().string();
    #else
        std::string pidStr = (*iter_dir).filename();
    #endif
#endif
        try
        {
            int pid = boost::lexical_cast<int>(pidStr);
            if (!isProcessRunning(pid))
            {
                eraseDumpFolder(pid);
            }
        }
        catch (...)
        {
            // nothing to do ignore this directory
        }
    }
}

//------------------------------------------------------------------------------

void System::cleanDumpFolder() const
{
    // remove all files in subfolder
    ::boost::filesystem::directory_iterator end_itr; // default  construction yields past-the-end
    ::boost::filesystem::directory_iterator iter_dir( m_dumpFolder );
    for ( ; iter_dir != end_itr; ++iter_dir )
    {
        ::boost::filesystem::remove_all(*iter_dir);
    }
}

//------------------------------------------------------------------------------

void System::eraseDumpFolder(int pid) const
{
    SLM_ASSERT("Process "<< pid << " must be stopped before EraseDumpFolder", !isProcessRunning(pid) );
    namespace fs = ::boost::filesystem;
    fs::path dumpFolderPID= System::getTemporaryFolder() / ::boost::lexical_cast<std::string>(pid);
    fs::remove_all(dumpFolderPID);
}

//------------------------------------------------------------------------------

bool System::isProcessRunning(int pid)
{
#ifdef WIN32
    HANDLE hProcess;
    //hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid);hProcess==INVALID_HANDLE_VALUE
    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, pid);
    if ( hProcess==INVALID_HANDLE_VALUE ) // FIXME find the correct windows value
    {
        CloseHandle( hProcess );
        return false;
    }
    else if ( hProcess==0 )
    {
        return false;
    }
    else
    {
        CloseHandle( hProcess );
        return true;
    }
#else
    return kill(pid,0) == 0 ;
#endif

    return true;
}

//------------------------------------------------------------------------------


} //end namespace fwTools
