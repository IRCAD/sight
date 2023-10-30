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

#pragma once

#include <io/__/service/writer.hpp>

#include <service/op.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <filesystem>

namespace sight::io::ut
{

// Defines a Writer class, with a dummy open_location_dialog implementation
class s_test_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(s_test_writer, sight::io::service::writer);

    s_test_writer(sight::io::service::path_type_t _pt) :
        m_path_type(_pt)
    {
    }

    //------------------------------------------------------------------------------

    void set_path_type(sight::io::service::path_type_t _pt)
    {
        m_path_type = _pt;
    }

protected:

    //------------------------------------------------------------------------------

    sight::io::service::path_type_t get_path_type() const override
    {
        return m_path_type;
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

    //------------------------------------------------------------------------------

    void open_location_dialog() override
    {
    }

private:

    io::service::path_type_t m_path_type {io::service::path_type_t::file};
};

/**
 * @brief Test many methods to write data.
 */
class writer_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(writer_test);
CPPUNIT_TEST(test_type_ok);
CPPUNIT_TEST(test_has_location_defined);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    // Tests the correct-ness of types
    void test_type_ok();

    // Test the has_location_defined method
    void test_has_location_defined();

    // Test the has_location_defined method with a base folder
    void test_has_location_defined_with_base_folder();

    // Test the constructed output paths
    void test_output_paths();

protected:

    // Store test values for files/folders
    std::string m_file {"test.png"};
    std::string m_folder {"test"};
    std::filesystem::path m_root_path;
};

} // namespace sight::io::ut
