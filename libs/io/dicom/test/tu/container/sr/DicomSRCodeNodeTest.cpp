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

#include "DicomSRCodeNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRCodeNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::DicomSRCodeNodeTest);

#define CODE_NODE 0x0040, 0xa168
#define VALUE 0x0008, 0x0100
#define DESIGNATOR 0x0008, 0x0102
#define VERSION 0x0008, 0x0103
#define MEANING 0x0008, 0x0104
#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

void DicomSRCodeNodeTest::basicTest()
{
    using namespace std::literals::string_literals;
    using DicomDataReader = io::dicom::helper::DicomDataReader;

    gdcm::DataSet dataset;
    DicomSRCodeNode({}, "friend", {"value", "designator", "meaning", "version"}).write(dataset);
    CPPUNIT_ASSERT_EQUAL("CODE"s, (DicomDataReader::getTagValue<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (DicomDataReader::getTagValue<RELATIONSHIP>(dataset)));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
        dataset.GetDataElement(gdcm::Attribute<CODE_NODE>::GetTag()).GetValueAsSQ();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), sequence->GetNumberOfItems());
    gdcm::DataSet& item = sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("value"s, (DicomDataReader::getTagValue<VALUE>(item)));
    CPPUNIT_ASSERT_EQUAL("designator"s, (DicomDataReader::getTagValue<DESIGNATOR>(item)));
    CPPUNIT_ASSERT_EQUAL("meaning"s, (DicomDataReader::getTagValue<MEANING>(item)));
    // TODO: fix
    // CPPUNIT_ASSERT_EQUAL("version"s, (DicomDataReader::getTagValue<VERSION>(item)));

    DicomSRCodeNode({}, "enemy", {"anotherValue", "anotherDesignator", "42", "anotherVersion"}).write(dataset);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), sequence->GetNumberOfItems());
    gdcm::DataSet& firstItem = sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("value"s, (DicomDataReader::getTagValue<VALUE>(firstItem)));
    CPPUNIT_ASSERT_EQUAL("designator"s, (DicomDataReader::getTagValue<DESIGNATOR>(firstItem)));
    CPPUNIT_ASSERT_EQUAL("meaning"s, (DicomDataReader::getTagValue<MEANING>(firstItem)));
    // TODO: fix
    // CPPUNIT_ASSERT_EQUAL("version"s, (DicomDataReader::getTagValue<VERSION>(firstItem)));
    gdcm::DataSet& secondItem = sequence->GetItem(2).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("anotherValue"s, (DicomDataReader::getTagValue<VALUE>(secondItem)));
    CPPUNIT_ASSERT_EQUAL("anotherDesignator"s, (DicomDataReader::getTagValue<DESIGNATOR>(secondItem)));
    CPPUNIT_ASSERT_EQUAL("42"s, (DicomDataReader::getTagValue<MEANING>(secondItem)));
    // TODO: fix
    // CPPUNIT_ASSERT_EQUAL("anotherVersion"s, (DicomDataReader::getTagValue<VERSION>(secondItem)));
}

} // namespace sight::io::dicom::container::sr::ut
