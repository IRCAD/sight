/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMSERIESWRITER_HPP__
#define __FWGDCMIO_HELPER_DICOMSERIESWRITER_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwData/location/Folder.hpp>
#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <string>

namespace fwJobs
{
class Observer;
class IJob;
}

namespace fwMedData
{
class DicomSeries;
}

namespace fwZip
{
class IWriteArchive;
}

namespace fwGdcmIO
{

namespace helper
{
class DicomAnonymizer;



/**
 * @brief Write a DicomSeries in DICOM format.
 *
 * This writer optionally manages anonymisation if an instance of DicomAnonymizer is set.
 * The output DICOM files can be exported into an archive (folder or zipped file) if output archive is set.
 *
 * If the DicomSeries contains the binaries of the DICOM files, the files are extracted (or zipped).
 * If the DicomSeries contains the paths of the DICOM files, a simple copy (or zip) is performed.
 */
class DicomSeriesWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::DicomSeries >,
                          public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((DicomSeriesWriter)(
                                               ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::DicomSeries>),
                                           (()), ::fwDataIO::writer::factory::New< DicomSeriesWriter >);
    fwCoreAllowSharedFromThis();

    /// Constructor. Does nothing
    FWGDCMIO_API DicomSeriesWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Write the image series in DICOM format.
    FWGDCMIO_API void write();

    /// Return an empty string
    FWGDCMIO_API std::string  extension();

    /// Get job observer
    FWGDCMIO_API SPTR(::fwJobs::IJob) getJob() const;

    /// Defines optional anonymizer.
    FWGDCMIO_API void setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer);

    /**
     * @brief Defines output archive for DICOM files.
     *
     * @param archive Archive instance
     * @param subPath Path within archive where DICOM files are saved (only if an anonymizer instance is defined)
     */
    FWGDCMIO_API void setOutputArchive(const SPTR(::fwZip::IWriteArchive)& archive,
                                       const std::string& subPath = "");

protected:

    /// Compute DICOM filename according to anonymizer or return default filename.
    std::string getFilename(const std::string& defaultName);

    /// Process inputStream to outputStream with anonymisation management.
    void processStream(std::istream& inputStream, std::ostream& outputStream);

    /// Process write on archive
    void processWriteArchive();

    /// Process write with Standard output stream
    void processWrite();

    /// Write count
    int m_writeCount;

    /// Job observer
    SPTR(::fwJobs::Observer) m_job;

    /// Optionnal anonymiser
    SPTR(DicomAnonymizer) m_anonymizer;

    /// Optionnal output archive
    SPTR(::fwZip::IWriteArchive) m_archive;

    /// Optional subPath (related to write archive
    std::string m_subPath;
};

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_DICOMSERIESWRITER_HPP__
