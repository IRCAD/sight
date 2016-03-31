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

    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_DRAG_WIDGET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_DROP_WIDGET_SLOT;

    VISUOGREADAPTOR_API SVolumeRender() throw();
    VISUOGREADAPTOR_API virtual ~SVolumeRender() throw();

protected:

    VISUOGREADAPTOR_API virtual void doStart() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doStop() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

    // use listener on camera node's parent to see if it has moved to call the update's function
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );

    VISUOGREADAPTOR_API virtual void updatingTFPoints();
    VISUOGREADAPTOR_API virtual void updatingTFWindowing(double window, double level);

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
        Z_POSITIVE = 0,
        Z_NEGATIVE = 1,
        Y_POSITIVE = 2,
        Y_NEGATIVE = 3,
        X_POSITIVE = 4,
        X_NEGATIVE = 5
    };

    typedef std::vector< ::Ogre::Vector3 > Polygon;

    typedef std::array< unsigned, 4 > CubeFacePositionList;

    static const std::map< CubeFace, CubeFacePositionList> s_cubeFaces;

    std::array< ::Ogre::Vector3, 4 > getFacePositions(CubeFace _faceName) const;

    ::Ogre::Vector3 getFaceCenter(CubeFace _faceName) const;

    std::array< ::Ogre::Vector3, 8 > getClippingBoxPositions() const;

    void selectFace(CubeFace _faceName);

    void deselectFace();

    void scaleCube(const ::fwData::Image::SpacingType& spacing);

    void initSlices();

    void initWidgets();

    void updateAllSlices();

    void updateSlice(const Polygon& _polygon, const unsigned _sliceIndex);

    void updateClippingCube();

    void updateWidgets();

    unsigned closestVertexIndex(const ::Ogre::Plane& _cameraPlane) const;

    bool planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex,
                               const unsigned _edgeVertexIndex0, const unsigned _edgeVertexIndex1,
                               Polygon& _result) const;

    Polygon cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex, const unsigned _closestVertexIndex) const;

    void newImage();

    void widgetPicked(::Ogre::MovableObject *_pickedWidget, int _screenX, int _screenY);

    void widgetReleased();

    std::map< ::Ogre::MovableObject *, std::pair< CubeFace, ::Ogre::SceneNode * > >  m_widgets;

    ::fwRenderOgre::TransferFunction m_gpuTF;

    ::Ogre::SceneManager *m_sceneManager;

    ::Ogre::SceneNode *m_volumeSceneNode;

    ::Ogre::RenderQueue *m_sceneRenderQueue;

    ::Ogre::Camera *m_camera;

    ::Ogre::ManualObject *m_intersectingPolygons;

    ::Ogre::ManualObject *m_boundingBox;

    ::Ogre::ManualObject *m_selectedFace;

    ::Ogre::Entity *m_sphereEntity;

    ::Ogre::TexturePtr m_3DOgreTexture;

    ::Ogre::Vector3 m_oldPos;

    uint16_t m_nbSlices;

    const ::Ogre::Vector3 m_imagePositions[8] = {
        ::Ogre::Vector3(1, 1, 1),
        ::Ogre::Vector3(1, 0, 1),
        ::Ogre::Vector3(1, 1, 0),
        ::Ogre::Vector3(0, 1, 1),
        ::Ogre::Vector3(0, 0, 1),
        ::Ogre::Vector3(1, 0, 0),
        ::Ogre::Vector3(0, 1, 0),
        ::Ogre::Vector3(0, 0, 0)
    }; // /!\ Order matters

    ::Ogre::Vector3 m_clippingCube[2] = {
        ::Ogre::Vector3(0, 0, 0),
        ::Ogre::Vector3(1, 1, 1)
    }; // aligned with image, defined in image space

    ::Ogre::Vector3 m_clippedImagePositions[8];
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
