/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_SERIESDB_HPP__
#define __FWGDCMIO_WRITER_SERIESDB_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/writer/Series.hpp"

#include <fwData/location/Folder.hpp>
#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwTools/ProgressAdviser.hpp>

namespace fwGdcmIO
{

namespace writer
{

/**
 * @brief   This class manages patient writing, in DICOM file format.
 */
class SeriesDB : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::SeriesDB >,
                 public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                 public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (SeriesDB)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::SeriesDB >),
        (()), ::fwDataIO::writer::factory::New< SeriesDB >);

    /// Constructor
    FWGDCMIO_API SeriesDB(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor
    FWGDCMIO_API ~SeriesDB();

    /**
     * @brief Manage writing tools to save every series.
     */
    FWGDCMIO_API void write();

    /**
     * Override
     * @brief Do nothing
     */
    FWGDCMIO_API std::string extension();

    /// Get Fiducials Export Mode
    const ::fwGdcmIO::writer::Series::FiducialsExportMode& getFiducialsExportMode() const
    {
        return m_fiducialsExportMode;
    }

    /// Set Fiducials Export Mode
    void setFiducialsExportMode(const ::fwGdcmIO::writer::Series::FiducialsExportMode& fiducialsExportMode)
    {
        m_fiducialsExportMode = fiducialsExportMode;
    }


protected:
    /**
     * @brief Function used to sort Series
     * @param[in] a First Series
     * @param[in] b Second Series
     */
    static bool seriesComparator(SPTR(::fwMedData::Series) a, SPTR(::fwMedData::Series) b);

    /// Fiducials Export Mode
    ::fwGdcmIO::writer::Series::FiducialsExportMode m_fiducialsExportMode;
};

} // namespace writer

} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_SERIESDB_HPP__
