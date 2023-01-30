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

static void genericTest(const std::string& graphicType, std::size_t graphicSize)
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    std::vector<float> graphicBuffer(graphicSize);
    std::iota(graphicBuffer.begin(), graphicBuffer.end(), 0.F);
    DicomSRSCoord3DNode({}, "friend", graphicType, graphicBuffer, "reference").write(dataset);
    CPPUNIT_ASSERT_EQUAL("SCOORD3D"s, (io::dicom::helper::DicomDataReader::getTagValue<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::DicomDataReader::getTagValue<RELATIONSHIP>(dataset)));
    CPPUNIT_ASSERT_EQUAL("reference"s, (io::dicom::helper::DicomDataReader::getTagValue<REFERENCE>(dataset)));
    CPPUNIT_ASSERT_EQUAL(graphicType, (io::dicom::helper::DicomDataReader::getTagValue<GRAPHIC_TYPE>(dataset)));
    gdcm::Attribute<GRAPHIC_DATA> graphicData;
    graphicData.SetFromDataSet(dataset);
    for(std::size_t i = 0 ; i < graphicSize ; i++)
    {
        CPPUNIT_ASSERT_EQUAL(float(i), graphicData[unsigned(i)]);
    }
}

//------------------------------------------------------------------------------

void DicomSRSCoord3DNodeTest::pointTest()
{
    genericTest("POINT", 3);
}

//------------------------------------------------------------------------------

void DicomSRSCoord3DNodeTest::polylineTest()
{
    genericTest("POLYLINE", 6);
}

} // namespace sight::io::dicom::container::sr::ut
