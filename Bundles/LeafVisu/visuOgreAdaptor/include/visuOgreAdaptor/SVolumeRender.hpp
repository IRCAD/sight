#ifndef __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
#define __VISUOGREADAPTOR_SVOLUMERENDER_HPP__

#include <array>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/TransferFunction.hpp>
#include <fwRenderOgre/ui/VRWidget.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgrePolygon.h>
#include <OGRE/OgreTexture.h>

#include <vector>

#include "visuOgreAdaptor/config.hpp"

namespace visuOgreAdaptor
{
class VISUOGREADAPTOR_CLASS_API SVolumeRender : public ::fwRenderOgre::IAdaptor,
                                                public ::fwRenderOgre::ITransformable,
                                                public ::fwComEd::helper::MedicalImageAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SVolumeRender)(::fwRenderOgre::IAdaptor) );

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SVolumeRender() throw();

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SVolumeRender() throw();

protected:

    /// Starts the service initializes scene objects.
    VISUOGREADAPTOR_API virtual void doStart() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doStop() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );

    /// Configures this service.
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );

    /// Slot called on TF update.
    VISUOGREADAPTOR_API virtual void updatingTFPoints();

    /// Slot called on TF window update.
    VISUOGREADAPTOR_API virtual void updatingTFWindowing(double window, double level);

    /// Get object/service connections.
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

private:

    /// Camera listener, updates proxy geometry on camera move.
    class CameraMotionListener : public Ogre::Camera::Listener
    {
    public:

        /// Constructor.
        CameraMotionListener(SVolumeRender *parent);

        /// Called before scene render.
        void cameraPreRenderScene(Ogre::Camera *);

    private:
        /// The volume to update.
        ::visuOgreAdaptor::SVolumeRender *m_parent;

    };

    typedef std::vector< ::Ogre::Vector3 > Polygon;

    /// Maps each cube faces to 4 vertex indices.
    static const ::fwRenderOgre::ui::VRWidget::CubeFacePositionsMap s_cubeFaces;

    /// Scale the volume based on the image's spacing.
    void scaleCube(const ::fwData::Image::SpacingType& spacing);

    /// Initializes the manual object containing the slices.
    void initSlices();

    /// Updates all the proxy geometry.
    void updateAllSlices();

    /// Updates a slice's mesh.
    void updateSlice(const Polygon& _polygon, const unsigned _sliceIndex);

    /// Returns the bounding cube's vertex who's closest to the camera.
    unsigned closestVertexIndex(const ::Ogre::Plane& _cameraPlane) const;

    /// Adds the intersection (if it exists) between a plane and a segment in the _result polygon.
    bool planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex,
                               const unsigned _edgeVertexIndex0, const unsigned _edgeVertexIndex1,
                               Polygon& _result) const;

    /// Returns the intersection between the bounding cube and a plane.
    Polygon cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex, const unsigned _closestVertexIndex) const;

    /// Slot called when a new image is loaded.
    void newImage();

    /// Creates widgets and connects its slots to interactor signals.
    void initWidgets();

    /// 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// TF texture used for rendering.
    ::fwRenderOgre::TransferFunction m_gpuTF;

    /// This object's scene manager.
    ::Ogre::SceneManager *m_sceneManager;

    /// This object's scene node.
    ::Ogre::SceneNode *m_volumeSceneNode;

    /// The scene manager's render queue.
    ::Ogre::RenderQueue *m_sceneRenderQueue;

    /// Camera used for rendering.
    ::Ogre::Camera *m_camera;

    /// Object containing the proxy geometry used for slice based VR.
    ::Ogre::ManualObject *m_intersectingPolygons;

    /// Widgets used for clipping.
    ::fwRenderOgre::ui::VRWidget::sptr m_widgets;

    /// Sampling rate.
    uint16_t m_nbSlices;

    /// Image local and texture coordinates /!\ This order matters to our intersection algorithm.
    const ::Ogre::Vector3 m_imagePositions[8] = {
        ::Ogre::Vector3(1, 1, 1),
        ::Ogre::Vector3(1, 0, 1),
        ::Ogre::Vector3(1, 1, 0),
        ::Ogre::Vector3(0, 1, 1),
        ::Ogre::Vector3(0, 0, 1),
        ::Ogre::Vector3(1, 0, 0),
        ::Ogre::Vector3(0, 1, 0),
        ::Ogre::Vector3(0, 0, 0)
    };

    /// Intersection between the image and the clipping box.
    ::Ogre::Vector3 m_clippedImagePositions[8];
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
