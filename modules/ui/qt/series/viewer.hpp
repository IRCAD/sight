/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <app/config_manager.hpp>

#include <data/vector.hpp>

#include <service/controller.hpp>

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
    <service type="sight::module::ui::qt::series::viewer">
        <in key="series" uid="..." auto_connect="true" />
        <parentView wid="..."/>
        <configs>
            <config id="2DSimpleConfig" type="data::image_series" />
            <config id="sight::io::dicom::2d_pacs_preview" type="data::dicom_series" >
                <parameter replace="PACS_CONFIGURATION" by="None" />
            </config>
            <config id="3DSimpleConfig" type="data::model_series" />
        </configs>
       </service>
   </service>
   @endcode
 * @subsection Input Input
 * - \b series [sight::data::vector]: vector containing the series to preview.
 * @subsection Configuration Configuration
 * - \b parentView: wid of the view where the config will install its windows.
 * - \b config: gives the available association between data type and associated config.
 *   - \b id: identifier of the config to launch
 *   - \b type: classname of the object stored in Vector associated to this config.
 *   - \b parameter: allow to pass specific value to the associated config
 *     - \b replace: name of the parameter to be replaced
 *     - \b by: specific value to replace for the parameter
 */
class viewer : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(viewer, service::controller);

    /// Constructor
    viewer() = default;

    /// Destructor
    ~viewer() noexcept override = default;

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
     * If there is a single selection : it launches an config on the object defined in this service configuration
     * (stored in m_seriesConfigs). The selected object fwID replaces the 'objectID' parameter in the config.
     * no configuration are launched if there is no selection, a multiple selection or if there is no configuration
     * associated with the selected object.
     */
    void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::ADDED_OBJECTS_SIG to this::service::slots::UPDATE
     * Connect Vector::REMOVED_OBJECTS_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

    void info(std::ostream& _sstream) override;

private:

    using replace_values_map_t = std::map<std::string, std::string>;

    /// Stucture to register configuration informations.
    struct series_config_info
    {
        /// Id of the configuration to launch.
        std::string config_id;

        /// Stores the parameters to pass to config.
        replace_values_map_t parameters;
    };

    using series_config_map_t = std::map<std::string, series_config_info>;

    /// config manager
    app::config_manager::sptr m_config_template_manager;

    /// Stores the wid of the view where the config will install its windows.
    std::string m_parent_view;

    /// Stores the association between data type and associated configuration.
    series_config_map_t m_series_configs;

    static constexpr std::string_view SERIES = "series";

    data::ptr<data::vector, data::access::in> m_series {this, SERIES};
};

} // namespace sight::module::ui::qt::series
