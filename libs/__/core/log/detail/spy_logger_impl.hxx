/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#pragma once

#include "stream_sink.hxx"

#include <boost/log/attributes.hpp>
#include <boost/log/attributes/current_process_id.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/expressions/keyword_fwd.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sink.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <mutex>
#include <string>

namespace sight::core::log::detail
{

using boost::posix_time::ptime;
namespace attributes  = boost::log::attributes;
namespace expressions = boost::log::expressions;
namespace trivial     = boost::log::trivial;

using boost_logger_t = boost::log::sources::severity_channel_logger_mt<boost::log::trivial::severity_level>;

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int);
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", trivial::severity_level);
BOOST_LOG_ATTRIBUTE_KEYWORD(process_id, "ProcessID", attributes::current_process_id::value_type);
BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", attributes::current_thread_id::value_type);
BOOST_LOG_ATTRIBUTE_KEYWORD(channel, "Channel", std::string);

BOOST_LOG_GLOBAL_LOGGER(lg, boost_logger_t);
BOOST_LOG_GLOBAL_LOGGER_DEFAULT(lg, boost_logger_t);

//------------------------------------------------------------------------------

inline static struct boost_initializer final
{
    boost_initializer()
    {
        boost::log::add_common_attributes();
        boost::log::core::get()->add_global_attribute("Uptime", attributes::timer());
    }
} s_boost_initializer;

//------------------------------------------------------------------------------

inline static auto console_stream_format()
{
    return expressions::stream << "["
           << line_id << "]["
           << expressions::format_date_time<ptime::time_duration_type>("Uptime", "%H:%M:%S.%f") << "]["
           << severity << "] "
           << expressions::smessage;
}

//------------------------------------------------------------------------------

inline static auto file_stream_format()
{
    return expressions::stream << "["
           << expressions::format_date_time<ptime>("TimeStamp", "%d.%m.%Y %H:%M:%S.%f") << "]["
           << expressions::format_date_time<ptime::time_duration_type>("Uptime", "%H:%M:%S.%f") << "]["
           << process_id << "]["
           << thread_id << "]["
           << severity << "] "
           << expressions::smessage;
}

class spy_logger_impl final
{
public:

    // Delete default constructors and assignment operators
    spy_logger_impl(const spy_logger_impl&)            = delete;
    spy_logger_impl(spy_logger_impl&&)                 = delete;
    spy_logger_impl& operator=(const spy_logger_impl&) = delete;
    spy_logger_impl& operator=(spy_logger_impl&&)      = delete;

    // Constructor
    explicit spy_logger_impl(spy_logger* const _spy_logger, boost_logger_t& _boost_logger = lg::get()) noexcept :
        m_spy_logger(_spy_logger),
        m_boost_logger(_boost_logger)
    {
    }

    // Destructor
    ~spy_logger_impl()
    {
        remove_sink();
    }

    // Returns the next log file name
    std::filesystem::path next_log_path(
        const std::filesystem::path& _log_path,
        const spy_logger::optional_password_t& _password = std::nullopt
)
    {
        // Get the original log name and the current index
        if(m_original_log_path.empty() && (_log_path.empty() || std::filesystem::is_directory(_log_path)))
        {
            const auto filename = _password ? ENCRYPTED_LOG_FILE : LOG_FILE;
            m_original_log_path = _log_path.empty() ? filename : _log_path / filename;
            m_log_path_index    = 0;
        }
        else if(!_log_path.empty() && m_original_log_path.empty())
        {
            m_original_log_path = _log_path;
            m_log_path_index    = 0;
        }
        else if(!_log_path.empty() && _log_path != m_original_log_path)
        {
            // If the filename are same, increment the index, to allow relocating previous log files
            if(_log_path.filename() == m_original_log_path.filename())
            {
                m_log_path_index++;
            }
            else
            {
                m_log_path_index = 0;
            }

            m_original_log_path = _log_path;
        }
        else
        {
            m_log_path_index++;
        }

        // Build the next log name
        std::filesystem::path new_log_path(m_original_log_path);

        auto original_log_filename = m_original_log_path.filename().string();
        auto pos                   = original_log_filename.find_first_of('.');

        if(pos == std::string::npos)
        {
            pos = std::max(original_log_filename.size() - 1, std::size_t(0));
        }

        // Insert the log counter
        original_log_filename.insert(pos, "." + std::to_string(m_log_path_index));

        new_log_path.replace_filename(original_log_filename);

        return new_log_path;
    }

