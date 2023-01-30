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

#include "DicomSRImageNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRImageNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::DicomSRImageNodeTest);

#define IMAGE_NODE 0x0008, 0x1199
#define CLASS 0x0008, 0x1150
#define INSTANCE 0x0008, 0x1155
#define FRAME 0x0008, 0x1160
#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

void DicomSRImageNodeTest::basicTest()
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    DicomSRImageNode({}, "friend", "class", "instance", 42).write(dataset);
    CPPUNIT_ASSERT_EQUAL("IMAGE"s, (io::dicom::helper::DicomDataReader::getTagValue<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::DicomDataReader::getTagValue<RELATIONSHIP>(dataset)));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
        dataset.GetDataElement(gdcm::Attribute<IMAGE_NODE>::GetTag()).GetValueAsSQ();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), sequence->GetNumberOfItems());
    gdcm::DataSet& item = sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("class"s, (io::dicom::helper::DicomDataReader::getTagValue<CLASS>(item)));
    CPPUNIT_ASSERT_EQUAL("instance"s, (io::dicom::helper::DicomDataReader::getTagValue<INSTANCE>(item)));
    CPPUNIT_ASSERT_EQUAL(42, (io::dicom::helper::DicomDataReader::getTagValue<FRAME, int>(item)));
}

} // namespace sight::io::dicom::container::sr::ut
