/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include <fwMemory/BufferManager.hpp>

#include <fwTools/System.hpp>
#include <fwTools/UUID.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/ReadZipArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <filesystem>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtomsBoostIO::ut::BoostIOTest );

namespace fwAtomsBoostIO
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
        ::fwTools::System::getTemporaryFolder() / ( ::fwTools::UUID::generateUUID() + ".zip");

    ::fwZip::IWriteArchive::sptr writeArchive;
    ::fwZip::IReadArchive::sptr readArchive;

    {
        writeArchive = ::fwZip::WriteZipArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.json", ::fwAtomsBoostIO::JSON);
        writeArchive.reset();

        readArchive = ::fwZip::ReadZipArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.json");
        readArchive.reset();

        bool suppr = std::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
    {
        writeArchive = ::fwZip::WriteZipArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.xml", ::fwAtomsBoostIO::XML);
        writeArchive.reset();

        readArchive = ::fwZip::ReadZipArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.xml", ::fwAtomsBoostIO::XML);
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
        ::fwTools::System::getTemporaryFolder() / ::fwTools::UUID::generateUUID();

    ::fwZip::IWriteArchive::sptr writeArchive;
    ::fwZip::IReadArchive::sptr readArchive;

    {
        writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.json", ::fwAtomsBoostIO::JSON);
        writeArchive.reset();

        readArchive = ::fwZip::ReadDirArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.json");
        readArchive.reset();

        bool suppr = std::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
    {
        writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.xml", ::fwAtomsBoostIO::XML);
        writeArchive.reset();

        readArchive = ::fwZip::ReadDirArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.xml", ::fwAtomsBoostIO::XML);
        readArchive.reset();

        bool suppr = std::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
}

//-----------------------------------------------------------------------------

void BoostIOTest::writeProcess(SequenceGenerator& gen,
                               ::fwZip::IWriteArchive::sptr writeArchive,
                               const std::filesystem::path& rootFilename,
                               ::fwAtomsBoostIO::FormatType format )
{
    ::fwAtoms::Sequence::sptr seq = gen.getSequence();
    ::fwAtomsBoostIO::Writer(seq).write(writeArchive, rootFilename, format);
}

//-----------------------------------------------------------------------------

void BoostIOTest::readProcess(SequenceGenerator& gen,
                              ::fwZip::IReadArchive::sptr readArchive,
                              const std::filesystem::path& rootFilename,
                              const ::fwAtomsBoostIO::FormatType& formatType)
{
    ::fwAtoms::Sequence::sptr readSeq;
    {
        readSeq = ::fwAtoms::Sequence::dynamicCast(::fwAtomsBoostIO::Reader().read(readArchive, rootFilename,
                                                                                   formatType));
    }
    gen.compare(readSeq);
    this->cleanSequence(*readSeq);
}

//------------------------------------------------------------------------------

void BoostIOTest::cleanSequence(::fwAtoms::Sequence& seq)
{
    // Break the reference cycles in the atom sequence
    auto blob = std::dynamic_pointer_cast< ::fwAtoms::Blob>(seq[2]);
    FW_RAISE_IF("That's not a blob!", !blob);
    auto bo = blob->getBufferObject();
    if(bo)
    {
        bo->destroy();
    }
    blob->setBufferObject(nullptr);
    auto map = std::dynamic_pointer_cast< ::fwAtoms::Map>(seq[3]);
    FW_RAISE_IF("That's not a map!", !map);
    map->clear();
    seq.clear();
}

}  // namespace ut
}  // namespace fwAtomsBoostIO
