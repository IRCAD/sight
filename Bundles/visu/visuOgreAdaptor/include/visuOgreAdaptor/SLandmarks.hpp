/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
 * - \b landmarks [::fwData::Landmarks]: landmarks to displayed.
 * @subsection Configuration Configuration:
 * -\b layer (mandatory): defines landmarks layer.
 * -\b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor.
 *
 */
class VISUOGREADAPTOR_CLASS_API SLandmarks : public ::fwRenderOgre::IAdaptor,
                                             public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro((SLandmarks)(::fwRenderOgre::IAdaptor))

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

    /// Use to store manual objects.
    std::vector< ::Ogre::ManualObject* > m_manualObjects {};

    /// Use to store label of each landmark.
    std::vector< ::fwRenderOgre::Text* > m_labels {};

    /// Text container.
    ::Ogre::OverlayContainer* m_text = {nullptr};

    /// Font.
    ::Ogre::FontPtr m_font = {nullptr};
};

} //namespace visuOgreAdaptor
