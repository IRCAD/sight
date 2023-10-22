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

#include "DicomSRSCoord3DNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRSCoord3DNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>

#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::DicomSRSCoord3DNodeTest);

#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010
#define REFERENCE 0x3006, 0x0024
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
    DicomSRSCoord3DNode({}, "friend", _graphic_type, graphic_buffer, "reference").write(dataset);
    CPPUNIT_ASSERT_EQUAL("SCOORD3D"s, (io::dicom::helper::DicomDataReader::getTagValue<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::DicomDataReader::getTagValue<RELATIONSHIP>(dataset)));
    CPPUNIT_ASSERT_EQUAL("reference"s, (io::dicom::helper::DicomDataReader::getTagValue<REFERENCE>(dataset)));
    CPPUNIT_ASSERT_EQUAL(_graphic_type, (io::dicom::helper::DicomDataReader::getTagValue<GRAPHIC_TYPE>(dataset)));
    gdcm::Attribute<GRAPHIC_DATA> graphic_data;
    graphic_data.SetFromDataSet(dataset);
    for(std::size_t i = 0 ; i < _graphic_size ; i++)
    {
        CPPUNIT_ASSERT_EQUAL(float(i), graphic_data[unsigned(i)]);
    }
}

//------------------------------------------------------------------------------

void DicomSRSCoord3DNodeTest::pointTest()
{
    generic_test("POINT", 3);
}

//------------------------------------------------------------------------------

void DicomSRSCoord3DNodeTest::polylineTest()
{
    generic_test("POLYLINE", 6);
}

} // namespace sight::io::dicom::container::sr::ut
