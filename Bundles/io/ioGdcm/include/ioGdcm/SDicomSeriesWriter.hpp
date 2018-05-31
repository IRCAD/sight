/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioGdcm/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwJobs
{
class IJob;
}

namespace fwMedData
{
class DicomSeries;
}

namespace ioGdcm
{

/**
 * @brief Services to write an DicomSeries in DICOM format.
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(::fwJobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::ioGdcm::SDicomSeriesWriter" >
           <in key="data" uid="..." />
       </service>
 * @endcode
 *
 * @subsection Input Input
 * - \b data [::fwMedData::DicomSeries]: data to save in Dicom.
 */
class IOGDCM_CLASS_API SDicomSeriesWriter : public ::fwIO::IWriter
{

public:
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    fwCoreServiceClassDefinitionsMacro( (SDicomSeriesWriter)( ::fwIO::IWriter) );

    /**
     * @brief   constructor
     */
    IOGDCM_API SDicomSeriesWriter() noexcept;

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SDicomSeriesWriter() noexcept override;

    /// Propose select a directory where to save the DICOM files.
    IOGDCM_API virtual void configureWithIHM() override;

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() override;

    /// Does nothing
    IOGDCM_API virtual void stopping() override;

    /// Configuring method. This method is used to configure the service.
    IOGDCM_API virtual void configuring() override;

    /// Write the DicomSeries in DICOM format.
    IOGDCM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:
    /// Save the selected Dicom series
    void saveDicomSeries( const ::boost::filesystem::path folder,
                          const CSPTR(::fwMedData::DicomSeries)& series ) const;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;
};

} // namespace ioGdcm
