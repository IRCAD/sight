/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "visuOgreAdaptor/SLandmarks.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/Landmarks.hpp>

#include <fwRenderOgre/helper/Font.hpp>
#include <fwRenderOgre/helper/ManualObject.hpp>
#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

namespace visuOgreAdaptor
{

static const std::string s_LANDMARKS_INPUT = "landmarks";
static const std::string s_IMAGE_INPUT     = "image";

static const ::fwCom::Slots::SlotKeyType s_REMOVE_GROUP_SLOT     = "removeGroup";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_GROUP_SLOT     = "modifyGroup";
static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT        = "addPoint";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT     = "removePoint";
static const ::fwCom::Slots::SlotKeyType s_INSERT_POINT_SLOT     = "insertPoint";
static const ::fwCom::Slots::SlotKeyType s_INITIALIZE_IMAGE_SLOT = "initializeImage";
static const ::fwCom::Slots::SlotKeyType s_SLICE_TYPE_SLOT       = "sliceType";
static const ::fwCom::Slots::SlotKeyType s_SLICE_INDEX_SLOT      = "sliceIndex";

static const std::string s_FONT_SIZE_CONFIG   = "fontSize";
static const std::string s_FONT_SOURCE_CONFIG = "fontSource";
static const std::string s_LABEL_CONFIG       = "label";
static const std::string s_ORIENTATION_CONFIG = "orientation";

//-----------------------------------------------------------------------------

SLandmarks::SLandmarks() noexcept
{
    newSlot(s_REMOVE_GROUP_SLOT, &SLandmarks::removeGroup, this);
    newSlot(s_MODIFY_GROUP_SLOT, &SLandmarks::modifyGroup, this);
    newSlot(s_ADD_POINT_SLOT, &SLandmarks::addPoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &SLandmarks::removePoint, this);
    newSlot(s_INSERT_POINT_SLOT, &SLandmarks::insertPoint, this);
    newSlot(s_INITIALIZE_IMAGE_SLOT, &SLandmarks::initializeImage, this);
    newSlot(s_SLICE_TYPE_SLOT, &SLandmarks::changeSliceType, this);
    newSlot(s_SLICE_INDEX_SLOT, &SLandmarks::changeSliceIndex, this);
}

//-----------------------------------------------------------------------------

SLandmarks::~SLandmarks() noexcept
{
}

//-----------------------------------------------------------------------------

void SLandmarks::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    this->setTransformId(config.get<std::string>( ::fwRenderOgre::ITransformable::s_TRANSFORM_CONFIG,
                                                  this->getID() + "_transform"));

    m_fontSource   = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize     = config.get< size_t >(s_FONT_SIZE_CONFIG, m_fontSize);
    m_enableLabels = config.get< bool >(s_LABEL_CONFIG, m_enableLabels);

    const std::string orientation = config.get<std::string>(s_ORIENTATION_CONFIG, "axial");
    if(orientation == "axial")
    {
        m_orientation = OrientationMode::Z_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = OrientationMode::Y_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = OrientationMode::X_AXIS;
    }
    else
    {
        SLM_ERROR("Unknown orientation, allow values are `axial`, `frontal` and `sagittal`");
    }
}

//-----------------------------------------------------------------------------

void SLandmarks::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_transNode                      = this->getTransformNode(rootSceneNode);

    m_material = ::fwData::Material::New();

    // Register the material adaptor.
    m_materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >("::visuOgreAdaptor::SMaterial");
    m_materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->setID(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService( this->getRenderService() );
    m_materialAdaptor->setLayerID(m_layerID);
    m_materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    // Draw landmarks.
    this->updating();
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SLandmarks::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;

    connections.push(s_TRANSFORM_CONFIG, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_REMOVED_SIG, s_REMOVE_GROUP_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_GROUP_MODIFIED_SIG, s_MODIFY_GROUP_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_REMOVED_SIG, s_REMOVE_POINT_SLOT);
    connections.push(s_LANDMARKS_INPUT, ::fwData::Landmarks::s_POINT_INSERTED_SIG, s_INSERT_POINT_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_INITIALIZE_IMAGE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICE_INDEX_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SLandmarks::updating()
{
    // Get landmarks.
    const auto landmarksLocked                 = this->getLockedInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);
    const ::fwData::Landmarks::csptr landmarks = landmarksLocked.get_shared();

    // Delete all groups.
    for(const std::string& groupName : landmarks->getGroupNames())
    {
        this->removeGroup(groupName);
    }

    // Create all point.
    for(const std::string& groupName : landmarks->getGroupNames())
    {
        const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);
        for(size_t index = 0; index < group.m_points.size(); ++index)
        {
            this->insertPoint(groupName, index);
        }
    }
}

//-----------------------------------------------------------------------------