    //------------------------------------------------------------------------------

    void remove_sink()
    {
        if(m_log_sink)
        {
            auto core = boost::log::core::get();
            core->flush();
            core->remove_sink(m_log_sink);

            m_log_sink->flush();
            m_log_sink.reset();
        }
    }

    //------------------------------------------------------------------------------

    void start(
        const std::filesystem::path& _log_path,
        spy_logger::level_t _level                       = spy_logger::level_t::trace,
        const spy_logger::optional_password_t& _password = std::nullopt,
        bool _asynchronous                               = false,
        const std::optional<std::string>& _channel       = std::nullopt
)
    {
        // If the logger is started, stop it by closing the input stream
        remove_sink();

        // Build the log name
        const auto& new_log_path = next_log_path(_log_path);

        // Store the log name
        m_log_paths.emplace_back(new_log_path);
        std::cerr << "\nLog file: " << new_log_path.string() << std::endl;

        // To avoid the logger to restart after changing the global password
        m_encrypted = _password.has_value();

        // Store the level, so we can launch the logger later when changing the password
        m_level = _level;

        // Store the asynchronous mode
        m_asynchronous = _asynchronous;

        // Store the channel, only if relevant for filtering, otherwise keep it to continue to use the same channel
        if(_channel)
        {
            m_channel = _channel;
        }

        auto sink =
            [&_asynchronous, &new_log_path, &_password, this](auto _sink)
            {
                // Set the formatter
                _sink->set_formatter(file_stream_format());

                // Set the filter
                if(m_channel)
                {
                    _sink->set_filter(
                        severity.or_default(trivial::fatal) >= trivial::severity_level(m_level)
                        && channel.or_default(std::string()) == *m_channel
                    );
                }
                else
                {
                    _sink->set_filter(
                        severity >= trivial::severity_level(m_level)
                    );
                }

                // Set the auto flush to false for maximum performance or true for maximum safety
                auto backend = _sink->locked_backend();
                backend->auto_flush(!_asynchronous);

                // Add a stream to write log to
                if(_password)
                {
                    backend->add_stream(
                        boost::make_shared<boost::iostreams::stream<stream_sink> >(
                            new_log_path,
                            stream_sink::parameters {_password->get(), _asynchronous
                            })
                    );
                }
                else
                {
                    auto stream = boost::make_shared<std::ofstream>(new_log_path);
                    SIGHT_THROW_IF("Unable to open the log file '" << new_log_path.string() << "'.", !stream->good());

                    backend->add_stream(stream);
                }

                return _sink;
            };

        if(_asynchronous)
        {
            // Create the sink as asynchronous for minimum latency
            m_log_sink = sink(
                boost::make_shared<boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> >()
            );
        }
        else
        {
            // Create the sink as synchronous for maximum safety
            m_log_sink = sink(
                boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> >()
            );
        }

        // Register the sink in the logging core
        boost::log::core::get()->add_sink(m_log_sink);
    }

    //------------------------------------------------------------------------------

    void stop()
    {
        // Stop the logger
        remove_sink();

        // Reset everything to pristine state
        m_log_path_index = 0;
        m_original_log_path.clear();
        m_log_paths.clear();
        m_level     = spy_logger::level_t::trace;
        m_encrypted = false;
    }

    //------------------------------------------------------------------------------

