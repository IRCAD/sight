#include "visuOgreAdaptor/SVolumeRender.hpp"

#include <boost/lexical_cast.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePlane.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTechnique.h>

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

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWIMAGE_SLOT   = "newImage";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_volumeSceneNode      (nullptr),
    m_sceneRenderQueue     (nullptr),
    m_camera               (nullptr),
    m_intersectingPolygons (nullptr),
    m_nbSlices             (512)
{
    this->installTFSlots(this);
    newSlot(s_NEWIMAGE_SLOT, &SVolumeRender::newImage, this);
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

fwServices::IService::KeyConnectionsType SVolumeRender::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_NEWIMAGE_SLOT ) );
    return connections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStart() throw ( ::fwTools::Failed )
{
    m_sceneManager     = this->getSceneManager();
    m_volumeSceneNode  = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    m_sceneRenderQueue = m_sceneManager->getRenderQueue();
    m_camera           = m_sceneManager->getCamera("PlayerCam");

//    ::Ogre::MovableObject *movableCamera = dynamic_cast< ::Ogre::MovableObject * >(m_camera);
    m_camera->addListener(new CameraMotionListener(this));

    m_volumeSceneNode->setPosition(0, 0, 0);

    m_3DOgreTexture = ::Ogre::TextureManager::getSingletonPtr()->create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    ::Ogre::MaterialPtr volumeMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("SliceVolume");
    ::Ogre::TextureUnitState *texState = volumeMtl->getTechnique(0)->getPass(0)->getTextureUnitState("image");

    SLM_ASSERT("'image' texture unit is not found", texState);

    texState->setTexture(m_3DOgreTexture);

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

    m_intersectingPolygons = m_sceneManager->createManualObject("__VolumeRenderSlices__");

    // create m_nbSlices slices
    m_intersectingPolygons->estimateVertexCount(6 * m_nbSlices);

    for(uint16_t sliceNumber = 0; sliceNumber < m_nbSlices; ++ sliceNumber)
    {
        m_intersectingPolygons->begin("SliceVolume", ::Ogre::RenderOperation::OT_TRIANGLE_STRIP);
        {
            // initialize slice with a 4 triangle strip
            for(unsigned i = 0; i < 6 ; ++ i)
            {
                m_intersectingPolygons->position(0, 0, 0);
                m_intersectingPolygons->textureCoord(0, 0, 0);
            }
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
    // set transform for testing only
    m_transform = ::Ogre::Matrix4::IDENTITY;

    // intersections are done in world space
    for(int i = 0; i < 8; ++i)
    {
        m_worldSpaceCubePositions[i] = m_transform * m_boundingCubePositions[i];
    }

    ::Ogre::Vector3 planeNormal    = m_camera->getRealDirection();
    ::Ogre::Vector3 cameraPosition = m_camera->getRealPosition();

    planeNormal.normalise();

    ::Ogre::Plane cameraPlane(planeNormal, cameraPosition);

    // get the cube's closest and furthest vertex to the camera
    unsigned closestVtxIndex = closestVertexIndex(cameraPlane);

    auto comp = [&cameraPlane](const ::Ogre::Vector3& v1, const ::Ogre::Vector3& v2)
            { return cameraPlane.getDistance(v1) < cameraPlane.getDistance(v2); };

    ::Ogre::Vector3 furthestVtx = *std::max_element(m_worldSpaceCubePositions, m_worldSpaceCubePositions + 8, comp);
    ::Ogre::Vector3 closestVtx  = m_worldSpaceCubePositions[ closestVtxIndex ];

    // get distance between slices
    float closestVtxDistance  = cameraPlane.getDistance(closestVtx);
    float furthestVtxDistance = cameraPlane.getDistance(furthestVtx);

    float firstToLastSliceDistance = std::abs(closestVtxDistance - furthestVtxDistance);

    float distanceBetweenSlices =  firstToLastSliceDistance / m_nbSlices;

    // set first plane
    ::Ogre::Vector3 planeVertex = furthestVtx - planeNormal * distanceBetweenSlices;

    // compute all slices
    for(uint16_t sliceNumber = m_nbSlices - 1; sliceNumber > 0; --sliceNumber)
    {
        Polygon intersections = cubePlaneIntersection(planeNormal, planeVertex, closestVtxIndex);

        if(intersections.m_vertices.size() >= 3)
        {
            updateSlice(intersections, sliceNumber);
        }

        // set next plane
        planeVertex -= planeNormal * distanceBetweenSlices;
    }
}

//-----------------------------------------------------------------------------

unsigned SVolumeRender::closestVertexIndex(const ::Ogre::Plane& _cameraPlane) const
{
    int min = 0;
    float minDist = _cameraPlane.getDistance(m_worldSpaceCubePositions[0]);

    for(int i = 1; i < 8; ++ i)
    {
        float dist = _cameraPlane.getDistance(m_worldSpaceCubePositions[i]);

        if(dist < minDist)
        {
            min = i;
            minDist = dist;
        }
    }

    return min;
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateSlice(const Polygon& _polygon ,const unsigned _sliceIndex)
{
    const size_t nbVertices = _polygon.m_vertices.size();

    // triangulate polygon into a triangle strip
    m_intersectingPolygons->beginUpdate(_sliceIndex);
    {
        m_intersectingPolygons->position( _polygon.m_vertices[1] );
        m_intersectingPolygons->textureCoord( _polygon.m_textureUVW[1] );
        m_intersectingPolygons->position( _polygon.m_vertices[0] );
        m_intersectingPolygons->textureCoord( _polygon.m_textureUVW[0] );
        m_intersectingPolygons->position( _polygon.m_vertices[2] );
        m_intersectingPolygons->textureCoord( _polygon.m_textureUVW[2] );

        for(unsigned i = 0; i < nbVertices - 3; ++ i)
        {
            m_intersectingPolygons->position( _polygon.m_vertices[nbVertices - 1 - i] );
            m_intersectingPolygons->textureCoord( _polygon.m_textureUVW[nbVertices - 1 - i] );

            if(i + 3 < nbVertices - 1)
            {
                m_intersectingPolygons->position( _polygon.m_vertices[i + 3] );
                m_intersectingPolygons->textureCoord( _polygon.m_textureUVW[i + 3] );
            }
        }
    }
    m_intersectingPolygons->end();
}

//-----------------------------------------------------------------------------

bool SVolumeRender::planeEdgeIntersection(const ::Ogre::Vector3& _planeNormal,
                                          const ::Ogre::Vector3& _planeVertex,
                                          const unsigned _edgeVertexIndex0,
                                          const unsigned _edgeVertexIndex1,
                                          Polygon& _result
                                         ) const
{
    bool planeIntersectsEdge = false;

    const ::Ogre::Vector3 edgePoint0 = m_worldSpaceCubePositions[_edgeVertexIndex0];
    const ::Ogre::Vector3 edgePoint1 = m_worldSpaceCubePositions[_edgeVertexIndex1];

    const ::Ogre::Vector3 uvw0 = m_boundingCubePositions[_edgeVertexIndex0];
    const ::Ogre::Vector3 uvw1 = m_boundingCubePositions[_edgeVertexIndex1];

    if(_planeNormal.dotProduct(edgePoint1 - edgePoint0) != 0) // plane and edge are not parallel
    {
        // intersectPoint represents the intersection point on the parametric line
        // S(r) = _egdePoint0 + r * (_edgePoint1 - _edgePoint0)
        ::Ogre::Real intersectPoint = _planeNormal.dotProduct(_planeVertex - edgePoint0)
                / _planeNormal.dotProduct(edgePoint1 - edgePoint0);

        planeIntersectsEdge = (intersectPoint >= 0) && (intersectPoint <= 1);

        if(planeIntersectsEdge)
        {
            const ::Ogre::Vector3 position = edgePoint0 + intersectPoint * (edgePoint1 - edgePoint0);
            const ::Ogre::Vector3 uvw      = uvw0       + intersectPoint * (uvw1       - uvw0      );

            _result.m_vertices.push_back(position);
            _result.m_textureUVW.push_back(uvw);
        }
    }
    //TODO: else check if _planeVertex == _edgePoint0 or _edgePoint1

    return planeIntersectsEdge;
}

//-----------------------------------------------------------------------------

SVolumeRender::Polygon SVolumeRender::cubePlaneIntersection(const ::Ogre::Vector3& _planeNormal,
                                                            const ::Ogre::Vector3& _planeVertex,
                                                            const unsigned _closestVertexIndex) const
{
    Polygon intersections;
    intersections.m_vertices.reserve(6); // there is a maximum of 6 intersections
    intersections.m_textureUVW.reserve(6);

    // sequence[i] represents the order in which we must traverse
    // the cube's edges starting with the ith vertex
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
        unsigned v1, v2;

        const unsigned path[] = {
            sequences[_closestVertexIndex][0],
            sequences[_closestVertexIndex][pathNum + 1],
            sequences[_closestVertexIndex][pathNum + 4],
            sequences[_closestVertexIndex][7],
            sequences[_closestVertexIndex][ lastEdge[ pathNum ] ]
        };

        for(int i = 0; i < 3 && !hasIntersection; ++ i)
        {
            v1 = path[i    ];
            v2 = path[i + 1];

            hasIntersection = planeEdgeIntersection(_planeNormal, _planeVertex, v1, v2, intersections);
        }

        v1 = path[1];
        v2 = path[4];

        if( hasIntersection )
        {
            planeEdgeIntersection(_planeNormal, _planeVertex, v1, v2, intersections);
        }
    }

    return intersections;
}

//-----------------------------------------------------------------------------

void SVolumeRender::newImage()
{
    this->updateImageInfos(this->getObject< ::fwData::Image >());

    this->getRenderService()->makeCurrent();

    ::fwData::Image::sptr image = this->getImage();

    // Retrieves or creates the slice index fields
    this->updateImageInfos(image);

    ::fwRenderOgre::Utils::convertImageForNegato(m_3DOgreTexture.get(), image);

    this->requestRender();
}

//-----------------------------------------------------------------------------

}
