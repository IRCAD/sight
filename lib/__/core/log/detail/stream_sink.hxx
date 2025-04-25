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

#include "core/crypto/aes256.hpp"
#include "core/crypto/sha256.hpp"
#include "core/exceptionmacros.hpp"
#include "core/log/spy_logger.hpp"
#include "core/macros.hpp"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4702)
#endif

#include <boost/iostreams/stream.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <openssl/err.h>
#include <openssl/evp.h>

#include <zstd.h>

#include <fstream>

namespace sight::core::log::detail
{

//------------------------------------------------------------------------------

static constexpr auto check_aes(const auto& _expr)
{
    decltype(ERR_get_error()) error_code = 0;
    const bool error                     = _expr() == 0;

    if(error)
    {
        error_code = ERR_get_error();
        throw std::runtime_error(std::string(ERR_error_string(error_code, nullptr)));
    }

    return std::pair {error_code, error};
}

//------------------------------------------------------------------------------

static constexpr auto check_aes_nothrow(const auto& _expr)
{
    decltype(ERR_get_error()) error_code = 0;
    const bool error                     = _expr() == 0;

    if(error)
    {
        error_code = ERR_get_error();
        std::cerr << "AES: " << std::string(ERR_error_string(error_code, nullptr)) << std::endl;
    }

    return std::pair {error_code, error};
}

//------------------------------------------------------------------------------

static constexpr auto check_zstd(const auto& _expr)
{
    const auto result = _expr();
    const bool error  = ZSTD_isError(result) != 0;

    if(error)
    {
        throw std::runtime_error("ZSTD: " + std::string(ZSTD_getErrorName(result)));
    }

    return std::pair {result, error};
}

//------------------------------------------------------------------------------

static constexpr auto check_zstd_nothrow(const auto& _expr)
{
    const auto result = _expr();
    const bool error  = ZSTD_isError(result) != 0;

    if(error)
    {
        std::cerr << "ZSTD: " + std::string(ZSTD_getErrorName(result)) << std::endl;
    }

    return std::pair {result, error};
}

//------------------------------------------------------------------------------

static constexpr void expand(auto& _container, auto _size)
{
    if(std::size_t(_container.size()) < std::size_t(_size))
    {
        _container.resize(std::size_t(_size));
    }
}

//------------------------------------------------------------------------------

inline static crypto::secure_string encrypt_password_hash(const crypto::secure_string& _password)
{
    std::array<std::uint8_t, crypto::HASH_SIZE> password_hash {};
    crypto::hash(_password, password_hash);

    return crypto::encrypt(
        crypto::secure_string(reinterpret_cast<const char*>(password_hash.data()), crypto::HASH_SIZE),
        _password
    );
}

static struct openssl_initializer final
{
    openssl_initializer()
    {
        EVP_add_cipher(EVP_aes_256_cbc());
        ERR_load_crypto_strings();
    }

    ~openssl_initializer()
    {
        ERR_free_strings();
        EVP_cleanup();
    }
} s_openssl_initializer;

/// Initialize the ZSTD compression context. Use RAII to ensure proper cleanup.
struct zstd final
{
    explicit zstd(bool _best = false)
    {
        SIGHT_THROW_IF("Unable to create the ZSTD compression context.", !m_context);

        // Set the compression level to 1 - "fast" (default: 0 - balanced)
        check_zstd(
            [&]
            {
                return ZSTD_CCtx_setParameter(m_context, ZSTD_c_compressionLevel, _best ? ZSTD_maxCLevel() : 1);
            });

        // Set the strategy to 1 - "fast" (default: 0 - balanced)
        // cspell:ignore btultra2
        check_zstd([&]{return ZSTD_CCtx_setParameter(m_context, ZSTD_c_strategy, _best ? ZSTD_btultra2 : ZSTD_fast);});

        // No checksum for faster operation
        check_zstd([&]{return ZSTD_CCtx_setParameter(m_context, ZSTD_c_checksumFlag, _best ? 1 : 0);});

        // No threading for compression as log message are usually small
        check_zstd([&]{return ZSTD_CCtx_setParameter(m_context, ZSTD_c_nbWorkers, 0);});

        // Enable long distance matching for best compression
        check_zstd([&]{return ZSTD_CCtx_setParameter(m_context, ZSTD_c_enableLongDistanceMatching, _best ? 1 : 0);});
    }