    static std::ostream& extract(
        std::istream& _input,
        std::ostream& _output,
        const spy_logger::optional_password_t& _password = std::nullopt
)
    {
        if(_password)
        {
            struct aes_context final
            {
                explicit aes_context(const core::crypto::secure_string& _password)
                {
                    SIGHT_THROW_IF("Cannot create cipher context.", !m_context);

                    // Compute password hash
                    crypto::hash(_password, m_key);

                    // Initialize decryption
                    check_aes(
                        [&]
                        {
                            return EVP_DecryptInit_ex(m_context, EVP_aes_256_cbc(), nullptr, m_key.data(), m_iv.data());
                        });
                }

                ~aes_context()
                {
                    // Free the compression context
                    EVP_CIPHER_CTX_free(m_context);
                    OPENSSL_cleanse(m_key.data(), m_key.size());
                    OPENSSL_cleanse(m_iv.data(), m_iv.size());
                }

                EVP_CIPHER_CTX* const m_context {EVP_CIPHER_CTX_new()};

                std::array<std::uint8_t, crypto::HASH_SIZE> m_key {};
                std::array<std::uint8_t, EVP_MAX_IV_LENGTH> m_iv {0};
            };

            std::unique_ptr<aes_context> aes;

            if(const auto& password = _password->get(); !password.empty())
            {
                aes = std::make_unique<aes_context>(password);
            }

            // Create the decompression context
            struct zstd_context final
            {
                explicit zstd_context()
                {
                    SIGHT_THROW_IF("Cannot create zstd context.", !m_context);
                }

                ~zstd_context()
                {
                    ZSTD_freeDCtx(m_context);
                }

                ZSTD_DCtx* m_context {ZSTD_createDCtx()};
            } zstd;

            // Initialize the buffers
            std::vector<char> in_buffer(ZSTD_DStreamInSize());
            std::vector<char> out_buffer(ZSTD_DStreamOutSize());
            std::vector<char> aes_buffer;

            // Read the first block to check if the password is correct
            if(aes)
            {
                const auto& password_hash = encrypt_password_hash(_password->get());

                _input.read(in_buffer.data(), EVP_MAX_BLOCK_LENGTH);
                if(std::memcmp(in_buffer.data(), password_hash.data(), EVP_MAX_BLOCK_LENGTH) != 0)
                {
                    SIGHT_THROW_EXCEPTION(
                        spy_logger::bad_password("The provided password is incorrect.")
                    );
                }
            }

            while(!_input.eof())
            {
                // Read data from file
                _input.read(in_buffer.data(), std::streamsize(in_buffer.size()));
                int processed_size = 0;

                // Decrypt the input
                if(aes)
                {
                    // Adjust buffers size if needed
                    expand(aes_buffer, _input.gcount() + EVP_MAX_BLOCK_LENGTH);

                    // Decrypt the input
                    processed_size = int(aes_buffer.size());

                    check_aes(
                        [&]
                        {
                            return EVP_DecryptUpdate(
                                aes->m_context,
                                reinterpret_cast<unsigned char*>(aes_buffer.data()),
                                &processed_size,
                                reinterpret_cast<const unsigned char*>(in_buffer.data()),
                                int(_input.gcount())
                            );
                        });
                }

                // Prepare input output buffers
                ZSTD_inBuffer zstd_in_buffer =
                    aes
                    ? ZSTD_inBuffer {aes_buffer.data(), std::size_t(processed_size), 0}
                    : ZSTD_inBuffer {in_buffer.data(), std::size_t(_input.gcount()), 0};

                // Decompress the input
                while(zstd_in_buffer.pos < zstd_in_buffer.size)
                {
                    ZSTD_outBuffer zstd_out_buffer {out_buffer.data(), out_buffer.size(), 0};
                    check_zstd([&]{return ZSTD_decompressStream(zstd.m_context, &zstd_out_buffer, &zstd_in_buffer);});
                    _output.write(out_buffer.data(), std::streamsize(zstd_out_buffer.pos));
                }
            }

            // If we reached the end of the stream, finalize the decryption
            if(aes)
            {
                auto processed_size = int(aes_buffer.size());

                const auto& [aes_error_code, aes_error] = check_aes_nothrow(
                    [&]
                    {
                        return EVP_DecryptFinal_ex(
                            aes->m_context,
                            reinterpret_cast<unsigned char*>(aes_buffer.data()),
                            &processed_size
                        );
                    });

                if(aes_error)
                {
                    SIGHT_THROW_EXCEPTION(spy_logger::premature_end());
                }

                // Prepare input output buffers
                ZSTD_inBuffer zstd_in_buffer {aes_buffer.data(), std::size_t(processed_size), 0};

                while(zstd_in_buffer.pos < zstd_in_buffer.size)
                {
                    ZSTD_outBuffer zstd_out_buffer {out_buffer.data(), out_buffer.size(), 0};

                    const auto& [zstd_result, zstd_error] = check_zstd_nothrow(
                        [&]
                        {
                            return ZSTD_decompressStream(zstd.m_context, &zstd_out_buffer, &zstd_in_buffer);
                        });

                    if(zstd_error)
                    {
                        SIGHT_THROW_EXCEPTION(spy_logger::premature_end());
                    }

                    _output.write(out_buffer.data(), std::streamsize(zstd_out_buffer.pos));
                }
            }
        }
        else
        {
            // No compression and no encrytion: copy the input to the output
            _output << _input.rdbuf();
        }

        return _output;
    }

