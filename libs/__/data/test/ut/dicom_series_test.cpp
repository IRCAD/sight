/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "dicom_series_test.hpp"

#include <core/os/temp_path.hpp>
#include <core/type.hpp>

#include <data/array.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::dicom_series_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void dicom_series_test::setUp()
{
    // Set up context before running a test.
    m_series = std::make_shared<data::dicom_series>();
}

//------------------------------------------------------------------------------

void dicom_series_test::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void dicom_series_test::dicom_test()
{
    CPPUNIT_ASSERT(m_series);

    //Create Path
    core::os::temp_file tmp_file;
    std::ofstream file;
    file.open(tmp_file, std::ofstream::out);
    file << "42";
    file.close();

    //None
    const std::size_t nb_instances = 100;
    CPPUNIT_ASSERT(!m_series->is_instance_available(42));
    m_series->set_number_of_instances(nb_instances);
    CPPUNIT_ASSERT_EQUAL(nb_instances, m_series->num_instances());

    //Paths
    m_series->add_dicom_path(42, tmp_file);
    CPPUNIT_ASSERT(m_series->is_instance_available(42));

    //Binaries
    core::memory::buffer_object::sptr buffer_obj = std::make_shared<core::memory::buffer_object>();
    m_series->add_binary(1664, buffer_obj);
    CPPUNIT_ASSERT_EQUAL(buffer_obj, m_series->get_dicom_container().at(1664));
}

//------------------------------------------------------------------------------

void dicom_series_test::equality_test()
{
    auto dicom_series1 = std::make_shared<data::dicom_series>();
    auto dicom_series2 = std::make_shared<data::dicom_series>();

    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->set_number_of_instances(666);
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->set_number_of_instances(dicom_series1->num_instances());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->set_sop_class_ui_ds({"1", "2", "3"});
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->set_sop_class_ui_ds(dicom_series1->get_sop_class_ui_ds());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->set_computed_tag_values({{"4", "4"}, {"5", "5"}, {"6", "6"}});
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->set_computed_tag_values(dicom_series1->get_computed_tag_values());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    auto buffer_object = std::make_shared<core::memory::buffer_object>();
    buffer_object->allocate(2);
    auto lock    = buffer_object->lock();
    auto* buffer = reinterpret_cast<char*>(lock.buffer());
    buffer[0] = 'a';
    buffer[1] = '\0';

    dicom_series1->set_dicom_container({{7, buffer_object}});
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->set_dicom_container(dicom_series1->get_dicom_container());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    dicom_series1->set_first_instance_number(1);
    CPPUNIT_ASSERT(*dicom_series1 != *dicom_series2 && !(*dicom_series1 == *dicom_series2));
    dicom_series2->set_first_instance_number(dicom_series1->get_first_instance_number());
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series2 && !(*dicom_series1 != *dicom_series2));

    // Test also deepcopy, just for fun
    auto dicom_series3 = std::make_shared<data::dicom_series>();
    dicom_series3->deep_copy(dicom_series1);
    CPPUNIT_ASSERT(*dicom_series1 == *dicom_series3 && !(*dicom_series1 != *dicom_series3));
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
