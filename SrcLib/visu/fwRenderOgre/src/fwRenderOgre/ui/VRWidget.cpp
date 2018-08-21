/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/ui/VRWidget.hpp"

#include "fwRenderOgre/compositor/Core.hpp"

#include <fwCom/Signal.hxx>

#include <boost/algorithm/clamp.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreLogManager.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePlane.h>
#include <OGRE/OgreRay.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreViewport.h>

#include <numeric>

namespace fwRenderOgre
{
namespace ui
{

const std::string s_VR_MATERIALS_GROUP = "VRMaterials";

//-----------------------------------------------------------------------------

VRWidget::VRWidget(const std::string& id,
                   ::Ogre::SceneNode* parentSceneNode,
                   ::Ogre::Camera* camera,
                   SRender::sptr renderService,
                   ::Ogre::SceneManager* sceneManager,
                   ::fwRenderOgre::vr::IVolumeRenderer* renderer,
                   ::fwData::TransformationMatrix3D::sptr clippingMatrix) :
    m_selectionMode(NONE),
    m_id(id),
    m_sceneManager(sceneManager),
    m_camera(camera),
    m_renderService(renderService),
    m_volumeSceneNode(parentSceneNode),
    m_widgetSceneNode(m_volumeSceneNode->createChildSceneNode()),
    m_renderer(renderer),
    m_boundingBox(nullptr),
    m_selectedFace(nullptr),
    m_selectedWidget(nullptr),
    m_clippingMatrix(clippingMatrix)
{
    m_clippingCube = {{ ::Ogre::Vector3(0, 0, 0), ::Ogre::Vector3(1, 1, 1) }};

    // Initialize the clipping cube with the clipping matrix if it is provided
    if(m_clippingMatrix != nullptr)
    {
        const ::Ogre::Matrix4 ogreCropMx = Utils::convertTM3DToOgreMx(m_clippingMatrix);

        // Convert to world position because that's how VTK stores its crop matrices...
        const ::Ogre::Vector3 worldCubeMin = m_volumeSceneNode->convertLocalToWorldPosition(m_clippingCube[0]);
        const ::Ogre::Vector3 worldCubeMax = m_volumeSceneNode->convertLocalToWorldPosition(m_clippingCube[1]);

        m_clippingCube[0] = m_volumeSceneNode->convertWorldToLocalPosition(ogreCropMx * worldCubeMin);
        m_clippingCube[1] = m_volumeSceneNode->convertWorldToLocalPosition(ogreCropMx * worldCubeMax);
    }
    this->initWidgets();
}

//-----------------------------------------------------------------------------

VRWidget::~VRWidget()
{
    m_widgetSceneNode->detachAllObjects();

    m_sceneManager->destroyManualObject(m_selectedFace);
    m_sceneManager->destroyManualObject(m_boundingBox);

    for(auto& widget : m_widgets)
    {
        const ::Ogre::MovableObject* mo = widget.first;
        m_sceneManager->destroyMovableObject(mo->getName(), mo->getMovableType());
    }

    ::Ogre::MaterialManager::getSingleton().remove(m_sphereHighlightMtl->getHandle());
    ::Ogre::MaterialManager::getSingleton().remove(m_frameMtl->getHandle());
    ::Ogre::MaterialManager::getSingleton().remove(m_frameHighlightMtl->getHandle());
    ::Ogre::MaterialManager::getSingleton().remove(m_faceMtl->getHandle());

    m_sceneManager->destroySceneNode(m_widgetSceneNode);
}

//-----------------------------------------------------------------------------

bool VRWidget::belongsToWidget(const Ogre::MovableObject* const _object) const
{
    return m_widgets.find(_object) != m_widgets.end();
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3, 4 >
VRWidget::getFacePositions(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName) const
{
    const ::fwRenderOgre::vr::IVolumeRenderer::CubeFacePositionList positionIndices =
        m_renderer->s_cubeFaces.at(_faceName);
    std::array< ::Ogre::Vector3, 4 > facePositions;

    const auto BBpositions = this->getClippingBoxPositions();

    std::transform(positionIndices.begin(), positionIndices.end(), facePositions.begin(),
                   [&](unsigned i){ return BBpositions[i]; });

    return facePositions;
}

//-----------------------------------------------------------------------------

Ogre::Vector3 VRWidget::getFaceCenter(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName) const
{
    const auto facePositions = this->getFacePositions(_faceName);
    return std::accumulate(facePositions.cbegin() + 1, facePositions.cend(), facePositions[0]) / 4.f;
}

//-----------------------------------------------------------------------------

std::array< ::Ogre::Vector3, 8 > VRWidget::getClippingBoxPositions() const
{
    return {{
                ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[1].z),
                ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[1].z),
                ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[1].y, m_clippingCube[0].z),
                ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[1].z),
                ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[1].z),
                ::Ogre::Vector3(m_clippingCube[1].x, m_clippingCube[0].y, m_clippingCube[0].z),
                ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[1].y, m_clippingCube[0].z),
                ::Ogre::Vector3(m_clippingCube[0].x, m_clippingCube[0].y, m_clippingCube[0].z)
            }};
}

