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

#include "DownloadTest.hpp"

#include "io/http/Download.hpp"

#include <core/Exception.hpp>
#include <core/tools/System.hpp>

#include <filesystem>

namespace fs = std::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::http::ut::DownloadTest);

namespace sight::io::http::ut
{

//------------------------------------------------------------------------------

void DownloadTest::setUp()
{
}

//------------------------------------------------------------------------------

void DownloadTest::tearDown()
{
}

//------------------------------------------------------------------------------

void DownloadTest::downloadTestSuccess()
{
    const fs::path dir = core::tools::System::getTemporaryFolder();

    CPPUNIT_ASSERT_NO_THROW(
        io::http::downloadFile(
            "https://cloud.ircad.fr/index.php/s/xGt4afRfMiWyeo4/download",
            dir / "sample.bin"
        )
    );

    CPPUNIT_ASSERT_NO_THROW(
        io::http::downloadFile(
            "https://cloud.ircad.fr/index.php/s/M2CESbGgH3NPoDo/download",
            dir / "sample.txt"
        )
    );

    CPPUNIT_ASSERT(fs::exists(dir / "sample.bin"));
    CPPUNIT_ASSERT(fs::exists(dir / "sample.txt"));

    CPPUNIT_ASSERT_EQUAL(std::uintmax_t(1024), fs::file_size(dir / "sample.bin"));
    CPPUNIT_ASSERT_EQUAL(std::uintmax_t(54), fs::file_size(dir / "sample.txt"));
}

//------------------------------------------------------------------------------

void DownloadTest::downloadTestFailure()
{
    const fs::path dir = core::tools::System::getTemporaryFolder();

    CPPUNIT_ASSERT_THROW(
        io::http::downloadFile(
            "https://whateverTheCloud.com/fileDoesNotExists.txt/download",
            dir / "test.bin"
        ),
        sight::core::Exception
    );

    CPPUNIT_ASSERT(!fs::exists(dir / "test.bin"));
}

//------------------------------------------------------------------------------

void DownloadTest::downloadTestWrongInputs()
{
    const fs::path dir = core::tools::System::getTemporaryFolder();

    CPPUNIT_ASSERT_THROW(io::http::downloadFile("", dir / "test2.bin"), sight::core::Exception);

    CPPUNIT_ASSERT_THROW(
        io::http::downloadFile("https://whateverTheCloud.com/fileDoesNotExists.txt/download", ""),
        sight::core::Exception
    );
}

//------------------------------------------------------------------------------

void DownloadTest::downloadTestOverwritesFile()
{
    const fs::path dir = core::tools::System::getTemporaryFolder();

    CPPUNIT_ASSERT(fs::exists(dir / "sample.bin"));

    CPPUNIT_ASSERT_THROW(
        io::http::downloadFile(
            "https://cloud.ircad.fr/index.php/s/xGt4afRfMiWyeo4/download",
            dir / "sample.bin"
        ),
        sight::core::Exception
    );
}

} // namespace sight::io::http::ut
