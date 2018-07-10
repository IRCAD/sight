/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

#include <boost/filesystem/operations.hpp>

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

    ::boost::filesystem::path folderPath =
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

        bool suppr = ::boost::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
    {
        writeArchive = ::fwZip::WriteZipArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.xml", ::fwAtomsBoostIO::XML);
        writeArchive.reset();

        readArchive = ::fwZip::ReadZipArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.xml", ::fwAtomsBoostIO::XML);
        readArchive.reset();

        bool suppr = ::boost::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
}

//-----------------------------------------------------------------------------

void BoostIOTest::readWriteDirTest()
{
    SequenceGenerator gen;
    ::boost::filesystem::path folderPath =
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

        bool suppr = ::boost::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
    {
        writeArchive = ::fwZip::WriteDirArchive::New(folderPath.string());
        this->writeProcess(gen, writeArchive, "root.xml", ::fwAtomsBoostIO::XML);
        writeArchive.reset();

        readArchive = ::fwZip::ReadDirArchive::New(folderPath.string());
        this->readProcess(gen, readArchive, "root.xml", ::fwAtomsBoostIO::XML);
        readArchive.reset();

        bool suppr = ::boost::filesystem::remove_all(folderPath);
        CPPUNIT_ASSERT(suppr);
    }
}

//-----------------------------------------------------------------------------

void BoostIOTest::writeProcess(SequenceGenerator& gen,
                               ::fwZip::IWriteArchive::sptr writeArchive,
                               const ::boost::filesystem::path& rootFilename,
                               ::fwAtomsBoostIO::FormatType format )
{
    ::fwAtoms::Sequence::sptr seq = gen.getSequence();
    ::fwAtomsBoostIO::Writer(seq).write(writeArchive, rootFilename, format);
}

//-----------------------------------------------------------------------------

void BoostIOTest::readProcess(SequenceGenerator& gen,
                              ::fwZip::IReadArchive::sptr readArchive,
                              const ::boost::filesystem::path& rootFilename,
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
