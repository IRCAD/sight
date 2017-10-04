/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIO_ACTION_SSERIESDBMERGER_HPP__
#define __UIIO_ACTION_SSERIESDBMERGER_HPP__

#include "uiIO/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwMedData/SeriesDB.hpp>

namespace fwJobs
{
class IJob;
}

namespace uiIO
{
namespace action
{

/**
 * @brief   This action allows to load a new SeriesDB and merge it with the current SeriesDB
 *
 * The available reader can be configured
 * @code{.xml}
    <service uid="action_seriesDBMerger" type="::fwGui::IActionSrv" impl="::uiIO::action::SSeriesDBMerger"
 * autoConnect="no">
        <IOSelectorSrvConfig name="seriesDBImporterConfig" />
    </service>
   @endcode
 * With :
 *  - \b seriesDBImporterConfig : the id of a configuration for SIOSelector
 *  for example
 *  @code{.xml}
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>seriesDBImporterConfig</id>
        <service>::uiIO::editor::SIOSelector</service>
        <desc>IOSelector config to import SeriesDB</desc>
        <config>
            <type mode="reader" />
            <selection mode="exclude" />
            <addSelection service="::ioAtoms::SReader" />
        </config>
    </extension>
    @endcode
 */
class UIIO_CLASS_API SSeriesDBMerger : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (SSeriesDBMerger)( ::fwGui::IActionSrv) );

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;
    typedef ::fwCom::Slot< void ( SPTR(::fwJobs::IJob) ) > ForwardJobSlotType;

    UIIO_API SSeriesDBMerger() noexcept;

    UIIO_API virtual ~SSeriesDBMerger() noexcept;

protected:

    /**
     * @brief Configure action. Retrieves the IOSelector config.
     *
     * @code{.xml}
        <service impl="::uiIO::action::SSeriesDBMerger" autoConnect="no">
            <IOSelectorSrvConfig name="myConfig" />
        </service>
       @endcode
     * - \b IOSelectorSrvConfig: identifier of the ServiceConfig used for SIOSelector
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
    void forwardJob(SPTR(::fwJobs::IJob) iJob);

    std::string m_ioSelectorSrvConfig;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
    SPTR(ForwardJobSlotType) m_slotForwardJob;

};

} // namespace action
} // namespace uiIO

#endif // __UIIO_ACTION_SSERIESDBMERGER_HPP__
