/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiIO/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>

#include <fwGui/IActionSrv.hpp>

namespace fwJobs
{
class IJob;
}

namespace uiIO
{
namespace action
{

/**
 * @brief   This action allows to export the current series using a temporary seriesDB.
 *
 * This service must work on a series.
 * It creates a new SeriesDB containing this series, and then launchs an ::uiIO::editor::SIOSelector service on the
 * created SeriesDB with the given configuration.
 *
 * @section Signal Signal
 * - \b jobCreated(SPTR(::fwJobs::IJob)) : This signal is emitted by the slot 'forwardJob' to forward job process
 *   between SIOSelector service (jobCreated signal) and other services.
 *
 * @section Slot Slot
 * - \b forwardJob(SPTR(::fwJobs::IJob)) : This slot allows to forward job process between SIOSelector service
 *   and other services. It is connected to SIOSelector 'jobCreated' signal.
 *
 * @section XML XML Configuration
 *
 * The available reader can be configured
 * @code{.xml}
    <service uid="action_seriesExporter" impl="::uiIO::action::SExportWithSeriesDB">
        <inout key="series" uid="..." />
        <IOSelectorSrvConfig name="seriesDBImporterConfig" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b series [::fwMedData::Series]: series to export using a SeriesDB writer
 *
 * @subsection Configuration Configuration
 * With :
 *  - \b IOSelectorSrvConfig : the id of a configuration for SIOSelector
 *  for example
 *  @code{.xml}
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>seriesDBExporterConfig</id>
        <service>::uiIO::editor::SIOSelector</service>
        <desc>IOSelector config to export SeriesDB</desc>
        <config>
            <type mode="writer" />
            <selection mode="include" />
            <addSelection service="::ioAtoms::SWriter" />
        </config>
    </extension>
    @endcode
 */
class UIIO_CLASS_API SExportWithSeriesDB : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (SExportWithSeriesDB)( ::fwGui::IActionSrv) );

    /**
     * @name Signal/Slot typedefs
     * @{
     */
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;
    typedef ::fwCom::Slot< void ( SPTR(::fwJobs::IJob) ) > ForwardJobSlotType;
    /// @}

    UIIO_API SExportWithSeriesDB() noexcept;

    UIIO_API virtual ~SExportWithSeriesDB() noexcept;

protected:

    /**
     * @brief Configure action. Retrieves the IOSelector config.
     */
    void configuring() override;

    /// Start action.
    void starting() override;

    /// Show the SeriesDB reader selector, load the new SeriesDB and merge it the the current SeriesDB
    void updating() override;

    /// Stop action.
    void stopping() override;

    void info(std::ostream& _sstream ) override;

private:

    /// SLOT: Allows to forward job process between io selector service and other services.
    void forwardJob(SPTR(::fwJobs::IJob) iJob);

    std::string m_ioSelectorSrvConfig; ///< Configuration used for launched SIOSelector service

    SPTR(JobCreatedSignalType) m_sigJobCreated; ///< signal emitted to forward SIOSelector job process
    SPTR(ForwardJobSlotType) m_slotForwardJob; ///< slot used to forward SIOSelector job process

};

} // namespace action
} // namespace uiIO