//-----------------------------------------------------------------------------

void VRWidget::updateClippingCube()
{
    const ::Ogre::Vector3 min = m_clippingCube[0];
    const ::Ogre::Vector3 max = m_clippingCube[1];

    const ::Ogre::AxisAlignedBox aaBox(min, max);

    // Update clipping matrix if it is provided
    if(m_clippingMatrix != nullptr)
    {
        // VTK scales the cube along the box's center so we do the same for compatibility.
        const ::Ogre::Vector3 initialCenter = m_volumeSceneNode->convertLocalToWorldPosition(::Ogre::Vector3(0.5f));
        ::Ogre::Matrix4 invCenterTransMx = ::Ogre::Matrix4::IDENTITY;
        invCenterTransMx.setTrans(-initialCenter);

        const ::Ogre::Vector3 scale = aaBox.getSize();
        ::Ogre::Matrix4 scaleMx = ::Ogre::Matrix4::IDENTITY;
        scaleMx.setScale(scale);

        const ::Ogre::Vector3 center = m_volumeSceneNode->convertLocalToWorldPosition(aaBox.getCenter());
        ::Ogre::Matrix4 centerTransMx = ::Ogre::Matrix4::IDENTITY;
        centerTransMx.setTrans(center);

        const ::Ogre::Matrix4 clippingMx = centerTransMx * scaleMx * invCenterTransMx;

        Utils::copyOgreMxToTM3D(clippingMx, m_clippingMatrix);

        auto sig = m_clippingMatrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

    m_renderer->clipImage(aaBox);
}

//-----------------------------------------------------------------------------

void VRWidget::updateWidgets()
{
    const auto clippingBoxPositions = this->getClippingBoxPositions();

    m_boundingBox->beginUpdate(0);
    {
        // Box
        for(unsigned i = 0; i < 12; ++i)
        {
            m_boundingBox->position(clippingBoxPositions[m_renderer->s_cubeEdges[i].first ]);
            m_boundingBox->position(clippingBoxPositions[m_renderer->s_cubeEdges[i].second]);
        }

        // Cross
        for(unsigned i = 0; i < 6; i += 2)
        {
            const auto faceCenter         = this->getFaceCenter(static_cast< vr::IVolumeRenderer::CubeFace>(i  ));
            const auto oppositeFaceCenter = this->getFaceCenter(static_cast< vr::IVolumeRenderer::CubeFace>(i+1));

            m_boundingBox->position(faceCenter);
            m_boundingBox->position(oppositeFaceCenter);
        }
    }
    m_boundingBox->end();

    // Recenter widgets
    for(auto& widget : m_widgets)
    {
        const auto& widgetFaceNodePair   = widget.second;
        const ::Ogre::Vector3 faceCenter = this->getFaceCenter(widgetFaceNodePair.first);

        auto* widgetSceneNode = widgetFaceNodePair.second;
        widgetSceneNode->setPosition(faceCenter);
        widgetSceneNode->needUpdate();
    }
}

//-----------------------------------------------------------------------------

void VRWidget::initWidgets()
{
    // Create widget materials
    {
        // Put those materials in a separate group so that we can't propose them as a choice for user meshes
        auto& resourceMgr = ::Ogre::ResourceGroupManager::getSingleton();
        if(!resourceMgr.resourceGroupExists(s_VR_MATERIALS_GROUP))
        {
            resourceMgr.createResourceGroup(s_VR_MATERIALS_GROUP);
        }

        ::Ogre::MaterialPtr sphereMtl = ::Ogre::MaterialManager::getSingleton().getByName("Default");
        m_sphereHighlightMtl          = sphereMtl->clone(m_id + "_SphereHighlight", true, s_VR_MATERIALS_GROUP);

        m_sphereHighlightMtl->setAmbient(0.3f, 0.f, 0.f);
        m_sphereHighlightMtl->setDiffuse(0.5f, 0.1f, 0.1f, 1.f);

        m_frameMtl = sphereMtl->clone(m_id + "_Frame", true, s_VR_MATERIALS_GROUP);
        m_frameMtl->setAmbient(1.f, 1.f, 1.f);
        m_frameMtl->setDiffuse(0.f, 0.f, 0.f, 1.f);
        m_frameMtl->setSpecular(0.f, 0.f, 0.f, 1.f);

        m_frameHighlightMtl = m_frameMtl->clone(m_id + "_FrameHighlight", true, s_VR_MATERIALS_GROUP);
        m_frameHighlightMtl->setAmbient(0.f, 1.f, 0.f);

        m_faceMtl = sphereMtl->clone(m_id + "_FaceHighlight", true, s_VR_MATERIALS_GROUP);
        m_faceMtl->setAmbient(1.f, 1.f, 0.f);
        m_faceMtl->setDiffuse(0.f, 0.f, 0.f, 0.6f);
        m_faceMtl->setSpecular(0.f, 0.f, 0.f, 0.6f);
        m_faceMtl->setSceneBlending(::Ogre::SBT_TRANSPARENT_ALPHA);
        m_faceMtl->setDepthWriteEnabled(false);
    }

    m_boundingBox  = m_sceneManager->createManualObject(m_id + "_VolumeBB");
    m_selectedFace = m_sceneManager->createManualObject(m_id +"_VRSelectedFace");

    this->updateClippingCube();

    const auto clippingBoxPositions = this->getClippingBoxPositions();

    m_boundingBox->begin(m_id + "_Frame", Ogre::RenderOperation::OT_LINE_LIST);
    {
        for(unsigned i = 0; i < 12; ++i)
        {
            m_boundingBox->position(clippingBoxPositions[m_renderer->s_cubeEdges[i].first ]);
            m_boundingBox->position(clippingBoxPositions[m_renderer->s_cubeEdges[i].second]);
        }

        // Cross
        for(unsigned i = 0; i < 6; i += 2)
        {
            const auto faceCenter         = this->getFaceCenter(static_cast< vr::IVolumeRenderer::CubeFace>(i  ));
            const auto oppositeFaceCenter = this->getFaceCenter(static_cast< vr::IVolumeRenderer::CubeFace>(i+1));

            m_boundingBox->position(faceCenter);
            m_boundingBox->position(oppositeFaceCenter);
        }
    }
    m_boundingBox->end();

    m_widgetSceneNode->attachObject(m_boundingBox);

    m_selectedFace->begin(m_id + "_FaceHighlight", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    {
        for(unsigned i = 0; i < 4; ++i)
        {
            m_selectedFace->position(0, 0, 0);
        }
    }
    m_selectedFace->end();

    // Render highlighted faces after other surfaces but before volumes.
    m_selectedFace->setRenderQueueGroup(compositor::Core::s_SURFACE_RQ_GROUP_ID + 1);

    // Create a pickable sphere for each cube face
    for(unsigned i = 0; i < 6; ++i)
    {
        const auto currentFace = static_cast< vr::IVolumeRenderer::CubeFace>(i);

        ::Ogre::Entity* newWidget = m_sceneManager->createEntity( ::Ogre::SceneManager::PT_SPHERE );
        newWidget->setMaterialName("Default");

        ::Ogre::SceneNode* sphereSceneNode = m_widgetSceneNode->createChildSceneNode();

        m_widgets[newWidget] = std::make_pair(currentFace, sphereSceneNode);

        const ::Ogre::Vector3 faceCenter = this->getFaceCenter(currentFace);

        sphereSceneNode->setPosition(faceCenter);
        sphereSceneNode->setInheritScale(false);

        const auto volScale         = m_volumeSceneNode->getScale();
        const ::Ogre::Real scaleMin = std::min(volScale[0], std::min(volScale[1], volScale[2]));

        const ::Ogre::Vector3 widgetScale((0.05f * scaleMin) / newWidget->getBoundingRadius());
        sphereSceneNode->setScale(widgetScale);

        sphereSceneNode->attachObject(newWidget);
    }
}

//-----------------------------------------------------------------------------

void VRWidget::selectFace(::fwRenderOgre::vr::IVolumeRenderer::CubeFace _faceName)
{
    m_selectedFace->beginUpdate(0);
    {
        const std::array< ::Ogre::Vector3, 4 > facePositions = this->getFacePositions(_faceName);
        m_selectedFace->position(facePositions[1]);
        m_selectedFace->position(facePositions[0]);
        m_selectedFace->position(facePositions[2]);
        m_selectedFace->position(facePositions[3]);
    }
    m_selectedFace->end();

    m_widgetSceneNode->attachObject(m_selectedFace);
}

//-----------------------------------------------------------------------------

void VRWidget::deselectFace()
{
    if(m_selectedFace->isAttached())
    {
        m_widgetSceneNode->detachObject(m_selectedFace->getName());
    }
}

//-----------------------------------------------------------------------------

void VRWidget::widgetPicked(::Ogre::MovableObject* _pickedWidget, int _screenX, int _screenY)
{
    const int height = m_camera->getViewport()->getActualHeight();
    const int width  = m_camera->getViewport()->getActualWidth();

    SLM_ASSERT("The picked widget does not belong to this widget.", this->belongsToWidget(_pickedWidget));
    const auto face = m_widgets.at(_pickedWidget);

    this->deselectFace();

    if(this->getVisibility())
    {
        const ::fwRenderOgre::vr::IVolumeRenderer::CubeFace widgetFace = face.first;
        const ::Ogre::SceneNode* widgetSceneNode                       = face.second;

        const ::Ogre::Ray mouseRay = m_camera->getCameraToViewportRay(
            static_cast< ::Ogre::Real>(_screenX) / static_cast< ::Ogre::Real>(width),
            static_cast< ::Ogre::Real>(_screenY) / static_cast< ::Ogre::Real>(height));

        const ::Ogre::Vector3 oldPos = m_volumeSceneNode->convertLocalToWorldPosition(widgetSceneNode->getPosition());

        const ::Ogre::Real distance = mouseRay.getOrigin().distance(oldPos);

        const ::Ogre::Vector3 newPos = m_volumeSceneNode->convertWorldToLocalPosition(mouseRay.getPoint(distance));

        std::array< ::Ogre::Vector3, 2> tmpClippingCube;
        std::copy(m_clippingCube.begin(), m_clippingCube.end(), tmpClippingCube.begin());

        switch(widgetFace)
        {
            case ::fwRenderOgre::vr::IVolumeRenderer::X_NEGATIVE: tmpClippingCube[0].x = newPos.x; break;
            case ::fwRenderOgre::vr::IVolumeRenderer::X_POSITIVE: tmpClippingCube[1].x = newPos.x; break;
            case ::fwRenderOgre::vr::IVolumeRenderer::Y_NEGATIVE: tmpClippingCube[0].y = newPos.y; break;
            case ::fwRenderOgre::vr::IVolumeRenderer::Y_POSITIVE: tmpClippingCube[1].y = newPos.y; break;
            case ::fwRenderOgre::vr::IVolumeRenderer::Z_NEGATIVE: tmpClippingCube[0].z = newPos.z; break;
            case ::fwRenderOgre::vr::IVolumeRenderer::Z_POSITIVE: tmpClippingCube[1].z = newPos.z; break;
        }

        // Check for overlap.
        const float eps = 0.001f;
        for(size_t i = 0; i < 3; ++i)
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

        std::copy(tmpClippingCube.begin(), tmpClippingCube.end(), m_clippingCube.begin());

        this->updateClippingCube();
        this->updateWidgets();
        this->selectFace(widgetFace);

        m_selectedWidget = dynamic_cast< ::Ogre::Entity*>(_pickedWidget);
        m_selectedWidget->setMaterialName(m_id + "_SphereHighlight");
        m_selectedWidget->setRenderQueueGroupAndPriority(compositor::Core::s_SURFACE_RQ_GROUP_ID, 65535);

        m_renderService->requestRender();
    }
}

//-----------------------------------------------------------------------------

void VRWidget::widgetReleased()
{
    if(m_selectedWidget)
    {
        this->deselectFace();
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
    const int width  = m_camera->getViewport()->getActualWidth();
    const int height = m_camera->getViewport()->getActualHeight();

    const ::Ogre::Vector2 cursor(
        static_cast< ::Ogre::Real>(x) / static_cast< ::Ogre::Real>(width),
        static_cast< ::Ogre::Real>(y) / static_cast< ::Ogre::Real>(height));

    const ::Ogre::Ray oldPosRay = m_camera->getCameraToViewportRay(cursor.x, cursor.y);

    // Get ray in image space.
    const ::Ogre::Ray mouseRayImgSpace(
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
        if(inter.first && this->getVisibility())
        {
            // Get picked point in box space.
            m_pickedBoxPoint = (mouseRayImgSpace.getPoint(inter.second) - min) / (max - min);
            m_selectionMode  = BOX;

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
        const ::Ogre::Vector3 boxPos = m_pickedBoxPoint * (max - min) + min;

        // Image to world space.
        oldPos            = m_volumeSceneNode->convertLocalToWorldPosition(boxPos);
        intersectionPlane = ::Ogre::Plane(camDir, oldPos);
    }
    else
    {
        intersectionPlane = ::Ogre::Plane(camDir, m_volumeSceneNode->getPosition());
        oldPos            = oldPosRay.getPoint(oldPosRay.intersects(intersectionPlane).second);
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

    this->updateClippingCube();
    this->updateWidgets();

    m_renderService->requestRender();
}

//-----------------------------------------------------------------------------

void VRWidget::scaleClippingBox(int x, int y, int dy)
{
    const int width  = m_camera->getViewport()->getActualWidth();
    const int height = m_camera->getViewport()->getActualHeight();

    const ::Ogre::Vector2 cursor(
        static_cast< ::Ogre::Real>(x) / static_cast< ::Ogre::Real>(width),
        static_cast< ::Ogre::Real>(y) / static_cast< ::Ogre::Real>(height));

    const ::Ogre::Ray oldPosRay = m_camera->getCameraToViewportRay(cursor.x, cursor.y);

    // Get ray in image space.
    const ::Ogre::Ray mouseRayImgSpace(
        m_volumeSceneNode->convertWorldToLocalPosition(oldPosRay.getOrigin()),
        m_volumeSceneNode->convertWorldToLocalDirection(oldPosRay.getDirection(), true)
        );

    const ::Ogre::Vector3 min = m_clippingCube[0];
    const ::Ogre::Vector3 max = m_clippingCube[1];

    // Ray clipping box intersection
    const std::pair<bool, float> inter = mouseRayImgSpace.intersects(::Ogre::AxisAlignedBox(min, max));

    if(m_selectionMode == NONE)
    {
        if(inter.first && this->getVisibility())
        {
            // Get picked point in box space.
            m_selectionMode = BOX;

            m_boundingBox->setMaterialName(0, m_id + "_FrameHighlight");
        }
        else
        {
            m_selectionMode = CAMERA;
        }
    }

    if(m_selectionMode == CAMERA)
    {
        const ::Ogre::Vector3 transVec(0.f, 0.f, static_cast<float>(dy));

        m_camera->getParentNode()->translate(transVec, ::Ogre::Node::TS_LOCAL);
    }
    else if(m_selectionMode == BOX)
    {
        const auto volumeSize = m_volumeSceneNode->getScale();

        // A displacement of 1 pixel along the height axis scales the box by 1/100th of the image's length.
        const float speed = (volumeSize.length() / 100.f) / static_cast<float>(height);
        const float scale = 1.0f + static_cast<float>(dy) * speed;

        const ::Ogre::Vector3 ccCenter = (m_clippingCube[1] + m_clippingCube[0]) / 2.f;

        // Scale clipping cube along its center.
        ::Ogre::Vector3 cc[2] = {
            (m_clippingCube[0] - ccCenter) / (::Ogre::Vector3::UNIT_SCALE - ccCenter),
            (m_clippingCube[1] - ccCenter) / (::Ogre::Vector3::UNIT_SCALE - ccCenter)
        };

        cc[0] *= scale;
        cc[1] *= scale;

        m_clippingCube[0] = cc[0] * (::Ogre::Vector3::UNIT_SCALE - ccCenter) + ccCenter;
        m_clippingCube[1] = cc[1] * (::Ogre::Vector3::UNIT_SCALE - ccCenter) + ccCenter;
    }

    this->updateClippingCube();
    this->updateWidgets();

    m_renderService->requestRender();
}

//-----------------------------------------------------------------------------

bool VRWidget::getVisibility() const
{
    return m_widgetSceneNode->getAttachedObject(0)->getVisible();
}

//-----------------------------------------------------------------------------

void VRWidget::setVisibility(bool visibility)
{
    m_widgetSceneNode->setVisible(visibility);
}

//-----------------------------------------------------------------------------

} // namespace ui
} // namespace fwRenderOgre
