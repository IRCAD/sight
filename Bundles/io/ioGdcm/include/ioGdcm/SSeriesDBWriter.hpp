/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioGdcm/config.hpp"

#include <fwGdcmIO/writer/Series.hpp>

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
}

namespace ioGdcm
{

/**
 * @brief Services to write several series in DICOM format.
 */
class IOGDCM_CLASS_API SSeriesDBWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (SSeriesDBWriter)( ::fwIO::IWriter) );

    /**
     * @brief Constructor
     */
    IOGDCM_API SSeriesDBWriter() noexcept;

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SSeriesDBWriter() noexcept;

    /// Propose a directory selection where to save the DICOM files.
    IOGDCM_API virtual void configureWithIHM() override;

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() override;

    /// Does nothing
    IOGDCM_API virtual void stopping() override;

    /// Does nothing
    IOGDCM_API virtual void configuring() override;

    /// Write the ImageSeries in DICOM format.
    IOGDCM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:

    /**
     * @brief Write a SeriesDB to a folder in DICOM format
     * @param[in] folder DICOM folder
     * @param[in] seriesDB SeriesDB that must be writen
     */
    void saveSeriesDB( const ::boost::filesystem::path folder, SPTR(::fwMedData::SeriesDB) seriesDB );

    /// Select Fiducial Export Mode using a dialog
    bool selectFiducialsExportMode();

    /// Fiducial Export Mode
    ::fwGdcmIO::writer::Series::FiducialsExportMode m_fiducialsExportMode;

};

} // namespace ioGdcm
