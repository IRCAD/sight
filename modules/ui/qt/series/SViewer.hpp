/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/qt/config.hpp"

#include <data/Vector.hpp>

#include <service/IAppConfigManager.hpp>
#include <service/IController.hpp>

#include <map>

namespace sight::module::ui::qt::series
{

/**
 * @brief  This Service allows to preview the selected series in the Vector. For the moment, it works only on a
 * single selection.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::ui::qt::series::SViewer">
        <in key="series" uid="..." autoConnect="true" />
        <parentView wid="..."/>
        <configs>
            <config id="2DSimpleConfig" type="data::ImageSeries" />
            <config id="2DPacsPreviewConfig" type="data::DicomSeries" >
                <parameter replace="PACS_CONFIGURATION" by="None" />
            </config>
            <config id="3DSimpleConfig" type="data::ModelSeries" />
        </configs>
       </service>
   </service>
   @endcode
 * @subsection Input Input
 * - \b series [sight::data::Vector]: vector containing the series to preview.
 * @subsection Configuration Configuration
 * - \b parentView: wid of the view where the config will install its windows.
 * - \b config: gives the available association between data type and associated config.
 *   - \b id: identifier of the AppConfig to launch
 *   - \b type: classname of the object stored in Vector associated to this config.
 *   - \b parameter: allow to pass specific value to the associated config
 *     - \b replace: name of the parameter to be replaced
 *     - \b by: specific value to replace for the parameter
 */
class MODULE_UI_QT_CLASS_API SViewer : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SViewer, service::IController);

    /// Constructor
    MODULE_UI_QT_API SViewer() = default;

    /// Destructor
    MODULE_UI_QT_API ~SViewer() noexcept override = default;

protected:

    /// Calls updating on starting.
    void starting() override;

    /// Stops the config if it is running.
    void stopping() override;

    /// Configures the service.
    void configuring() override;

    /**
     * @brief Launch the config on the object if possible.
     *
     * If there is a single selection : it launches an AppConfig on the object defined in this service configuration
     * (stored in m_seriesConfigs). The selected object fwID replaces the 'objectID' parameter in the AppConfig.
     * no configuration are launched if there is no selection, a multiple selection or if there is no configuration
     * associated with the selected object.
     */
    void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::s_ADDED_OBJECTS_SIG to this::IService::slots::s_UPDATE
     * Connect Vector::s_REMOVED_OBJECTS_SIG to this::IService::slots::s_UPDATE
     */
    MODULE_UI_QT_API KeyConnectionsMap getAutoConnections() const override;

    void info(std::ostream& _sstream) override;

private:

    typedef std::map<std::string, std::string> ReplaceValuesMapType;

    /// Stucture to register configuration informations.
    struct SeriesConfigInfo
    {
        /// Id of the configuration to launch.
        std::string configId;

        /// Stores the parameters to pass to config.
        ReplaceValuesMapType parameters;
    };

    typedef std::map<std::string, SeriesConfigInfo> SeriesConfigMapType;

    /// Config manager
    service::IAppConfigManager::sptr m_configTemplateManager;

    /// Stores the wid of the view where the config will install its windows.
    std::string m_parentView;

    /// Stores the association between data type and associated configuration.
    SeriesConfigMapType m_seriesConfigs;

    static constexpr std::string_view s_SERIES = "series";

    data::ptr<data::Vector, data::Access::in> m_series {this, s_SERIES, true};
};

} // namespace sight::module::ui::qt::series
