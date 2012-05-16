/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/make_shared.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>

#include <boost/lexical_cast.hpp>

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


#define F4S_TMP_EXT "fw4spl-tmp"

namespace fwTools
{

std::string System::s_tempPrefix;

struct RemoveTemporaryFolder
{
    typedef ::boost::shared_ptr< RemoveTemporaryFolder > sptr;

    RemoveTemporaryFolder(const ::boost::filesystem::path &path) : m_path(path)
    {}

    ~RemoveTemporaryFolder()
    {
        ::boost::filesystem::remove_all(m_path);
    }
    ::boost::filesystem::path m_path;
};
static RemoveTemporaryFolder::sptr autoRemoveTempFolder;



static struct CleanZombies
{
    CleanZombies()
    {
        System::cleanZombies(System::getTempPath());
    }
} autoCleanZombies;


//------------------------------------------------------------------------------

int System::getPID() throw()
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

const ::boost::filesystem::path &System::getTempPath() throw()
{
    static ::boost::filesystem::path sysTmp;

    if(!sysTmp.empty())
    {
        return sysTmp;
    }

    ::boost::system::error_code err;
    sysTmp = ::boost::filesystem::temp_directory_path(err);

    if(err.value() != 0)
    {
#ifdef WIN32
        ::boost::filesystem::path fallback("C:\\");
#else
        ::boost::filesystem::path fallback("/tmp");
#endif
        OSLM_ERROR("Temporary Path Error : " << err.message() << ". " << "Falling back to " << fallback );

        sysTmp = fallback;
    }

    return sysTmp;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path &System::getTemporaryFolder() throw()
{
    namespace fs = ::boost::filesystem;
    static fs::path tmpDirPath;

    if(!tmpDirPath.empty() && fs::exists(tmpDirPath))
    {
        return tmpDirPath;
    }

    const fs::path &sysTmp = getTempPath();

    const std::string tmpDirName = s_tempPrefix + (s_tempPrefix.empty() ? "" : "-") + "%%%%%%%%%%%%." F4S_TMP_EXT;

    fs::path tmpDir;

    bool created = false;

    do
    {
        tmpDir = fs::unique_path(sysTmp/tmpDirName);

        if(! fs::exists(tmpDir))
        {
            fs::create_directories(tmpDir);
            tmpDirPath = tmpDir;
            created = true;
        }

    }
    while ( !created );


    fs::path pidFile = tmpDir / (::boost::lexical_cast<std::string>(getPID()) + ".pid");
    fs::fstream( pidFile, std::ios::out ).close();

    autoRemoveTempFolder = ::boost::make_shared<RemoveTemporaryFolder>(tmpDirPath);

    OSLM_INFO("Temporary folder is : " << tmpDirPath);
    return tmpDirPath;
}

//------------------------------------------------------------------------------

bool System::isProcessRunning(int pid) throw()
{
#ifdef WIN32
    HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, pid);
    if (!hProcess)
    {
        return false;
    }
    else
    {
        CloseHandle(hProcess);
        return true;
    }
#else
    return kill(pid,0) == 0 ;
#endif

    return true;
}

//------------------------------------------------------------------------------

int System::tempFolderPID(const ::boost::filesystem::path &dir) throw()
{
    namespace fs = ::boost::filesystem;

    const ::boost::regex pidFilter( "([[:digit:]]+)\\.pid" );

    ::boost::filesystem::directory_iterator i( dir );
    ::boost::filesystem::directory_iterator endIter;

    int pid = 0;

    for( ; i != endIter; ++i )
    {
        // Skip if not a dir
        if( !::boost::filesystem::is_regular_file( i->status() ) )
        {
            continue;
        }

        ::boost::smatch what;

        // Skip if no match
        std::string s = i->path().filename().string();
        if( !::boost::regex_match( s, what, pidFilter ) )
        {
            continue;
        }

        try
        {
            pid = ::boost::lexical_cast< int >(what.str(1));
            break;
        }
        catch (boost::bad_lexical_cast&)
        {}

    }
    return pid;
}

//------------------------------------------------------------------------------

void System::cleanZombies(const ::boost::filesystem::path &dir) throw()
{
    namespace fs = ::boost::filesystem;

    const ::boost::regex tmpFolderFilter( ".*\\." F4S_TMP_EXT );

    std::vector< ::boost::filesystem::path > allTempFolders;

    ::boost::filesystem::directory_iterator i( dir );
    ::boost::filesystem::directory_iterator endIter;

    for( ; i != endIter; ++i )
    {
        // Skip if not a dir
        if( !::boost::filesystem::is_directory( i->status() ) )
        {
            continue;
        }

        ::boost::smatch what;

        // Skip if no match
        if( !::boost::regex_match( i->path().filename().string(), what, tmpFolderFilter ) )
        {
            continue;
        }

        allTempFolders.push_back( i->path() );
    }


    BOOST_FOREACH( const fs::path &foundTmpDir, allTempFolders)
    {
        int pid = tempFolderPID(foundTmpDir);

        if(pid && !isProcessRunning(pid))
        {
            OSLM_INFO("Removing old temp dir : " << foundTmpDir);
            ::boost::filesystem::remove_all(foundTmpDir);
        }
    }


}

//------------------------------------------------------------------------------

} //end namespace fwTools
