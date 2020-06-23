/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCore/macros.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Text.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor displays landmarks.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SLandmarks">
        <in key="landmarks" uid="..." />
        <config layer="default" transform="transformUID" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b landmarks [::fwData::Landmarks]: landmarks to display.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines landmarks layer.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified
 * - \b fontSource (optional, string, default=DejaVuSans.ttf): TrueType font (*.ttf) source file.
 * - \b fontSize (optional, unsigned int, default=16): font size in points.
 *
 */
class VISUOGREADAPTOR_CLASS_API SLandmarks final :
    public ::fwRenderOgre::IAdaptor,

    public ::fwRenderOgre::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SLandmarks, ::fwRenderOgre::IAdaptor)

    /// Creates the adaptor.
    VISUOGREADAPTOR_API SLandmarks() noexcept;

    /// Does nothing.
    VISUOGREADAPTOR_API ~SLandmarks() noexcept override;

protected:

    /// Configure the adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// Initialize material and update the service.
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Landmarks::s_MODIFIED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_ADDED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_REMOVED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_POINT_ADDED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_POINT_REMOVED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_POINT_INSERTED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_POINT_MODIFIED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_MODIFIED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Landmarks::s_GROUP_RENAMED_SIG of s_LANDMARKS_INPUT to s_UPDATE_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Draws landmarks.
    VISUOGREADAPTOR_API void updating() override;

    /// Destroys Ogre resources and the material.
    VISUOGREADAPTOR_API void stopping() override;

private:

    /// Destroy Ogre's resources.
    void clearData();

    /// Contains the material data.
    ::fwData::Material::sptr m_material { nullptr };

    /// Contains the Ogre material adaptor.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor { nullptr };

    /// Contains the root scene node.
    ::Ogre::SceneNode* m_transNode { nullptr };

    /// Stores manual objects.
    std::vector< ::Ogre::ManualObject* > m_manualObjects;

    /// Stores label of each landmark.
    std::vector< ::fwRenderOgre::Text* > m_labels;

    /// Stores landmark`s nodes.
    std::vector< ::Ogre::SceneNode* > m_nodes;

    /// Contains the text container.
    ::Ogre::OverlayContainer* m_text { nullptr };

    /// Defines the label font size in points.
    size_t m_fontSize { 16 };

    /// Defines the TrueType font source file.
    std::string m_fontSource { "DejaVuSans.ttf" };

};

} // namespace visuOgreAdaptor.
