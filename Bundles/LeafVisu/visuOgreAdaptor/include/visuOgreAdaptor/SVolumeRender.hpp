#ifndef __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
#define __VISUOGREADAPTOR_SVOLUMERENDER_HPP__

#include <array>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

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
     * @name Signals API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_DRAG_WIDGET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_DROP_WIDGET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_MOVE_CLIPPING_BOX_SLOT;
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

    /// Updates proxy geometry called before rendering.
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

    friend class CameraMotionListener;

//    struct Polygon
//    {
//        std::vector< ::Ogre::Vector3 > m_vertices;
//        std::vector< ::Ogre::Vector3 > m_textureUVW;
//    };
    enum CubeFace
    {
        X_NEGATIVE = 0,
        X_POSITIVE = 1,
        Y_NEGATIVE = 2,
        Y_POSITIVE = 3,
        Z_NEGATIVE = 4,
        Z_POSITIVE = 5
    };

    typedef std::vector< ::Ogre::Vector3 > Polygon;

    typedef std::array< unsigned, 4 > CubeFacePositionList;

    static const std::map< CubeFace, CubeFacePositionList> s_cubeFaces;

    /// Get the face's image positions.
    std::array< ::Ogre::Vector3, 4 > getFacePositions(CubeFace _faceName) const;

    /// Get the center of a clipping box face
    ::Ogre::Vector3 getFaceCenter(CubeFace _faceName) const;

    /// Returns the clipping box's image space positions.
    std::array< ::Ogre::Vector3, 8 > getClippingBoxPositions() const;

    /// Highlight a clipping box face.
    void selectFace(CubeFace _faceName);

    /// Unhighlight face.
    void deselectFace();

    /// Scale the volume based on the image's spacing.
    void scaleCube(const ::fwData::Image::SpacingType& spacing);

    /// Initializes the manual object containing the slices.
    void initSlices();

    /// Creates the widget objects and scene nodes.
    void initWidgets();

    /// Updates all the proxy geometry.
    void updateAllSlices();

    /// Updates a slice's mesh.
    void updateSlice(const Polygon& _polygon, const unsigned _sliceIndex);

    /// Recomputes the intersection between the volume and it's clipping box.
    void updateClippingCube();

    /// Updates the widget's positions based on the clipping box.
    void updateWidgets();

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

    /**
     * @brief Drags a widget toward a screen position.
     *
     * @param _pickedWidget picked widget.
     * @param _screenX cursor's horizontal position.
     * @param _screenY cursor's vertical position.
     */
    void widgetPicked(::Ogre::MovableObject *_pickedWidget, int _screenX, int _screenY);

    /// Drops the currently selected widget.
    void widgetReleased();

    /**
     * @brief Translates the clipping box along the screen's axes.
     *
     * @param x  cursor horizontal position before move
     * @param y  cursor vertical position before move
     * @param dx displacement along the horizontal axis.
     * @param dy displacement along the vertical axis.
     */
    void moveClippingBox(int x, int y, int dx, int dy);

    /// Maps widget objects to their scene node and to a cube face.
    std::map< ::Ogre::MovableObject *, std::pair< CubeFace, ::Ogre::SceneNode * > >  m_widgets;

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

    /// Object used to display the clipping box.
    ::Ogre::ManualObject *m_boundingBox;

    /// Oject holding the highlighted face geometry.
    ::Ogre::ManualObject *m_selectedFace;

    /// Widget currently being dragged.
    ::Ogre::Entity *m_selectedWidget;

    /// 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

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

    /// Image aligned clipping box.
    ::Ogre::Vector3 m_clippingCube[2] = {
        ::Ogre::Vector3(0, 0, 0),
        ::Ogre::Vector3(1, 1, 1)
    };

    /// Intersection between the image and the clipping box.
    ::Ogre::Vector3 m_clippedImagePositions[8];
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
