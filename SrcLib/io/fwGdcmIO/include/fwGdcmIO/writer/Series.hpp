/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_SERIES_HPP__
#define __FWGDCMIO_WRITER_SERIES_HPP__

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

    fwCoreClassDefinitionsWithFactoryMacro( (Series)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::Series >),
                                            (()), ::fwDataIO::writer::factory::New< Series >);

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
     */
    FWGDCMIO_API void write() throw (::fwGdcmIO::exception::Failed)override;

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

#endif // __FWGDCMIO_WRITER_SERIES_HPP__
