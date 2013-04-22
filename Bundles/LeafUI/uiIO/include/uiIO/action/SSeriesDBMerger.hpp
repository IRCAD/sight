/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIO_ACTION_SSERIESDBMERGER_HPP_
#define _UIIO_ACTION_SSERIESDBMERGER_HPP_

#include <fwMedData/SeriesDB.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiIO/config.hpp"

namespace uiIO
{
namespace action
{

/**
 * @brief   This action allows to load a new SeriesDB and merge it with the current SeriesDB
 * @class   SSeriesDBMerger.
 * @date    2013.
 *
 * The available reader can be configured
 * @verbatim
    <service uid="action_seriesDBMerger" type="::fwGui::IActionSrv" impl="::uiIO::action::SSeriesDBMerger" autoConnect="no">
        <IOSelectorSrvConfig name="seriesDBImporterConfig" />
    </service>
   @endverbatim
 * With :
 *  - \b seriesDBImporterConfig : the id of a configuration for IOSelectorService
 *  for example
 *  @verbatim
    <extension implements="::fwServices::registry::ServiceConfig">
        <id>seriesDBImporterConfig</id>
        <service>::uiIO::editor::IOSelectorService</service>
        <desc>IOSelector config to import SeriesDB</desc>
        <config>
            <type mode="reader" />
            <selection mode="exclude" />
            <addSelection service="::ioAtoms::SReader" />
        </config>
    </extension>
    @endverbatim
 */
class UIIO_CLASS_API SSeriesDBMerger : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SSeriesDBMerger)( ::fwGui::IActionSrv) ) ;

    UIIO_API SSeriesDBMerger() throw() ;

    UIIO_API virtual ~SSeriesDBMerger() throw() ;

protected:

    /// Configure action. Retrieves the IOSelector config
    void configuring() throw( ::fwTools::Failed ) ;

    /// Start action.
    void starting() throw (::fwTools::Failed);

    /// Show the SeriesDB reader selector, load the new SeriesDB and merge it the the current SeriesDB
    void updating() throw (::fwTools::Failed);

    /// Does nothing
    void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    /// Stop action.
    void stopping() throw (::fwTools::Failed);
    void info(std::ostream &_sstream ) ;

private:
    std::string m_ioSelectorSrvConfig;

};

} // namespace action
} // namespace uiIO

#endif // _UIIO_ACTION_SSERIESDBMERGER_HPP_
