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
 * @brief  This Service allows to preview the selected series in the Vector. For the moment, it works only on a
 * single selection.
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

    /// Calls updating on starting.
    virtual void starting() throw(::fwTools::Failed);

    /// Stops the config if it is running.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     * @brief Configures the service.
     * @verbatim
    <service uid="seriesViewer" type="::fwServices::IController" impl="::uiMedData::SSeriesViewer" autoConnect="yes">
        <parentView>preview</parentView>
        <configs>
            <config id="2DSimpleConfig" type="::fwMedData::ImageSeries">
                <extract name="@image" pattern="imageID" />
            </config>
            <config id="3DSimpleConfig" type="::fwMedData::ModelSeries" />
        </config>
    </service>
     @endverbatim
     * - \<parentView\>preview\</parentView\> wid of the view where the config will install its windows.
     * - \<config id="2DSimpleConfig" type="::fwMedData::ImageSeries"/\> gives the available association between
     *   data type and associated config.
     * - \<extract name="@values.imgSeriesRegister.image" pattern="imageID" /\> extracts the object from name and
     *   replaces pattern with its fwID
     */
    virtual void configuring() throw (::fwTools::Failed);

    /**
     * @brief Launch the config on the object if possible.
     *
     * If there is a single selection : launch a config on the object if it is defines in this service configuration
     * (stored in m_seriesConfigs). The selected object is added in config root composite with the key 'series'.
     * Else doesn't launch config.
     */
    virtual void updating() throw (::fwTools::Failed);

    /// Listen Vector notification when objects are added or deleted.
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    virtual void info( std::ostream &_sstream );

private:

    typedef std::map<std::string, std::string> ReplaceValuesMapType;

    struct SeriesConfigInfo
    {
        std::string configId;
        ReplaceValuesMapType replaceValues;
    };

    typedef std::map<std::string, SeriesConfigInfo> SeriesConfigMapType;

    /// Config manager
    ::fwServices::AppConfigManager::sptr m_configTemplateManager;

    /// Stores the wid of the view where the config will install its windows.
    std::string m_parentView;

    /// Stores the association between data type and associated configuration.
    SeriesConfigMapType m_seriesConfigs;
};
} // namespace uiMedData

#endif // _UIMEDDATAQT_SSERIESVIEWER_HPP_

