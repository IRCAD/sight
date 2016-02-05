#ifndef __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
#define __VISUOGREADAPTOR_SVOLUMERENDER_HPP__

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

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

    VISUOGREADAPTOR_API SVolumeRender() throw();
    VISUOGREADAPTOR_API virtual ~SVolumeRender() throw();

protected:

    VISUOGREADAPTOR_API virtual void doStart() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doStop() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

    // use listener on camera node's parent to see if it has moved to call the update's function
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );

    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

private:

    friend class CameraMotionListener;

    struct Polygon
    {
        std::vector< ::Ogre::Vector3 > m_vertices;
        std::vector< ::Ogre::Vector3 > m_textureUVW;
    };

    void createTransformService();

    void initSlices();

    void updateAllSlices();

    void updateSlice(const Polygon& _polygon, const unsigned _sliceIndex);

    unsigned closestVertexIndex(const ::Ogre::Plane& _cameraPlane) const;

    bool planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex,
                               const unsigned _edgeVertexIndex0, const unsigned _edgeVertexIndex1,
                               Polygon& _result) const;

    Polygon cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex, const unsigned _closestVertexIndex) const;

    void newImage();

    ::Ogre::SceneManager *m_sceneManager;

    ::Ogre::SceneNode *m_volumeSceneNode;

    ::Ogre::RenderQueue *m_sceneRenderQueue;

    ::Ogre::Camera *m_camera;

    ::Ogre::ManualObject *m_intersectingPolygons;

    ::Ogre::TexturePtr m_3DOgreTexture;

    ::Ogre::Vector3 m_worldSpaceCubePositions[8];

    uint16_t m_nbSlices;

    ::Ogre::Vector3 m_boundingCubePositions[8] = {
        ::Ogre::Vector3(1, 1, 1),
        ::Ogre::Vector3(1, 0, 1),
        ::Ogre::Vector3(1, 1, 0),
        ::Ogre::Vector3(0, 1, 1),
        ::Ogre::Vector3(0, 0, 1),
        ::Ogre::Vector3(1, 0, 0),
        ::Ogre::Vector3(0, 1, 0),
        ::Ogre::Vector3(0, 0, 0)
    }; // /!\ Order matters
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
