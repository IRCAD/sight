/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SNEGATO2D_HPP__
#define __VISUOGREADAPTOR_SNEGATO2D_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/Plane.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Adaptor to display a 2D negato
 */
class VISUOGREADAPTOR_CLASS_API SNegato2D : public ::fwRenderOgre::IAdaptor,
                                            public ::fwDataTools::helper::MedicalImageAdaptor
{
public:

    typedef ::fwDataTools::helper::MedicalImageAdaptor::Orientation OrientationMode;

    fwCoreServiceClassDefinitionsMacro( (SNegato2D)(::fwRenderOgre::IAdaptor) );

    /// Constructor.
    VISUOGREADAPTOR_API SNegato2D() throw();
    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SNegato2D() throw();
    /// Sets the filtering type
    VISUOGREADAPTOR_API void setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering );

protected:
    /// Instanciates the texture, material, pass and texture unit state
    /// Sets the connection between attached data and the receive slot
    VISUOGREADAPTOR_API void doStart() throw(fwTools::Failed);
    /// Disconnects the attached data from the receive slot
    VISUOGREADAPTOR_API void doStop() throw(fwTools::Failed);
    /// Requests a rendering of the scene.
    VISUOGREADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /**
     * @brief Configures the service
     * @code{.xml}
        <adaptor uid="SNegato2D" class="::visuOgreAdaptor::SNegato2D" objectId="image">
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

    /// updates image slice index for the current fragment program.
    void updateShaderSliceIndexParameter();

    /// Makes the plane processes his mesh
    void createPlane(const ::fwData::Image::SpacingType& _spacing);

    /// Adapts the camera to the width and height of the displayed plane.
    void updateCameraWindowBounds();

    /// Ogre texture which will be displayed on the negato
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view)
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// The plane on which we will apply our texture
    ::fwRenderOgre::Plane* m_plane;

    /// The scene node allowing to move the entire negato
    ::Ogre::SceneNode* m_negatoSceneNode;

    /// Defines the filtering type for this negato
    ::fwRenderOgre::Plane::FilteringEnumType m_filtering;

    /// Stores the current slice index for each axis.
    std::vector<float> m_currentSliceIndex;

    /// Connections.
    ::fwCom::Connection m_connection;
};

//------------------------------------------------------------------------------
// Inline functions

inline void SNegato2D::setFiltering( ::fwRenderOgre::Plane::FilteringEnumType _filtering )
{
    m_filtering = _filtering;
}

//------------------------------------------------------------------------------
} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SNEGATO2D_HPP__
