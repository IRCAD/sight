#include "visuOgreAdaptor/SVolumeRender.hpp"

#include <algorithm>

#include <boost/algorithm/clamp.hpp>
#include <boost/lexical_cast.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>

#include <numeric>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePlane.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreRay.h>

#include <sstream>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SVolumeRender, ::fwData::Image);

//-----------------------------------------------------------------------------

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

SVolumeRender::CameraMotionListener::CameraMotionListener(SVolumeRender *parent) :
    m_parent(parent)
{

}

//-----------------------------------------------------------------------------

void SVolumeRender::CameraMotionListener::cameraPreRenderScene(Ogre::Camera *)
{
    m_parent->updateAllSlices();
}

//-----------------------------------------------------------------------------

const std::map< SVolumeRender::CubeFace, SVolumeRender::CubeFacePositionList> SVolumeRender::s_cubeFaces = {
    { SVolumeRender::Z_POSITIVE, { 0, 1, 4, 3 } },
    { SVolumeRender::Z_NEGATIVE, { 2, 5, 7, 6 } },
    { SVolumeRender::Y_POSITIVE, { 0, 3, 6, 2 } },
    { SVolumeRender::Y_NEGATIVE, { 1, 4, 7, 5 } },
    { SVolumeRender::X_POSITIVE, { 0, 1, 5, 2 } },
    { SVolumeRender::X_NEGATIVE, { 3, 4, 7, 6 } }
};

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWIMAGE_SLOT   = "newImage";

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_DRAG_WIDGET_SLOT = "dragWidgetSlot";

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_DROP_WIDGET_SLOT = "dropWidgetSlot";

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_MOVE_CLIPPING_BOX_SLOT = "moveClippingBoxSlot";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_sceneManager         (nullptr),
    m_volumeSceneNode      (nullptr),
    m_sceneRenderQueue     (nullptr),
    m_camera               (nullptr),
    m_intersectingPolygons (nullptr),
    m_boundingBox          (nullptr),
    m_selectedFace         (nullptr),
    m_selectedWidget       (nullptr),
    m_nbSlices             (512)
{
    this->installTFSlots(this);
    newSlot(s_NEWIMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_DRAG_WIDGET_SLOT, &SVolumeRender::widgetPicked, this);
    newSlot(s_DROP_WIDGET_SLOT, &SVolumeRender::widgetReleased, this);
    newSlot(s_MOVE_CLIPPING_BOX_SLOT, &SVolumeRender::moveClippingBox, this);

    updateClippingCube();

    // set transform for testing only
    m_transform = ::Ogre::Matrix4::IDENTITY;
}

//-----------------------------------------------------------------------------

SVolumeRender::~SVolumeRender() throw()
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::doConfigure() throw ( ::fwTools::Failed )
{
    SLM_ASSERT("No config tag", m_configuration->getName() == "config");

//    if(m_configuration->hasAttribute(""))
//    {

//    }

    this->parseTFConfig(m_configuration);
}

//-----------------------------------------------------------------------------

