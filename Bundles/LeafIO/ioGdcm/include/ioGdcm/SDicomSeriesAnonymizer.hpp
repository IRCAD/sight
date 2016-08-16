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
 * @brief   Services to anonymize a DicomSeries
 */
class IOGDCM_CLASS_API SDicomSeriesAnonymizer : public ::fwServices::IController
{

public:
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignal;

    fwCoreServiceClassDefinitionsMacro ( (SDicomSeriesAnonymizer)( ::fwServices::IController) );

    /**
     * @brief   constructor
     */
    IOGDCM_API SDicomSeriesAnonymizer() throw();

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~SDicomSeriesAnonymizer() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @code{.xml}
       <service uid="anonymizeSelection" impl="::ioGdcm::SDicomSeriesAnonymizer" autoConnect="yes">
         <config seriesDBUID="mySeriesDB" />
       </service>
       @endcode
     */
    IOGDCM_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void info(std::ostream& _sstream );

    /// Override
    IOGDCM_API void anonymize();

    /// SeriesDB UID
    std::string m_seriesDBUID;

    /// SeriesDB of the Series
    SPTR(::fwMedData::SeriesDB) m_seriesDB;

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;

    /// Cancel information for jobs
    bool m_cancelled;
};

} // namespace ioGdcm

#endif //__IOGDCM_SDICOMSERIESANONYMIZER_HPP__
