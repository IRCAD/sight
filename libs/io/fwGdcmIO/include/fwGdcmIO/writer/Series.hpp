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

#pragma once

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwData/location/Folder.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwGdcmIO
{
namespace writer
{

/**
 * @brief This class handles DICOM files writing.
 * It defines needs of data storage and delegates writing to appropriate tools.
 * It manages all related data of one patient.
 */
class FWGDCMIO_CLASS_API Series : public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::Series >,
                                  public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                                  public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassMacro(Series, ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::Series >,
                     ::fwDataIO::writer::factory::New< Series >);

    typedef enum
    {
        SPATIAL_FIDUCIALS   = 1, /*! Export Fiducials using SpatialFiducialsIOD */
        COMPREHENSIVE_SR    = 2, /*! Export Fiducials using ComprehensiveSRIOD */
        COMPREHENSIVE_3D_SR = 3 /*! Export Fiducials using Comprehensive3DSRIOD */
    } FiducialsExportMode;

    typedef std::map< std::string, SPTR(::fwGdcmIO::container::DicomInstance) > DicomInstanceMapType;

    /// Constructor
    FWGDCMIO_API Series(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor
    FWGDCMIO_API ~Series();

    /**
     * @brief Load and start appropriate writing tools.
     * In addition, it creates an adapted folder tree where DICOM files will be stored.
     * It manages writing of the following general DICOM modules : patient, study and series
     * @note Currently, it handles image, SR and RT document writing.
     * @throw ::fwGdcmIO::exception::Failed
     */
    FWGDCMIO_API void write() override;

    /**
     * Override
     * @brief Do nothing
     */
    FWGDCMIO_API std::string extension() override;

    /// Get Fiducials Export Mode
    const FiducialsExportMode& getFiducialsExportMode() const
    {
        return m_fiducialsExportMode;
    }

    /// Set Fiducials Export Mode);
    void setFiducialsExportMode(const FiducialsExportMode& fiducialsExportMode)
    {
        m_fiducialsExportMode = fiducialsExportMode;
    }

private:

    /**
     * @brief Check if there is comment on acquisition.
     * @param[in] series ImageSeries that must be checked.
     */
    bool hasDocumentSR(const ::fwMedData::ImageSeries::csptr& series) const;

    /**
     * @brief Returns the image instance used to create the reconstruction
     * @note This function assume that one and only one image instance has been created prior calling this function.
     * The created image instance must be the one used to generate the reconstruction.
     */
    SPTR(::fwGdcmIO::container::DicomInstance) getImageInstance();

    /// Dicom Instance map used to keep information between series writing calls
    DicomInstanceMapType m_dicomInstanceMap;

    /// Fiducials Export Mode
    FiducialsExportMode m_fiducialsExportMode;

};

} // namespace writer
} // namespace fwGdcmIO