void SVolumeRender::updatingTFPoints()
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();
    this->updateTransferFunction(this->getImage());

    m_gpuTF.updateTexture(tf);

    ::Ogre::MaterialPtr volumeMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("SliceVolume");
    ::Ogre::Pass *pass = volumeMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState *texTFState = pass->getTextureUnitState("transferFunction");

    texTFState->setTexture(m_gpuTF.getTexture());

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updatingTFWindowing(double window, double level)
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    m_gpuTF.updateTexture(tf);

    this->requestRender();
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
    this->updateImageInfos(this->getObject< ::fwData::Image >());
    this->updateTransferFunction(this->getImage());

    this->installTFConnections();

    m_sceneManager     = this->getSceneManager();
    m_volumeSceneNode  = m_sceneManager->getRootSceneNode()->createChildSceneNode();
    m_sceneRenderQueue = m_sceneManager->getRenderQueue();
    m_camera           = m_sceneManager->getCamera("PlayerCam");

    //    ::Ogre::MovableObject *movableCamera = dynamic_cast< ::Ogre::MovableObject * >(m_camera);
    m_camera->addListener(new CameraMotionListener(this));

    // Create textures
    m_3DOgreTexture = ::Ogre::TextureManager::getSingletonPtr()->create(
        this->getID() + "_Texture",
        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        true);

    m_gpuTF.createTexture(this->getID());

    ::Ogre::MaterialPtr volumeMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("SliceVolume");
    ::Ogre::Pass *pass = volumeMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState *tex3DState = pass->getTextureUnitState("image");
    ::Ogre::TextureUnitState *texTFState = pass->getTextureUnitState("transferFunction");

    SLM_ASSERT("'image' texture unit is not found", tex3DState);

    tex3DState->setTexture(m_3DOgreTexture);
    texTFState->setTexture(m_gpuTF.getTexture());

    initSlices();
    updateAllSlices();
    initWidgets();

    // This must be called to set the camera once all our geometry has been created
    this->getRenderService()->resetCameraCoordinates(m_layerID);
}

//-----------------------------------------------------------------------------

void SVolumeRender::doStop() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::doUpdate() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SVolumeRender::doSwap() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3, 4 > SVolumeRender::getFacePositions(SVolumeRender::CubeFace _faceName) const
{
    const CubeFacePositionList positionIndices = s_cubeFaces.at(_faceName);
    std::array< ::Ogre::Vector3, 4 > facePositions;

    auto BBpositions = getClippingBoxPositions();

    std::transform(positionIndices.begin(), positionIndices.end(), facePositions.begin(),
                   [&](unsigned i){ return BBpositions[i]; });

    return facePositions;
}

//-----------------------------------------------------------------------------

Ogre::Vector3 SVolumeRender::getFaceCenter(SVolumeRender::CubeFace _faceName) const
{
    const auto facePositions = getFacePositions(_faceName);
    return std::accumulate(facePositions.cbegin() + 1, facePositions.cend(), facePositions[0]) / 4.f;
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3, 8 > SVolumeRender::getClippingBoxPositions() const
{
    return {
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[0].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[1].z),
        ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[0].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[0].z),
        ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[0].z)
    };
}

//-----------------------------------------------------------------------------

void SVolumeRender::selectFace(SVolumeRender::CubeFace _faceName)
{
    m_selectedFace->beginUpdate(0);
    {
        std::array< ::Ogre::Vector3, 4 > facePositions = getFacePositions(_faceName);
        m_selectedFace->position(facePositions[1]);
        m_selectedFace->position(facePositions[0]);
        m_selectedFace->position(facePositions[2]);
        m_selectedFace->position(facePositions[3]);
    }
    m_selectedFace->end();

    m_volumeSceneNode->attachObject(m_selectedFace);
}

//-----------------------------------------------------------------------------

