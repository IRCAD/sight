/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE hicpp NOLINT

#ifdef _WIN32
#include <windows.h>
#endif

#include <core/crypto/base64.hpp>
#include <core/crypto/obfuscated_string.hpp>
#include <core/crypto/password_keeper.hpp>
#include <core/spy_log.hpp>
#include <core/tools/system.hpp>

#include <io/zip/archive_reader.hpp>
#include <io/zip/archive_writer.hpp>
#include <io/zip/exception/read.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/log/core.hpp>
#include <boost/optional.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <QApplication>
#include <QInputDialog>

#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

// NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

//#define SIGHTLOG_DEBUG_FILE
#if defined(SIGHTLOG_DEBUG_FILE)
#define DEBUG_LOG(msg) do{std::ofstream log("sightlog.log", std::ios_base::app); \
                          log << __LINE__ << ": " << msg << std::endl;}while(false)
#else
#define DEBUG_LOG(msg) (std::clog << __LINE__ << ": " << msg << std::endl) /* NOLINT: bugprone-macro-parentheses */
#endif

constexpr static auto SIGHT_HELP         = "help";
constexpr static auto SIGHT_VERSION      = "version";
constexpr static auto SIGHT_INPUT        = "input";
constexpr static auto SIGHT_OUTPUT       = "output";
constexpr static auto SIGHT_PASSWORD     = "password";
constexpr static auto SIGHT_OLD_PASSWORD = "old_password";
constexpr static auto SIGHT_DIRECTORY    = "directory";
constexpr static auto SIGHT_BASE64       = "base64";
constexpr static auto SIGHT_RAW          = "raw";
constexpr static auto SIGHT_ASK_PASS     = "ask-password";

static std::atomic_bool g_interrupted = false;

//------------------------------------------------------------------------------

inline static void signal_handler([[maybe_unused]] int _signal)
{
    switch(_signal)
    {
        case SIGINT:
        case SIGTERM:
#ifndef WIN32
        case SIGHUP:
        case SIGQUIT:
#endif
            g_interrupted = true;
            break;

        default:
            break;
    }

    DEBUG_LOG("SIGNAL: " << _signal << " received");
}

//------------------------------------------------------------------------------

inline static void send_password(const sight::core::crypto::secure_string& _password)
{
    const int pid = sight::core::tools::system::get_pid();
    std::cout.write(reinterpret_cast<const char*>(&pid), sizeof(pid));

    const auto password_size = static_cast<std::streamsize>(_password.size());
    std::cout.write(reinterpret_cast<const char*>(&password_size), sizeof(password_size));
    std::cout.write(_password.c_str(), password_size);

    std::cout.flush();
}

//------------------------------------------------------------------------------

template<typename T>
inline static T get_option_value(
    const boost::program_options::variables_map& _variables_map,
    const std::string& _option
)
{
    static const bool s_USE_BASE64 = _variables_map.count(SIGHT_BASE64) > 0;

    if(_variables_map.count(_option))
    {
        if(s_USE_BASE64)
        {
            if constexpr(std::is_base_of_v<std::filesystem::path, T>)
            {
                return static_cast<T>(sight::core::crypto::from_base64(_variables_map[_option].as<T>().string()));
            }
            else
            {
                return static_cast<T>(sight::core::crypto::from_base64(_variables_map[_option].as<T>()));
            }
        }

        return _variables_map[_option].as<T>();
    }

    return T();
}

//------------------------------------------------------------------------------

inline static sight::core::crypto::secure_string get_password(
    int _argc,
    char* _argv[],
    const boost::program_options::variables_map& _variables_map
)
{
    // In "raw" mode, there is no need of a password
    if(_variables_map.count(SIGHT_RAW) > 0)
    {
        return {};
    }

    // Get the given password, if any
    auto password = get_option_value<sight::core::crypto::secure_string>(_variables_map, SIGHT_PASSWORD);

    // If we MUST ask the user for a password,
    if(_variables_map.count(SIGHT_ASK_PASS) > 0)
    {
        QApplication app(_argc, _argv);

        bool ok = false;

        const QString& entered_password = QInputDialog::getText(
            nullptr,
            QObject::tr("Enter password"),
            QObject::tr("Please enter application password:"),
            QLineEdit::Password,
            QString::fromLocal8Bit(password.c_str()),
            &ok
        );

        if(ok)
        {
            // Use the entered password as password
            password = entered_password.toStdString();
        }
    }

    // Try the compiled password
    if(password.empty())
    {
        if constexpr(sight::core::crypto::password_keeper::has_default_password())
        {
            // Use compiled password
            password = sight::core::crypto::password_keeper::get_default_password();
        }
    }

    return password;
}

