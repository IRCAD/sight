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

// cspell:ignore ЁЂЃЄЅІЇЈ

#include "EncodingTest.hpp"

#include <io/dicom/helper/Encoding.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::encoding_test);

namespace sight::io::dicom::helper::ut
{

using namespace std::literals::string_literals;

//------------------------------------------------------------------------------

void encoding_test::no_code_extension_test()
{
    static const std::string s_SOURCE = "\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8"; // ISO-8859-1 string
    CPPUNIT_ASSERT_EQUAL("¡¢£¤¥¦§¨"s, encoding::convert_string(s_SOURCE, "ISO_IR 100"));
}

//------------------------------------------------------------------------------

void encoding_test::code_extension_test()
{
    static const std::string s_SOURCE =
        "\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8"  // ISO-8859-1 string
        "\033\x2D\x4C"                      // ISO-8859-5 escape sequence
        "\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8"; // ISO-8859-5 string
    CPPUNIT_ASSERT_EQUAL("¡¢£¤¥¦§¨ЁЂЃЄЅІЇЈ"s, encoding::convert_string(s_SOURCE, "ISO_IR 100\\ISO_IR 144"));
}

} // namespace sight::io::dicom::helper::ut
