/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <core/crypto/Base64.hpp>
#include <core/crypto/obfuscated_string.hpp>
#include <core/crypto/PasswordKeeper.hpp>
#include <core/spyLog.hpp>
#include <core/tools/System.hpp>

#include <io/zip/ArchiveReader.hpp>
#include <io/zip/ArchiveWriter.hpp>

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

//#define SIGHTLOG_DEBUG_FILE
#if defined(SIGHTLOG_DEBUG_FILE)
#define DEBUG_LOG(msg) do{std::ofstream log("sightlog.log", std::ios_base::app); \
                          log << __LINE__ << ": " << msg << std::endl;}while(false)
#else
#define DEBUG_LOG(msg) (std::clog << __LINE__ << ": " << msg << std::endl) /* NOLINT: bugprone-macro-parentheses */
#endif

//------------------------------------------------------------------------------

inline static void signalHandler([[maybe_unused]] int signal)
{
    DEBUG_LOG("SIGNAL: " << signal << " received");
}

//------------------------------------------------------------------------------

inline static void sendPassword(const sight::core::crypto::secure_string& password)
{
    const int pid = sight::core::tools::System::getPID();
    std::cout.write(reinterpret_cast<const char*>(&pid), sizeof(pid));

    const auto password_size = static_cast<std::streamsize>(password.size());
    std::cout.write(reinterpret_cast<const char*>(&password_size), sizeof(password_size));
    std::cout.write(password.c_str(), password_size);

    std::cout.flush();
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
    sight::core::log::SpyLogger::add_console_log(std::clog, sight::core::log::SpyLogger::SL_WARN);

    // Register program options
    // Common options
    namespace po = boost::program_options;
    constexpr static auto HELP      = "help";
    constexpr static auto VERSION   = "version";
    constexpr static auto INPUT     = "input";
    constexpr static auto OUTPUT    = "output";
    constexpr static auto PASSWORD  = "password";
    constexpr static auto DIRECTORY = "directory";
    constexpr static auto BASE64    = "base64";
    constexpr static auto RAW       = "raw";
    constexpr static auto ASK_PASS  = "ask-password";

    po::options_description options("Sightlog logger options");
    options.add_options()
    (
        (std::string(HELP) + ",h").c_str(),
        "Display this help message."
    )
    (
        (std::string(VERSION) + ",v").c_str(),
        "Display the version of the program."
    )
    (
        (std::string(INPUT) + ",i").c_str(),
        po::value<std::filesystem::path>(),
        "Log archive to extract."
    )
    (
        (std::string(PASSWORD) + ",p").c_str(),
        po::value<sight::core::crypto::secure_string>(),
        "Password to use for encryption and decryption."
    )
    (
        (std::string(ASK_PASS) + ",a").c_str(),
        "Show a popup to enter the password."
    )
    (
        (std::string(DIRECTORY) + ",d").c_str(),
        po::value<std::filesystem::path>(),
        "Output directory when extracting a log archive."
    );

    // Hidden options
    po::options_description hidden_options("Hidden options");
    hidden_options.add_options()
    (
        (std::string(RAW) + ",r").c_str(),
        "Do not compress, do not encrypt, just write raw logs."
    )
    (
        (std::string(BASE64) + ",b").c_str(),
        "Used by sight to pass arguments without worrying about quotes and spaces characters."
    )
    (
        (std::string(OUTPUT) + ",o").c_str(),
        po::value<std::filesystem::path>(),
        "Log archive to write."
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

        // Check mutually exclusive options
        if((variables_map.count(INPUT) != 0U) && !variables_map[INPUT].defaulted()
           && (variables_map.count(OUTPUT) != 0U) && !variables_map[OUTPUT].defaulted())
        {
            std::cerr << "Error: Options -i and -o are mutually exclusive." << std::endl;
            std::cerr << options << std::endl;
            return __LINE__;
        }

        if((variables_map.count(HELP) == 0U)
           && (variables_map.count(VERSION) == 0U)
           && (variables_map.count(INPUT) == 0U)
           && (variables_map.count(OUTPUT) == 0U))
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
    if(variables_map.count(HELP) > 0)
    {
        std::cerr << options << std::endl;
        return 0;
    }

    // Display version
    if(variables_map.count(VERSION) > 0)
    {
        std::cerr << "1.0" << std::endl;
        return 0;
    }

    const bool is_logging = variables_map.count(OUTPUT) > 0;
    const bool use_base64 = variables_map.count(BASE64) > 0;

    // Archive path
    const auto& archive_path =
        [&]
        {
            std::filesystem::path path = is_logging
                                         ? variables_map[OUTPUT].as<std::filesystem::path>()
                                         : variables_map[INPUT].as<std::filesystem::path>();

            if(use_base64)
            {
                path = sight::core::crypto::from_base64(path.string());
            }

            // Force log extension for raw logs
            if(variables_map.count(RAW) > 0)
            {
                path = path.replace_extension(".log");
            }

            return path;
        }();

    // Get the password
    const auto& password =
        [&]
        {
            sight::core::crypto::secure_string password;

            // In "raw" mode, there is no need of a password
            if(variables_map.count(RAW) > 0)
            {
                return password;
            }

            // Get the given password, if any
            if(variables_map.count(PASSWORD) > 0)
            {
                password = variables_map[PASSWORD].as<sight::core::crypto::secure_string>();

                if(use_base64)
                {
                    password = sight::core::crypto::from_base64(password);
                }
            }

            // If we MUST ask the user for a password,
            if(variables_map.count(ASK_PASS) > 0)
            {
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
                QApplication app(argc, argv);

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

            if(password.empty())
            {
                if constexpr(sight::core::crypto::PasswordKeeper::has_default_password())
                {
                    // Use compiled password
                    password = sight::core::crypto::PasswordKeeper::get_default_password();
                }
                else
                {
                    DEBUG_LOG(argv[0] << ": Log encryption has been enabled, but no default password has been set.");
                    exit(__LINE__);
                }
            }

            return password;
        }();

    // Start logging loop
    if(is_logging)
    {
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

        try
        {
            // 1kB buffer
            std::array<char, 1024> buffer {};

            // 100 MB rotation limit
            constexpr std::streamsize rotate_size = 128LL * 1024 * 1024;

            // Store the read size, so we can rotate the log file
            std::streamsize written_size = 0;

            // Log without encryption, without compression
            if(variables_map.count(RAW) > 0)
            {
                // Open the log file
                std::ofstream log_file_stream(archive_path.string());

                if(!log_file_stream.good())
                {
                    DEBUG_LOG(argv[0] << ": cannot write '" << archive_path.string() << "'.");
                    return __LINE__;
                }

                // Tell to the parent process to start logging things
                sendPassword(password);

                // Start logging
                while(!std::cin.eof())
                {
                    // Read from std::cin
                    std::cin.read(buffer.data(), buffer.size());

                    // Store the read size
                    const std::streamsize read_size = std::cin.gcount();

                    if(read_size > 0)
                    {
                        // Write to the archive
                        log_file_stream.write(buffer.data(), read_size);

                        written_size += read_size;

                        // Rotate the log file, if needed
                        if(written_size > rotate_size)
                        {
                            // Close everything
                            log_file_stream.close();
                            std::filesystem::remove_all(archive_path);

                            // Re-create the archive writer
                            log_file_stream = std::ofstream(archive_path.string());

                            if(!log_file_stream.good())
                            {
                                DEBUG_LOG(argv[0] << ": cannot write '" << archive_path.string() << "'.");
                                return __LINE__;
                            }

                            // Reset the written size
                            written_size = 0;
                        }
                    }
                }
            }
            else
            {
                // Create the archive writer
                auto archive_writer = sight::io::zip::ArchiveWriter::get(
                    archive_path,
                    sight::io::zip::Archive::ArchiveFormat::OPTIMIZED
                );

                // Write a new  log file in the archive
                auto archive_stream = archive_writer->openFile(
                    sight::core::log::LOG_FILE,
                    password
                );

                // Tell to the parent process to start logging things
                sendPassword(password);

                // Start the writing loop
                while(!std::cin.eof())
                {
                    // Read from std::cin
                    std::cin.read(buffer.data(), buffer.size());

                    // Store the read size
                    const std::streamsize read_size = std::cin.gcount();

                    if(read_size > 0)
                    {
                        // Write to the archive
                        archive_stream->write(buffer.data(), read_size);

                        written_size += read_size;

                        // Rotate the log file, if needed
                        if(written_size > rotate_size)
                        {
                            // Close everything
                            archive_stream.reset();
                            archive_writer.reset();
                            std::filesystem::remove_all(archive_path);

                            // Re-create the archive writer
                            archive_writer = sight::io::zip::ArchiveWriter::get(
                                archive_path,
                                sight::io::zip::Archive::ArchiveFormat::OPTIMIZED
                            );

                            // Write a new log file in the archive
                            archive_stream = archive_writer->openFile(
                                sight::core::log::LOG_FILE,
                                password
                            );

                            // Reset the written size
                            written_size = 0;
                        }
                    }
                }
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
    }
    else
    {
        try
        {
            // Create the archive reader
            auto archive_reader = sight::io::zip::ArchiveReader::get(archive_path);

            // Open log file from the archive
            auto archive_stream = archive_reader->openFile(
                sight::core::log::LOG_FILE,
                password
            );

            // Write to the file
            const auto& log_path =
                [&]
                {
                    if(variables_map.count(DIRECTORY) > 0)
                    {
                        // Get the output directory
                        const auto& directory = variables_map[DIRECTORY].as<std::filesystem::path>();

                        if(use_base64)
                        {
                            const std::filesystem::path decoded_directory(
                                sight::core::crypto::from_base64(directory.string())
                            );

                            // Create directories, if needed
                            std::filesystem::create_directories(decoded_directory);

                            return decoded_directory / sight::core::log::LOG_FILE;
                        }

                        // Create directories, if needed
                        std::filesystem::create_directories(directory);
                        return directory / sight::core::log::LOG_FILE;
                    }

                    // Use current working directory
                    return std::filesystem::path(sight::core::log::LOG_FILE);
                }();

            std::ofstream log_file_stream(log_path.string());

            if(!log_file_stream.good())
            {
                DEBUG_LOG(argv[0] << ": cannot write '" << log_path << "'.");
                return __LINE__;
            }

            log_file_stream << archive_stream->rdbuf();
            log_file_stream.close();
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
