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

// cspell:ignore SEM_NOGPFAULTERRORBOX

#ifdef _WIN32
#include <windows.h>
#endif

#include <core/crypto/PasswordKeeper.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/profile/Profile.hpp>
#include <core/tools/Os.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

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

//------------------------------------------------------------------------------

template<class A1, class A2>
inline static std::ostream& operator<<(std::ostream& s, std::vector<A1, A2> const& vec)
{
    copy(vec.begin(), vec.end(), std::ostream_iterator<A1>(s, " "));
    return s;
}

//-----------------------------------------------------------------------------

/// Wrapper for std::filesystem::absolute, needed by clang 3.0 in use with
/// std::transform
inline static std::filesystem::path absolute(const std::filesystem::path& path)
{
    return std::filesystem::weakly_canonical(path);
}

//-----------------------------------------------------------------------------

volatile sig_atomic_t gSignalStatus = 0;
//------------------------------------------------------------------------------

void signalHandler(int signal)
{
    gSignalStatus = signal;

    try
    {
        const auto& profile = sight::core::runtime::getCurrentProfile();

        if(profile != nullptr)
        {
            profile->stop();
        }
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

//------------------------------------------------------------------------------

inline static std::filesystem::path buildLogFilePath(const std::filesystem::path& log_dir, bool encrypted_log)
{
    // Use the default log file name as base
    std::filesystem::path log_file_path = encrypted_log
                                          ? sight::core::log::ENCRYPTED_LOG_FILE
                                          : sight::core::log::LOG_FILE;

    // Prepend the log directory
    if(!log_dir.empty())
    {
        std::filesystem::create_directories(log_dir);
        log_file_path = log_dir / log_file_path;
    }

    // Test if the directory is writable. An exception will be thrown if not
    {
        std::ofstream ostream;
        ostream.open(log_file_path, std::ios::out | std::ios::binary | std::ios::trunc);
    }

    // Cleanup
    std::filesystem::remove_all(log_file_path);

    return log_file_path;
}

//------------------------------------------------------------------------------

inline static std::filesystem::path findLogFilePath(
    const std::filesystem::path& log_file,
    const std::filesystem::path& profile_file,
    bool encrypted_log
)
{
    if(log_file.empty())
    {
        // Parse the profile.xml to get the name
        boost::property_tree::ptree profile_tree;
        boost::property_tree::read_xml(profile_file.string(), profile_tree);
        const auto& profile_name = profile_tree.get<std::string>("profile.<xmlattr>.name");

        try
        {
            // Try the user cache directory
            return buildLogFilePath(
                sight::core::tools::os::getUserCacheDir(profile_name),
                encrypted_log
            );
        }
        catch(...)
        {
            // Fallback: take temporary directory
            return buildLogFilePath(
                std::filesystem::temp_directory_path() / "sight" / profile_name,
                encrypted_log
            );
        }
    }

    // Take the user choice
    return log_file;
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

    using SpyLogger = sight::core::log::SpyLogger;
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
        po::value(&log_file),
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
    bool macroMode = false;
#ifdef WIN32
    bool enableAbortDialog = true;
#endif

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("profile", po::value(&profileFile)->default_value("profile.xml"), "Profile file")
        ("profile-args", po::value(&profileArgs)->multitoken(), "Profile args")
        ("macro", po::value(&macroMode)->implicit_value(true)->zero_tokens(), "Enable macro mode")
        ("no-macro", po::value(&macroMode)->implicit_value(false)->zero_tokens(), "Disable macro mode")
#ifdef WIN32
    ("abort-dialog", po::value(&enableAbortDialog)->implicit_value(true)->zero_tokens(), "Enable abort dialog")
        ("no-abort-dialog", po::value(&enableAbortDialog)->implicit_value(false)->zero_tokens(), "Disable abort dialog")
#endif
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
    if(vm.count("help") != 0U)
    {
        std::cout << "usage: " << argv[0] << " [options] [profile(=profile.xml)] [profile-args ...]" << std::endl;
        std::cout << "  use '--' to stop processing args for sightrun" << std::endl << std::endl;
        std::cout << options << std::endl << logOptions << std::endl;
        return 0;
    }

#ifdef WIN32
    if(!enableAbortDialog)
    {
        _set_error_mode(_OUT_TO_STDERR);
        SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    }
#endif

    SIGHT_INFO_IF("Profile path: " << profileFile << " => " << ::absolute(profileFile), vm.count("profile"));
    SIGHT_INFO_IF("Profile-args: " << profileArgs, vm.count("profile-args"));

    // Check if profile path exist
    profileFile = ::absolute(profileFile);

    SIGHT_FATAL_IF(
        "Profile file " << profileFile << " do not exists or is not a regular file.",
        !std::filesystem::is_regular_file(profileFile)
    );

    // Log file
    SpyLogger& logger = SpyLogger::get();

    if(console_log)
    {
        SpyLogger::add_console_log(std::clog, static_cast<SpyLogger::LevelType>(log_level));
    }

    if(file_log)
    {
        std::filesystem::path log_file_path = findLogFilePath(log_file, profileFile, encrypted_log);

        if(encrypted_log)
        {
            const secure_string& password =
                PasswordKeeper::has_default_password()
                ? PasswordKeeper::get_default_password()
                : secure_string();

            logger.start_encrypted_logger(
                log_file_path,
                static_cast<SpyLogger::LevelType>(log_level),
                password,
                ask_password
            );
        }
        else
        {
            logger.start_logger(log_file_path, static_cast<SpyLogger::LevelType>(log_level));
        }
    }

    std::transform(modulePaths.begin(), modulePaths.end(), modulePaths.begin(), ::absolute);

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

    sight::core::runtime::Profile::sptr profile;

    try
    {
        profile = sight::core::runtime::io::ProfileReader::createProfile(profileFile);

        // Install a signal handler
        if(std::signal(SIGINT, signalHandler) == SIG_ERR)
        {
            perror("std::signal(SIGINT)");
        }

        if(std::signal(SIGTERM, signalHandler) == SIG_ERR)
        {
            perror("std::signal(SIGTERM)");
        }

#ifndef WIN32
        if(std::signal(SIGHUP, signalHandler) == SIG_ERR)
        {
            perror("std::signal(SIGHUP)");
        }

        if(std::signal(SIGQUIT, signalHandler) == SIG_ERR)
        {
            perror("std::signal(SIGQUIT)");
        }
#endif
        profile->setParams(profileArgs);

        profile->start();
        if(macroMode)
        {
            sight::core::runtime::loadModule("sight::module::ui::test");
        }

        profile->run();
        if(gSignalStatus == 0)
        {
            profile->stop();
        }

        if(macroMode)
        {
            sight::core::runtime::unloadModule("sight::module::ui::test");
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

    return 0;
}

//-----------------------------------------------------------------------------

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}

#endif // _WIN32
