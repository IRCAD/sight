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

#pragma once

#include <io/base/service/IWriter.hpp>

#include <service/base.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <filesystem>

namespace sight::io::base
{

namespace ut
{

// Defines a Writer class, with a dummy openLocationDialog implementation
class STestWriter : public sight::io::base::service::IWriter
{
public:

    SIGHT_DECLARE_SERVICE(STestWriter, sight::io::base::service::IWriter);

    //------------------------------------------------------------------------------

    void setPathType(sight::io::base::service::IOPathType pt)
    {
        m_pathType = pt;
    }

protected:

    //------------------------------------------------------------------------------

    sight::io::base::service::IOPathType getIOPathType() const override
    {
        return m_pathType;
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

    void openLocationDialog() override
    {
    }

private:

    io::base::service::IOPathType m_pathType;
};

/**
 * @brief Test many methods to write data.
 */
class WriterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(WriterTest);
CPPUNIT_TEST(test_typeOK);
CPPUNIT_TEST(test_hasLocationDefined);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    // Tests the correct-ness of types
    void test_typeOK();

    // Test the hasLocationDefined method
    void test_hasLocationDefined();

    // Test the hasLocationDefined method with a base folder
    void test_hasLocationDefinedWithBaseFolder();

    // Test the constructed output paths
    void test_outputPaths();

protected:

    // Store test values for files/folders
    std::string m_file {"test.png"};
    std::string m_folder {"test"};
    std::filesystem::path m_rootPath;
};

} //namespace ut

} //namespace sight::io::base