void SVolumeRender::deselectFace()
{
    if(m_selectedFace->isAttached())
    {
        m_volumeSceneNode->detachObject(m_selectedFace->getName());
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::scaleCube(const fwData::Image::SpacingType& spacing)
{
    const double width  = static_cast< double > (m_3DOgreTexture->getWidth() ) * spacing[0];
    const double height = static_cast< double > (m_3DOgreTexture->getHeight()) * spacing[1];
    const double depth  = static_cast< double > (m_3DOgreTexture->getDepth() ) * spacing[2];

    const double maxDim = std::max(width, std::max(height, depth));

    const ::Ogre::Vector3 scaleFactors(
                static_cast<float>(width  / maxDim),
                static_cast<float>(height / maxDim),
                static_cast<float>(depth  / maxDim));

    m_volumeSceneNode->setScale(scaleFactors);
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
    m_intersectingPolygons->estimateVertexCount(6 /** m_nbSlices*/ * 2);
    m_intersectingPolygons->setDynamic(true);

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

void SVolumeRender::initWidgets()
{
    // Create widget materials
    {
        ::Ogre::MaterialPtr sphereMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("Default");
        ::Ogre::MaterialPtr sphereHighlightMtl = sphereMtl->clone("SphereHighlight");

        sphereHighlightMtl->setAmbient(0.3f, 0.f, 0.f);
        sphereHighlightMtl->setDiffuse(0.5f, 0.1f, 0.1f, 1.f);

        ::Ogre::MaterialPtr frameMtl = sphereMtl->clone("Frame");
        frameMtl->setAmbient(1.f, 1.f, 1.f);
        frameMtl->setDiffuse(0.f, 0.f, 0.f, 1.f);
        frameMtl->setSpecular(0.f, 0.f, 0.f, 1.f);

        ::Ogre::MaterialPtr faceMtl = sphereMtl->clone("FaceHighlight");
        faceMtl->setAmbient(1.f, 1.f, 0.f);
        faceMtl->setDiffuse(0.f, 0.f, 0.f, 0.6f);
        faceMtl->setSpecular(0.f, 0.f, 0.f, 0.6f);
        faceMtl->setSceneBlending(::Ogre::SBT_TRANSPARENT_ALPHA);
        faceMtl->setDepthWriteEnabled(false);
    }

    m_boundingBox = m_sceneManager->createManualObject("__VolumeBB__");
    m_selectedFace = m_sceneManager->createManualObject("__VRSelectedFace__");

    updateClippingCube();

    const auto clippingBoxPositions = getClippingBoxPositions();

    m_boundingBox->begin("Frame", Ogre::RenderOperation::OT_LINE_LIST);
    {
        const unsigned edges[][2] = {
            { 0, 1 }, { 1, 4 }, { 4, 3 }, { 3, 0 },
            { 0, 2 }, { 1, 5 }, { 4, 7 }, { 3, 6 },
            { 2, 5 }, { 5, 7 }, { 7, 6 }, { 6, 2 }
        };

        for(unsigned i = 0; i < 12; ++ i)
        {
            m_boundingBox->position(clippingBoxPositions[edges[i][0]]);
            m_boundingBox->position(clippingBoxPositions[edges[i][1]]);
        }

        // Cross
        for(unsigned i = 0; i < 6; i += 2)
        {
            m_boundingBox->position(getFaceCenter(static_cast<CubeFace>(i  )));
            m_boundingBox->position(getFaceCenter(static_cast<CubeFace>(i+1)));
        }
    }
    m_boundingBox->end();

    m_volumeSceneNode->attachObject(m_boundingBox);

    m_selectedFace->begin("FaceHighlight", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    {
        for(unsigned i = 0; i < 4; ++ i)
        {
            m_selectedFace->position(0, 0, 0);
        }
    }
    m_selectedFace->end();

    // Render this last.
    m_selectedFace->setRenderQueueGroup(::Ogre::RENDER_QUEUE_BACKGROUND);

    // Create a pickable sphere for each cube face
    for(unsigned i = 0; i < 6; ++ i)
    {
        CubeFace currentFace = static_cast<CubeFace>(i);

        ::Ogre::Entity *newWidget = m_sceneManager->createEntity( ::Ogre::SceneManager::PT_SPHERE );
        newWidget->setMaterialName("Default");

        ::Ogre::SceneNode *widgetSceneNode = m_volumeSceneNode->createChildSceneNode();

        m_widgets[newWidget] = std::make_pair(currentFace, widgetSceneNode);

        ::Ogre::Vector3 faceCenter = getFaceCenter(currentFace);

        widgetSceneNode->setPosition(faceCenter);
        widgetSceneNode->setInheritScale(false);
        widgetSceneNode->setScale(0.0002f, 0.0002f, 0.0002f);

        widgetSceneNode->attachObject(newWidget);
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateAllSlices()
{    
    // intersections are done in object space
    const ::Ogre::Vector3 cameraPosition = m_volumeSceneNode->convertWorldToLocalPosition(m_camera->getRealPosition());
    ::Ogre::Vector3 planeNormal = m_volumeSceneNode->convertWorldToLocalDirection(m_camera->getRealDirection(), true);
    planeNormal.normalise();

    const ::Ogre::Plane cameraPlane(planeNormal, cameraPosition);

    // get the cube's closest and furthest vertex to the camera
    const unsigned closestVtxIndex = closestVertexIndex(cameraPlane);

    const auto comp = [&cameraPlane](const ::Ogre::Vector3& v1, const ::Ogre::Vector3& v2)
            { return cameraPlane.getDistance(v1) < cameraPlane.getDistance(v2); };

    const ::Ogre::Vector3 furthestVtx = *std::max_element(m_clippedImagePositions, m_clippedImagePositions + 8, comp);
    const ::Ogre::Vector3 closestVtx  = m_clippedImagePositions[ closestVtxIndex ];

    // get distance between slices
    const float closestVtxDistance  = cameraPlane.getDistance(closestVtx);
    const float furthestVtxDistance = cameraPlane.getDistance(furthestVtx);

    const float firstToLastSliceDistance = std::abs(closestVtxDistance - furthestVtxDistance);

    const float distanceBetweenSlices =  firstToLastSliceDistance / m_nbSlices;

    // set first plane
    ::Ogre::Vector3 planeVertex = furthestVtx - planeNormal * distanceBetweenSlices;

    // compute all slices
    for(uint16_t sliceNumber = 0; sliceNumber < m_nbSlices; ++ sliceNumber)
    {
        Polygon intersections = cubePlaneIntersection(planeNormal, planeVertex, closestVtxIndex);

        if(intersections.size() >= 3)
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
    float minDist = _cameraPlane.getDistance(m_clippedImagePositions[0]);

    for(int i = 1; i < 8; ++ i)
    {
        float dist = _cameraPlane.getDistance(m_clippedImagePositions[i]);

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
    const size_t nbVertices = _polygon.size();

    // triangulate polygon into a triangle strip
    m_intersectingPolygons->beginUpdate(_sliceIndex);
    {
        m_intersectingPolygons->position( _polygon[1] );
        m_intersectingPolygons->textureCoord( _polygon[1] );
        m_intersectingPolygons->position( _polygon[0] );
        m_intersectingPolygons->textureCoord( _polygon[0] );
        m_intersectingPolygons->position( _polygon[2] );
        m_intersectingPolygons->textureCoord( _polygon[2] );

        for(unsigned i = 0; i < nbVertices - 3; ++ i)
        {
            m_intersectingPolygons->position( _polygon[nbVertices - 1 - i] );
            m_intersectingPolygons->textureCoord( _polygon[nbVertices - 1 - i] );

            if(i + 3 < nbVertices - 1)
            {
                m_intersectingPolygons->position( _polygon[i + 3] );
                m_intersectingPolygons->textureCoord( _polygon[i + 3] );
            }
        }
    }
    m_intersectingPolygons->end();
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateClippingCube()
{
    const ::Ogre::Vector3 min = m_clippingCube[0];
    const ::Ogre::Vector3 max = m_clippingCube[1];

    for(unsigned i = 0; i < 8; ++ i)
    {
        m_clippedImagePositions[i] = ::Ogre::Vector3(
                    ::boost::algorithm::clamp(m_imagePositions[i].x, min.x, max.x),
                    ::boost::algorithm::clamp(m_imagePositions[i].y, min.y, max.y),
                    ::boost::algorithm::clamp(m_imagePositions[i].z, min.z, max.z));
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::updateWidgets()
{
    const auto clippingBoxPositions = getClippingBoxPositions();

    m_boundingBox->beginUpdate(0);
    {
        const unsigned edges[][2] = {
            { 0, 1 }, { 1, 4 }, { 4, 3 }, { 3, 0 },
            { 0, 2 }, { 1, 5 }, { 4, 7 }, { 3, 6 },
            { 2, 5 }, { 5, 7 }, { 7, 6 }, { 6, 2 }
        };

        for(unsigned i = 0; i < 12; ++ i)
        {
            m_boundingBox->position(clippingBoxPositions[edges[i][0]]);
            m_boundingBox->position(clippingBoxPositions[edges[i][1]]);
        }

        // Cross
        for(unsigned i = 0; i < 6; i += 2)
        {
            m_boundingBox->position(getFaceCenter(static_cast<CubeFace>(i  )));
            m_boundingBox->position(getFaceCenter(static_cast<CubeFace>(i+1)));
        }
    }
    m_boundingBox->end();

    // Recenter widgets
    for(auto& widget : m_widgets)
    {
        CubeFace cf = widget.second.first;
        ::Ogre::Vector3 faceCenter = getFaceCenter(cf);

        widget.second.second->setPosition(faceCenter);
        widget.second.second->needUpdate();
    }
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

    const ::Ogre::Vector3 edgePoint0 = m_clippedImagePositions[_edgeVertexIndex0];
    const ::Ogre::Vector3 edgePoint1 = m_clippedImagePositions[_edgeVertexIndex1];

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

            _result.push_back(position);
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
    intersections.reserve(6); // there is a maximum of 6 intersections

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

    scaleCube(image->getSpacing());

    updatingTFPoints();

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::widgetPicked(::Ogre::MovableObject * _pickedWidget, int _screenX, int _screenY)
{
    int height = m_camera->getViewport()->getActualHeight();
    int width  = m_camera->getViewport()->getActualWidth();

    auto face = m_widgets.find(_pickedWidget);

    deselectFace();

    if(face != m_widgets.end())
    {
        CubeFace widgetFace = face->second.first;
        ::Ogre::SceneNode *widgetSceneNode = face->second.second;

        ::Ogre::Ray mouseRay = m_camera->getCameraToViewportRay(
            static_cast< ::Ogre::Real>(_screenX) / static_cast< ::Ogre::Real>(width),
            static_cast< ::Ogre::Real>(_screenY) / static_cast< ::Ogre::Real>(height));

        ::Ogre::Vector3 oldPos = m_volumeSceneNode->convertLocalToWorldPosition(widgetSceneNode->getPosition());

        ::Ogre::Real distance = mouseRay.getOrigin().distance(oldPos);

        ::Ogre::Vector3 newPos = m_volumeSceneNode->convertWorldToLocalPosition(mouseRay.getPoint(distance));

        ::Ogre::Vector3 tmpClippingCube[2];
         std::copy(m_clippingCube, m_clippingCube + 2, tmpClippingCube);

        switch(widgetFace)
        {
        case X_NEGATIVE: tmpClippingCube[0].x = newPos.x; break;
        case X_POSITIVE: tmpClippingCube[1].x = newPos.x; break;
        case Y_NEGATIVE: tmpClippingCube[0].y = newPos.y; break;
        case Y_POSITIVE: tmpClippingCube[1].y = newPos.y; break;
        case Z_NEGATIVE: tmpClippingCube[0].z = newPos.z; break;
        case Z_POSITIVE: tmpClippingCube[1].z = newPos.z; break;
        }

        // Check for overlap.
        const float eps = 0.001f;
        for(int i = 0; i < 3; ++ i)
        {
            if(tmpClippingCube[0][i] > m_clippingCube[1][i])
            {
                tmpClippingCube[0][i] = m_clippingCube[1][i] - eps;
            }
            if(tmpClippingCube[1][i] < m_clippingCube[0][i])
            {
                tmpClippingCube[1][i] = m_clippingCube[0][i] + eps;
            }
        }

        std::copy(tmpClippingCube, tmpClippingCube + 2, m_clippingCube);

        updateClippingCube();
        updateWidgets();
        updateAllSlices();
        selectFace(widgetFace);

        m_selectedWidget = dynamic_cast< ::Ogre::Entity *>(_pickedWidget);
        m_selectedWidget->setMaterialName("SphereHighlight");

        this->requestRender();
    }
    else
    {
        OSLM_WARN("The object picked is not a VR widget.");
    }
}

//-----------------------------------------------------------------------------

void SVolumeRender::widgetReleased()
{
    deselectFace();
    m_selectedWidget->setMaterialName("Default");
    m_selectedWidget = nullptr;
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::moveClippingBox(int x, int y, int dx, int dy)
{
    int width  = m_camera->getViewport()->getActualWidth();
    int height = m_camera->getViewport()->getActualHeight();

    ::Ogre::Vector2 cursor(
                static_cast< ::Ogre::Real>(x) / static_cast< ::Ogre::Real>(width),
                static_cast< ::Ogre::Real>(y) / static_cast< ::Ogre::Real>(height));

    ::Ogre::Ray oldPosRay = m_camera->getCameraToViewportRay(cursor.x, cursor.y);

    // Get ray in image space.
    ::Ogre::Ray mouseRayImgSpace(
                m_volumeSceneNode->convertWorldToLocalPosition(oldPosRay.getOrigin()),
                m_volumeSceneNode->convertWorldToLocalDirection(oldPosRay.getDirection(), true)
    );


    const ::Ogre::Vector3 min = m_clippingCube[0];
    const ::Ogre::Vector3 max = m_clippingCube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouseRayImgSpace.intersects(::Ogre::AxisAlignedBox(min, max));

    const ::Ogre::Vector3 camDir = m_camera->getRealDirection();

    ::Ogre::Vector3 oldPos;
    ::Ogre::Plane intersectionPlane;

    if(inter.first)
    {
        oldPos = m_volumeSceneNode->convertLocalToWorldPosition(mouseRayImgSpace.getPoint(inter.second));
        intersectionPlane = ::Ogre::Plane(camDir, oldPos);
    }
    else
    {
        // Get closest vertex to cursor in screen space.
        ::Ogre::Matrix4 camTransform = m_camera->getProjectionMatrix() * m_camera->getViewMatrix();

        const auto sortFunc = [&](::Ogre::Vector3 v1, ::Ogre::Vector3 v2)
        {
            const auto cursorDistFunc = [&](::Ogre::Vector3 v)
            {
                const auto vtmp = camTransform * m_volumeSceneNode->convertLocalToWorldPosition(v);
                return cursor.distance(::Ogre::Vector2(vtmp.x, vtmp.y));
            };

            return cursorDistFunc(v1) < cursorDistFunc(v2);
        };

        ::Ogre::Vector3 closestVertToCursor = *std::min_element(m_imagePositions, m_imagePositions + 8, sortFunc);

        intersectionPlane = ::Ogre::Plane(camDir, m_volumeSceneNode->getPosition());

        oldPos = oldPosRay.getPoint(oldPosRay.intersects(intersectionPlane).second);
    }

    const ::Ogre::Ray newPosRay = m_camera->getCameraToViewportRay(
                static_cast< ::Ogre::Real>(x + dx) / static_cast< ::Ogre::Real>(width),
                static_cast< ::Ogre::Real>(y + dy) / static_cast< ::Ogre::Real>(height));

    const std::pair<bool, float> planeInter = newPosRay.intersects(intersectionPlane);

    const ::Ogre::Vector3 newPos = newPosRay.getPoint(planeInter.second);

    ::Ogre::Vector3 d = newPos - oldPos;

    if(inter.first)
    {
         // Translate clipping box.
        d = m_volumeSceneNode->convertWorldToLocalDirection(d, true);

        m_clippingCube[0] += d;
        m_clippingCube[1] += d;
    }
    else
    {
        // Translate the whole scene node.
        m_volumeSceneNode->translate(d, ::Ogre::Node::TS_WORLD);
    }

    updateClippingCube();
    updateWidgets();
    updateAllSlices();
    this->requestRender();
}

//-----------------------------------------------------------------------------

}
