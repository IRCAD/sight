/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/ITransformable.hpp>
#include <viz/scene3d/Material.hpp>

namespace sight::data
{
class Material;
}

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a modelSeries. It creates an adaptor for each reconstruction in the model.
 *
 * @section Slots Slots
 * - \b changeField(): update all reconstructions visibility.
 * - \b updateVisibility(bool): sets whether all reconstructions are shown or not.
 * - \b toggleVisibility(): toggle whether all reconstructions are shown or not.
 * - \b show(): shows all reconstructions.
 * - \b hide(): hides all reconstructions.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::sight::module::viz::scene3d::adaptor::SModelSeries">
        <in key="model" uid="..." />
        <config layer="..." transform="..." material="..." autoresetcamera="yes" dynamic="no" dynamicVertices="no"
        queryFlags="0x40000000" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out:
 * - \b model [sight::data::ModelSeries]: adapted model series.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the modelSeries's layer
 * - \b transform (optional, string, default=""): the transformation matrix to associate to the adaptor.
 * - \b material (optional, string, default=""): the name of the base Ogre material to pass to the mesh adaptors.
 * - \b autoresetcamera (optional, yes/no, default=yes): reset the camera when this mesh is modified, "yes" or "no".
 * - \b dynamic (optional, yes/no, default=no): if the modelSeries topolgy is likely to be updated frequently. This is
 *      a performance hint that will choose a specific GPU memory pool accordingly.
 * - \b dynamicVertices (optional, yes/no, default=no): if the modelSeries geometry is likely to be updated frequently.
 *      This is a performance hint that will choose a specific GPU memory pool accordingly.
 * - \b queryFlags (optional, uint32, default=0x40000000): Used for picking. Picked only by pickers whose mask that
 *      match the flag.
 * - \b visible (optional, true/false, default=true): Used to define the default visibility of the modelSeries. If the
 *      tag is not present, the visibility will be set by the value of the modelSeries field. If the tag is present,
 *      the visibility is set by the value of this tag.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SModelSeries final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SModelSeries, ::sight::viz::scene3d::IAdaptor)

    /// Initialisa slots.
    MODULE_VIZ_SCENE3D_API SModelSeries() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SModelSeries() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Starts the service and updates it.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::ModelSeries::s_VERTEX_MODIFIED_SIG to s_UPDATE_SLOT
     * Connect data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG to s_UPDATE_SLOT
     * Connect data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG to s_UPDATE_SLOT
     * Connect data::ModelSeries::s_ADDED_FIELDS_SIG to s_CHANGE_FIELD_SLOT
     * Connect data::ModelSeries::s_REMOVED_FIELDS_SIG to s_CHANGE_FIELD_SLOT
     * Connect data::ModelSeries::s_CHANGED_FIELDS_SIG to s_CHANGE_FIELD_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Redraws all (stops then restarts sub services).
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Closes connections and unregisters service.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the model series visibility.
     * @param _visible the visibility status of the model series.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

private:

    /// SLOT: updates all reconstructions visibility from the input data field.
    void showReconstructionsOnFieldChanged();

    /// Defines if the camera must be reset automatically
    bool m_autoResetCamera { true };

    /// Defines the texture name.
    std::string m_textureAdaptorUID { "" };

    /// Defines the material name.
    std::string m_materialTemplateName {sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME};

    /// Defines if the model series is dynamic.
    bool m_isDynamic {false};

    /// Defines if the model series' vertices are dynamic.
    bool m_isDynamicVertices {false};

    /// Defines the mask used for picking request.
    std::uint32_t m_queryFlags {::Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines if the visibility tag is present in the configuration.
    bool m_isVisibleTag{false};
};

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
