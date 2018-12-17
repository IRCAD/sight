/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "AttachmentSeriesTest.hpp"

#include <fwAtomConversion/convert.hpp>

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/Writer.hpp>
#include <fwAtomsBoostIO/types.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/System.hpp>
#include <fwTools/Type.hpp>

#include <fwZip/ReadDirArchive.hpp>
#include <fwZip/ReadZipArchive.hpp>
#include <fwZip/WriteDirArchive.hpp>
#include <fwZip/WriteZipArchive.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::ut::AttachmentSeriesTest );

namespace fwMedData
{
namespace ut
{

::fwAtoms::Object::sptr readAttachment( const ::boost::filesystem::path& filePath )
{
    const ::boost::filesystem::path folderPath = filePath.parent_path();
    const ::boost::filesystem::path filename   = filePath.filename();
    const std::string extension                = ::boost::filesystem::extension(filePath);

    ::fwZip::IReadArchive::sptr readArchive;
    ::boost::filesystem::path archiveRootName;
    ::fwAtomsBoostIO::FormatType format = ::fwAtomsBoostIO::UNSPECIFIED;

    if ( extension == ".json" )
    {
        readArchive     = ::fwZip::ReadDirArchive::New(folderPath.string());
        archiveRootName = filename;
        format          = ::fwAtomsBoostIO::JSON;
    }
    else if ( extension == ".jsonz" )
    {
        readArchive     = ::fwZip::ReadZipArchive::New(filePath.string());
        archiveRootName = "root.json";
        format          = ::fwAtomsBoostIO::JSON;
    }
    else
    {
        FW_RAISE( "This file extension '" << extension << "' is not managed" );
    }

    ::fwAtomsBoostIO::Reader reader;
    ::fwAtoms::Object::sptr atom =
        ::fwAtoms::Object::dynamicCast(reader.read(readArchive, archiveRootName, format));

    return atom;
}

//------------------------------------------------------------------------------

void writeAttachment( const ::boost::filesystem::path& filePath, const ::fwAtoms::Object::sptr atom )
{
    const ::boost::filesystem::path folderPath = filePath.parent_path();
    const ::boost::filesystem::path filename   = filePath.filename();
    std::string extension                      = ::boost::filesystem::extension(filePath);

    // Write atom
    ::fwZip::IWriteArchive::sptr writeArchive;
    ::fwAtomsBoostIO::FormatType format;
    ::boost::filesystem::path archiveRootName;

    if ( extension == ".json" )
    {
        writeArchive    = ::fwZip::WriteDirArchive::New(folderPath.string());
        archiveRootName = filename;
        format          = ::fwAtomsBoostIO::JSON;
    }
    else if ( extension == ".jsonz" )
    {
        if ( ::boost::filesystem::exists( filePath ) )
        {
            ::boost::filesystem::remove( filePath );
        }
        writeArchive    = ::fwZip::WriteZipArchive::New(filePath.string());
        archiveRootName = "root.json";
        format          = ::fwAtomsBoostIO::JSON;
    }
    else
    {
        FW_RAISE( "This file extension '" << extension << "' is not managed" );
    }

    ::fwAtomsBoostIO::Writer(atom).write( writeArchive, archiveRootName, format );
    writeArchive.reset();
}

//------------------------------------------------------------------------------

void AttachmentSeriesTest::setUp()
{
    // Set up context before running a test.
    m_series = ::fwMedData::AttachmentSeries::New();
}

//------------------------------------------------------------------------------

void AttachmentSeriesTest::tearDown()
{
    // Clean up after the test run.
    m_series.reset();
}

//------------------------------------------------------------------------------

void AttachmentSeriesTest::attachmentTest()
{
    CPPUNIT_ASSERT(m_series);

    //Create Path
    const ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "attachmenttest";
    ::boost::filesystem::create_directories(path);
    const std::string filename                     = "attach.ext";
    const ::boost::filesystem::path attachmentFile = path / filename;
    const std::string mediaType                    = "text";

    std::ofstream file;
    file.open(attachmentFile.string().c_str(), std::ofstream::out);
    file << "42";
    file.close();
    size_t fileSize = ::boost::filesystem::file_size(attachmentFile);

    //None
    CPPUNIT_ASSERT_EQUAL(::fwMedData::AttachmentSeries::NONE, m_series->getAttachmentAvailability());
    CPPUNIT_ASSERT(m_series->getBufferObject()->isEmpty());

    //Local
    m_series->setAttachmentPath(attachmentFile);
    m_series->setMediaType(mediaType);
    CPPUNIT_ASSERT_EQUAL(::fwMedData::AttachmentSeries::LOCAL, m_series->getAttachmentAvailability());
    CPPUNIT_ASSERT_EQUAL(attachmentFile, m_series->getAttachmentPath());
    CPPUNIT_ASSERT(!m_series->getBufferObject()->isEmpty());
    CPPUNIT_ASSERT_EQUAL(fileSize, m_series->getBufferObject()->getSize());
    CPPUNIT_ASSERT_EQUAL(mediaType, m_series->getMediaType());

    //Embedded
    const std::string jsonzFile = "AttachmentSeriesTest.jsonz";
    ::fwAtoms::Object::sptr atom1 = ::fwAtomConversion::convert(m_series);
    writeAttachment((path/jsonzFile), atom1);
    ::fwAtoms::Object::sptr atom2 = readAttachment(path/jsonzFile);

    ::fwData::Object::sptr object;
    ::fwMedData::AttachmentSeries::sptr attachmentSeries;

    object           = ::fwAtomConversion::convert(atom2, ::fwAtomConversion::AtomVisitor::ChangePolicy());
    attachmentSeries = ::fwMedData::AttachmentSeries::dynamicCast(object);

    CPPUNIT_ASSERT(attachmentSeries);
    CPPUNIT_ASSERT_EQUAL(::fwMedData::AttachmentSeries::EMBEDDED, attachmentSeries->getAttachmentAvailability());
    CPPUNIT_ASSERT_EQUAL(filename, attachmentSeries->getAttachmentPath().string());
    CPPUNIT_ASSERT_EQUAL(mediaType, attachmentSeries->getMediaType());

    ::fwMemory::BufferObject::sptr buff = attachmentSeries->getBufferObject();
    ::fwMemory::BufferObject::Lock lock = buff->lock();
    CPPUNIT_ASSERT_EQUAL(fileSize, buff->getSize());
    CPPUNIT_ASSERT( lock.getBuffer() != nullptr );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedData
