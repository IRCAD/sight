/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include "aes256.hpp"

#include "sha256.hpp"

#include <openssl/err.h>
#include <openssl/evp.h>

#include <array>
#include <cstring>

namespace sight::core::crypto
{

//------------------------------------------------------------------------------

static constexpr auto check_aes(const auto& _expr)
{
    const auto result = _expr();
    if(result == 0)
    {
        throw std::runtime_error(std::string(ERR_error_string(ERR_get_error(), nullptr)));
    }

    return result;
}

// Load the necessary cipher
static struct openssl_initializer final
{
    openssl_initializer()
    {
        ERR_load_crypto_strings();
        check_aes([&]{return EVP_add_cipher(EVP_aes_256_cbc());});
    }

    ~openssl_initializer()
    {
        ERR_free_strings();
        EVP_cleanup();
    }
} s_openssl_initializer;

// en/decrypt a message using the given password.
// Uses function pointers to minimize code duplication
template<typename T>
inline static T xx_crypt(
    decltype(&::EVP_DecryptInit_ex) _xx_crypt_init,
    decltype(&::EVP_DecryptUpdate) _xx_crypt_update,
    decltype(&::EVP_DecryptFinal_ex) _xx_crypt_final,
    const T& _message,
    const secure_string& _password
)
{
    static_assert(
        std::is_same_v<secure_string, T>|| std::is_same_v<std::string, T>,
        "T must be a string"
    );

    // Initialize key (265 bits) and iv(128 bits)
    std::array<unsigned char, HASH_SIZE> key {};
    std::array<unsigned char, EVP_MAX_IV_LENGTH> iv = {0};

    // Compute password hash and randomize initialization vector
    hash(_password, key);

    // Initialize the output with size of the original message expanded to block size
    T output;
    output.resize(_message.size() + iv.size());

    int processed_size = static_cast<int>(output.size());

    // Initialize context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    // Initialize en/decryption
    check_aes([&]{return _xx_crypt_init(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());});

    // Update en/decryption
    check_aes(
        [&]
        {
            return _xx_crypt_update(
                ctx,
                reinterpret_cast<unsigned char*>(output.data()),
                &processed_size,
                reinterpret_cast<const unsigned char*>(_message.data()),
                static_cast<int>(_message.size())
            );
        });

    // Compute remaining size
    int remaining_size = static_cast<int>(output.size()) - processed_size;

    // Finalize en/decryption
    check_aes(
        [&]
        {
            return _xx_crypt_final(
                ctx,
                reinterpret_cast<unsigned char*>(output.data()) + processed_size,
                &remaining_size
            );
        });

    // Adjust output to the real size
    output.resize(static_cast<std::size_t>(processed_size) + static_cast<std::size_t>(remaining_size));
    output.shrink_to_fit();

    // Cleaning
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(key.data(), key.size());
    OPENSSL_cleanse(iv.data(), iv.size());

    return output;
}

//------------------------------------------------------------------------------

std::string decrypt(const std::string& _message, const secure_string& _password)
{
    return xx_crypt<std::string>(
        &::EVP_DecryptInit_ex,
        &::EVP_DecryptUpdate,
        &::EVP_DecryptFinal_ex,
        _message,
        _password
    );
}

//------------------------------------------------------------------------------

secure_string decrypt(const secure_string& _message, const secure_string& _password)
{
    return xx_crypt<secure_string>(
        &::EVP_DecryptInit_ex,
        &::EVP_DecryptUpdate,
        &::EVP_DecryptFinal_ex,
        _message,
        _password
    );
}

//------------------------------------------------------------------------------

secure_string decrypt(const char* const _message, const secure_string& _password)
{
    return decrypt(secure_string(_message), _password);
}

//------------------------------------------------------------------------------

std::string encrypt(const std::string& _message, const secure_string& _password)
{
    return xx_crypt<std::string>(
        &::EVP_EncryptInit_ex,
        &::EVP_EncryptUpdate,
        &::EVP_EncryptFinal_ex,
        _message,
        _password
    );
}

//------------------------------------------------------------------------------

secure_string encrypt(const secure_string& _message, const secure_string& _password)
{
    return xx_crypt<secure_string>(
        &::EVP_EncryptInit_ex,
        &::EVP_EncryptUpdate,
        &::EVP_EncryptFinal_ex,
        _message,
        _password
    );
}

//------------------------------------------------------------------------------

secure_string encrypt(const char* const _message, const secure_string& _password)
{
    return encrypt(secure_string(_message), _password);
}

} // namespace sight::core::crypto