    //------------------------------------------------------------------------------

    std::filesystem::path relocate(
        const std::filesystem::path& _new_path,
        const spy_logger::optional_password_t& _password,
        const spy_logger::optional_password_t& _old_password
)
    {
        // Create the new directory if needed
        const auto& new_dir = _new_path.has_parent_path()
                              ? _new_path.parent_path()
                              : _new_path.has_root_path()
                              ? _new_path.root_path()
                              : std::filesystem::current_path();

        std::filesystem::create_directories(new_dir);

        // Restart the logger. If it was encrypted, it will be encrypted again
        start(
            _new_path,
            m_level,
            _password,
            m_asynchronous
        );

        // Save last element (the new log created just before !) and remove it from the list so we won't merge it
        const auto new_log = std::filesystem::weakly_canonical(m_log_paths.back());
        m_log_paths.pop_back();

        // Save the old log path in case of relocation
        // Reserve a path for the merge of previous log
        const auto merged_log = m_log_paths.empty()
                                ? std::filesystem::path()
                                : std::filesystem::path(new_log).replace_filename(m_log_paths.back().filename());

        const auto tmp_merged_log = m_log_paths.empty()
                                    ? std::filesystem::path()
                                    : std::filesystem::path(merged_log).replace_extension(".tmp");

        // Remove the new log from the list
        m_log_paths.erase(
            std::remove_if(
                m_log_paths.begin(),
                m_log_paths.end(),
                [&new_log](const auto& _path)
            {
                return std::filesystem::weakly_canonical(_path) == new_log;
            }),
            m_log_paths.end()
        );

        // Merge the previous logs and relocate them
        if(!merged_log.empty() && !m_log_paths.empty())
        {
            // Scope the file stream to ensure it is closed before renaming
            {
                std::ofstream file_stream(
                    tmp_merged_log,
                    _password ? std::ios_base::out | std::ios::binary : std::ios_base::out
                );

                // Create the compressor sink, if a password is provided
                // Use best compression for the merge
                std::unique_ptr<std::ostream> stream_sink =
                    _password
                    ? std::make_unique<boost::iostreams::stream<detail::stream_sink> >(
                        file_stream,
                        stream_sink::parameters {_password->get(), true, true
                        })
                    : nullptr;

                // Create the merged log
                for(const auto& old_log_path : m_log_paths)
                {
                    {
                        std::ifstream in_stream(
                            old_log_path,
                            _old_password ? std::ios_base::in | std::ios::binary : std::ios_base::in
                        );

                        // Extract to the compressor sink or directly to the file
                        detail::spy_logger_impl::extract(
                            in_stream,
                            stream_sink ? *stream_sink : file_stream,
                            _old_password
                        );
                    }

                    // Remove the old log
                    std::filesystem::remove_all(old_log_path);
                }
            }

            // Replace the old log by the merged one
            std::filesystem::remove_all(merged_log);
            std::filesystem::rename(tmp_merged_log, merged_log);

            std::cerr << "\nRelocated log file: " << merged_log.string() << std::endl;

            // Keep the merged and current logs
            m_log_paths = {merged_log, new_log};
            return merged_log;
        }
        else
        {
            // Only keep the current log
            m_log_paths = {new_log};
            return {};
        }
    }

    // Pointer to the public interface
    spy_logger* const m_spy_logger;

    // The boost logger
    boost_logger_t& m_boost_logger;

    // Log file index counter, used to generate the log file name in an encrypted log
    int m_log_path_index {0};

    // The original asked log path
    std::filesystem::path m_original_log_path;

    // The real log path
    std::vector<std::filesystem::path> m_log_paths;

    // The asked level of logging
    spy_logger::level_t m_level {spy_logger::level_t::trace};

    // False if encrypted
    bool m_encrypted {false};

    // Mutex to protect the logger
    std::recursive_mutex m_mutex;

    // The sink to write the log
    boost::shared_ptr<boost::log::sinks::sink> m_log_sink;

    // True for fast asynchronous un-flushing log
    bool m_asynchronous {false};

    // The channel to filter the log in case of multiple loggers
    std::optional<std::string> m_channel {std::nullopt};
};

} // namespace sight::core::log
