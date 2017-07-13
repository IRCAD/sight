/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SDICOMSERIESWRITER_HPP__
#define __IOGDCM_SDICOMSERIESWRITER_HPP__

#include "ioGdcm/config.hpp"

#include <io/IWriter.hpp>

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
 */
class IOGDCM_CLASS_API SDicomSeriesWriter : public ::io::IWriter
{

public:
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    fwCoreServiceClassDefinitionsMacro( (SDicomSeriesWriter)( ::io::IWriter) );

    /**
     * @brief   constructor
     */
    IOGDCM_API SDicomSeriesWriter() throw();

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SDicomSeriesWriter() throw();

    /// Propose select a directory where to save the DICOM files.
    IOGDCM_API virtual void configureWithIHM();

protected:

    /// Does nothing
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Does nothing
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    IOGDCM_API virtual void configuring() throw(::fwTools::Failed);

    /// Write the DicomSeries in DICOM format.
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Return path type managed by the service, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

private:
    /// Save the selected Dicom series
    void saveDicomSeries( const ::boost::filesystem::path folder, SPTR(::fwMedData::DicomSeries) series ) const;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;
};

} // namespace ioGdcm

#endif //__IOGDCM_SDICOMSERIESWRITER_HPP__