    ~zstd()
    {
        // Free the compression context
        ZSTD_freeCCtx(m_context);
    }

    ZSTD_CCtx* const m_context {ZSTD_createCCtx()};

    /// The ZSTD out buffer
    std::vector<char> m_buffer {std::vector<char>(ZSTD_CStreamOutSize())};
};

/// Initialize the OpenSSL AES context. Use RAII to ensure proper cleanup.
struct aes final
{
    explicit aes(std::ostream& _stream, const core::crypto::secure_string& _password)
    {
        SIGHT_THROW_IF("Unable to create the cipher context.", !m_context);

        // Compute password hash
        crypto::hash(_password, m_key);

        // Initialize encryption
        check_aes([&]{return EVP_EncryptInit_ex(m_context, EVP_aes_256_cbc(), nullptr, m_key.data(), m_iv.data());});

        // Write the encrypted password hash to the stream so we can check password correctness
        const auto& password_hash = encrypt_password_hash(_password);
        _stream.write(password_hash.data(), crypto::HASH_SIZE);
    }

    ~aes()
    {
        // Free the compression context
        EVP_CIPHER_CTX_free(m_context);
        OPENSSL_cleanse(m_key.data(), m_key.size());
        OPENSSL_cleanse(m_iv.data(), m_iv.size());
    }

    EVP_CIPHER_CTX* const m_context {EVP_CIPHER_CTX_new()};

    std::array<std::uint8_t, crypto::HASH_SIZE> m_key {};
    std::array<std::uint8_t, EVP_MAX_IV_LENGTH> m_iv {0};

    std::vector<char> m_buffer {std::vector<char>(ZSTD_CStreamOutSize() + EVP_MAX_BLOCK_LENGTH)};
};

/// The stream context to handle the compression and encryption
/// This is separated from sink because boost sinks are shallow copied and we need to keep the context alive and unique
/// RAII is used to ensure proper cleanup and proper writing of the file epilogue
struct stream_context final
{
    explicit stream_context(
        const std::filesystem::path& _log_path,
        const core::crypto::secure_string& _password,
        bool _asynchronous = false,
        bool _best         = false
    ) :
        m_asynchronous(_asynchronous),
        m_best(_best),
        m_file_stream(_log_path, std::ios::binary),
        m_stream(m_file_stream),
        m_zstd(_best),
        m_aes(_password.empty() ? nullptr : std::make_unique<aes>(m_stream, _password))
    {
        SIGHT_THROW_IF("Unable to open the log file '" << _log_path << "'.", !m_stream.good());
    }

    explicit stream_context(
        std::ostream& _out_stream,
        const core::crypto::secure_string& _password,
        bool _asynchronous = false,
        bool _best         = false
    ) :
        m_asynchronous(_asynchronous),
        m_best(_best),
        m_stream(_out_stream),
        m_zstd(_best),
        m_aes(_password.empty() ? nullptr : std::make_unique<aes>(m_stream, _password))
    {
        SIGHT_THROW_IF("The provided output stream is not good.", !m_stream.good());
    }

    ~stream_context()
    {
        // ZSTD epilogue
        ZSTD_inBuffer zstd_in_buffer {nullptr, 0, 0};
        ZSTD_outBuffer zstd_out_buffer {m_zstd.m_buffer.data(), m_zstd.m_buffer.size(), 0};

        check_zstd_nothrow(
            [&]
            {
                return ZSTD_compressStream2(m_zstd.m_context, &zstd_out_buffer, &zstd_in_buffer, ZSTD_e_end);
            });

        // AES epilogue
        if(m_aes)
        {
            // Encrypt ZSTD epilogue
            if(zstd_out_buffer.pos > 0)
            {
                // Adjust buffers size if needed
                expand(m_aes->m_buffer, zstd_out_buffer.pos + EVP_MAX_BLOCK_LENGTH);
                auto aes_out_size = int(m_aes->m_buffer.size());

                check_aes_nothrow(
                    [&]
                    {
                        return EVP_EncryptUpdate(
                            m_aes->m_context,
                            reinterpret_cast<unsigned char*>(m_aes->m_buffer.data()),
                            &aes_out_size,
                            reinterpret_cast<const unsigned char*>(m_zstd.m_buffer.data()),
                            int(zstd_out_buffer.pos)
                        );
                    });

                m_stream.write(m_aes->m_buffer.data(), aes_out_size);
            }

            // AES epilogue
            int aes_out_size = int(m_aes->m_buffer.size());

            // Finalize encryption
            check_aes_nothrow(
                [&]
                {
                    return EVP_EncryptFinal_ex(
                        m_aes->m_context,
                        reinterpret_cast<unsigned char*>(m_aes->m_buffer.data()),
                        &aes_out_size
                    );
                });

            m_stream.write(m_aes->m_buffer.data(), aes_out_size);
        }
        else if(zstd_out_buffer.pos > 0)
        {
            // No encryption, just write ZSTD epilogue
            m_stream.write(m_zstd.m_buffer.data(), std::streamsize(zstd_out_buffer.pos));
        }
    }

