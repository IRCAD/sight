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

#include "tags_test.hpp"

#include <io/dicom/exception/invalid_tag.hpp>
#include <io/dicom/helper/tags.hpp>

#include <gdcmTag.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::helper::ut::tags_test);

namespace sight::io::dicom::helper::ut
{

//------------------------------------------------------------------------------

void tags_test::get_gdcm_tag_nominal_test()
{
    CPPUNIT_ASSERT_EQUAL((gdcm::Tag {0x10, 0x20}), helper::get_gdcm_tag("10", "20"));
}

//------------------------------------------------------------------------------

void tags_test::get_gdcm_tag_not_a_number_test()
{
    CPPUNIT_ASSERT_THROW(helper::get_gdcm_tag("group", "element"), io::dicom::exception::invalid_tag);
}

//------------------------------------------------------------------------------

void tags_test::get_gdcm_tag_too_big_test()
{
    CPPUNIT_ASSERT_THROW(helper::get_gdcm_tag("100000", "20"), io::dicom::exception::invalid_tag);
    CPPUNIT_ASSERT_THROW(
        helper::get_gdcm_tag("10", std::to_string(std::numeric_limits<std::uint64_t>::max())),
        io::dicom::exception::invalid_tag
    );
}

} // namespace sight::io::dicom::helper::ut
