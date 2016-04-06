#include "fwRenderOgre/ui/VRWidget.hpp"


#include <boost/algorithm/clamp.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePlane.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreRay.h>
#include <OGRE/OgreViewport.h>

namespace fwRenderOgre
{
namespace ui
{

//-----------------------------------------------------------------------------

VRWidget::VRWidget(const std::string id,
                   ::Ogre::SceneNode *parentSceneNode,
                   ::Ogre::Camera    *camera,
                   SRender::sptr renderService,
                   const CubeFacePositionsMap& cubeFaces,
                   const CubeEdgeList& edges,
                   const ::Ogre::Vector3 *imgPositions,
                   ::Ogre::Vector3 *imgClippedPositions) throw() :
    m_selectionMode        (NONE),
    m_id                   (id),
    m_sceneManager         (renderService->getSceneManager()),
    m_camera               (camera),
    m_renderService        (renderService),
    m_volumeSceneNode      (parentSceneNode),
    m_cubeFaces            (cubeFaces),
    m_edges                (edges),
    m_imagePositions       (imgPositions),
    m_clippedImagePositions(imgClippedPositions),
    m_boundingBox          (nullptr),
    m_selectedFace         (nullptr),
    m_selectedWidget       (nullptr)
{
    initWidgets();
}

//-----------------------------------------------------------------------------

VRWidget::~VRWidget()
{
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3, 4 > VRWidget::getFacePositions(VRWidget::CubeFace _faceName) const
{
    const CubeFacePositionList positionIndices = m_cubeFaces.at(_faceName);
    std::array< ::Ogre::Vector3, 4 > facePositions;

    auto BBpositions = getClippingBoxPositions();

    std::transform(positionIndices.begin(), positionIndices.end(), facePositions.begin(),
                   [&](unsigned i){ return BBpositions[i]; });

    return facePositions;
}

//-----------------------------------------------------------------------------

Ogre::Vector3 VRWidget::getFaceCenter(VRWidget::CubeFace _faceName) const
{
    const auto facePositions = getFacePositions(_faceName);
    return std::accumulate(facePositions.cbegin() + 1, facePositions.cend(), facePositions[0]) / 4.f;
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3, 8 > VRWidget::getClippingBoxPositions() const
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

void VRWidget::updateClippingCube()
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

void VRWidget::updateWidgets()
{
    const auto clippingBoxPositions = getClippingBoxPositions();

    m_boundingBox->beginUpdate(0);
    {
        // Box
        for(unsigned i = 0; i < 12; ++ i)
        {
            m_boundingBox->position(clippingBoxPositions[m_edges[i].first ]);
            m_boundingBox->position(clippingBoxPositions[m_edges[i].second]);
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

void VRWidget::initWidgets()
{
    // Create widget materials
    {
        ::Ogre::MaterialPtr sphereMtl = ::Ogre::MaterialManager::getSingletonPtr()->getByName("Default");
        ::Ogre::MaterialPtr sphereHighlightMtl = sphereMtl->clone(m_id + "_SphereHighlight");

        sphereHighlightMtl->setAmbient(0.3f, 0.f, 0.f);
        sphereHighlightMtl->setDiffuse(0.5f, 0.1f, 0.1f, 1.f);

        ::Ogre::MaterialPtr frameMtl = sphereMtl->clone(m_id + "_Frame");
        frameMtl->setAmbient(1.f, 1.f, 1.f);
        frameMtl->setDiffuse(0.f, 0.f, 0.f, 1.f);
        frameMtl->setSpecular(0.f, 0.f, 0.f, 1.f);

        ::Ogre::MaterialPtr frameHighlightMtl = frameMtl->clone(m_id + "_FrameHighlight");
        frameHighlightMtl->setAmbient(0.f, 1.f, 0.f);

        ::Ogre::MaterialPtr faceMtl = sphereMtl->clone(m_id + "_FaceHighlight");
        faceMtl->setAmbient(1.f, 1.f, 0.f);
        faceMtl->setDiffuse(0.f, 0.f, 0.f, 0.6f);
        faceMtl->setSpecular(0.f, 0.f, 0.f, 0.6f);
        faceMtl->setSceneBlending(::Ogre::SBT_TRANSPARENT_ALPHA);
        faceMtl->setDepthWriteEnabled(false);
    }

    m_boundingBox  = m_sceneManager->createManualObject(m_id + "_VolumeBB");
    m_selectedFace = m_sceneManager->createManualObject(m_id +"_VRSelectedFace");

    updateClippingCube();

    const auto clippingBoxPositions = getClippingBoxPositions();

    m_boundingBox->begin(m_id + "_Frame", Ogre::RenderOperation::OT_LINE_LIST);
    {
        for(unsigned i = 0; i < 12; ++ i)
        {
            m_boundingBox->position(clippingBoxPositions[m_edges[i].first ]);
            m_boundingBox->position(clippingBoxPositions[m_edges[i].second]);
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

    m_selectedFace->begin(m_id + "_FaceHighlight", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    {
        for(unsigned i = 0; i < 4; ++ i)
        {
            m_selectedFace->position(0, 0, 0);
        }
    }
    m_selectedFace->end();

    // Render this first.
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

void VRWidget::selectFace(VRWidget::CubeFace _faceName)
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

void VRWidget::deselectFace()
{
    if(m_selectedFace->isAttached())
    {
        m_volumeSceneNode->detachObject(m_selectedFace->getName());
    }
}

//-----------------------------------------------------------------------------

void VRWidget::widgetPicked(::Ogre::MovableObject * _pickedWidget, int _screenX, int _screenY)
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
        selectFace(widgetFace);

        m_selectedWidget = dynamic_cast< ::Ogre::Entity *>(_pickedWidget);
        m_selectedWidget->setMaterialName(m_id + "_SphereHighlight");

        m_renderService->requestRender();
    }
    else
    {
        OSLM_WARN("The object picked is not a VR widget.");
    }
}

//-----------------------------------------------------------------------------

void VRWidget::widgetReleased()
{
    if(m_selectedWidget)
    {
        deselectFace();
        m_selectedWidget->setMaterialName("Default");
        m_selectedWidget = nullptr;
    }

    if(m_selectionMode == BOX)
    {
        m_boundingBox->setMaterialName(0, m_id + "_Frame");
    }

    m_selectionMode = NONE;

    m_renderService->requestRender();
}

//-----------------------------------------------------------------------------

void VRWidget::moveClippingBox(int x, int y, int dx, int dy)
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

    if(m_selectionMode == NONE)
    {
        if(inter.first)
        {
            // Get picked point in box space.
            pickedBoxPoint = (mouseRayImgSpace.getPoint(inter.second) - min) / (max - min);
            m_selectionMode = BOX;

            m_boundingBox->setMaterialName(0, m_id + "_FrameHighlight");
        }
        else
        {
            m_selectionMode = CAMERA;
        }
    }

    const bool boxSelected = (m_selectionMode == BOX);

    if(boxSelected)
    {
        // Box to image space.
        const ::Ogre::Vector3 boxPos = pickedBoxPoint * (max - min) + min;

        // Image to world space.
        oldPos = m_volumeSceneNode->convertLocalToWorldPosition(boxPos);
        intersectionPlane = ::Ogre::Plane(camDir, oldPos);
    }
    else
    {
        intersectionPlane = ::Ogre::Plane(camDir, m_volumeSceneNode->getPosition());
        oldPos = oldPosRay.getPoint(oldPosRay.intersects(intersectionPlane).second);
    }

    const ::Ogre::Ray newPosRay = m_camera->getCameraToViewportRay(
                static_cast< ::Ogre::Real>(x + dx) / static_cast< ::Ogre::Real>(width),
                static_cast< ::Ogre::Real>(y + dy) / static_cast< ::Ogre::Real>(height));

    const std::pair<bool, float> planeInter = newPosRay.intersects(intersectionPlane);

    const ::Ogre::Vector3 newPos = newPosRay.getPoint(planeInter.second);

    ::Ogre::Vector3 d = newPos - oldPos;

    if(boxSelected)
    {
         // Translate clipping box in image space.
        d = m_volumeSceneNode->convertWorldToLocalDirection(d, true);

        m_clippingCube[0] += d;
        m_clippingCube[1] += d;
    }
    else
    {
        // Translate the camera.
        m_camera->getParentNode()->translate(-d, ::Ogre::Node::TS_WORLD);
    }

    updateClippingCube();
    updateWidgets();

    m_renderService->requestRender();
}

} // namespace ui
} // namespace fwRenderOgre
