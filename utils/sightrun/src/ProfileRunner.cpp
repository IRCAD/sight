/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <core/runtime/operations.hpp>
#include <core/runtime/profile/Profile.hpp>

#include <filesystem>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <csignal>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

#ifdef _WIN32
# define CONSOLE_LOG false;
# define FILE_LOG true;
#else
# define CONSOLE_LOG true;
# define FILE_LOG false;
#endif

#ifndef DEFAULT_PROFILE
#define DEFAULT_PROFILE profile.xml
#endif

#define GET_DEFAULT_PROFILE(x) #x
#define GET_DEFAULT_PROFILE2(x) GET_DEFAULT_PROFILE(x)
#define DEFAULT_PROFILE_STRING GET_DEFAULT_PROFILE2(DEFAULT_PROFILE)

//------------------------------------------------------------------------------

namespace po = ::boost::program_options;
namespace fs = std::filesystem;

typedef fs::path PathType;
typedef std::vector<PathType> PathListType;
typedef std::vector<std::string> StringListType;

namespace std
{

//------------------------------------------------------------------------------

template<class A1, class A2>
inline ostream& operator<<(ostream& s, vector<A1, A2> const& vec)
{
    copy(vec.begin(), vec.end(), ostream_iterator<A1>(s, " "));
    return s;
}

}

//-----------------------------------------------------------------------------

#ifdef __APPLE__
std::pair<std::string, std::string> parsePns(const std::string& s)
{
    if(s.substr(0, 5) == "-psn_")
    {
        return std::make_pair(std::string("psn"), s.substr(5));
    }

    return std::make_pair(std::string(), std::string());
}

#endif

/// Wrapper for std::filesystem::absolute, needed by clang 3.0 in use with
/// std::transform
PathType absolute(const PathType& path)
{
    return fs::weakly_canonical(path);
}

//-----------------------------------------------------------------------------

volatile sig_atomic_t gSignalStatus = 0;
//------------------------------------------------------------------------------

void signal_handler(int signal)
{
    gSignalStatus = signal;

    try
    {
        const auto& profile = sight::core::runtime::getCurrentProfile();
        profile->cleanup();
        profile->stop();
    }
    catch(const std::exception& e)
    {
        SIGHT_FATAL(e.what());
    }
    catch(...)
    {
        SIGHT_FATAL("An unrecoverable error has occurred.");
    }

    // We use brutal exit because when interrupted by a signal, we never get out from run,
    // even if the program is fully terminated
    exit(0);
}

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    PathListType modulePaths;
    PathType profileFile;
    sight::core::runtime::Profile::ParamsContainer profileArgs;

    // Runner options
    po::options_description options("Sight application launcher options");
    options.add_options()
        ("help,h", "Show help message")
        ("module-path,B", po::value(&modulePaths), "Adds a module path")
    ;

    // Log options
    bool consoleLog = CONSOLE_LOG;
    bool fileLog    = FILE_LOG;
    std::string logFile;
    const std::string defaultLogFile = "SLM.log";

    typedef sight::core::log::SpyLogger SpyLogger;
    int logLevel = SpyLogger::SL_WARN;

    po::options_description logOptions("Log options");
    logOptions.add_options()
        ("clog", po::value(&consoleLog)->implicit_value(true)->zero_tokens(), "Enable log output to console")
        ("no-clog", po::value(&consoleLog)->implicit_value(false)->zero_tokens(), "Disable log output to console")
        ("flog", po::value(&fileLog)->implicit_value(true)->zero_tokens(), "Enable log output to file")
        ("no-flog", po::value(&fileLog)->implicit_value(false)->zero_tokens(), "Disable log output to file")
        ("log-output", po::value(&logFile)->default_value(defaultLogFile), "Log output filename")

        ("log-trace", po::value(&logLevel)->implicit_value(SpyLogger::SL_TRACE)->zero_tokens(), "Set loglevel to trace")
        ("log-debug", po::value(&logLevel)->implicit_value(SpyLogger::SL_DEBUG)->zero_tokens(), "Set loglevel to debug")
        ("log-info", po::value(&logLevel)->implicit_value(SpyLogger::SL_INFO)->zero_tokens(), "Set loglevel to info")
        ("log-warn", po::value(&logLevel)->implicit_value(SpyLogger::SL_WARN)->zero_tokens(), "Set loglevel to warn")
        ("log-error", po::value(&logLevel)->implicit_value(SpyLogger::SL_ERROR)->zero_tokens(), "Set loglevel to error")
        ("log-fatal", po::value(&logLevel)->implicit_value(SpyLogger::SL_FATAL)->zero_tokens(), "Set loglevel to fatal")
    ;

    // Hidden options
    po::options_description hidden("Hidden options");
    hidden.add_options()
#ifdef __APPLE__
    ("psn", po::value<std::string>(), "Application PSN number")
        ("NSDocumentRevisionsDebugMode", po::value<std::string>()->zero_tokens(), "DocumentRevisionsDebugMode")
