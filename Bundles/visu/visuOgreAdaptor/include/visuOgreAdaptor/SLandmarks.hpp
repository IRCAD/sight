/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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
 * @subsection Input Input:
 * - \b landmarks [::fwData::Landmarks]: landmarks to display.
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines landmarks layer.
 * - \b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * - \b textSize (optional): text size relatively to the screen's height (default 0.025f).
 * in the STransform adaptor.
 *
 */
class VISUOGREADAPTOR_CLASS_API SLandmarks : public ::fwRenderOgre::IAdaptor,
                                             public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SLandmarks, ::fwRenderOgre::IAdaptor);

    /// Constructor.
    VISUOGREADAPTOR_API SLandmarks() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SLandmarks() noexcept;

    /// Connect landmark modification signals to the update slot.
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    /// Configure the adaptor.
    virtual void configuring() override;

    /// Initialize material et update the service.
    virtual void starting() override;

    /// Stop the service.
    virtual void stopping() override;

    /// Update landmarks displaying.
    virtual void updating() override;

    /// Destroy Ogre's data.
    void clearData();

    /// Pointer to the material data.
    ::fwData::Material::sptr m_material {nullptr};

    /// Pointer to the material adaptor.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor {nullptr};

    /// Root node.
    ::Ogre::SceneNode* m_transNode {nullptr};

    /// Used to store manual objects.
    std::vector< ::Ogre::ManualObject* > m_manualObjects;

    /// Used to store label of each landmark.
    std::vector< ::fwRenderOgre::Text* > m_labels;

    /// Used to store landmark`s nodes.
    std::vector< ::Ogre::SceneNode* > m_nodes;

    /// Text size relatively to the screen's height.
    /// FIXME: should be computed from the font size and the screen's physical size.
    float m_textSize {0.025f};

    /// Text container.
    ::Ogre::OverlayContainer* m_text {nullptr};

    /// Font.
    ::Ogre::FontPtr m_font;
};

} //namespace visuOgreAdaptor