    const bool m_asynchronous {false};
    const bool m_best {false};

    /// The output file stream
    std::ofstream m_file_stream;
    std::ostream& m_stream {m_file_stream};

    /// The ZSTD compression context
    zstd m_zstd;

    /// AES context
    const std::unique_ptr<aes> m_aes;
};

class stream_sink final
{
public:

    // Needed by Boost
    using char_type = char;
    using category  = boost::iostreams::sink_tag;

    // To pass more than 3 parameters to Boost, we use a struct
    struct parameters final
    {
        const core::crypto::secure_string& password;
        const bool asynchronous {false};
        const bool best {false};
    };

    // BEWARE: Boost makes shallow copies of the sink, that's why we use shared_ptr...
    explicit stream_sink(
        const std::filesystem::path& _log_path,
        const parameters& _p
    ) :
        m_context(std::make_shared<stream_context>(_log_path, _p.password, _p.asynchronous, _p.best))
    {
    }

    explicit stream_sink(
        std::ostream& _out_stream,
        const parameters& _p
    ) :
        m_context(std::make_shared<stream_context>(_out_stream, _p.password, _p.asynchronous, _p.best))
    {
    }

    // Boost use this to write things
    std::streamsize write(const char* _src, std::streamsize _src_size)
    {
        // Estimate the compressed size (and also check that the input is not too big)
        const auto& [bound, error] = check_zstd([&]{return ZSTD_compressBound(std::size_t(_src_size));});

        // Adjust buffers size if needed
        expand(m_context->m_zstd.m_buffer, bound);

        // Prepare input output buffers
        ZSTD_inBuffer zstd_in_buffer {_src, std::size_t(_src_size), 0};
        ZSTD_outBuffer zstd_out_buffer {
            m_context->m_zstd.m_buffer.data(),
            m_context->m_zstd.m_buffer.size(),
            0
        };

        // Compress the input
        while(zstd_in_buffer.pos < zstd_in_buffer.size)
        {
            check_zstd(
                [&]
                {
                    return ZSTD_compressStream2(
                        m_context->m_zstd.m_context,
                        &zstd_out_buffer,
                        &zstd_in_buffer,
                        m_context->m_asynchronous && m_context->m_best ? ZSTD_e_continue : ZSTD_e_flush
                    );
                });

            if(zstd_out_buffer.pos > 0)
            {
                if(m_context->m_aes)
                {
                    // Adjust buffers size if needed
                    expand(m_context->m_aes->m_buffer, zstd_out_buffer.pos + EVP_MAX_BLOCK_LENGTH);

                    auto aes_out_size = int(m_context->m_aes->m_buffer.size());

                    check_aes(
                        [&]
                        {
                            return EVP_EncryptUpdate(
                                m_context->m_aes->m_context,
                                reinterpret_cast<unsigned char*>(m_context->m_aes->m_buffer.data()),
                                &aes_out_size,
                                reinterpret_cast<const unsigned char*>(m_context->m_zstd.m_buffer.data()),
                                int(zstd_out_buffer.pos)
                            );
                        });

                    m_context->m_stream.write(m_context->m_aes->m_buffer.data(), aes_out_size);
                }
                else
                {
                    m_context->m_stream.write(m_context->m_zstd.m_buffer.data(), std::streamsize(zstd_out_buffer.pos));
                }

                zstd_out_buffer.pos = 0;
            }
        }

        if(!m_context->m_asynchronous)
        {
            m_context->m_stream.flush();
        }

        return _src_size;
    }

private:

    // BEWARE: Boost makes shallow copies of the sink, that's why we use shared_ptr...
    std::shared_ptr<stream_context> m_context;
};

} // namespace sight::core::log::detail
