/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_SDICOMSERIESCONVERTER_HPP__
#define __IOGDCM_SDICOMSERIESCONVERTER_HPP__

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
 * @brief   This service is used to read and push DicomSeries to a SeriesDB
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
    IOGDCM_API virtual ~SDicomSeriesConverter() noexcept;

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @code{.xml}
       <service uid="actionConvertSeries" impl="::ioGdcm::SDicomSeriesConverter">
         <config destinationSeriesDBID="mySeriesDB" />
       </service>
       @endcode
     */
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

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;
};

} // namespace ioGdcm

#endif // __IOGDCM_SDICOMSERIESCONVERTER_HPP__
