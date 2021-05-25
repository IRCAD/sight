/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "CryptoTest.hpp"

#include <core/crypto/AES256.hpp>
#include <core/crypto/Base64.hpp>
#include <core/crypto/SHA256.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::crypto::ut::CryptoTest);

namespace sight::core::crypto
{

namespace ut
{

//------------------------------------------------------------------------------

void CryptoTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CryptoTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CryptoTest::hashTest()
{
    // Test various well known hashes
    // See https://www.di-mgt.com.au/sha_testvectors.html
    const core::crypto::secure_string sha256_empty("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    CPPUNIT_ASSERT_EQUAL(sha256_empty, core::crypto::hash(""));

    const core::crypto::secure_string sha256_abc("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    CPPUNIT_ASSERT_EQUAL(sha256_abc, core::crypto::hash("abc"));

    const core::crypto::secure_string sha256_abcd("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    CPPUNIT_ASSERT_EQUAL(sha256_abcd, core::crypto::hash("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"));
}

//------------------------------------------------------------------------------

void CryptoTest::cryptoTest()
{
    const auto empty_encrypted = core::crypto::encrypt("", "");
    const auto empty_decrypted = core::crypto::decrypt(empty_encrypted, "");
    CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string(""), empty_decrypted);

    const auto abc_encrypted = core::crypto::encrypt("abc", "abc");
    const auto abc_decrypted = core::crypto::decrypt(abc_encrypted, "abc");
    CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), abc_decrypted);
}

//------------------------------------------------------------------------------

void CryptoTest::base64Test()
{
    // Test vector from https://tools.ietf.org/html/rfc4648#section-8
    const core::crypto::secure_string f("f");
    const core::crypto::secure_string fo("fo");
    const core::crypto::secure_string foo("foo");
    const core::crypto::secure_string foob("foob");
    const core::crypto::secure_string fooba("fooba");
    const core::crypto::secure_string foobar("foobar");

    const core::crypto::secure_string base64_f("Zg==");
    const core::crypto::secure_string base64_fo("Zm8=");
    const core::crypto::secure_string base64_foo("Zm9v");
    const core::crypto::secure_string base64_foob("Zm9vYg==");
    const core::crypto::secure_string base64_fooba("Zm9vYmE=");
    const core::crypto::secure_string base64_foobar("Zm9vYmFy");

    // Test encoding
    const auto encoded_f      = core::crypto::to_base64(f);
    const auto encoded_fo     = core::crypto::to_base64(fo);
    const auto encoded_foo    = core::crypto::to_base64(foo);
    const auto encoded_foob   = core::crypto::to_base64(foob);
    const auto encoded_fooba  = core::crypto::to_base64(fooba);
    const auto encoded_foobar = core::crypto::to_base64(foobar);

    CPPUNIT_ASSERT_EQUAL(base64_f, encoded_f);
    CPPUNIT_ASSERT_EQUAL(base64_fo, encoded_fo);
    CPPUNIT_ASSERT_EQUAL(base64_foo, encoded_foo);
    CPPUNIT_ASSERT_EQUAL(base64_foob, encoded_foob);
    CPPUNIT_ASSERT_EQUAL(base64_fooba, encoded_fooba);
    CPPUNIT_ASSERT_EQUAL(base64_foobar, encoded_foobar);

    // Test decoding
    const auto decoded_f      = core::crypto::from_base64(encoded_f);
    const auto decoded_fo     = core::crypto::from_base64(encoded_fo);
    const auto decoded_foo    = core::crypto::from_base64(encoded_foo);
    const auto decoded_foob   = core::crypto::from_base64(encoded_foob);
    const auto decoded_fooba  = core::crypto::from_base64(encoded_fooba);
    const auto decoded_foobar = core::crypto::from_base64(encoded_foobar);

    CPPUNIT_ASSERT_EQUAL(f, decoded_f);
    CPPUNIT_ASSERT_EQUAL(fo, decoded_fo);
    CPPUNIT_ASSERT_EQUAL(foo, decoded_foo);
    CPPUNIT_ASSERT_EQUAL(foob, decoded_foob);
    CPPUNIT_ASSERT_EQUAL(fooba, decoded_fooba);
    CPPUNIT_ASSERT_EQUAL(foobar, decoded_foobar);

    // test binary string
    const core::crypto::secure_string abc("abc");
    const auto encrypted   = core::crypto::encrypt(abc, "abc");
    const auto to_base64   = core::crypto::to_base64(encrypted);
    const auto from_base64 = core::crypto::from_base64(to_base64);
    const auto decrypted   = core::crypto::decrypt(from_base64, "abc");

    CPPUNIT_ASSERT_EQUAL(encrypted, from_base64);
    CPPUNIT_ASSERT_EQUAL(abc, decrypted);
}

} // namespace ut

} // namespace sight::core::memory
