#include "visuOgreAdaptor/SVolumeRender.hpp"

#include <boost/lexical_cast.hpp>

#include <fwData/Image.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgrePlane.h>
#include <OGRE/OgreSceneNode.h>

#include <sstream>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

namespace visuOgreAdaptor
{

class CameraMotionListener : public Ogre::Camera::Listener
{
public:
    CameraMotionListener(SVolumeRender *parent) :
        m_parent(parent)
    {
    }

    void cameraPreRenderScene(Ogre::Camera *)
    {
        m_parent->doUpdate();
    }

private:
    ::visuOgreAdaptor::SVolumeRender *m_parent;

};

// /!\ TODO: Hardcoded variables, must be removed later
const uint16_t SVolumeRender::s_NB_SLICES = 1024;

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_volumeSceneNode      (nullptr),
    m_sceneRenderQueue     (nullptr),
    m_camera               (nullptr),
    m_intersectingPolygons (nullptr)
{
}

//-----------------------------------------------------------------------------

SVolumeRender::~SVolumeRender() throw()
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::doConfigure() throw ( ::fwTools::Failed )
{
    SLM_ASSERT("No config tag", m_configuration->getName() == "config");

    if(m_configuration->hasAttribute(""))
    {

    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStart() throw ( ::fwTools::Failed )
{
    m_sceneManager = this->getSceneManager();
    m_volumeSceneNode = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    m_sceneRenderQueue = m_sceneManager->getRenderQueue();
    m_camera = m_sceneManager->getCamera("PlayerCam");

//    ::Ogre::MovableObject *movableCamera = dynamic_cast< ::Ogre::MovableObject * >(m_camera);
    m_camera->addListener(new CameraMotionListener(this));

    m_volumeSceneNode->setPosition(0, 0, 0);

    m_intersectingPolygons = m_sceneManager->createManualObject("Slices");

    initSlices();
    updateAllSlices();
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStop() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::doUpdate() throw ( ::fwTools::Failed )
{
    updateAllSlices();
}

//-----------------------------------------------------------------------------

void SVolumeRender::doSwap() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::createTransformService()
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::initSlices()
{
    if(m_intersectingPolygons)
    {
        m_sceneManager->destroyManualObject(m_intersectingPolygons);
    }

    m_intersectingPolygons = m_sceneManager->createManualObject("Slices");

    // create s_NB_SLICES slices
    m_intersectingPolygons->estimateVertexCount(3 * 6 * s_NB_SLICES);

    for(uint16_t sliceNumber = 0; sliceNumber < s_NB_SLICES; ++ sliceNumber)
    {
        m_intersectingPolygons->begin("test", ::Ogre::RenderOperation::OT_TRIANGLE_LIST);

        // initialize slice with 6 triangles
        for(unsigned i = 0; i < 6 ; ++ i)
        {
            m_intersectingPolygons->position(0, 0, 0);
            m_intersectingPolygons->position(0, 0, 0);
            m_intersectingPolygons->position(0, 0, 0);
        }
        m_intersectingPolygons->end();

        ::Ogre::Renderable *slice = m_intersectingPolygons->getSection(sliceNumber);

        m_sceneRenderQueue->addRenderable(
                    slice,
                    ::Ogre::RENDER_QUEUE_MAIN,
                    sliceNumber //Priority level
        );
    }
    m_volumeSceneNode->attachObject(m_intersectingPolygons);
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateAllSlices()
{
    ::Ogre::Vector3 planeNormal    = m_camera->getRealDirection();
    ::Ogre::Vector3 cameraPosition = m_camera->getRealPosition();

    planeNormal.normalise();

    // set transform for testing only
    m_transform = ::Ogre::Matrix4::IDENTITY;
//    m_transform.setScale(::Ogre::Vector3(2.f, 2.f, 2.f));
//    ::Ogre::Matrix4 rot(::Ogre::Quaternion(::Ogre::Radian(45), ::Ogre::Vector3(0.5, 0.5, 0.5)));
//    m_transform = rot * m_transform;

    for(int i = 0; i < 8; ++i)
    {
        m_worldSpaceCubePositions[i] = m_transform * m_boundingCubePositions[i];
    }

    ::Ogre::Plane cameraPlane(planeNormal, cameraPosition);

    unsigned closest = closestVertex(cameraPlane);

    ::Ogre::Vector3 closestVertexToCamera  = m_worldSpaceCubePositions[closest];
    ::Ogre::Vector3 furthestVertexToCamera = m_worldSpaceCubePositions[7];

    float closestVtxDistanceToCamera  = cameraPlane.getDistance(closestVertexToCamera);
    float furthestVtxDistanceToCamera = cameraPlane.getDistance(furthestVertexToCamera);

    float distanceBetweenClosestAndFuthest = std::abs(closestVtxDistanceToCamera - furthestVtxDistanceToCamera);

    float distanceBetweenSlices =  distanceBetweenClosestAndFuthest / s_NB_SLICES;

    ::Ogre::Vector3 planeVertex = furthestVertexToCamera - planeNormal * distanceBetweenSlices;

    for(uint16_t sliceNumber = s_NB_SLICES - 1; sliceNumber > 0; --sliceNumber)
    {
        Polygon intersections = cubePlaneIntersection(planeNormal, planeVertex, closest);

        if(intersections.size() >= 3)
        {
            updateSlice(intersections, sliceNumber);
        }

        planeVertex -= planeNormal * distanceBetweenSlices;
    }

}

unsigned SVolumeRender::closestVertex(::Ogre::Plane& cameraPlane) const
{
    int min = 0;
    float minDist = cameraPlane.getDistance(m_worldSpaceCubePositions[0]);

    for(int i = 1; i < 8; ++ i)
    {
        float dist = cameraPlane.getDistance(m_worldSpaceCubePositions[i]);

        if(dist < minDist)
        {
            min = i;
            minDist = dist;
        }
    }

    return min;
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSlice(Polygon& _polygon ,unsigned _sliceIndex)
{
    ::Ogre::Vector3 center(0, 0, 0);

    for(unsigned i = 0; i < _polygon.size(); ++ i)
    {
        center += _polygon[i];
    }
    center /= static_cast<float>(_polygon.size());

    const size_t nbVertices = _polygon.size();

    m_intersectingPolygons->beginUpdate(_sliceIndex);
    {
        for(unsigned i = 0; i < nbVertices ; ++ i)
        {
            m_intersectingPolygons->position(center);
            m_intersectingPolygons->position(_polygon[i]);
            m_intersectingPolygons->position(_polygon[(i + 1) % nbVertices]);
        }
    }
    m_intersectingPolygons->end();
}

//-----------------------------------------------------------------------------

bool SVolumeRender::planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex,
                                          const ::Ogre::Vector3& _edgePoint0, const ::Ogre::Vector3& _edgePoint1,
                                          ::Ogre::Vector3& result) const
{
    bool planeIntersectsEdge = false;

    if(_planeNormal.dotProduct(_edgePoint1 - _edgePoint0) != 0) // plane and edge are not parallel
    {
        // intersectPoint represents the intersection point on the parametric line
        // S(r) = _egdePoint0 + r * (_edgePoint1 - _edgePoint0)
        ::Ogre::Real intersectPoint = _planeNormal.dotProduct(_planeVertex - _edgePoint0)
                / _planeNormal.dotProduct(_edgePoint1 - _edgePoint0);

        result = _edgePoint0 + intersectPoint * (_edgePoint1 - _edgePoint0);

        planeIntersectsEdge = (intersectPoint > 0) && (intersectPoint < 1);
    } //TODO: else check if _planeVertex == _edgePoint0 or _edgePoint1

    return planeIntersectsEdge;
}

//-----------------------------------------------------------------------------

SVolumeRender::Polygon SVolumeRender::cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal, const ::Ogre::Vector3& _planeVertex, const unsigned closestVertexIndex) const
{
    Polygon intersections;
    intersections.reserve(6); // there is a maximum of 6 intersections

    const unsigned sequences[8][8] = {
        { 0, 1, 2, 3, 4, 5, 6, 7 },
        { 1, 4, 5, 0, 3, 7, 2, 6 },
        { 2, 6, 0, 5, 7, 3, 1, 4 },
        { 3, 0, 6, 4, 1, 2, 7, 5 },
        { 4, 3, 7, 1 ,0, 6, 5, 2 },
        { 5, 2, 1, 7, 6, 0, 4, 3 },
        { 6, 7, 3, 2, 5, 4, 0, 1 },
        { 7, 5, 4, 6, 2, 1, 3, 0 }
    };

//    const unsigned paths[3][5] = { { 0, 1 ,4 ,7, 5 } ,
//                                   { 0, 2, 5, 7, 6 } ,
//                                   { 0, 3, 6, 7, 4 } };
    const unsigned lastEdge[3] = { 5, 6, 4 };

    for(int pathNum = 0; pathNum < 3; ++ pathNum)
    {
        bool hasIntersection = false;

        ::Ogre::Vector3 intersection;
        ::Ogre::Vector3 v1, v2;

        const unsigned path[] = {
            sequences[closestVertexIndex][0],
            sequences[closestVertexIndex][pathNum + 1],
            sequences[closestVertexIndex][pathNum + 4],
            sequences[closestVertexIndex][7]
        };

        for(int i = 0; i < 3 && !hasIntersection; ++ i)
        {
            v1 = m_worldSpaceCubePositions[ path[i    ] ];
            v2 = m_worldSpaceCubePositions[ path[i + 1] ];

            hasIntersection = planeEdgeIntersection(_planeNormal, _planeVertex, v1, v2, intersection);

            if(hasIntersection)
            {
                intersections.push_back(intersection);
            }
        }

        const unsigned lastEdgeIndex = sequences[ closestVertexIndex ][ lastEdge[pathNum] ];

        v1 = m_worldSpaceCubePositions[ path[ 1 ]     ];
        v2 = m_worldSpaceCubePositions[ lastEdgeIndex ];

        if( hasIntersection && planeEdgeIntersection(_planeNormal, _planeVertex, v1, v2, intersection) )
        {
            intersections.push_back(intersection);
        }
    }

//    orderVertices(intersections, _planeNormal);

    return intersections;
}

//-----------------------------------------------------------------------------

}
