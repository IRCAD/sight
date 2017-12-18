/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SAXIS_HPP__
#define __VISUOGREADAPTOR_SAXIS_HPP__

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"
#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <Ogre.h>

namespace fwData
{
class Material;
}
namespace fwData
{
class Mesh;
}
namespace fwRenderOgre
{
class R2VBRenderable;
}

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor shows a coordinate system.
 *
 * TODO
 *
 */
class VISUOGREADAPTOR_CLASS_API SAxis : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:
    fwCoreServiceClassDefinitionsMacro((SAxis)(::fwRenderOgre::IAdaptor));

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SAxis() noexcept;
    /// Destructor: if an entity exists in the Ogre Scene, asks Ogre to destroy it.
    VISUOGREADAPTOR_API virtual ~SAxis() noexcept;

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;

    /// Sets whether the mesh is to be seen or not.
    VISUOGREADAPTOR_API void updateVisibility(bool isVisible);

    /** @} */

    /// Returns if the SMesh is visible in the scene or not.
    VISUOGREADAPTOR_API bool getVisibility() const;

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    /// Configures the adaptor
    void configuring() override;
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void starting() override;
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void updating() override;

    /// Node in the scene graph
    ::Ogre::Entity* m_entity;

    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;
    ::fwData::Material::sptr m_material;

};

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SAXIS_HPP__

