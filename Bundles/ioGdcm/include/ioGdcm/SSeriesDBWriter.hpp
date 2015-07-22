/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SSERIESDBWRITER_HPP__
#define __IOGDCM_SSERIESDBWRITER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>
#include <gdcmIO/writer/Series.hpp>

#include "ioGdcm/config.hpp"

namespace fwMedData
{
class SeriesDB;
}

namespace ioGdcm
{

/**
 * @brief Services to write several series in DICOM format.
 * @class SSeriesDBWriter
 */
class IOGDCM_CLASS_API SSeriesDBWriter : public ::io::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBWriter)( ::io::IWriter) );

    /**
     * @brief Constructor
     */
    IOGDCM_API SSeriesDBWriter() throw();

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SSeriesDBWriter() throw();

    /// Propose a directory selection where to save the DICOM files.
    IOGDCM_API virtual void configureWithIHM();

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Write the ImageSeries in DICOM format.
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

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
    ::gdcmIO::writer::Series::FiducialsExportMode m_fiducialsExportMode;

};

} // namespace ioGdcm

#endif //__IOGDCM_SSERIESDBWRITER_HPP__
