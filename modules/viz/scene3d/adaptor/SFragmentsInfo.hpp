/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <OGRE/OgreCompositionTechnique.h>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor takes a snapshot of layer fragments information and output it as a data::Image.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SFragmentsInfo">
        <config layer="default  width="1280" height="720"/>
        <inout key="image" uid="..." />
        <inout key="depth" uid="..." />
        <inout key="primitiveID" uid="..." />
    </service>
   @endcode
 *
 * @subsection InOut InOut:
 * - \b image [sight::data::Image] (optional): image containing the snapshot of the layer color buffer.
 * - \b depth [sight::data::Image] (optional): image containing the snapshot of the layer depth buffer.
 * - \b primitiveID [sight::data::Image] (optional): image containing the primitive ID of the layer.
 *
 * @subsection Configuration Configuration:
 * - \b width (optional): fixed width of snapshot.
 * - \b height (optional): fixed height of snaphot.
 *      NOTE: if width & height are missing (or one of them), size of the snapshot will be connected to the layer:
 *      if the layer is resized the snaphot will be resized.
 * - \b flip (optional): flip result images, default: false.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SFragmentsInfo final :
    public sight::viz::scene3d::IAdaptor,
    public Ogre::Viewport::Listener,
    public Ogre::RenderTargetListener
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SFragmentsInfo, sight::viz::scene3d::IAdaptor);

    /// Initializes the adaptor.
    MODULE_VIZ_SCENE3D_API SFragmentsInfo() noexcept = default;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SFragmentsInfo() noexcept override = default;

    /**
     * @brief Resizes the global render target, called by the related viewport since this adaptor is a listener.
     *
     * Call @ref module::viz::scene3d::adaptor::destroyCompositor() and
     * module::viz::scene3d::adaptor::createCompositor(int,
     * int).
     *
     * @param _viewport related layer's viewport.
     */
    MODULE_VIZ_SCENE3D_API void viewportDimensionsChanged(Ogre::Viewport* _viewport) override;

    /// Calls updating(). This is called right after the layer render target has been rendered.
    MODULE_VIZ_SCENE3D_API void postRenderTargetUpdate(const Ogre::RenderTargetEvent& /*evt*/) override;

protected:

    /// Configures the layer and retrieves the size of the output image.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Initializes adaptor and connection to layer signals.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /// Updates the service. Convert render target texture to data::Image.
    MODULE_VIZ_SCENE3D_API void updating() noexcept override;

    /// Destroys adaptor, only calls module::viz::scene3d::adaptor::destroyCompositor().
    MODULE_VIZ_SCENE3D_API void stopping() override;

private:

    /**
     * @brief Creates the compositor which copy the layer color buffer to a global render target.
     * @param _width width of the global render target (color and depth).
     * @param _height height of the global render target (color and depth).
     */
    void createCompositor(int _width, int _height);

    /// Destroys compositor.
    void destroyCompositor();

    /// Contains the created compositor.
    Ogre::CompositorPtr m_compositor {nullptr};

    /// Defines the compositor name.
    std::string m_compositorName;

    /// Defines the global render target name used to get back color and depth.
    std::string m_targetName;

    /// Defines the global render target name used to get back the primitive ID.
    std::string m_targetPrimitiveIDName;

    /// Enables the fixed size, if width & height parameters are found in config xml, use fixed size.
    /// If not use the layer's viewport size and listen the resize event.
    bool m_fixedSize {false};

    /// Defines the width and the height of the compositor's render target.
    /// Only used if width & height are found in <config /> of xml configuration
    int m_width {-1};

    /// Defines the compositor target fixed height.
    int m_height {-1};

    /// Flips Ogre texture when converting to sight, can be useful when using VTK to save images.
    bool m_flipImage {false};

    static constexpr std::string_view s_IMAGE_INOUT = "image";
    data::ptr<data::Image, data::Access::inout> m_image {this, s_IMAGE_INOUT};

    static constexpr std::string_view s_DEPTH_INOUT = "depth";
    data::ptr<data::Image, data::Access::inout> m_depth {this, s_DEPTH_INOUT};

    static constexpr std::string_view s_PRIMITIVE_ID_INOUT = "primitiveID";
    data::ptr<data::Image, data::Access::inout> m_primitive {this, s_PRIMITIVE_ID_INOUT};
};

} // namespace sight::module::viz::scene3d::adaptor.
