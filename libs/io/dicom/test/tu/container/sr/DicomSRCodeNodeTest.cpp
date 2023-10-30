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

#include "DicomSRCodeNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRCodeNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::dicom_sr_code_node_test);

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

void dicom_sr_code_node_test::basic_test()
{
    using namespace std::literals::string_literals;
    using dicom_data_reader = io::dicom::helper::dicom_data_reader;

    gdcm::DataSet dataset;
    dicom_sr_code_node({}, "friend", {"value", "designator", "meaning", "version"}).write(dataset);
    CPPUNIT_ASSERT_EQUAL("CODE"s, (dicom_data_reader::get_tag_value<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (dicom_data_reader::get_tag_value<RELATIONSHIP>(dataset)));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
        dataset.GetDataElement(gdcm::Attribute<CODE_NODE>::GetTag()).GetValueAsSQ();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), sequence->GetNumberOfItems());
    gdcm::DataSet& item = sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("value"s, (dicom_data_reader::get_tag_value<VALUE>(item)));
    CPPUNIT_ASSERT_EQUAL("designator"s, (dicom_data_reader::get_tag_value<DESIGNATOR>(item)));
    CPPUNIT_ASSERT_EQUAL("meaning"s, (dicom_data_reader::get_tag_value<MEANING>(item)));
    // TODO: fix
    // CPPUNIT_ASSERT_EQUAL("version"s, (dicom_data_reader::getTagValue<VERSION>(item)));

    dicom_sr_code_node({}, "enemy", {"anotherValue", "anotherDesignator", "42", "anotherVersion"}).write(dataset);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), sequence->GetNumberOfItems());
    gdcm::DataSet& first_item = sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("value"s, (dicom_data_reader::get_tag_value<VALUE>(first_item)));
    CPPUNIT_ASSERT_EQUAL("designator"s, (dicom_data_reader::get_tag_value<DESIGNATOR>(first_item)));
    CPPUNIT_ASSERT_EQUAL("meaning"s, (dicom_data_reader::get_tag_value<MEANING>(first_item)));
    // TODO: fix
    // CPPUNIT_ASSERT_EQUAL("version"s, (dicom_data_reader::getTagValue<VERSION>(firstItem)));
    gdcm::DataSet& second_item = sequence->GetItem(2).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("anotherValue"s, (dicom_data_reader::get_tag_value<VALUE>(second_item)));
    CPPUNIT_ASSERT_EQUAL("anotherDesignator"s, (dicom_data_reader::get_tag_value<DESIGNATOR>(second_item)));
    CPPUNIT_ASSERT_EQUAL("42"s, (dicom_data_reader::get_tag_value<MEANING>(second_item)));
    // TODO: fix
    // CPPUNIT_ASSERT_EQUAL("anotherVersion"s, (dicom_data_reader::getTagValue<VERSION>(secondItem)));
}

} // namespace sight::io::dicom::container::sr::ut
