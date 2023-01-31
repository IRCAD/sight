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

// cspell:ignore ЁЂЃЄЅІЇЈ

#include "EncodingTest.hpp"

#include <io/dicom/helper/Encoding.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::EncodingTest);

namespace sight::io::dicom::helper::ut
{

using namespace std::literals::string_literals;

//------------------------------------------------------------------------------

void EncodingTest::noCodeExtensionTest()
{
    static const std::string source = "\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8"; // ISO-8859-1 string
    CPPUNIT_ASSERT_EQUAL("¡¢£¤¥¦§¨"s, Encoding::convertString(source, "ISO_IR 100"));
}

//------------------------------------------------------------------------------

void EncodingTest::codeExtensionTest()
{
    static const std::string source =
        "\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8"  // ISO-8859-1 string
        "\033\x2D\x4C"                      // ISO-8859-5 escape sequence
        "\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8"; // ISO-8859-5 string
    CPPUNIT_ASSERT_EQUAL("¡¢£¤¥¦§¨ЁЂЃЄЅІЇЈ"s, Encoding::convertString(source, "ISO_IR 100\\ISO_IR 144"));
}

} // namespace sight::io::dicom::helper::ut
