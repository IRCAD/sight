/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/runtime/runtime.hpp>

#include <core/crypto/password_keeper.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/tools/os.hpp>

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

using sight::core::crypto::password_keeper;
using sight::core::crypto::secure_string;

//------------------------------------------------------------------------------

template<class A1, class A2>
inline static std::ostream& operator<<(std::ostream& _s, std::vector<A1, A2> const& _vec)
{
    copy(_vec.begin(), _vec.end(), std::ostream_iterator<A1>(_s, " "));
    return _s;
}

//-----------------------------------------------------------------------------

/// Wrapper for std::filesystem::absolute, needed by clang 3.0 in use with
/// std::transform
inline static std::filesystem::path absolute(const std::filesystem::path& _path)
{
    return std::filesystem::weakly_canonical(_path);
}

//-----------------------------------------------------------------------------

volatile sig_atomic_t g_signal_status = 0;
//------------------------------------------------------------------------------

void signal_handler(int _signal)
{
    g_signal_status = _signal;

    try
    {
        const auto& profile = sight::core::runtime::get_current_profile();

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

inline static std::filesystem::path build_log_file_path(const std::filesystem::path& _log_dir, bool _encrypted_log)
{
    // Use the default log file name as base
    std::filesystem::path log_file_path = _encrypted_log
                                          ? sight::core::log::ENCRYPTED_LOG_FILE
                                          : sight::core::log::LOG_FILE;

    // Prepend the log directory
    if(!_log_dir.empty())
    {
        std::filesystem::create_directories(_log_dir);
        log_file_path = _log_dir / log_file_path;
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

inline static std::filesystem::path find_log_file_path(
    const std::filesystem::path& _log_file,
    const std::filesystem::path& _profile_file,
    bool _encrypted_log
)
{
    if(_log_file.empty())
    {
        // Parse the profile.xml to get the name
        boost::property_tree::ptree profile_tree;
        boost::property_tree::read_xml(_profile_file.string(), profile_tree);
        const auto& profile_name = profile_tree.get<std::string>("profile.<xmlattr>.name");

        try
        {
            // Try the user cache directory
            return build_log_file_path(
                sight::core::tools::os::get_user_cache_dir(profile_name),
                _encrypted_log
            );
        }
        catch(...)
        {
            // Fallback: take temporary directory
            return build_log_file_path(
                std::filesystem::temp_directory_path() / "sight" / profile_name,
                _encrypted_log
            );
        }
    }

    // Take the user choice
    return _log_file;
}

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    std::vector<std::filesystem::path> module_paths;
    std::filesystem::path profile_file;
    sight::core::runtime::profile::params_container profile_args;

    // Runner options
    po::options_description options("Sight application launcher options");
    options.add_options()
        ("help,h", "Show help message")
        ("module-path,B", po::value(&module_paths), "Adds a module path")
    ;

    // Log options
#if defined(SIGHT_ENABLE_ENCRYPTED_LOG)
    const bool encrypted_log = true;

    // By default, don't ask for a password if there is a default password
    bool ask_password = !password_keeper::has_default_password();
#else
    const bool encrypted_log = false;
    bool ask_password        = false;
#endif

    /// @warning file_log cannot work with console_log on
    /// @todo fix this
    bool console_log = false;
    bool file_log    = true;

    std::string log_file;

    using spy_logger = sight::core::log::spy_logger;
    int log_level = spy_logger::sl_warn;

    po::options_description log_options("Log options");
    log_options.add_options()
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
        "Enable log output to console. Will disable file log."
    )
    (
        "no-clog",
        po::value(&console_log)->implicit_value(false)->zero_tokens(),
        "Disable log output to console"
    )
    (
        "flog",
        po::value(&file_log)->implicit_value(true)->zero_tokens(),
        "Enable log output to file. Will disable console log."
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
        po::value(&log_level)->implicit_value(spy_logger::sl_trace)->zero_tokens(),
        "Set log_level to trace"
    )
    (
        "log-debug",
        po::value(&log_level)->implicit_value(spy_logger::sl_debug)->zero_tokens(),
        "Set log_level to debug"
    )
    (
        "log-info",
        po::value(&log_level)->implicit_value(spy_logger::sl_info)->zero_tokens(),
        "Set log_level to info"
    )
    (
        "log-warn",
        po::value(&log_level)->implicit_value(spy_logger::sl_warn)->zero_tokens(),
        "Set log_level to warn"
    )
    (
        "log-error",
        po::value(&log_level)->implicit_value(spy_logger::sl_error)->zero_tokens(),
        "Set log_level to error"
    )
    (
        "log-fatal",
        po::value(&log_level)->implicit_value(spy_logger::sl_fatal)->zero_tokens(),
        "Set log_level to fatal"
    );

    // Hidden options
    bool macro_mode = false;
#ifdef WIN32
    bool enableAbortDialog = true;
#endif

    po::options_description hidden("Hidden options");
    hidden.add_options()
        ("profile", po::value(&profile_file)->default_value("profile.xml"), "Profile file")
        ("profile-args", po::value(&profile_args)->multitoken(), "Profile args")
        ("macro", po::value(&macro_mode)->implicit_value(true)->zero_tokens(), "Enable macro mode")
        ("no-macro", po::value(&macro_mode)->implicit_value(false)->zero_tokens(), "Disable macro mode")
#ifdef WIN32
    ("abort-dialog", po::value(&enableAbortDialog)->implicit_value(true)->zero_tokens(), "Enable abort dialog")
        ("no-abort-dialog", po::value(&enableAbortDialog)->implicit_value(false)->zero_tokens(), "Disable abort dialog")
#endif
    ;

    // Set options
    po::options_description cmdline_options;
    cmdline_options.add(options).add(log_options).add(hidden);

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

        if(vm.count("flog") > 0 && vm.count("clog") > 0)
        {
            throw po::error("Cannot enable both file and console log");
            return 1;
        }
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
        std::cout << options << std::endl << log_options << std::endl;
        return 0;
    }

    // If we enable console log, but not file log, we disable file log (which is enabled by default)
    if(console_log)
    {
        file_log = false;
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

    SIGHT_INFO_IF("Profile path: " << profile_file << " => " << ::absolute(profile_file), vm.count("profile"));
    SIGHT_INFO_IF("Profile-args: " << profile_args, vm.count("profile-args"));

    // Check if profile path exist
    profile_file = ::absolute(profile_file);

    SIGHT_FATAL_IF(
        "Profile file " << profile_file << " do not exists or is not a regular file.",
        !std::filesystem::is_regular_file(profile_file)
    );

    // Log file
    spy_logger& logger = spy_logger::get();

    if(console_log)
    {
        spy_logger::add_console_log(std::clog, static_cast<spy_logger::level_t>(log_level));
    }

    if(file_log)
    {
        std::filesystem::path log_file_path = find_log_file_path(log_file, profile_file, encrypted_log);

        if(encrypted_log)
        {
            const secure_string& password =
                password_keeper::has_default_password()
                ? password_keeper::get_default_password()
                : secure_string();

            logger.start_encrypted_logger(
                log_file_path,
                static_cast<spy_logger::level_t>(log_level),
                password,
                ask_password
            );
        }
        else
        {
            logger.start_logger(log_file_path, static_cast<spy_logger::level_t>(log_level));
        }
    }

    // Automatically adds the module folders where the profile.xml is located if it was not already there
    const auto profile_module_path = profile_file.parent_path().parent_path();
    bool find_profile_module_path  = false;
    for(const std::filesystem::path& module_path : module_paths)
    {
        if(profile_module_path == module_path)
        {
            find_profile_module_path = true;
        }
    }

    if(!find_profile_module_path)
    {
        module_paths.push_back(profile_module_path);
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

    sight::core::runtime::init();

    for(const std::filesystem::path& module_path : module_paths)
    {
        sight::core::runtime::add_modules(module_path);
    }

    sight::core::runtime::profile::sptr profile;

    try
    {
        profile = sight::core::runtime::io::profile_reader::create_profile(profile_file);

        // Install a signal handler
        if(std::signal(SIGINT, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGINT)");
        }

        if(std::signal(SIGTERM, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGTERM)");
        }

#ifndef WIN32
        if(std::signal(SIGHUP, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGHUP)");
        }

        if(std::signal(SIGQUIT, signal_handler) == SIG_ERR)
        {
            perror("std::signal(SIGQUIT)");
        }
#endif
        profile->set_params(profile_args);

        profile->start();
        if(macro_mode)
        {
            sight::core::runtime::load_module("sight::module::ui::test");
        }

        profile->run();
        if(g_signal_status == 0)
        {
            profile->stop();
        }

        if(macro_mode)
        {
            sight::core::runtime::unload_module("sight::module::ui::test");
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