//------------------------------------------------------------------------------

inline static int log(
    int _argc,
    char* _argv[],
    const boost::program_options::variables_map& _variables_map
)
{
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

    // start logging
    // 1kB buffer
    std::array<char, 1024> buffer {};

    // Get the output path
    const auto& output_path = get_option_value<std::filesystem::path>(_variables_map, SIGHT_OUTPUT);

    // Get the password
    const auto& password = get_password(_argc, _argv, _variables_map);

    // Log without encryption, without compression
    if(_variables_map.count(SIGHT_RAW) > 0)
    {
        // Open the log file
        std::ofstream log_file_stream(output_path.string());

        if(!log_file_stream.good())
        {
            DEBUG_LOG(_argv[0] << ": cannot write '" << output_path.string() << "'.");
            return __LINE__;
        }

        // Tell to the parent process to start logging things
        send_password(password);

        // Start logging
        while(!std::cin.eof() && std::cin.good() && !g_interrupted)
        {
            // Read from std::cin
            std::cin.read(buffer.data(), buffer.size());

            // Store the read size
            const std::streamsize read_size = std::cin.gcount();

            if(read_size > 0)
            {
                // Write to the archive
                log_file_stream.write(buffer.data(), read_size);
                log_file_stream.flush();
            }
        }
    }
    else
    {
        // Create the archive writer
        auto archive_writer = sight::io::zip::archive_writer::get(output_path);

        // Write a new log file in the archive
        auto archive_stream = archive_writer->open_file(
            sight::core::log::LOG_FILE,
            password
        );

        // Tell to the parent process to start logging things
        send_password(password);

        // Start the writing loop
        while(!std::cin.eof() && std::cin.good() && !g_interrupted)
        {
            // Read from std::cin
            std::cin.read(buffer.data(), buffer.size());

            // Store the read size
            const std::streamsize read_size = std::cin.gcount();

            if(read_size > 0)
            {
                // Write to the archive
                archive_stream->write(buffer.data(), read_size);
                archive_stream->flush();
            }
        }
    }

    return 0;
}

//------------------------------------------------------------------------------

inline static int extract(
    int _argc,
    char* _argv[],
    const boost::program_options::variables_map& _variables_map
)
{
    // Get the input path
    const std::filesystem::path& input_path = get_option_value<std::string>(_variables_map, SIGHT_INPUT);

    // Create the archive reader
    auto archive_reader = sight::io::zip::archive_reader::get(input_path);

    const auto& password = get_password(_argc, _argv, _variables_map);

    // Open log file from the archive
    std::unique_ptr<std::istream> archive_istream;

    try
    {
        archive_istream = archive_reader->open_file(
            sight::core::log::LOG_FILE,
            password
        );
    }
    catch(const sight::io::zip::exception::bad_password&)
    {
        if constexpr(sight::core::crypto::password_keeper::has_default_password())
        {
            archive_istream = archive_reader->open_file(
                sight::core::log::LOG_FILE,
                sight::core::crypto::password_keeper::get_global_password()
            );
        }
        else
        {
            throw;
        }
    }

    // Write to the file
    const auto& extracted_log_path =
        [&]
        {
            if(_variables_map.count(SIGHT_DIRECTORY) > 0)
            {
                // Get the output directory
                const auto& directory = get_option_value<std::filesystem::path>(_variables_map, SIGHT_DIRECTORY);

                // Create directories, if needed
                std::filesystem::create_directories(directory);
                return directory / sight::core::log::LOG_FILE;
            }

            // Use current working directory
            return std::filesystem::path(sight::core::log::LOG_FILE);
        }();

    std::ofstream log_file_stream(extracted_log_path.string());

    if(!log_file_stream.good())
    {
        DEBUG_LOG(_argv[0] << ": cannot write '" << extracted_log_path.string() << "'.");
        return __LINE__;
    }

    log_file_stream << archive_istream->rdbuf();

    return 0;
}

