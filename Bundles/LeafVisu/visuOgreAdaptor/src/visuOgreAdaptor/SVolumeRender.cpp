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
#include <OGRE/OgreGpuProgramParams.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreTechnique.h>

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

const ::fwRenderOgre::ui::VRWidget::CubeFacePositionsMap SVolumeRender::s_cubeFaces = {
    { ::fwRenderOgre::ui::VRWidget::Z_POSITIVE, { 0, 1, 4, 3 } },
    { ::fwRenderOgre::ui::VRWidget::Z_NEGATIVE, { 2, 5, 7, 6 } },
    { ::fwRenderOgre::ui::VRWidget::Y_POSITIVE, { 0, 3, 6, 2 } },
    { ::fwRenderOgre::ui::VRWidget::Y_NEGATIVE, { 1, 4, 7, 5 } },
    { ::fwRenderOgre::ui::VRWidget::X_POSITIVE, { 0, 1, 5, 2 } },
    { ::fwRenderOgre::ui::VRWidget::X_NEGATIVE, { 3, 4, 7, 6 } }
};

//-----------------------------------------------------------------------------

const ::fwRenderOgre::ui::VRWidget::CubeEdgeList SVolumeRender::s_cubeEdges = { {
    { 0, 1 }, { 1, 4 }, { 4, 3 }, { 3, 0 },
    { 0, 2 }, { 1, 5 }, { 4, 7 }, { 3, 6 },
    { 2, 5 }, { 5, 7 }, { 7, 6 }, { 6, 2 }
} };

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWIMAGE_SLOT   = "newImage";

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SVolumeRender::s_NEWSAMPLING_SLOT = "updateSampling";

//-----------------------------------------------------------------------------

SVolumeRender::SVolumeRender() throw() :
    m_sceneManager         (nullptr),
    m_volumeSceneNode      (nullptr),
    m_sceneRenderQueue     (nullptr),
    m_camera               (nullptr),
    m_intersectingPolygons (nullptr),
    m_nbSlices             (512)
{
    this->installTFSlots(this);
    newSlot(s_NEWIMAGE_SLOT, &SVolumeRender::newImage, this);
    newSlot(s_NEWSAMPLING_SLOT, &SVolumeRender::samplingChanged, this);

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

    m_intersectingPolygons = m_sceneManager->createManualObject(this->getID() + "_VolumeRenderSlices");

    // create m_nbSlices slices
    m_intersectingPolygons->estimateVertexCount(6 /* * m_nbSlices*/ * 2);
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

void SVolumeRender::samplingChanged(int nbSamples)
{
    OSLM_ASSERT("Sampling rate must fit in a 16 bit uint.", nbSamples < 65536 && nbSamples >= 0);
    m_nbSlices = static_cast<uint16_t>(nbSamples);

    initSlices();
    updateAllSlices();

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SVolumeRender::initWidgets()
{
    // Create widgets.
    {
        auto widget = new ::fwRenderOgre::ui::VRWidget(this->getID(),
                                                       m_volumeSceneNode,
                                                       m_camera,
                                                       this->getRenderService(),
                                                       s_cubeFaces,
                                                       s_cubeEdges,
                                                       m_imagePositions,
                                                       m_clippedImagePositions);

        m_widgets = std::shared_ptr< ::fwRenderOgre::ui::VRWidget >(widget);
    }

    // Connect widgets to interactor.
    {
        ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);
        ::fwRenderOgre::interactor::IInteractor::sptr interactor = layer->getInteractor();

        auto vrInteractor =
                std::dynamic_pointer_cast< ::fwRenderOgre::interactor::VRWidgetsInteractor >(interactor);

        vrInteractor->initPicker();
        vrInteractor->attachWidget(m_widgets);
    }
}

//-----------------------------------------------------------------------------

}
