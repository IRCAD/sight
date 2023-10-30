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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::dicom::helper::ut
{

class dicom_coded_attribute_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(dicom_coded_attribute_test);
CPPUNIT_TEST(convert_entry_to_coded_attribute_test);
CPPUNIT_TEST(convert_entry_to_gdcm_coded_attribute_test);
CPPUNIT_TEST(check_and_format_entry_with_wrong_value_test);
CPPUNIT_TEST(check_and_format_entry_with_single_value_test);
CPPUNIT_TEST(check_and_format_entry_with_multiple_values_test);
CPPUNIT_TEST_SUITE_END();

public:

    static void convert_entry_to_coded_attribute_test();
    static void convert_entry_to_gdcm_coded_attribute_test();
    static void check_and_format_entry_with_wrong_value_test();
    static void check_and_format_entry_with_single_value_test();
    static void check_and_format_entry_with_multiple_values_test();
};

} // namespace sight::io::dicom::helper::ut
