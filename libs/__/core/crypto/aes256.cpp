/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "core/crypto/sha256.hpp"

#include <openssl/evp.h>

#include <array>
#include <cstring>

namespace sight::core::crypto
{

// en/decrypt a message using the given password.
// Uses function pointers to minimize code duplication
template<typename T>
inline static T xxCrypt(
    decltype(&::EVP_DecryptInit_ex) xxCryptInit,
    decltype(&::EVP_DecryptUpdate) xxCryptUpdate,
    decltype(&::EVP_DecryptFinal_ex) xxCryptFinal,
    const T& message,
    const secure_string& password
)
{
    static_assert(
        std::is_same_v<secure_string, T>|| std::is_same_v<std::string, T>,
        "T must be a string"
    );

    // Load the necessary cipher
    EVP_add_cipher(EVP_aes_256_cbc());

    // Initialize key (265 bits) and iv(128 bits)
    std::array<unsigned char, HASH_SIZE> key {};
    std::array<unsigned char, EVP_MAX_IV_LENGTH> iv = {0};

    // Compute password hash and randomize initialization vector
    hash(password, key);

    // Initialize the output with size of the original message expanded to block size
    T output;
    output.resize(message.size() + iv.size());

    int processed_size = static_cast<int>(output.size());

    // Initialize context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    // Initialize en/decryption
    xxCryptInit(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());

    // Update en/decryption
    xxCryptUpdate(
        ctx,
        reinterpret_cast<unsigned char*>(output.data()),
        &processed_size,
        reinterpret_cast<const unsigned char*>(message.data()),
        static_cast<int>(message.size())
    );

    // Compute remaining size
    int remaining_size = static_cast<int>(output.size()) - processed_size;

    // Finalize en/decryption
    xxCryptFinal(
        ctx,
        reinterpret_cast<unsigned char*>(output.data()) + processed_size,
        &remaining_size
    );

    // Adjust output to the real size
    output.resize(static_cast<std::size_t>(processed_size) + static_cast<std::size_t>(remaining_size));

    // Cleaning
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(key.data(), key.size());
    OPENSSL_cleanse(iv.data(), iv.size());

    return output;
}

//------------------------------------------------------------------------------

std::string decrypt(const std::string& message, const secure_string& password)
{
    return xxCrypt<std::string>(
        &::EVP_DecryptInit_ex,
        &::EVP_DecryptUpdate,
        &::EVP_DecryptFinal_ex,
        message,
        password
    );
}

//------------------------------------------------------------------------------

secure_string decrypt(const secure_string& message, const secure_string& password)
{
    return xxCrypt<secure_string>(
        &::EVP_DecryptInit_ex,
        &::EVP_DecryptUpdate,
        &::EVP_DecryptFinal_ex,
        message,
        password
    );
}

//------------------------------------------------------------------------------

secure_string decrypt(const char* const message, const secure_string& password)
{
    return decrypt(secure_string(message), password);
}

//------------------------------------------------------------------------------

std::string encrypt(const std::string& message, const secure_string& password)
{
    return xxCrypt<std::string>(
        &::EVP_EncryptInit_ex,
        &::EVP_EncryptUpdate,
        &::EVP_EncryptFinal_ex,
        message,
        password
    );
}

//------------------------------------------------------------------------------

secure_string encrypt(const secure_string& message, const secure_string& password)
{
    return xxCrypt<secure_string>(
        &::EVP_EncryptInit_ex,
        &::EVP_EncryptUpdate,
        &::EVP_EncryptFinal_ex,
        message,
        password
    );
}

//------------------------------------------------------------------------------

secure_string encrypt(const char* const message, const secure_string& password)
{
    return encrypt(secure_string(message), password);
}

} // namespace sight::core::crypto
