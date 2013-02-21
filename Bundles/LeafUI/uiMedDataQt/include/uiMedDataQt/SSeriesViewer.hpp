/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEDDATAQT_SSERIESVIEWER_HPP_
#define _UIMEDDATAQT_SSERIESVIEWER_HPP_

#include <map>

#include <fwServices/IController.hpp>
#include <fwServices/AppConfigManager.hpp>

#include "uiMedDataQt/config.hpp"

namespace uiMedData
{
/**
 * @brief  This Service allows to preview the selected series.
 *          It works on a simple click and single selection.Only, model series and image series can be previewed.
 * @class   SSeriesViewer.
 * @author  IRCAD (Research and Development Team).
 * @date    2013.
 */
class UIMEDDATAQT_CLASS_API SSeriesViewer : public ::fwServices::IController
{

    public :
    fwCoreServiceClassDefinitionsMacro ( (SSeriesViewer)(::fwServices::IController) ) ;

    /// Constructor
    UIMEDDATAQT_API SSeriesViewer();

    /// Destructor
    UIMEDDATAQT_API virtual ~SSeriesViewer() throw() ;

protected:

    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief configures the service.
     * @verbatim
    <service uid="seriesViewer" type="::fwServices::IController" impl="::uiMedData::SSeriesViewer" autoConnect="yes">
        <parentView>preview</parentView>
        <configs>
            <config id="2DSimpleConfig" type="::fwMedData::ImageSeries"/>
            <config id="3DSimpleConfig" type="::fwMedData::ModelSeries"/>
        </config>
    </service>
     @endverbatim
     * \ <parentView>preview</parentView> uid of the view where the config will be installed its windows.
     * \ <config id="2DSimpleConfig" type="::fwMedData::ImageSeries"/\> gives the available association between
     *   data type and associated config.
     */
    virtual void configuring() throw (::fwTools::Failed);

    virtual void updating() throw (::fwTools::Failed);

    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    virtual void info( std::ostream &_sstream );

private:
    typedef std::map<std::string, std::string> SeriesConfigMapType;

    /// Config manager
    ::fwServices::AppConfigManager::sptr m_configTemplateManager;

    /// Stores the uid of the view wher the config will be installed  its windows.
    std::string m_parentView;
    /// Stores the association between data type and associated configuration.
    SeriesConfigMapType m_seriesConfigs;
};
} // namespace uiMedData

#endif // _UIMEDDATAQT_SSERIESVIEWER_HPP_

