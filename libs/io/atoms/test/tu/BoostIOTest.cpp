/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "BoostIOTest.hpp"

#include <core/memory/BufferManager.hpp>
#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <io/atoms/Reader.hpp>
#include <io/atoms/Writer.hpp>
#include <io/zip/ReadDirArchive.hpp>
#include <io/zip/ReadZipArchive.hpp>
#include <io/zip/WriteDirArchive.hpp>
#include <io/zip/WriteZipArchive.hpp>

#include <filesystem>

CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::io::atoms::ut::BoostIOTest );

namespace sight::io::atoms
{
namespace ut
{

//-----------------------------------------------------------------------------

void BoostIOTest::setUp()
{
}

//------------------------------------------------------------------------------

void BoostIOTest::tearDown()
{
}

//-----------------------------------------------------------------------------

void BoostIOTest::readWriteZipTest()
{
    SequenceGenerator gen;

    std::filesystem::path folderPath =
        core::tools::System::getTemporaryFolder() / ( core::tools::UUID::generateUUID() + ".zip");

    io::zip::IWriteArchive::sptr writeArchive;
    io::zip::IReadArchive::sptr readArchive;

    {
        writeArchive = io::zip::WriteZipArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.json", io::atoms::JSON);
        writeArchive.reset();

        readArchive = io::zip::ReadZipArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.json");
        readArchive.reset();

        bool suppr = std::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
    {
        writeArchive = io::zip::WriteZipArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.xml", io::atoms::XML);
        writeArchive.reset();

        readArchive = io::zip::ReadZipArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.xml", io::atoms::XML);
        readArchive.reset();

        bool suppr = std::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
}

//-----------------------------------------------------------------------------

void BoostIOTest::readWriteDirTest()
{
    SequenceGenerator gen;
    std::filesystem::path folderPath =
        core::tools::System::getTemporaryFolder() / core::tools::UUID::generateUUID();

    io::zip::IWriteArchive::sptr writeArchive;
    io::zip::IReadArchive::sptr readArchive;

    {
        writeArchive = io::zip::WriteDirArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.json", io::atoms::JSON);
        writeArchive.reset();

        readArchive = io::zip::ReadDirArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.json");
        readArchive.reset();

        bool suppr = std::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
    {
        writeArchive = io::zip::WriteDirArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.xml", io::atoms::XML);
        writeArchive.reset();

        readArchive = io::zip::ReadDirArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.xml", io::atoms::XML);
        readArchive.reset();

        bool suppr = std::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
}

//-----------------------------------------------------------------------------

void BoostIOTest::writeProcess(SequenceGenerator& gen,
                               io::zip::IWriteArchive::sptr writeArchive,
                               const std::filesystem::path& rootFilename,
                               io::atoms::FormatType format )
{
    sight::atoms::Sequence::sptr seq = gen.getSequence();
    io::atoms::Writer(seq).write(writeArchive, rootFilename, format);
}

//-----------------------------------------------------------------------------

void BoostIOTest::readProcess(SequenceGenerator& gen,
                              io::zip::IReadArchive::sptr readArchive,
                              const std::filesystem::path& rootFilename,
                              const io::atoms::FormatType& formatType)
{
    sight::atoms::Sequence::sptr readSeq;
    {
        readSeq = sight::atoms::Sequence::dynamicCast(io::atoms::Reader().read(readArchive, rootFilename,
                                                                               formatType));
    }
    gen.compare(readSeq);
    this->cleanSequence(*readSeq);
}

//------------------------------------------------------------------------------

void BoostIOTest::cleanSequence(sight::atoms::Sequence& seq)
{
    // Break the reference cycles in the atom sequence
    auto blob = std::dynamic_pointer_cast< sight::atoms::Blob>(seq[2]);
    SIGHT_THROW_IF("That's not a blob!", !blob);
    auto bo = blob->getBufferObject();
    if(bo)
    {
        bo->destroy();
    }
    blob->setBufferObject(nullptr);
    auto map = std::dynamic_pointer_cast< sight::atoms::Map>(seq[3]);
    SIGHT_THROW_IF("That's not a map!", !map);
    map->clear();
    seq.clear();
}

}  // namespace ut
}  // namespace sight::io::atoms
