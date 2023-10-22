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

#include "DicomSRNumNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRNumNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::DicomSRNumNodeTest);

#define VALUE 0x0008, 0x0100
#define DESIGNATOR 0x0008, 0x0102
#define VERSION 0x0008, 0x0103
#define MEANING 0x0008, 0x0104
#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010
#define NUM_VALUE 0x0040, 0xa30a
#define NUM_NODE 0x0040, 0xa300
#define UNITS 0x0040, 0x08ea

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

void DicomSRNumNodeTest::basicTest()
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    DicomSRNumNode({}, "friend", 42, {"value", "designator", "meaning", "version"}).write(dataset);
    CPPUNIT_ASSERT_EQUAL("NUM"s, (io::dicom::helper::DicomDataReader::getTagValue<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::DicomDataReader::getTagValue<RELATIONSHIP>(dataset)));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
        dataset.GetDataElement(gdcm::Attribute<NUM_NODE>::GetTag()).GetValueAsSQ();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), sequence->GetNumberOfItems());
    gdcm::DataSet& item = sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL(42., (io::dicom::helper::DicomDataReader::getTagValue<NUM_VALUE, double>(item)));
    gdcm::SmartPointer<gdcm::SequenceOfItems> units_sequence =
        item.GetDataElement(gdcm::Attribute<UNITS>::GetTag()).GetValueAsSQ();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), units_sequence->GetNumberOfItems());
    gdcm::DataSet& units_item = units_sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("value"s, (io::dicom::helper::DicomDataReader::getTagValue<VALUE>(units_item)));
    CPPUNIT_ASSERT_EQUAL("designator"s, (io::dicom::helper::DicomDataReader::getTagValue<DESIGNATOR>(units_item)));
    CPPUNIT_ASSERT_EQUAL("meaning"s, (io::dicom::helper::DicomDataReader::getTagValue<MEANING>(units_item)));
    // TODO: fix
    // CPPUNIT_ASSERT_EQUAL("version"s, (io::dicom::helper::DicomDataReader::getTagValue<VERSION>(unitsItem)));
}

} // namespace sight::io::dicom::container::sr::ut
