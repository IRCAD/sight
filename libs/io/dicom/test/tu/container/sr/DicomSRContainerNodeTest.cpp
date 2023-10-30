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

#include "DicomSRContainerNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRContainerNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmDataSet.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::dicom_sr_container_node_test);

#define CONTINUITY 0x0040, 0xa050
#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

void dicom_sr_container_node_test::basic_test()
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    dicom_sr_container_node({}, "friend").write(dataset);
    CPPUNIT_ASSERT_EQUAL("CONTAINER"s, (io::dicom::helper::dicom_data_reader::get_tag_value<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::dicom_data_reader::get_tag_value<RELATIONSHIP>(dataset)));
    CPPUNIT_ASSERT_EQUAL("SEPARATE"s, (io::dicom::helper::dicom_data_reader::get_tag_value<CONTINUITY>(dataset)));
}

} // namespace sight::io::dicom::container::sr::ut
