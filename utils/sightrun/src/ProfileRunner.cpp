/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <core/crypto/PasswordKeeper.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <csignal>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

//------------------------------------------------------------------------------

namespace po = boost::program_options;

using sight::core::crypto::PasswordKeeper;
using sight::core::crypto::secure_string;
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

/// Wrapper for std::filesystem::absolute, needed by clang 3.0 in use with
/// std::transform
std::filesystem::path absolute(const std::filesystem::path& path)
{
    return std::filesystem::weakly_canonical(path);
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
    std::vector<std::filesystem::path> modulePaths;
    std::filesystem::path profileFile;
    sight::core::runtime::Profile::ParamsContainer profileArgs;

    // Runner options
    po::options_description options("Sight application launcher options");
    options.add_options()
        ("help,h", "Show help message")
        ("module-path,B", po::value(&modulePaths), "Adds a module path")
    ;

    // Log options
#if defined(SIGHT_ENABLE_ENCRYPTED_LOG)
    const bool encrypted_log = true;

    // By default, don't ask for a password if there is a default password
    bool ask_password = !PasswordKeeper::has_default_password();
#else
    const bool encrypted_log = false;
    bool ask_password        = false;
#endif

#if defined(WIN32)
    bool console_log = false;
    bool file_log    = true;
#else
    bool console_log = !encrypted_log;
    bool file_log    = encrypted_log;
#endif

    std::string log_file;

    typedef sight::core::log::SpyLogger SpyLogger;
    int log_level = SpyLogger::SL_WARN;

    po::options_description logOptions("Log options");
    logOptions.add_options()
#if defined(SIGHT_ENABLE_ENCRYPTED_LOG)
    (
        "ask-password",
        po::value(&ask_password)->implicit_value(true)->zero_tokens(),
        "Force asking for a password"
    )
#endif
    (
        "clog",
        po::value(&console_log)->implicit_value(true)->zero_tokens(),
        "Enable log output to console"
    )
    (
        "no-clog",
        po::value(&console_log)->implicit_value(false)->zero_tokens(),
        "Disable log output to console"
    )
    (
        "flog",
        po::value(&file_log)->implicit_value(true)->zero_tokens(),
        "Enable log output to file"
    )
    (
        "no-flog",
        po::value(&file_log)->implicit_value(false)->zero_tokens(),
        "Disable log output to file"
    )
    (
        "log-output",
        po::value(&log_file)->default_value(
            encrypted_log ? sight::core::log::ENCRYPTED_LOG_FILE : sight::core::log::LOG_FILE
        ),
        "Log output filename"
    )
    (
        "log-trace",
        po::value(&log_level)->implicit_value(SpyLogger::SL_TRACE)->zero_tokens(),
        "Set log_level to trace"
    )
    (
        "log-debug",
        po::value(&log_level)->implicit_value(SpyLogger::SL_DEBUG)->zero_tokens(),
        "Set log_level to debug"
    )
    (
        "log-info",
        po::value(&log_level)->implicit_value(SpyLogger::SL_INFO)->zero_tokens(),
        "Set log_level to info"
    )
    (
        "log-warn",
        po::value(&log_level)->implicit_value(SpyLogger::SL_WARN)->zero_tokens(),
        "Set log_level to warn"
    )
    (
        "log-error",
        po::value(&log_level)->implicit_value(SpyLogger::SL_ERROR)->zero_tokens(),
        "Set log_level to error"
    )
    (
        "log-fatal",
        po::value(&log_level)->implicit_value(SpyLogger::SL_FATAL)->zero_tokens(),
        "Set log_level to fatal"
    );

    // Hidden options
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("profile", po::value(&profileFile)->default_value("profile.xml"), "Profile file")
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
    SpyLogger& logger = SpyLogger::get();

    if(console_log)
    {
        logger.add_console_log(std::clog, static_cast<SpyLogger::LevelType>(log_level));
    }

    if(file_log)
    {
        if(encrypted_log)
        {
            const secure_string& password =
                PasswordKeeper::has_default_password()
                ? PasswordKeeper::get_default_password()
                : secure_string();

            logger.start_encrypted_logger(
                log_file,
                static_cast<SpyLogger::LevelType>(log_level),
                password,
                ask_password
            );
        }
        else
        {
            logger.start_logger(log_file, static_cast<SpyLogger::LevelType>(log_level));
        }
    }

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
    for(const std::filesystem::path& modulePath : modulePaths)
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
    for(const std::filesystem::path& modulePath : modulePaths)
    {
        SIGHT_INFO_IF(
            "Module paths are: " << modulePath.string() << " => " << ::absolute(modulePath),
            vm.count("module-path")
        );
    }
#endif
    for(const std::filesystem::path& modulePath : modulePaths)
    {
        SIGHT_FATAL_IF(
            "Module path doesn't exist: " << modulePath.string() << " => " << ::absolute(
                modulePath
            ),
            !std::filesystem::exists(modulePath.string())
        );
    }

    sight::core::runtime::init();

    for(const std::filesystem::path& modulePath : modulePaths)
    {
        if(std::filesystem::is_directory(modulePath))
        {
            sight::core::runtime::addModules(modulePath);
        }
        else
        {
            SIGHT_ERROR("Module path " << modulePath << " do not exists or is not a directory.");
        }
    }

    if(std::filesystem::is_regular_file(profileFile))
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
            return 3;
        }
        catch(...)
        {
            SIGHT_FATAL("An unrecoverable error has occurred.");
            return 4;
        }
    }
    else
    {
        SIGHT_ERROR("Profile file " << profileFile << " do not exists or is not a regular file.");
        return 5;
    }

    return 0;
}

//-----------------------------------------------------------------------------

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}

#endif // _WIN32