#endif
    ("profile", po::value(&profileFile)->default_value(DEFAULT_PROFILE_STRING), "Profile file")
        ("profile-args", po::value(&profileArgs)->multitoken(), "Profile args")
    ;

    // Set options
    po::options_description cmdline_options;
    cmdline_options.add(options).add(logOptions).add(hidden);

    po::positional_options_description p;
    p.add("profile", 1).add("profile-args", -1);

    // Get options
    po::variables_map vm;

    try
    {
        po::store(
            po::command_line_parser(argc, argv)
            .options(cmdline_options)
#ifdef __APPLE__
            .extra_parser(parsePns)
#endif
            .positional(p)
            .run(),
            vm
        );
        po::notify(vm);
    }
    catch(const po::error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // If help
    if(vm.count("help"))
    {
        std::cout << "usage: " << argv[0] << " [options] [profile(=profile.xml)] [profile-args ...]" << std::endl;
        std::cout << "  use '--' to stop processing args for sightrun" << std::endl << std::endl;
        std::cout << options << std::endl << logOptions << std::endl;
        return 0;
    }

    // Log file
    SpyLogger& logger = sight::core::log::SpyLogger::getSpyLogger();

    if(consoleLog)
    {
        logger.addStreamAppender(std::clog, static_cast<SpyLogger::LevelType>(logLevel));
    }

    if(fileLog)
    {
        std::ofstream logFileStream(logFile.c_str());
        const bool logFileExists = logFileStream.good();
        logFileStream.close();
        if(!logFileExists)
        {
            std::error_code err;
            PathType sysTmp = fs::temp_directory_path(err);
            if(err.value() != 0)
            {
                // replace log file appender by stream appender: default dir and temp dir unreachable
                logger.addStreamAppender(std::clog, static_cast<SpyLogger::LevelType>(logLevel));
            }
            else
            {
                // creates SLM.log in temp directory: default dir unreachable
                sysTmp  = sysTmp / "SLM.log";
                logFile = sysTmp.string();
                logger.addFileAppender(logFile, static_cast<SpyLogger::LevelType>(logLevel));
            }
        }
        else
        {
            // creates SLM.log in default logFile directory
            logger.addFileAppender(logFile, static_cast<SpyLogger::LevelType>(logLevel));
        }
    }

#ifdef __APPLE__
    fs::path execPath = argv[0];

    if(execPath.string().find(".app/") != std::string::npos || vm.count("psn"))
    {
        bool isChdirOkOSX = false;

        fs::path execPath = argv[0];

        while(fs::extension(execPath) != ".app"
              && execPath != execPath.parent_path()
              && !fs::is_directory(execPath / fs::path(SIGHT_MODULE_RC_PREFIX)))
        {
            execPath = execPath.parent_path();
        }

        if(fs::is_directory(execPath / "Contents" / fs::path(SIGHT_MODULE_RC_PREFIX)))
        {
            execPath = execPath / "Contents";
        }
        else
        {
            SIGHT_ERROR_IF(
                "Module directory not found.",
                !fs::is_directory(
                    execPath / fs::path(
                        SIGHT_MODULE_RC_PREFIX
                    )
                )
            );
        }

        isChdirOkOSX = (chdir(execPath.string().c_str()) == 0);

        SIGHT_ERROR_IF("Was not able to find a directory to change to.", !isChdirOkOSX);
    }
#endif

    SIGHT_INFO_IF("Profile path: " << profileFile << " => " << ::absolute(profileFile), vm.count("profile"));
    SIGHT_INFO_IF("Profile-args: " << profileArgs, vm.count("profile-args"));

    // Check if path exist
    SIGHT_FATAL_IF(
        "Profile path doesn't exist: " << profileFile.string() << " => " << ::absolute(
            profileFile
        ),
        !std::filesystem::exists(profileFile.string())
    );

    std::transform(modulePaths.begin(), modulePaths.end(), modulePaths.begin(), ::absolute);
    profileFile = ::absolute(profileFile);

    // Automatically adds the module folders where the profile.xml is located if it was not already there
    const auto profileModulePath = profileFile.parent_path().parent_path();
    bool findProfileModulePath   = false;
    for(const fs::path& modulePath : modulePaths)
    {
        if(profileModulePath == modulePath)
        {
            findProfileModulePath = true;
        }
    }

    if(!findProfileModulePath)
    {
        modulePaths.push_back(profileModulePath);
    }

#if SIGHT_INFO_ENABLED
    for(const fs::path& modulePath : modulePaths)
    {
        SIGHT_INFO_IF(
            "Module paths are: " << modulePath.string() << " => " << ::absolute(modulePath),
            vm.count("module-path")
        );
    }
#endif
    for(const fs::path& modulePath : modulePaths)
    {
        SIGHT_FATAL_IF(
            "Module path doesn't exist: " << modulePath.string() << " => " << ::absolute(
                modulePath
            ),
            !std::filesystem::exists(modulePath.string())
        );
    }

    sight::core::runtime::init();

    for(const fs::path& modulePath : modulePaths)
    {
        if(fs::is_directory(modulePath))
        {
            sight::core::runtime::addModules(modulePath);
        }
        else
        {
            SIGHT_ERROR("Module path " << modulePath << " do not exists or is not a directory.");
        }
    }

    int retValue = 0;

    if(fs::is_regular_file(profileFile))
    {
        sight::core::runtime::Profile::sptr profile;

        try
        {
            profile = sight::core::runtime::io::ProfileReader::createProfile(profileFile);

            // Install a signal handler
            std::signal(SIGINT, signal_handler);
            std::signal(SIGTERM, signal_handler);

#ifndef WIN32
            std::signal(SIGHUP, signal_handler);
            std::signal(SIGQUIT, signal_handler);
#endif

            profile->setParams(profileArgs);

            profile->start();
            profile->run();
            if(gSignalStatus == 0)
            {
                profile->stop();
            }
        }
        catch(const std::exception& e)
        {
            SIGHT_FATAL(e.what());
            retValue = 3;
        }
        catch(...)
        {
            SIGHT_FATAL("An unrecoverable error has occurred.");
            retValue = 4;
        }
    }
    else
    {
        SIGHT_ERROR("Profile file " << profileFile << " do not exists or is not a regular file.");
        retValue = 5;
    }

    return retValue;
}

//-----------------------------------------------------------------------------

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}

#endif // _WIN32
