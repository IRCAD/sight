/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>

#include <OGRE/OgreCompositionTechnique.h>

namespace visuOgreAdaptor
{
/**
 * @brief This adaptor take a snapshot of a layer and output it as a fwData::Image.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SSnapshot">
        <config layer="default  width="1280" height="720"/>
    </service>
   @endcode
 *
 * @subsection Slots Slots:
 * -\b resizeRenderTarget(int _width, int _height): resize the output texture with _width * height.
 * If width & height are not present in <config /> this slot is connected to the resize signal of the layer.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : defines the mesh's layer.
 * - \b width (optionnal) : fixed width of snapshot.
 * - \b height (optionnal): fixed height of snaphot.
 * NOTE: if width & height are missing (or one of them), size of the snapshot will be connected to the layer:
 * if the layer is resized the snaphot will be resized.
 *
 * @subsection InOut InOut:
 * - \b image [::fwData::Image] : image containing the snapshot of the layer.
 *
 */
class VISUOGREADAPTOR_CLASS_API SSnapshot final : public ::fwRenderOgre::IAdaptor
{
public:

    fwCoreServiceMacro(SSnapshot, ::fwRenderOgre::IAdaptor)

    /// Constructor, initializes the adaptor.
    VISUOGREADAPTOR_API SSnapshot() noexcept;

    /// Destructor, destroys the adaptor.
    VISUOGREADAPTOR_API virtual ~SSnapshot() noexcept final;

private:

    /// Configures IAdaptor.
    virtual void configuring() final;

    /// Intializes adaptor and connection to layer signals.
    virtual void starting() final;

    /// Updates the service. Convert render target texture to fwData::Image.
    virtual void updating() noexcept final;

    /// Destroys adaptor, call destroyCompositor().
    virtual void stopping() final;

    /// Creates the compositor with _with * _height size.
    void createCompositor(int _width, int _height);

    /// Destroys compositor.
    void destroyCompositor();

    /// Slot: resizes render target, new size = _width * _height.
    void resizeRenderTarget(int _width, int _height);

    /// Ogre compositor pointer.
    ::Ogre::CompositorPtr m_compositor { nullptr };

    /// Ogre compositor name.
    std::string m_compositorName;

    /// Ogre target name (used to get back the texture).
    std::string m_targetName;

    /// Ogre Texture definition.
    ::Ogre::CompositionTechnique::TextureDefinition* m_target { nullptr };

    /// Handles connection the to layer.
    ::fwCom::helper::SigSlotConnection m_layerConnection;

    /// If width & height parmeters are found in config xml, use fixed size.
    /// If not use the layer's viewport size and listen the resize event.
    bool m_fixedSize { false };

    /// Only used if width & height are found in <config /> of xml configuration
    int m_width { -1 }; ///< Fixed width.
    int m_height { -1 }; ///< Fixed height.

};

} //namespace visuOgreAdaptor
