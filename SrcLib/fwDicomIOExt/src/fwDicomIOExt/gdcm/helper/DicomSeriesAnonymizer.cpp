/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwTools/System.hpp>

#include "fwDicomIOExt/gdcm/helper/DicomSeriesAnonymizer.hpp"

namespace fwDicomIOExt
{
namespace gdcm
{
namespace helper
{

DicomSeriesAnonymizer::DicomSeriesAnonymizer()
{
    m_writer = ::fwDicomIOExt::gdcm::DicomSeriesWriter::New();
    m_reader = ::fwDicomIOExt::gdcm::DicomSeriesDBReader::New();
}

//------------------------------------------------------------------------------

DicomSeriesAnonymizer::~DicomSeriesAnonymizer()
{

}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::addHandler(const ::fwTools::ProgressAdviser::ProgessHandler &handler)
{
    m_dicomAnonymizer.addHandler(handler);
    m_writer->addHandler(handler);
    m_reader->addHandler(handler);
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(::fwDicomData::DicomSeries::sptr source)
{
    this->anonymize(source, source);
}

//------------------------------------------------------------------------------

void DicomSeriesAnonymizer::anonymize(::fwDicomData::DicomSeries::sptr source,
                                      ::fwDicomData::DicomSeries::sptr destination)
{
    FW_RAISE_IF("Dicom series should contain binaries.",
                source->getDicomAvailability() != ::fwDicomData::DicomSeries::BINARIES);

    // Create destination directory
    const ::boost::filesystem::path destPath = ::fwTools::System::getTemporaryFolder() / "DicomSeriesAnonymizer";
    ::boost::filesystem::create_directories( destPath );

    // Write binaries
    m_writer->setObject(source);
    ::fwData::location::Folder::sptr loc = ::fwData::location::Folder::New();
    loc->setFolder(destPath);
    m_writer->setLocation(loc);
    m_writer->write();

    // Anonymize directory
    m_dicomAnonymizer.anonymize(destPath);

    // Read anonymized series
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();
    m_reader->setObject(seriesDB);
    m_reader->setFolder(destPath);
    m_reader->read();

    // Update DicomSeries
    ::fwDicomData::DicomSeries::sptr anonymizedSeries =
        ::fwDicomData::DicomSeries::dynamicCast(seriesDB->getContainer().front());
    destination->deepCopy(anonymizedSeries);

    // Delete directory
    ::boost::filesystem::remove_all(destPath);
}

} // namespace helper
} // namespace gdcm
} // namespace fwDicomIOExt


