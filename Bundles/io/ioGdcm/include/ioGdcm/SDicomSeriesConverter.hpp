/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioGdcm/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwMedData/SeriesDB.hpp>

namespace fwJobs
{
class IJob;
}

namespace ioGdcm
{

/**
 * @brief   This service is used to convert DicomSeries from source SeriesDB
 * and push result (ImageSeries, ModelSeries, ...) in target SeriesDB
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(::fwJobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="..." impl="::ioGdcm::SDicomSeriesConverter">
     <in key="source" uid="..." />
     <inout key="target" uid="..." />
   </service>
   @endcode
 * @subsection Input Input:
 * - \b source [::fwMedData::SeriesDB]: Source SeriesDB containing DicomSeries.
 * @subsection In-Out In-Out:
 * - \b target [::fwMedData::SeriesDB]: Destination SeriesDB.
 */
class IOGDCM_CLASS_API SDicomSeriesConverter : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro( (SDicomSeriesConverter)( ::fwGui::IActionSrv ) );

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    /**
     * @brief Constructor
     */
    IOGDCM_API SDicomSeriesConverter() noexcept;

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SDicomSeriesConverter() noexcept override;

protected:

    /// Configuring method. This method is used to configure the service.
    IOGDCM_API virtual void configuring() override;

    /// Override
    IOGDCM_API virtual void starting() override;

    /// Override
    IOGDCM_API virtual void stopping() override;

    /// Override
    IOGDCM_API void updating() override;

    /// Override
    IOGDCM_API void info(std::ostream& _sstream ) override;

protected:

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;
};

} // namespace ioGdcm