void SLandmarks::stopping()
{
    // Get landmarks.
    const auto landmarksLocked                 = this->getLockedInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);
    const ::fwData::Landmarks::csptr landmarks = landmarksLocked.get_shared();

    // Delete all groups.
    for(const std::string& groupName : landmarks->getGroupNames())
    {
        this->removeGroup(groupName);
    }

    // Unregister the material adaptor.
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SLandmarks::removeGroup(std::string _groupName)
{
    // Make the context as current since we create data here.
    this->getRenderService()->makeCurrent();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto objectIt = m_manualObjects.begin(); objectIt != m_manualObjects.end();)
    {
        const std::string& name = objectIt->m_groupName;
        if(name.find(_groupName) != std::string::npos)
        {
            m_transNode->removeAndDestroyChild(objectIt->m_node);
            sceneMgr->destroyManualObject(objectIt->m_object);
            if(m_enableLabels)
            {
                sceneMgr->destroyMovableObject(objectIt->m_label);
            }
            objectIt = m_manualObjects.erase(objectIt);
        }
        else
        {
            ++objectIt;
        }
    }

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::modifyGroup(std::string _groupName)
{
    // Remove the group.
    this->removeGroup(_groupName);

    // Get landmarks.
    const auto landmarksLocked                 = this->getLockedInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);
    const ::fwData::Landmarks::csptr landmarks = landmarksLocked.get_shared();

    // Retreive group.
    const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(_groupName);

    // Re-create the group.
    for(size_t index = 0; index < group.m_points.size(); ++index)
    {
        this->insertPoint(_groupName, index);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::addPoint(std::string _groupName)
{
    // Find the last index.
    size_t index = 0;
    {
        // Get landmarks.
        const auto landmarksLocked                 = this->getLockedInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);
        const ::fwData::Landmarks::csptr landmarks = landmarksLocked.get_shared();

        // Retreive group.
        const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(_groupName);
        SLM_ASSERT("They must have at least one point in the group `" << _groupName << "`", group.m_points.size() > 0);

        // Get the last index.
        index = group.m_points.size() - 1;
    }

    // Add the new point.
    this->insertPoint(_groupName, index);
}

//------------------------------------------------------------------------------

