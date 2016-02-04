#ifndef __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
#define __VISUOGREADAPTOR_SVOLUMERENDER_HPP__

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgrePolygon.h>

#include <vector>

#include "visuOgreAdaptor/config.hpp"

namespace visuOgreAdaptor
{
class VISUOGREADAPTOR_CLASS_API SVolumeRender : public ::fwRenderOgre::IAdaptor,
                                                public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SVolumeRender)(::fwRenderOgre::IAdaptor) );

    VISUOGREADAPTOR_API SVolumeRender() throw();
    VISUOGREADAPTOR_API virtual ~SVolumeRender() throw();

protected:

    VISUOGREADAPTOR_API virtual void doStart() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doStop() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

    // use listener on camera node's parent to see if it has moved to call the update's function
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );

private:

    friend class CameraMotionListener;

    typedef std::vector< ::Ogre::Vector3 > Polygon;

    void createTransformService();

    void initSlices();

    void updateAllSlices();

    bool planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex,
                               const ::Ogre::Vector3& _edgePoint0, const ::Ogre::Vector3& _edgePoint1,
                               ::Ogre::Vector3& result) const;

    Polygon cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex, const unsigned closestVertexIndex) const;

    unsigned closestVertex(::Ogre::Plane& cameraPlane) const;

    void updateSlice(Polygon& _polygon, unsigned _sliceIndex);

    ::Ogre::SceneManager *m_sceneManager;

    ::Ogre::SceneNode *m_volumeSceneNode;

    ::Ogre::RenderQueue *m_sceneRenderQueue;

    ::Ogre::Camera *m_camera;

    ::Ogre::ManualObject* m_intersectingPolygons;

    ::Ogre::Vector3 m_worldSpaceCubePositions[8];

    // /!\ TODO: Hardcoded variables, must be removed later
    static const uint16_t s_NB_SLICES;
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
