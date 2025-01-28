/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <data/model_series.hpp>

#include <viz/scene3d/adaptor.hpp>
#include <viz/scene3d/material/standard.hpp>
#include <viz/scene3d/transformable.hpp>

namespace sight::data
{

class Material;

} // namespace sight::data

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a modelSeries. It creates an adaptor for each reconstruction in the model.
 *
 * @section Slots Slots
 * - \b changeField(): update all reconstructions visibility.
 * - \b update_visibility(bool): sets whether all reconstructions are shown or not.
 * - \b toggle_visibility(): toggle whether all reconstructions are shown or not.
 * - \b show(): shows all reconstructions.
 * - \b hide(): hides all reconstructions.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::viz::scene3d::adaptor::model_series">
        <in key="model" uid="..." />
        <config transform="..." material="..." autoresetcamera="true" dynamic="false"
 * dynamicVertices="false"
        queryFlags="0x40000000" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b model [sight::data::model_series]: adapted model series.
 *
 * @subsection Configuration Configuration:
 * - \b transform (optional, string, default=""): the transformation matrix to associate to the adaptor.
 * - \b material (optional, string, default=""): the name of the base Ogre material to pass to the mesh adaptors.
 * - \b autoresetcamera (optional, true/false, default=true): reset the camera when this mesh is modified, "true" or
 *"false".
 * - \b dynamic (optional, true/false, default=false): if the modelSeries topology is likely to be updated frequently.
 * This is
 *      a performance hint that will choose a specific GPU memory pool accordingly.
 * - \b dynamicVertices (optional, true/false, default=false): if the modelSeries geometry is likely to be updated
 * frequently.
 *      This is a performance hint that will choose a specific GPU memory pool accordingly.
 * - \b queryFlags (optional, uint32, default=0x40000000): Used for picking. Picked only by pickers whose mask that
 *      match the flag.
 * - \b visible (optional, true/false, default=true): Used to define the default visibility of the modelSeries. If the
 *      tag is not present, the visibility will be set by the value of the modelSeries field. If the tag is present,
 *      the visibility is set by the value of this tag.
 */
class model_series final :
    public sight::viz::scene3d::adaptor,
    public sight::viz::scene3d::transformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(model_series, sight::viz::scene3d::adaptor);

    /// Initialisa slots.
    model_series() noexcept;

    /// Destroys the adaptor.
    ~model_series() noexcept final = default;

protected:

    /// Configures the adaptor.
    void configuring() final;

    /// Starts the service and updates it.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::model_series::VERTEX_MODIFIED_SIG to service::slots::UPDATE
     * Connect data::model_series::RECONSTRUCTIONS_ADDED_SIG to service::slots::UPDATE
     * Connect data::model_series::RECONSTRUCTIONS_REMOVED_SIG to service::slots::UPDATE
     * Connect data::model_series::ADDED_FIELDS_SIG to CHANGE_FIELD_SLOT
     * Connect data::model_series::REMOVED_FIELDS_SIG to CHANGE_FIELD_SLOT
     * Connect data::model_series::CHANGED_FIELDS_SIG to CHANGE_FIELD_SLOT
     */
    service::connections_t auto_connections() const final;

    /// Redraws all (stops then restarts sub services).
    void updating() final;

    /// Closes connections and unregisters service.
    void stopping() final;

    /**
     * @brief Sets the model series visibility.
     * @param _visible the visibility status of the model series.
     */
    void set_visible(bool _visible) final;

private:

    /// SLOT: updates all reconstructions visibility from the input data field.
    void show_reconstructions_on_field_changed();

    /// Defines if the camera must be reset automatically
    bool m_auto_reset_camera {true};

    /// Defines the texture name.
    std::string m_texture_adaptor_uid;

    /// Defines the material name.
    std::string m_material_template_name {sight::viz::scene3d::material::standard::TEMPLATE};

    /// Defines if the model series is dynamic.
    bool m_is_dynamic {false};

    /// Defines if the model series' vertices are dynamic.
    bool m_is_dynamic_vertices {false};

    /// Defines the mask used for picking request.
    std::uint32_t m_query_flags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines if the visibility tag is present in the configuration.
    bool m_is_visible_tag {false};

    static constexpr std::string_view MODEL_INPUT = "model";
    data::ptr<data::model_series, data::access::in> m_model {this, MODEL_INPUT};
};

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
