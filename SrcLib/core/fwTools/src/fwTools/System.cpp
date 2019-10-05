/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwTools/System.hpp"

#include <fwCore/base.hpp>

#include <filesystem>
#include <fstream>
#include <regex>
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

#include <random>

#define SIGHT_TMP_EXT "sight-tmp"

namespace fwTools
{

std::string System::s_tempPrefix;

struct RemoveTemporaryFolder
{
    typedef std::shared_ptr< RemoveTemporaryFolder > sptr;

    RemoveTemporaryFolder(const std::filesystem::path& path) :
        m_path(path)
    {
    }

    ~RemoveTemporaryFolder()
    {
        std::error_code er;
        std::filesystem::remove_all(m_path, er);
        OSLM_ERROR_IF( "Failed to remove " << m_path << " : " << er.message(), er.value() != 0);
    }
    std::filesystem::path m_path;
};
static RemoveTemporaryFolder::sptr autoRemoveTempFolder;

static struct CleanZombies
{
    CleanZombies()
    {
        System::cleanAllTempFolders(System::getTempPath());
    }
} autoCleanZombies;

//------------------------------------------------------------------------------

int System::getPID() noexcept
{
    int pid = 0;
#ifdef WIN32
    pid = _getpid();
#else
    pid = getpid();
#endif

    return pid;
}

//------------------------------------------------------------------------------

const std::filesystem::path& System::getTempPath() noexcept
{
    namespace fs = std::filesystem;
    static fs::path sysTmp;

    if(!sysTmp.empty())
    {
        return sysTmp;
    }

    std::error_code err;
    sysTmp = fs::temp_directory_path(err);

    if(err.value() != 0)
    {
#ifdef WIN32
        fs::path fallback("C:\\");
#else
        fs::path fallback("/tmp");
#endif
        OSLM_ERROR("Temporary Path Error : " << err.message() << ". " << "Falling back to " << fallback );
        sysTmp = fallback;
    }
    return sysTmp;
}

//------------------------------------------------------------------------------

const std::string System::genTempFileName(size_t _length)
{
    static const char chrs[] = { "0123456789"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

    thread_local static std::mt19937 rg{std::random_device{} ()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

    std::string s;
    s.reserve(_length);
    size_t length = _length;

    while(length--)
    {
        s += chrs[pick(rg)];
    }

    return s;
}

//------------------------------------------------------------------------------

const std::filesystem::path createUniqueFolder(const std::filesystem::path& folderUniquePath)
{
    namespace fs = std::filesystem;
    bool created = false;
    fs::path tmpDir;

    do
    {
        tmpDir = folderUniquePath / System::genTempFileName();

        if(!fs::exists(tmpDir))
        {
            fs::create_directories(tmpDir);

            created = true;
        }

    }
    while ( !created );

    return tmpDir;
}

//------------------------------------------------------------------------------

const std::filesystem::path System::getTemporaryFolder(const std::string& subFolderPrefix) noexcept
{
    namespace fs = std::filesystem;
    static fs::path tmpDirPath;

    if(!tmpDirPath.empty() && fs::exists(tmpDirPath))
    {
        if(!subFolderPrefix.empty())
        {
            const std::string subDirName = subFolderPrefix + "-";
            fs::path tmpSubDir           = createUniqueFolder(tmpDirPath/subDirName);
            return tmpSubDir;
        }

        return tmpDirPath;
    }

    const fs::path& sysTmp = getTempPath();

    const std::string tmpDirName = s_tempPrefix + (s_tempPrefix.empty() ? "" : "-") + "." SIGHT_TMP_EXT;
    fs::path tmpDir              = createUniqueFolder(sysTmp/tmpDirName);
    tmpDirPath = tmpDir;    // tmpDirPath always set to root tmp dir

    fs::path pidFile = tmpDir / (::boost::lexical_cast<std::string>(getPID()) + ".pid");
    std::fstream( pidFile, std::ios::out ).close();

    autoRemoveTempFolder = std::make_shared<RemoveTemporaryFolder>(tmpDirPath);

    if(!subFolderPrefix.empty())
    {
        const std::string subDirName = subFolderPrefix + "-" + "%%%%%%%%%%%%";
        tmpDir = createUniqueFolder(tmpDir/subDirName);
    }

    OSLM_INFO("Temporary folder is : " << tmpDirPath);
    return tmpDir;
}

//------------------------------------------------------------------------------

bool System::isProcessRunning(int pid) noexcept
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
    return kill(pid, 0) == 0;
#endif
}

//------------------------------------------------------------------------------

int System::tempFolderPID(const std::filesystem::path& dir) noexcept
{
    namespace fs = std::filesystem;

    const std::regex pidFilter( "([\\d]+)\\.pid" );

    int pid = 0;

    try
    {
        fs::directory_iterator i( dir );
        fs::directory_iterator endIter;

        for(; i != endIter; ++i )
        {
            // Skip if not a dir
            if( !fs::is_regular_file( i->status() ) )
            {
                continue;
            }

            std::smatch what;

            // Skip if no match
            const std::string s = i->path().filename().string();
            if( !std::regex_match( s, what, pidFilter ) )
            {
                continue;
            }

            try
            {
                pid = ::boost::lexical_cast< int >(what.str(1));
                break;
            }
            catch (boost::bad_lexical_cast&)
            {
            }
        }
    }
    catch (std::exception&)
    {
        // The directory might have been removed by another process
        return 0;
    }

    return pid;
}

//------------------------------------------------------------------------------

void System::cleanAllTempFolders(const std::filesystem::path& dir) noexcept
{
    namespace fs = std::filesystem;

    const std::regex tmpFolderFilter( ".*\\." SIGHT_TMP_EXT );

    std::vector< fs::path > allTempFolders;

    fs::directory_iterator i( dir );
    fs::directory_iterator endIter;

    for(; i != endIter; ++i )
    {
        // Skip if not a dir
        if( !fs::is_directory( i->status() ) )
        {
            continue;
        }

        std::smatch what;

        // Skip if no match
        std::string s = i->path().filename().string();
        if( !std::regex_match( s, what, tmpFolderFilter ) )
        {
            continue;
        }

        allTempFolders.push_back( i->path() );
    }

    for( const fs::path& foundTmpDir :  allTempFolders)
    {
        const int pid = tempFolderPID(foundTmpDir);

        if(pid && !isProcessRunning(pid))
        {
            OSLM_INFO("Removing old temp dir : " << foundTmpDir);

            std::error_code er;
            fs::remove_all(foundTmpDir, er);

            OSLM_WARN_IF( "Failed to remove " << foundTmpDir << " : " << er.message(), er.value() != 0);
        }
    }

}

//------------------------------------------------------------------------------

} //end namespace fwTools
