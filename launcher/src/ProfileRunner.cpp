/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>
#include <fwRuntime/io/ProfileReader.hpp>

//------------------------------------------------------------------------------
#if defined(_WIN32) && defined(_DEBUG)
#if _MSC_VER >= 1400 // visual studio >= 2005

// add a dependency on the retail crt even in debug
#pragma comment(linker,"/manifestdependency:\"type='win32' " \
        "name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".CRT' "   \
        "version='" _CRT_ASSEMBLY_VERSION "' "               \
        "processorArchitecture='*' "                         \
        "publicKeyToken='" _VC_ASSEMBLY_PUBLICKEYTOKEN "' "   \
        "language='*'\"")

#endif    /* _MSC_VER >= 1400 */
#endif    /* _WIN32 && _DEBUG */
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int    m_argc;
char** m_argv;
//-----------------------------------------------------------------------------

/**
 * @brief Sets the fw4spl directory  in the system.
 *
 * @return bool   Returns true if fw4spl directory has been correctly initialized.
 */
bool initFwDir()
{
    ::boost::filesystem::path launcherPath;
    launcherPath = ::boost::filesystem::current_path();

    bool isChdirOk = false;

    if(! ::boost::filesystem::exists(launcherPath / "Bundles") )
    {
        // Are-you in /bin directory?
        launcherPath = launcherPath.parent_path();
    }

    if( ::boost::filesystem::exists(launcherPath / "Bundles") )
    {
        std::string strPath = launcherPath.string();
#ifdef _WIN32
        isChdirOk = (bool)(SetCurrentDirectory(strPath.c_str()) != 0);
#else
        isChdirOk = ( chdir(strPath.c_str()) == 0 );
#endif
    }
    else
    {
        OSLM_WARN("Bundles directory can't be found: " << launcherPath.string());
    }

    return isChdirOk;
}

//-----------------------------------------------------------------------------

/**
 * @brief   Returns the profile file path:
 *  - default file path is ./profile.xml
 *  - if there are arguments, we used argv[1] for profile file path (old style)
 *
 * @return ::boost::filesystem::path   The profile path.
 */
::boost::filesystem::path getProfilePath()
{
    ::boost::filesystem::path profilePath;

    // Default profile Path
    profilePath = ::boost::filesystem::current_path() / "profile.xml";

    // old style parameters
    // 1 argument :
    // launcher path/profile.xml
    if ( m_argc >= 2 )
    {
        profilePath = m_argv[1];
    }

    return profilePath;
}

//-----------------------------------------------------------------------------

#if defined(_WIN32) && defined(_WINDOWS)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR args, int)
{
    m_argc = __argc;
    m_argv = __argv;
#else
int main(int argc, char* argv[])
{
    m_argc = argc;
    m_argv = argv;
#endif
    bool isChdirOk = initFwDir();
    SLM_FATAL_IF("Current dir initialization failed" , !isChdirOk);

    ::boost::filesystem::path bundlePath = ::boost::filesystem::current_path() / "Bundles";
    // Bundles path is valid ?
    OSLM_FATAL_IF("Bundles path is not valid: "<< bundlePath.string(), !::boost::filesystem::exists( bundlePath ) );
    ::fwRuntime::addBundles(bundlePath);

    //Application Profile
    ::boost::filesystem::path profilePath = getProfilePath();
    // Profile path is valid ?
    OSLM_FATAL_IF("Profile path is not valid: "<< profilePath.string(), !::boost::filesystem::exists( profilePath ) );
    ::fwRuntime::profile::Profile::sptr profile;
    profile = ::fwRuntime::io::ProfileReader::createProfile(profilePath);
    OSLM_INFO("Launcher -- m_profile: " << profile);
    ::fwRuntime::profile::setCurrentProfile(profile);

    profile->setParams(m_argc, m_argv);
    profile->start();

    return 0;
}

