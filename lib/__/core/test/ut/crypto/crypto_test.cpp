/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <core/crypto/aes256.hpp>
#include <core/crypto/base64.hpp>
#include <core/crypto/sha256.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::crypto::secure_string")
{
// cspell: disable
//------------------------------------------------------------------------------

    TEST_CASE("hash")
    {
        // Test various well known hashes
        // See https://www.di-mgt.com.au/sha_testvectors.html
        const sight::core::crypto::secure_string sha256_empty(
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
        CHECK_EQ(sight::core::crypto::hash(""), sha256_empty);

        const sight::core::crypto::secure_string sha256_abc(
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        CHECK_EQ(sight::core::crypto::hash("abc"), sha256_abc);

        const sight::core::crypto::secure_string sha256_abcd(
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
        CHECK_EQ(sight::core::crypto::hash("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"), sha256_abcd);
    }

//------------------------------------------------------------------------------

    TEST_CASE("crypt")
    {
        const auto empty_encrypted = sight::core::crypto::encrypt("", "");
        const auto empty_decrypted = sight::core::crypto::decrypt(empty_encrypted, "");
        CHECK_EQ(empty_decrypted, sight::core::crypto::secure_string(""));

        const auto abc_encrypted = sight::core::crypto::encrypt("abc", "abc");
        const auto abc_decrypted = sight::core::crypto::decrypt(abc_encrypted, "abc");
        CHECK_EQ(abc_decrypted, sight::core::crypto::secure_string("abc"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("base64test")
    {
        // Test vector from https://tools.ietf.org/html/rfc4648#section-8
        const sight::core::crypto::secure_string f("f");
        const sight::core::crypto::secure_string fo("fo");
        const sight::core::crypto::secure_string foo("foo");
        const sight::core::crypto::secure_string foob("foob");
        const sight::core::crypto::secure_string fooba("fooba");
        const sight::core::crypto::secure_string foobar("foobar");

        const sight::core::crypto::secure_string base64_f("Zg==");
        const sight::core::crypto::secure_string base64_fo("Zm8=");
        const sight::core::crypto::secure_string base64_foo("Zm9v");
        const sight::core::crypto::secure_string base64_foob("Zm9vYg==");
        const sight::core::crypto::secure_string base64_fooba("Zm9vYmE=");
        const sight::core::crypto::secure_string base64_foobar("Zm9vYmFy");

        // Test encoding
        const auto encoded_f      = sight::core::crypto::to_base64(f);
        const auto encoded_fo     = sight::core::crypto::to_base64(fo);
        const auto encoded_foo    = sight::core::crypto::to_base64(foo);
        const auto encoded_foob   = sight::core::crypto::to_base64(foob);
        const auto encoded_fooba  = sight::core::crypto::to_base64(fooba);
        const auto encoded_foobar = sight::core::crypto::to_base64(foobar);

        CHECK_EQ(encoded_f, base64_f);
        CHECK_EQ(encoded_fo, base64_fo);
        CHECK_EQ(encoded_foo, base64_foo);
        CHECK_EQ(encoded_foob, base64_foob);
        CHECK_EQ(encoded_fooba, base64_fooba);
        CHECK_EQ(encoded_foobar, base64_foobar);

        // Test decoding
        const auto decoded_f      = sight::core::crypto::from_base64(encoded_f);
        const auto decoded_fo     = sight::core::crypto::from_base64(encoded_fo);
        const auto decoded_foo    = sight::core::crypto::from_base64(encoded_foo);
        const auto decoded_foob   = sight::core::crypto::from_base64(encoded_foob);
        const auto decoded_fooba  = sight::core::crypto::from_base64(encoded_fooba);
        const auto decoded_foobar = sight::core::crypto::from_base64(encoded_foobar);

        CHECK_EQ(decoded_f, f);
        CHECK_EQ(decoded_fo, fo);
        CHECK_EQ(decoded_foo, foo);
        CHECK_EQ(decoded_foob, foob);
        CHECK_EQ(decoded_fooba, fooba);
        CHECK_EQ(decoded_foobar, foobar);

        // test binary string
        const sight::core::crypto::secure_string abc("abc");
        const auto encrypted   = sight::core::crypto::encrypt(abc, "abc");
        const auto to_base64   = sight::core::crypto::to_base64(encrypted);
        const auto from_base64 = sight::core::crypto::from_base64(to_base64);
        const auto decrypted   = sight::core::crypto::decrypt(from_base64, "abc");

        CHECK_EQ(from_base64, encrypted);
        CHECK_EQ(decrypted, abc);
    }

// cspell: enable
} // TEST_SUITE
