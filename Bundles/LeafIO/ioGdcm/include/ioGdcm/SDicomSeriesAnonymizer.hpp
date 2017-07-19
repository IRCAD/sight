/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SDICOMSERIESANONYMIZER_HPP__
#define __IOGDCM_SDICOMSERIESANONYMIZER_HPP__

#include "ioGdcm/config.hpp"

#include <fwServices/IController.hpp>

namespace fwJobs
{
class IJob;
}

namespace fwMedData
{
class SeriesDB;
}

namespace ioGdcm
{

/**
 * @brief   Service to anonymize a DicomSeries.
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(::fwJobs::IJob) ) : Emitted when a job is created.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioGdcm::SDicomSeriesAnonymizer">
            <in key="seriesDB" uid="..." />
            <inout key="selectedSeries" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b seriesDB [::fwMedData::SeriesDB]: SeriesDB where the dicom series comes from.
 * - \b selectedSeries [::fwData::Vector]: List of DICOM series to be anonymized.
 */
class IOGDCM_CLASS_API SDicomSeriesAnonymizer : public ::fwServices::IController
{

public:
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesAnonymizer)( ::fwServices::IController) );

    /// Constructor
    IOGDCM_API SDicomSeriesAnonymizer() noexcept;

    /// Destructor
    IOGDCM_API virtual ~SDicomSeriesAnonymizer() noexcept;

protected:

    /// Do nothing.
    IOGDCM_API virtual void configuring();

    /// Override
    IOGDCM_API virtual void starting();

    /// Override
    IOGDCM_API virtual void stopping();

    /// Override
    IOGDCM_API void updating();

    /// Override
    IOGDCM_API void info(std::ostream& _sstream );

    /// Override
    IOGDCM_API void anonymize();

    /// SeriesDB of the Series
    SPTR(::fwMedData::SeriesDB) m_seriesDB;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;
};

} // namespace ioGdcm

#endif //__IOGDCM_SDICOMSERIESANONYMIZER_HPP__
