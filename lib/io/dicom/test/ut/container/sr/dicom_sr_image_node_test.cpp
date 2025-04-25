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

#include "dicom_sr_image_node_test.hpp"

#include <io/dicom/container/sr/dicom_sr_image_node.hpp>
#include <io/dicom/helper/dicom_data_reader.hxx>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmSequenceOfItems.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::dicom_sr_image_node_test);

#define IMAGE_NODE 0x0008, 0x1199
#define CLASS 0x0008, 0x1150
#define INSTANCE 0x0008, 0x1155
#define FRAME 0x0008, 0x1160
#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

void dicom_sr_image_node_test::basic_test()
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    dicom_sr_image_node({}, "friend", "class", "instance", 42).write(dataset);
    CPPUNIT_ASSERT_EQUAL("IMAGE"s, (io::dicom::helper::dicom_data_reader::get_tag_value<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::dicom_data_reader::get_tag_value<RELATIONSHIP>(dataset)));
    gdcm::SmartPointer<gdcm::SequenceOfItems> sequence =
        dataset.GetDataElement(gdcm::Attribute<IMAGE_NODE>::GetTag()).GetValueAsSQ();
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), sequence->GetNumberOfItems());
    gdcm::DataSet& item = sequence->GetItem(1).GetNestedDataSet();
    CPPUNIT_ASSERT_EQUAL("class"s, (io::dicom::helper::dicom_data_reader::get_tag_value<CLASS>(item)));
    CPPUNIT_ASSERT_EQUAL("instance"s, (io::dicom::helper::dicom_data_reader::get_tag_value<INSTANCE>(item)));
    CPPUNIT_ASSERT_EQUAL(42, (io::dicom::helper::dicom_data_reader::get_tag_value<FRAME, int>(item)));
}

} // namespace sight::io::dicom::container::sr::ut
