/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Material.hpp>

namespace fwData
{
class Material;
}

namespace visuOgreAdaptor
{

/**
 * @brief   This adaptor shows a modelSeries. It creates an adaptor for each reconstruction in the model.
 *
 * @section Slots Slots
 * - \b showReconstructions(bool): update all reconstructions visibility.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SModelSeries">
            <in key="model" uid="..." />
            <config layer="..." transform="..." material="..." autoresetcamera="yes" dynamic="no" dynamicVertices="no"
 * queryFlags="1073741824" />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b model [::fwData::ModelSeries]: adapted model series.
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the modelSeries's layer
 * - \b transform (mandatory) : the transformation matrix to associate to the adaptor.
 * - \b material (optional): the name of the base Ogre material to pass to the mesh adaptors.
 * - \b autoresetcamera (optional, default="yes"): reset the camera when this mesh is modified, "yes" or "no".
 * - \b dynamic (optional, default=no) : if the modelSeries topolgy is likely to be updated frequently. This is a
 * performance hint that will choose a specific GPU memory pool accordingly.
 * - \b dynamicVertices (optional, default=no) : if the modelSeries geometry is likely to be updated frequently. This
 * is a performance hint that will choose a specific GPU memory pool accordingly.
 * - \b queryFlags (optional, default=0x40000000) : Used for picking. Picked only by pickers whose mask that match the
 * flag.
 */
class VISUOGREADAPTOR_CLASS_API SModelSeries : public ::fwRenderOgre::IAdaptor,
                                               public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceMacro(SModelSeries, ::fwRenderOgre::IAdaptor)

    /// Slot used to show the reconstruction.
    static const ::fwCom::Slots::SlotKeyType s_SHOW_RECONSTRUCTIONS_SLOT;

    /// Initialisa slots.
    VISUOGREADAPTOR_API SModelSeries() noexcept;

    /// Does nothing
    VISUOGREADAPTOR_API virtual ~SModelSeries() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     * @return The connection map proposals.
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Creates a Transform Service, then updates.
    VISUOGREADAPTOR_API void starting() override;

    /// Configure the parameter.
    VISUOGREADAPTOR_API void configuring() override;

    /// Redraws all (stops then restarts sub services).
    VISUOGREADAPTOR_API void updating() override;

    /// Closes connections and unregisters service.
    VISUOGREADAPTOR_API void stopping() override;

private:

    /**
     * @brief Update all reconstructions visibility.
     * @param _show Set to true to show reconstructions.
     */
    void showReconstructions(bool _show);

    /// Update all reconstructions visibility using "ShowReconstructions" field.
    void showReconstructionsOnFieldChanged();

    /// Defines if the camera must be reset automatically
    bool m_autoResetCamera {true};

    /// Texture adaptor's UID
    std::string m_textureAdaptorUID {""};

    /// Material name
    std::string m_materialTemplateName {::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME};

    /// Defines if the model series is dynamic
    bool m_isDynamic {false};

    /// Defines if the model series' vertices are dynamic
    bool m_isDynamicVertices {false};

    /// Mask for picking requests
    std::uint32_t m_queryFlags {::Ogre::SceneManager::ENTITY_TYPE_MASK};
};

//------------------------------------------------------------------------------

} // visuOgreAdaptor
