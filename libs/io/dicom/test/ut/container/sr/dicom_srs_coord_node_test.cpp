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

#include "dicom_srs_coord_node_test.hpp"

#include <io/dicom/container/sr/dicom_srs_coord_node.hpp>
#include <io/dicom/helper/dicom_data_reader.hxx>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>

#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::dicom_srs_coord_node_test);

#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010
#define GRAPHIC_TYPE 0x0070, 0x0023
#define GRAPHIC_DATA 0x0070, 0x0022

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

static void generic_test(const std::string& _graphic_type, std::size_t _graphic_size)
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    std::vector<float> graphic_buffer(_graphic_size);
    std::iota(graphic_buffer.begin(), graphic_buffer.end(), 0.F);
    dicom_srs_coord_node({}, "friend", _graphic_type, graphic_buffer).write(dataset);
    CPPUNIT_ASSERT_EQUAL("SCOORD"s, (io::dicom::helper::dicom_data_reader::get_tag_value<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::dicom_data_reader::get_tag_value<RELATIONSHIP>(dataset)));
    CPPUNIT_ASSERT_EQUAL(_graphic_type, (io::dicom::helper::dicom_data_reader::get_tag_value<GRAPHIC_TYPE>(dataset)));
    gdcm::Attribute<GRAPHIC_DATA> graphic_data;
    graphic_data.SetFromDataSet(dataset);
    for(std::size_t i = 0 ; i < _graphic_size ; i++)
    {
        CPPUNIT_ASSERT_EQUAL(float(i), graphic_data[unsigned(i)]);
    }
}

//------------------------------------------------------------------------------

void dicom_srs_coord_node_test::point_test()
{
    generic_test("POINT", 2);
}

//------------------------------------------------------------------------------

void dicom_srs_coord_node_test::polyline_test()
{
    generic_test("POLYLINE", 4);
}

} // namespace sight::io::dicom::container::sr::ut
