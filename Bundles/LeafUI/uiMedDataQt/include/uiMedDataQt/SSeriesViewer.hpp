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
 * @brief
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
     *
     * @verbatim
    <service uid="seriesViewer" type="::fwServices::IController" impl="::uiMedData::SSeriesViewer" autoConnect="yes">
        <parentView>preview</parentView>
        <configs>
            <config id="2DSimpleConfig" type="::fwMedData::ImageSeries"/>
            <config id="3DSimpleConfig" type="::fwMedData::ModelSeries"/>
        </config>
    </service>
     @endverbatim
     */
    virtual void configuring() throw (::fwTools::Failed);

    virtual void updating() throw (::fwTools::Failed);

    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    virtual void info( std::ostream &_sstream );

private:
    typedef std::map<std::string, std::string> SeriesConfigMapType;

    /// Config manager
    ::fwServices::AppConfigManager::sptr m_configTemplateManager;

    std::string m_parentView;
    SeriesConfigMapType m_seriesConfigs;
};
} // namespace uiMedData

#endif // _UIMEDDATAQT_SSERIESVIEWER_HPP_

