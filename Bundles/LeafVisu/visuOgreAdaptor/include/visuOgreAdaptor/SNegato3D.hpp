/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SNEGATO3D_HPP__
#define __VISUOGREADAPTOR_SNEGATO3D_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Float.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/Plane.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Adaptor to display a 3D negato
 */
class VISUOGREADAPTOR_CLASS_API SNegato3D : public ::fwRenderOgre::IAdaptor,
                                            public ::fwRenderOgre::ITransformable,
                                            public ::fwDataTools::helper::MedicalImageAdaptor
{
public:

    typedef ::fwDataTools::helper::MedicalImageAdaptor::Orientation OrientationMode;

    fwCoreServiceClassDefinitionsMacro ( (SNegato3D)(::fwRenderOgre::IAdaptor) );

    /// Constructor
    VISUOGREADAPTOR_API SNegato3D() throw();
    /// Destructor. Does nothing.
    VISUOGREADAPTOR_API virtual ~SNegato3D() throw();
    /// Sets the filtering type
    VISUOGREADAPTOR_API void setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering );

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SLICETYPE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SLICEINDEX_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_OPACITY_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    ///@}

protected:

    /// Starts the service,
    VISUOGREADAPTOR_API void doStart() throw(fwTools::Failed);
    /// Stops the service, disconnects connections.
    VISUOGREADAPTOR_API void doStop() throw(fwTools::Failed);
    /// Requests rendering of the scene.
    VISUOGREADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /**
     * @brief Configures the service
     * @code{.xml}
        <adaptor uid="SNegato3D" class="::visuOgreAdaptor::SNegato3D" objectId="image">
             <config renderer="default" picker="negatodefault" sliceIndex="axial"
                     imageSource="imageKey" filtering="none" />
        </adaptor>
       @endcode
     * - \b renderer (optional): defines the renderer to show the arrow. It must be different from the 3D objects
     *      renderer.
     * - \b picker (optional): identifier of the picker
     * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
     * - \b filtering (optional, none/linear/anisotropic, default=none): texture filter type of the negato
     */
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );
    /// Performs stop, start and update.
    VISUOGREADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

    /// Called when transfer function points are modified.
    VISUOGREADAPTOR_API virtual void updatingTFPoints();

    /// Called when transfer function windowing is modified.
    VISUOGREADAPTOR_API virtual void updatingTFWindowing(double window, double level);

private:

    /// Slot: update image buffer
    void newImage();

    /// Slot: update image slice type
    void changeSliceType(int _from, int _to);

    /// Slot: update image slice index
    void changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex);


    /// Makes the planes process their meshes
    void createPlanes(const ::fwData::Image::SpacingType& _spacing, const fwData::Image::OriginType& _origin);

    /// Sets the planes's opacity
    /// Also a slot called when image opacity is modified
    void setPlanesOpacity();

    /// Sets whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera;

    /// Ogre texture which will be displayed on the negato
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view)
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// Stores the planes on which we will apply our texture
    ::fwRenderOgre::Plane* m_planes[3];

    // The current selected plane. This one will move in the scene
    ::fwRenderOgre::Plane* m_activePlane;

    /// The scene node allowing to move the entire negato
    ::Ogre::SceneNode* m_negatoSceneNode;

    /// Defines the filtering type for this negato
    ::fwRenderOgre::Plane::FilteringEnumType m_filtering;
};

//------------------------------------------------------------------------------
// Inline functions

inline void SNegato3D::setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

//------------------------------------------------------------------------------

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SNEGATO3D_HPP__