//------------------------------------------------------------------------------

inline static int merge(
    [[maybe_unused]] char* _argv[],
    const boost::program_options::variables_map& _variables_map,
    const sight::core::crypto::secure_string& _password,
    const sight::core::crypto::secure_string& _old_password,
    std::ostream& _ostream
)
{
    // Get input paths
    std::vector<std::filesystem::path> input_paths;
    boost::split(
        input_paths,
        get_option_value<std::string>(_variables_map, SIGHT_INPUT),
        boost::is_any_of(";")
    );

    const auto decrypt =
        [&](const std::filesystem::path& _input_path, const sight::core::crypto::secure_string& _secret)
        {
            // Create the archive reader
            auto archive_reader = sight::io::zip::archive_reader::get(_input_path);

            // Open log file from the archive
            auto archive_istream = archive_reader->open_file(
                sight::core::log::LOG_FILE,
                _secret
            );

            try
            {
                _ostream << archive_istream->rdbuf();
            }
            catch([[maybe_unused]] const std::exception& e)
            {
                DEBUG_LOG(_argv[0] << ": " << e.what());
                return __LINE__;
            }
            catch(...)
            {
                DEBUG_LOG(_argv[0] << ": Unknown exception.");
                return __LINE__;
            }

            return 0;
        };

    const auto try_decrypt =
        [&](const std::filesystem::path& _input_path)
        {
            try
            {
                return decrypt(_input_path, _password);
            }
            catch(const sight::io::zip::exception::bad_password&)
            {
                try
                {
                    return decrypt(_input_path, _old_password);
                }
                catch(const sight::io::zip::exception::bad_password&)
                {
                    if constexpr(sight::core::crypto::password_keeper::has_default_password())
                    {
                        return decrypt(_input_path, sight::core::crypto::password_keeper::get_default_password());
                    }

                    throw;
                }
            }
        };

    // Extract all the input paths
    for(const auto& input_path : input_paths)
    {
        try
        {
            // Try as an archive first
            if(const auto result = try_decrypt(input_path); result != 0)
            {
                return result;
            }
        }
        catch(...)
        {
            // try as a regular file
            std::ifstream istream(input_path.string(), std::ios::binary | std::ios::in);
            _ostream << istream.rdbuf();
        }
    }

    return 0;
}

//------------------------------------------------------------------------------