void SLandmarks::removePoint(std::string _groupName, size_t _index)
{
    // Make the context as current since we create data here.
    this->getRenderService()->makeCurrent();

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    // Find object where name match _groupName and the index, and delete Ogre's resources.
    for(auto objectIt = m_manualObjects.begin(); objectIt != m_manualObjects.end(); ++objectIt)
    {
        const std::string& name = objectIt->m_groupName;
        if(name.find(_groupName) != std::string::npos && objectIt->m_index == _index)
        {
            m_transNode->removeAndDestroyChild(objectIt->m_node);
            sceneMgr->destroyManualObject(objectIt->m_object);
            if(m_enableLabels)
            {
                sceneMgr->destroyMovableObject(objectIt->m_label);
            }
            objectIt = m_manualObjects.erase(objectIt);
            break;
        }
    }

    // Re-compute index of landmarks in the same group.
    for(auto objectIt = m_manualObjects.begin(); objectIt != m_manualObjects.end(); ++objectIt)
    {
        const std::string& name = objectIt->m_groupName;
        if(name.find(_groupName) != std::string::npos && objectIt->m_index > _index)
        {
            objectIt->m_index -= 1;
            if(m_enableLabels)
            {
                objectIt->m_label->setText(_groupName + "_" + std::to_string(objectIt->m_index));
            }
        }
    }

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::insertPoint(std::string _groupName, size_t _index)
{
    // Make the context as current since we create data here.
    this->getRenderService()->makeCurrent();

    // Get landmarks.
    const auto landmarksLocked                 = this->getLockedInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);
    const ::fwData::Landmarks::csptr landmarks = landmarksLocked.get_shared();

    // Retreive group.
    const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(_groupName);

    // Create the point name.
    const std::string pointName = _groupName + "_" + std::to_string(_index);

    // Create the manual object.
    const ::Ogre::ColourValue color
        = ::Ogre::ColourValue(group.m_color[0], group.m_color[1], group.m_color[2], group.m_color[3]);

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();
    ::Ogre::ManualObject* object   = sceneMgr->createManualObject(this->getID() + "_" + pointName + "_object");
    switch(group.m_shape)
    {
        case ::fwData::Landmarks::Shape::SPHERE:
            ::fwRenderOgre::helper::ManualObject::createSphere(object,
                                                               m_materialAdaptor->getMaterialName(),
                                                               color, group.m_size*0.5f);
            break;
        case ::fwData::Landmarks::Shape::CUBE:
            ::fwRenderOgre::helper::ManualObject::createCube(object,
                                                             m_materialAdaptor->getMaterialName(),
                                                             color, group.m_size);
            break;
    }

    ::Ogre::SceneNode* node = m_transNode->createChildSceneNode(this->getID() + "_" + pointName + "_node");

    // Set the point to the right position.
    const ::fwData::Landmarks::PointType& point = landmarks->getPoint(_groupName, _index);
    node->setPosition(::Ogre::Real(point[0]), ::Ogre::Real(point[1]), ::Ogre::Real(point[2]));

    // Attach data.
    node->attachObject(object);
    node->setVisible(group.m_visibility);

    // Create the label.
    ::fwRenderOgre::Text* text = nullptr;
    if(m_enableLabels)
    {
        // Get nesecaries data.
        const float dpi = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();
        ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();
        const std::string textName = this->getID() + "_" + pointName + "_text";
        ::Ogre::OverlayContainer* overlay = this->getLayer()->getOverlayTextPanel();

        // Create the label.
        text = fwRenderOgre::Text::New(textName, sceneMgr, overlay, m_fontSource, m_fontSize, dpi, cam);
        text->setText(pointName);
        text->setTextColor(color);
        text->setVisible(group.m_visibility);

        // Attach data.
        node->attachObject(text);
    }

    // Store the created data.
    m_manualObjects.push_back(Landmark{ node, object, _groupName, _index, text});

    // Hide landmarks if an image is given to the service.
    this->hideLandmark(m_manualObjects.back());

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::initializeImage()
{
    this->changeSliceIndex(0, 0, 0);
}

//------------------------------------------------------------------------------

void SLandmarks::changeSliceType(int _from, int _to)
{
    const auto toOrientation   = static_cast<OrientationMode>(_to);
    const auto fromOrientation = static_cast<OrientationMode>(_from);

    const auto newOrientation = m_orientation == toOrientation ? fromOrientation :
                                m_orientation == fromOrientation ? toOrientation : m_orientation;

    if(m_orientation != newOrientation)
    {
        m_orientation = newOrientation;
    }

    this->hideLandmarks();

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    const ::fwData::mt::weak_ptr< const ::fwData::Image > image = this->getWeakInput< ::fwData::Image >(s_IMAGE_INPUT);

    const ::fwData::mt::locked_ptr< const ::fwData::Image > imageLock = image.lock();

    if(imageLock)
    {
        this->getRenderService()->makeCurrent();

        const auto& imgSpacing = imageLock->getSpacing2();
        const auto& imgOrigin  = imageLock->getOrigin2();

        m_currentSlicePos = {
            static_cast<float>(_sagittalIndex+1) * static_cast<float>(imgSpacing[0]) + static_cast<float>(imgOrigin[0]),
            static_cast<float>(_frontalIndex+1) * static_cast<float>(imgSpacing[1]) + static_cast<float>(imgOrigin[1]),
            static_cast<float>(_axialIndex+1) * static_cast<float>(imgSpacing[2]) + static_cast<float>(imgOrigin[2])
        };

        this->hideLandmarks();

        // Request the rendering.
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::hideLandmarks()
{
    const ::fwData::mt::weak_ptr< const ::fwData::Image > image = this->getWeakInput< ::fwData::Image >(s_IMAGE_INPUT);

    const ::fwData::mt::locked_ptr< const ::fwData::Image > imageLock = image.lock();

    // Hide landmarks only if there is an image.
    if(imageLock)
    {
        for(Landmark& landmark : m_manualObjects)
        {
            this->hideLandmark(landmark);
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::hideLandmark(Landmark& _landmark)
{
    const ::fwData::mt::weak_ptr< const ::fwData::Image > image = this->getWeakInput< ::fwData::Image >(s_IMAGE_INPUT);

    const ::fwData::mt::locked_ptr< const ::fwData::Image > imageLock = image.lock();

    // Hide landmarks only if there is an image.
    if(imageLock)
    {
        // Get image spacing.
        const auto& imgSpac = imageLock->getSpacing2();

        // Show the landmark only if the slice is inside it.
        ::Ogre::SceneNode* node = _landmark.m_node;
        bool show = false;
        switch(m_orientation)
        {
            case OrientationMode::X_AXIS:
                show = node->getPosition()[0] >= m_currentSlicePos[0]-imgSpac[0] &&
                       node->getPosition()[0] <= m_currentSlicePos[0]+imgSpac[0];
                break;
            case OrientationMode::Y_AXIS:
                show = node->getPosition()[1] >= m_currentSlicePos[1]-imgSpac[1] &&
                       node->getPosition()[1] <= m_currentSlicePos[1]+imgSpac[1];
                break;
            case OrientationMode::Z_AXIS:
                show = node->getPosition()[2] >= m_currentSlicePos[2]-imgSpac[2] &&
                       node->getPosition()[2] <= m_currentSlicePos[2]+imgSpac[2];
                break;
            default:
                SLM_ERROR("Unhandle orientation mode");
                break;
        }

        // Get landmarks.
        const auto landmarksLocked                 = this->getLockedInput< ::fwData::Landmarks >(s_LANDMARKS_INPUT);
        const ::fwData::Landmarks::csptr landmarks = landmarksLocked.get_shared();

        // Retreive group.
        const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(_landmark.m_groupName);

        // Show or hide the landmark.
        _landmark.m_object->setVisible(show && group.m_visibility);
    }
}

} // namespace visuOgreAdaptor.
