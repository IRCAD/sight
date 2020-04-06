/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

#include <OGRE/OgreTexture.h>

namespace visuOgreAdaptor
{
/**
 * @brief This adaptor render a video frame from a 2D-image.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): Sets whether the video is visible or not.
 * - \b toggleVisibility(): Toggle whether the video is visible or not.
 * - \b updateTF(): Updates the displayed transfer function.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::visuOgreAdaptor::SVideo" autoConnect="yes" >
        <in key="image" uid="..." />
        <in key="tf" uid="..." optional="yes" />
        <config layer="default" reversed="true" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [::fwData::Image]: frame displayed.
 * - \b tf [::fwData::TransferFunction] (optional): a transfer function that can be applied to the video.
 * @subsection Configuration Configuration:
 *
 * - \b layer (mandatory, string): defines the video's layer
 * - \b reverse (optional, bool, default=true): if true, the actor is rotated by 180 degrees along the z and y axis.
 */
class VISUOGREADAPTOR_CLASS_API SVideo final : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceMacro(SVideo, ::fwRenderOgre::IAdaptor)

    /// Creates the adaptor an initialize slots.
    VISUOGREADAPTOR_API SVideo() noexcept;

    /// Does nothing.
    VISUOGREADAPTOR_API virtual ~SVideo() noexcept;

private:

    /// Configures the adaptor.
    virtual void configuring() override;

    /// Creates the Ogre texture and mapper used to show the video frame.
    virtual void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Image::s_MODIFIED_SIG of s_IMAGE_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::Image::s_BUFFER_MODIFIED_SIG of s_IMAGE_INPUT to s_UPDATE_SLOT
     * Connect ::fwData::TransferFunction::s_MODIFIED_SIG of s_TF_INPUT to :s_UPDATE_TF_SLOT
     * Connect ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG of s_TF_INPUT to s_UPDATE_TF_SLOT
     * Connect ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG of s_TF_INPUT to s_UPDATE_TF_SLOT
     */
    virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the frame from the current Image.
    virtual void updating() override;

    /// Removes the actor from the renderer
    virtual void stopping() override;

    /**
     * @brief SLOT: sets whether the video is to be seen or not.
     * @param _isVisible used true to show the video.
     */
    void updateVisibility(bool _isVisible);

    /// SLOT: toggles the visibility of the video.
    void toggleVisibility();

    /// SLOTS: updates the displayed transfer function.
    void updateTF();

    /// Cleans scene entities and associated mesh.
    void clearEntity();

    /// Contains the Ogre texture used to store the fwImage.
    ::Ogre::TexturePtr m_texture;

    /// Contains the Ogre material used to display the video plane.
    ::Ogre::MaterialPtr m_material;

    /// Contains the Ogre mesh used to display the video plane.
    ::Ogre::MeshPtr m_mesh;

    /// Contains the Ogre material used to display the video plane.
    ::Ogre::SceneNode* m_sceneNode { nullptr };

    /// Contains the Ogre entity used to display the video plane.
    ::Ogre::Entity* m_entity { nullptr };

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view).
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// Defines if the texture is initialized.
    bool m_isTextureInit { false };

    /// Defines the previous image size.
    size_t m_previousWidth { 0 };

    /// Defines the previous image spacing.
    size_t m_previousHeight { 0 };

    /// Defines the previous image type.
    ::fwTools::Type m_previousType;

    /// Defines if the actor is rotated in z and y axis.
    bool m_reverse { false };
};

} // namespace visuOgreAdaptor.