inline static int convert(
    int _argc,
    char* _argv[],
    const boost::program_options::variables_map& _variables_map
)
{
    // Get the output path
    const auto& output_path = get_option_value<std::filesystem::path>(_variables_map, SIGHT_OUTPUT);

    // Get the passwords
    const auto& password     = get_password(_argc, _argv, _variables_map);
    const auto& old_password = get_option_value<sight::core::crypto::secure_string>(_variables_map, SIGHT_OLD_PASSWORD);

    if(_variables_map.count(SIGHT_RAW) > 0)
    {
        std::ofstream ofstream(output_path.string(), std::ios::binary | std::ios::out | std::ios::trunc);

        return merge(_argv, _variables_map, password, old_password, ofstream);
    }

    // Create the archive reader
    auto archive_writer = sight::io::zip::archive_writer::get(output_path);

    // Write a new log file in the archive
    auto archive_ostream = archive_writer->open_file(
        sight::core::log::LOG_FILE,
        password
    );

    return merge(_argv, _variables_map, password, old_password, *archive_ostream);
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
#if defined(SIGHTLOG_DEBUG_FILE)
    // Debug arguments
    std::stringstream stream;
    for(int i = 0 ; i < argc ; ++i)
    {
        stream << "'" << argv[i] << "' ";
    }
    DEBUG_LOG(argv[0] << " arguments: " << stream.str());
#endif

    // Setup own boost::log logger to print stuff on stderr by default
    boost::log::core::get()->remove_all_sinks();
    sight::core::log::spy_logger::add_console_log(std::clog, sight::core::log::spy_logger::sl_warn);

    // Register program options
    // Common options
    namespace po = boost::program_options;

    po::options_description options("Sightlog logger options");
    options.add_options()
    (
        (std::string(SIGHT_HELP) + ",h").c_str(),
        "Display this help message."
    )
    (
        (std::string(SIGHT_VERSION) + ",v").c_str(),
        "Display the version of the program."
    )
    (
        (std::string(SIGHT_INPUT) + ",i").c_str(),
        po::value<std::string>(),
        "Log archive to extract."
    )
    (
        (std::string(SIGHT_PASSWORD) + ",p").c_str(),
        po::value<sight::core::crypto::secure_string>(),
        "Password to use for encryption and decryption."
    )
    (
        (std::string(SIGHT_ASK_PASS) + ",a").c_str(),
        "Show a popup to enter the password."
    )
    (
        (std::string(SIGHT_DIRECTORY) + ",d").c_str(),
        po::value<std::filesystem::path>(),
        "Output directory when extracting a log archive."
    );

    // Hidden options
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
    (
        (std::string(SIGHT_RAW) + ",r").c_str(),
        "Do not compress, do not encrypt, just write raw logs."
    )
    (
        (std::string(SIGHT_BASE64) + ",b").c_str(),
        "Used by sight to pass arguments without worrying about quotes and spaces characters."
    )
    (
        (std::string(SIGHT_OUTPUT) + ",o").c_str(),
        po::value<std::filesystem::path>(),
        "Log archive to write."
    )
    (
        (std::string(SIGHT_OLD_PASSWORD) + ",P").c_str(),
        po::value<sight::core::crypto::secure_string>(),
        "Old password used for conversion operation."
    );

    // Merge options
    po::options_description arguments;
    arguments.add(options).add(hidden_options);

    // Parse program options
    po::variables_map variables_map;

    try
    {
        store(
            po::command_line_parser(argc, argv)
            .options(arguments)
            .run(),
            variables_map
        );

        notify(variables_map);

        if((variables_map.count(SIGHT_HELP) == 0U)
           && (variables_map.count(SIGHT_VERSION) == 0U)
           && (variables_map.count(SIGHT_INPUT) == 0U)
           && (variables_map.count(SIGHT_OUTPUT) == 0U))
        {
            std::cerr << "Error: One of the options -i or -o is required." << std::endl;
            std::cerr << options << std::endl;
            return __LINE__;
        }
    }
    catch([[maybe_unused]] const po::error& e)
    {
        DEBUG_LOG(argv[0] << ": " << e.what());
        DEBUG_LOG(options);
        return __LINE__;
    }

    // Display help
    if(variables_map.count(SIGHT_HELP) > 0)
    {
        std::cerr << options << std::endl;
        return 0;
    }

    // Display version
    if(variables_map.count(SIGHT_VERSION) > 0)
    {
        std::cerr << "2.0" << std::endl;
        return 0;
    }

    const bool has_output = variables_map.count(SIGHT_OUTPUT) > 0;
    const bool has_input  = variables_map.count(SIGHT_INPUT) > 0;

    try
    {
        if(has_output && has_input)
        {
            // We must convert a log archive to another format
            return convert(argc, argv, variables_map);
        }

        if(has_output)
        {
            // We must log
            return log(argc, argv, variables_map);
        }

        if(has_input)
        {
            // We must extract
            return extract(argc, argv, variables_map);
        }
    }
    catch([[maybe_unused]] const std::exception& e)
    {
        DEBUG_LOG(argv[0] << ": " << e.what());
        return __LINE__;
    }
    catch(...)
    {
        DEBUG_LOG(argv[0] << ": Unknown exception.");
        return __LINE__;
    }

    return 0;
}

#ifdef _WIN32
//------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}

#endif // _WIN32

// NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
