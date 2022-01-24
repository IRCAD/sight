/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SLandmarks.hpp"

#include "modules/viz/scene3d/adaptor/STransform.hpp"

#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Font.hpp>
#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/helper/Scene.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_REMOVE_GROUP_SLOT     = "removeGroup";
static const core::com::Slots::SlotKeyType s_MODIFY_GROUP_SLOT     = "modifyGroup";
static const core::com::Slots::SlotKeyType s_MODIFY_POINT_SLOT     = "modifyPoint";
static const core::com::Slots::SlotKeyType s_ADD_POINT_SLOT        = "addPoint";
static const core::com::Slots::SlotKeyType s_REMOVE_POINT_SLOT     = "removePoint";
static const core::com::Slots::SlotKeyType s_INSERT_POINT_SLOT     = "insertPoint";
static const core::com::Slots::SlotKeyType s_SELECT_POINT_SLOT     = "selectPoint";
static const core::com::Slots::SlotKeyType s_DESELECT_POINT_SLOT   = "deselectPoint";
static const core::com::Slots::SlotKeyType s_INITIALIZE_IMAGE_SLOT = "initializeImage";
static const core::com::Slots::SlotKeyType s_SLICE_TYPE_SLOT       = "sliceType";
static const core::com::Slots::SlotKeyType s_SLICE_INDEX_SLOT      = "sliceIndex";

static const std::string s_FONT_SIZE_CONFIG       = "fontSize";
static const std::string s_FONT_SOURCE_CONFIG     = "fontSource";
static const std::string s_LABEL_CONFIG           = "label";
static const std::string s_ORIENTATION_CONFIG     = "orientation";
static const std::string s_LANDMARKS_FLAGS_CONFIG = "landmarksQueryFlags";
static const std::string s_INTERACTIVE_CONFIG     = "interactive";
static const std::string s_PRIORITY_CONFIG        = "priority";
static const std::string s_QUERY_MASK_CONFIG      = "queryMask";

//------------------------------------------------------------------------------

Ogre::Vector3 SLandmarks::getCamDirection(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

SLandmarks::SLandmarks() noexcept
{
    newSlot(s_REMOVE_GROUP_SLOT, &SLandmarks::removeGroup, this);
    newSlot(s_MODIFY_GROUP_SLOT, &SLandmarks::modifyGroup, this);
    newSlot(s_MODIFY_POINT_SLOT, &SLandmarks::modifyPoint, this);
    newSlot(s_ADD_POINT_SLOT, &SLandmarks::addPoint, this);
    newSlot(s_REMOVE_POINT_SLOT, &SLandmarks::removePoint, this);
    newSlot(s_INSERT_POINT_SLOT, &SLandmarks::insertPoint, this);
    newSlot(s_SELECT_POINT_SLOT, &SLandmarks::selectPoint, this);
    newSlot(s_DESELECT_POINT_SLOT, &SLandmarks::deselectPoint, this);
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

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    m_fontSource   = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize     = config.get<size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
    m_enableLabels = config.get<bool>(s_LABEL_CONFIG, m_enableLabels);
    m_interactive  = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority     = config.get<int>(s_PRIORITY_CONFIG, m_priority);

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
        SIGHT_ERROR("Unknown orientation, allow values are `axial`, `frontal` and `sagittal`");
    }

    std::string hexaMask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_QUERY_MASK_CONFIG + ") should start with '0x'"
                                                           "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_queryMask = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    hexaMask = config.get<std::string>(s_LANDMARKS_FLAGS_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_LANDMARKS_FLAGS_CONFIG + ") should start with '0x'"
                                                                "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_landmarksQueryFlag = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }
}

//-----------------------------------------------------------------------------

void SLandmarks::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_transNode = this->getTransformNode(rootSceneNode);

    m_material = data::Material::New();

    // Register the material adaptor.
    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::SMaterial>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    m_materialAdaptor->setInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->setID(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setMaterialName(this->getID() + m_materialAdaptor->getID());
    m_materialAdaptor->setRenderService(this->getRenderService());
    m_materialAdaptor->setLayerID(m_layerID);
    m_materialAdaptor->setMaterialTemplateName(sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
        this->getLayer()->addInteractor(interactor, m_priority);
    }

    // Draw landmarks.
    this->updating();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SLandmarks::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;

    connections.push(s_TRANSFORM_CONFIG, data::Matrix4::s_MODIFIED_SIG, s_UPDATE_SLOT);

    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_GROUP_REMOVED_SIG, s_REMOVE_GROUP_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_GROUP_MODIFIED_SIG, s_MODIFY_GROUP_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_POINT_MODIFIED_SIG, s_MODIFY_POINT_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_POINT_REMOVED_SIG, s_REMOVE_POINT_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_POINT_INSERTED_SIG, s_INSERT_POINT_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_POINT_SELECTED_SIG, s_SELECT_POINT_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_POINT_DESELECTED_SIG, s_DESELECT_POINT_SLOT);
    connections.push(s_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, s_INITIALIZE_IMAGE_SLOT);
    connections.push(s_IMAGE_INPUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INPUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICE_INDEX_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

void SLandmarks::updating()
{
    // Get landmarks.
    const auto landmarks = m_landmarks.lock();

    // Delete all groups.
    for(const std::string& groupName : landmarks->getGroupNames())
    {
        this->removeGroup(groupName);
    }

    // Create all point.
    for(const std::string& groupName : landmarks->getGroupNames())
    {
        const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);
        for(size_t index = 0 ; index < group.m_points.size() ; ++index)
        {
            this->insertMyPoint(groupName, index, landmarks.get_shared());
        }
    }
}

//-----------------------------------------------------------------------------

void SLandmarks::stopping()
{
    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
        this->getLayer()->removeInteractor(interactor);
    }

    // Stop all threads.
    for(auto landmarkIt = m_selectedLandmarks.begin() ; landmarkIt != m_selectedLandmarks.end() ; ++landmarkIt)
    {
        (*landmarkIt)->m_timer->stop();
    }

    data::Landmarks::GroupNameContainer groupNames;
    {
        // Get landmarks.
        const auto landmarks = m_landmarks.lock();
        groupNames = landmarks->getGroupNames();

        // Delete all groups.
        for(const std::string& groupName : groupNames)
        {
            this->removeGroup(groupName);
        }
    }

    // Unregister the material adaptor.
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SLandmarks::removeGroup(std::string _groupName)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* sceneMgr = this->getSceneManager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto objectIt = m_manualObjects.begin() ; objectIt != m_manualObjects.end() ; )
    {
        const std::string& name = (*objectIt)->m_groupName;
        if(name.find(_groupName) != std::string::npos)
        {
            // Stop the thread if it already run since we are deleting data.
            this->deselectPoint(_groupName, (*objectIt)->m_index);

            m_transNode->removeAndDestroyChild((*objectIt)->m_node);
            sceneMgr->destroyManualObject((*objectIt)->m_object);
            if(m_enableLabels)
            {
                sceneMgr->destroyMovableObject((*objectIt)->m_label);
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
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Get all selected point.
    std::vector<size_t> indexes;
    for(const std::shared_ptr<SelectedLandmark>& landmark : m_selectedLandmarks)
    {
        indexes.push_back(landmark->m_landmark->m_index);
    }

    // Remove the group.
    this->removeGroup(_groupName);

    // Get landmarks.
    const auto landmarks = m_landmarks.lock();

    // Retrieve group.
    const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(_groupName);

    size_t groupSize = group.m_points.size();

    // Re-create the group.
    for(size_t index = 0 ; index < groupSize ; ++index)
    {
        this->insertMyPoint(_groupName, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(size_t index : indexes)
    {
        this->selectPoint(_groupName, index);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::modifyPoint(std::string _groupName, size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto landmarks                    = m_landmarks.lock();
    const data::Landmarks::PointType& point = landmarks->getPoint(_groupName, _index);

    for(auto objectIt = m_manualObjects.begin() ; objectIt != m_manualObjects.end() ; ++objectIt)
    {
        const std::string& name = (*objectIt)->m_groupName;
        if(name.find(_groupName) != std::string::npos && (*objectIt)->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            (*objectIt)->m_node->setPosition(position);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::addPoint(std::string _groupName)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Get landmarks.
    const auto landmarks = m_landmarks.lock();

    // Retrieve group.
    const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(_groupName);
    SIGHT_ASSERT(
        "They must have at least one point in the group `" << _groupName << "`",
        group.m_points.size() > 0
    );

    // Get the last index.
    size_t index = group.m_points.size() - 1;

    // Add the new point.
    this->insertMyPoint(_groupName, index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

void SLandmarks::removePoint(std::string _groupName, size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* sceneMgr = this->getSceneManager();

    // Find object where name match _groupName and the index, and delete Ogre's resources.
    for(auto objectIt = m_manualObjects.begin() ; objectIt != m_manualObjects.end() ; ++objectIt)
    {
        const std::string& name = (*objectIt)->m_groupName;
        if(name.find(_groupName) != std::string::npos && (*objectIt)->m_index == _index)
        {
            // Stop the thread if it already run since we are deleting data.
            this->deselectPoint(_groupName, _index);

            m_transNode->removeAndDestroyChild((*objectIt)->m_node);
            sceneMgr->destroyManualObject((*objectIt)->m_object);
            if(m_enableLabels)
            {
                sceneMgr->destroyMovableObject((*objectIt)->m_label);
            }

            objectIt = m_manualObjects.erase(objectIt);
            break;
        }
    }

    // Re-compute index of landmarks in the same group.
    for(auto objectIt = m_manualObjects.begin() ; objectIt != m_manualObjects.end() ; ++objectIt)
    {
        const std::string& name = (*objectIt)->m_groupName;
        if(name.find(_groupName) != std::string::npos && (*objectIt)->m_index > _index)
        {
            (*objectIt)->m_index -= 1;
            if(m_enableLabels)
            {
                (*objectIt)->m_label->setText(_groupName + "_" + std::to_string((*objectIt)->m_index));
            }
        }
    }

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::insertPoint(std::string _groupName, size_t _index)
{
    // Make the context as current
    this->getRenderService()->makeCurrent();

    const auto landmarks = m_landmarks.lock();
    insertMyPoint(_groupName, _index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

void SLandmarks::insertMyPoint(std::string _groupName, size_t _index, const data::Landmarks::csptr& _landmarks)
{
    // Retrieve group.
    const data::Landmarks::LandmarksGroup& group = _landmarks->getGroup(_groupName);

    // Create the point name.
    const std::string pointName = _groupName + "_" + std::to_string(_index);

    // Create the manual object.
    const Ogre::ColourValue color =
        Ogre::ColourValue(group.m_color[0], group.m_color[1], group.m_color[2], group.m_color[3]);

    Ogre::SceneManager* sceneMgr = this->getSceneManager();
    Ogre::ManualObject* object   = sceneMgr->createManualObject(this->getID() + "_" + pointName + "_object");
    switch(group.m_shape)
    {
        case data::Landmarks::Shape::SPHERE:
            sight::viz::scene3d::helper::ManualObject::createSphere(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                group.m_size * 0.5f
            );
            break;

        case data::Landmarks::Shape::CUBE:
            sight::viz::scene3d::helper::ManualObject::createCube(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                group.m_size
            );
            break;
    }

    object->setQueryFlags(m_landmarksQueryFlag);

    Ogre::SceneNode* node = m_transNode->createChildSceneNode(this->getID() + "_" + pointName + "_node");

    // Set the point to the right position.
    const data::Landmarks::PointType& point = _landmarks->getPoint(_groupName, _index);
    node->setPosition(Ogre::Real(point[0]), Ogre::Real(point[1]), Ogre::Real(point[2]));

    // Attach data.
    node->attachObject(object);

    // Create the label.
    sight::viz::scene3d::Text* text = nullptr;
    if(m_enableLabels)
    {
        // Get necessary data.
        const float dpi                 = this->getRenderService()->getInteractorManager()->getLogicalDotsPerInch();
        Ogre::Camera* cam               = this->getLayer()->getDefaultCamera();
        const std::string textName      = this->getID() + "_" + pointName + "_text";
        Ogre::OverlayContainer* overlay = this->getLayer()->getOverlayTextPanel();

        // Create the label.
        text = sight::viz::scene3d::Text::New(textName, sceneMgr, overlay, m_fontSource, m_fontSize, dpi, cam);
        text->setText(pointName);
        text->setTextColor(color);
        text->setVisible(group.m_visibility && m_isVisible);

        // Attach data.
        node->attachObject(text);
    }

    // Store the created data.
    m_manualObjects.push_back(std::make_shared<Landmark>(node, object, _groupName, _index, text));

    // Hide landmarks if an image is given to the service.
    const auto imageLock = m_image.lock();
    this->hideMyLandmark(m_manualObjects.back(), (imageLock.operator bool()), _landmarks);

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::selectPoint(std::string _groupName, size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    for(auto objectIt = m_manualObjects.begin() ; objectIt != m_manualObjects.end() ; ++objectIt)
    {
        const std::string& name = (*objectIt)->m_groupName;
        if(name.find(_groupName) != std::string::npos && (*objectIt)->m_index == _index)
        {
            const auto it = std::find_if(
                m_selectedLandmarks.begin(),
                m_selectedLandmarks.end(),
                [&](std::shared_ptr<SelectedLandmark> _landmark)
                {
                    return _landmark->m_landmark->m_groupName == _groupName && _landmark->m_landmark->m_index == _index;
                });

            if(it == m_selectedLandmarks.end())
            {
                // This method must be synchronized with deselectPoint(std::string, size_t).
                std::lock_guard<std::mutex> guard(m_selectedMutex);

                // Create thread data.
                std::shared_ptr<SelectedLandmark> selectedLandmark =
                    std::make_shared<SelectedLandmark>(m_associatedWorker->createTimer(), *objectIt);
                m_selectedLandmarks.push_back(selectedLandmark);

                // Run a thread that change the selected point.
                core::thread::Timer::TimeDurationType duration = std::chrono::milliseconds(500);
                selectedLandmark->m_timer->setFunction(std::bind(&SLandmarks::hightlight, this, selectedLandmark));
                selectedLandmark->m_timer->setDuration(duration);
                selectedLandmark->m_timer->start();
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::deselectPoint(std::string _groupName, size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // This method must be synchronized with selectPoint(std::string, size_t).
    std::lock_guard<std::mutex> guard(m_selectedMutex);

    // Find the thread and stop it.
    for(auto landmarkIt = m_selectedLandmarks.begin() ; landmarkIt != m_selectedLandmarks.end() ; ++landmarkIt)
    {
        if((*landmarkIt)->m_landmark->m_groupName == _groupName && (*landmarkIt)->m_landmark->m_index == _index)
        {
            // Stop the timer.
            (*landmarkIt)->m_timer->stop();
            (*landmarkIt)->m_landmark->m_object->setVisible(true);
            this->hideLandmark((*landmarkIt)->m_landmark);

            // Request the rendering.
            this->requestRender();

            m_selectedLandmarks.erase(landmarkIt);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::hightlight(std::shared_ptr<SelectedLandmark> _selectedLandmark)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Hightlight the selected landmark.
    this->hideLandmark(_selectedLandmark->m_landmark);
    if(_selectedLandmark->m_landmark->m_object->isVisible())
    {
        if(_selectedLandmark->m_show)
        {
            _selectedLandmark->m_landmark->m_object->setVisible(true);
        }
        else
        {
            _selectedLandmark->m_landmark->m_object->setVisible(false);
        }

        this->requestRender();
    }

    _selectedLandmark->m_show = !_selectedLandmark->m_show;

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
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto toOrientation   = static_cast<OrientationMode>(_to);
    const auto fromOrientation = static_cast<OrientationMode>(_from);

    const auto newOrientation = m_orientation == toOrientation ? fromOrientation
                                                               : m_orientation
                                == fromOrientation ? toOrientation : m_orientation;

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
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto imageLock = m_image.lock();

    if(imageLock)
    {
        this->getRenderService()->makeCurrent();

        const auto& imgSpacing = imageLock->getSpacing();
        const auto& imgOrigin  = imageLock->getOrigin();

        m_currentSlicePos = {
            static_cast<float>(_sagittalIndex + 1) * static_cast<float>(imgSpacing[0])
            + static_cast<float>(imgOrigin[0]),
            static_cast<float>(_frontalIndex + 1) * static_cast<float>(imgSpacing[1])
            + static_cast<float>(imgOrigin[1]),
            static_cast<float>(_axialIndex + 1) * static_cast<float>(imgSpacing[2]) + static_cast<float>(imgOrigin[2])
        };

        this->hideLandmarks();

        // Request the rendering.
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::hideLandmarks()
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto imageLock = m_image.lock();

    // Hide landmarks only if there is an image.
    if(imageLock)
    {
        for(std::shared_ptr<Landmark> landmark : m_manualObjects)
        {
            this->hideLandmark(landmark);
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::hideLandmark(std::shared_ptr<Landmark> _landmark)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Get image.
    const auto imageLock = m_image.lock();
    const auto landmarks = m_landmarks.lock();
    hideMyLandmark(_landmark, (imageLock.operator bool()), landmarks.get_shared());
}

//------------------------------------------------------------------------------

void SLandmarks::hideMyLandmark(
    std::shared_ptr<Landmark> _landmark,
    const bool imageLock,
    data::Landmarks::csptr _landmarks
)
{
    // Retrieve group.
    const data::Landmarks::LandmarksGroup& group = _landmarks->getGroup(_landmark->m_groupName);

    // Hide landmarks only if there is an image.
    bool show = true;
    if(imageLock)
    {
        // Show the landmark only if the slice is inside it.
        Ogre::SceneNode* node = _landmark->m_node;
        switch(m_orientation)
        {
            case OrientationMode::X_AXIS:
                show = node->getPosition()[0] >= m_currentSlicePos[0] - group.m_size * 0.5
                       && node->getPosition()[0] <= m_currentSlicePos[0] + group.m_size * 0.5;
                break;

            case OrientationMode::Y_AXIS:
                show = node->getPosition()[1] >= m_currentSlicePos[1] - group.m_size * 0.5
                       && node->getPosition()[1] <= m_currentSlicePos[1] + group.m_size * 0.5;
                break;

            case OrientationMode::Z_AXIS:
                show = node->getPosition()[2] >= m_currentSlicePos[2] - group.m_size * 0.5
                       && node->getPosition()[2] <= m_currentSlicePos[2] + group.m_size * 0.5;
                break;

            default:
                SIGHT_ERROR("Unhandle orientation mode");
                break;
        }
    }

    // Show or hide the landmark.
    _landmark->m_object->setVisible(show && group.m_visibility && m_isVisible);
}

//------------------------------------------------------------------------------

void SLandmarks::setVisible(bool _visible)
{
    const auto landmarks = m_landmarks.lock();
    for(std::shared_ptr<Landmark> landmark : m_manualObjects)
    {
        const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(landmark->m_groupName);
        landmark->m_object->setVisible(_visible && group.m_visibility);
        if(m_enableLabels)
        {
            landmark->m_label->setVisible(_visible && group.m_visibility);
        }
    }

    this->requestRender();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> SLandmarks::getNearestPickedPosition(int _x, int _y)
{
    sight::viz::scene3d::picker::IPicker picker;
    Ogre::SceneManager* sm = this->getSceneManager();
    picker.setSceneManager(sm);
    picker.executeRaySceneQuery(_x, _y, m_queryMask);

    if(picker.getSelectedObject())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = camera->getCameraToViewportRay(vpX, vpY);

        Ogre::Vector3 normal = -ray.getDirection();
        normal.normalise();

        return picker.getIntersectionInWorldSpace() + normal * 0.01f;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void SLandmarks::buttonPressEvent(MouseButton _button, Modifier, int _x, int _y)
{
    if(_button == LEFT)
    {
        const auto layer = this->getLayer();

        Ogre::SceneManager* const sceneMgr = layer->getSceneManager();

        const Ogre::Camera* const cam = layer->getDefaultCamera();
        const auto* const vp          = cam->getViewport();

        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

        bool found                               = false;
        Ogre::RaySceneQuery* const raySceneQuery = sceneMgr->createRayQuery(ray, m_landmarksQueryFlag);
        raySceneQuery->setSortByDistance(false);
        if(raySceneQuery->execute().size() != 0)
        {
            const Ogre::Real scale = 1.15f;

            const Ogre::RaySceneQueryResult& queryResult = raySceneQuery->getLastResults();
            for(size_t qrIdx = 0 ; qrIdx < queryResult.size() && !found ; qrIdx++)
            {
                const Ogre::MovableObject* const object = queryResult[qrIdx].movable;
                for(std::shared_ptr<Landmark>& landmark : m_manualObjects)
                {
                    if(landmark->m_object == object)
                    {
                        const auto landmarks = m_landmarks.lock();

                        m_pickedData = landmark;
                        landmark->m_node->setScale(scale, scale, scale);

                        found = true;
                        break;
                    }
                }
            }
        }

        delete raySceneQuery;

        if(found)
        {
            this->getLayer()->cancelFurtherInteraction();

            // Check if something is picked to update the position of the distance.
            std::optional<Ogre::Vector3> pickedPos = this->getNearestPickedPosition(_x, _y);
            if(pickedPos.has_value())
            {
                // Update the data, the autoconnection will call modifyPoint.
                auto landmarks                   = m_landmarks.lock();
                data::Landmarks::PointType point = landmarks->getPoint(
                    m_pickedData->m_groupName,
                    m_pickedData->m_index
                );
                const Ogre::Vector3 newPos = pickedPos.value();
                point[0] = newPos[0];
                point[1] = newPos[1];
                point[2] = newPos[2];

                const auto& sig = landmarks->signal<data::Landmarks::PointModifiedSigType>(
                    data::Landmarks::s_POINT_MODIFIED_SIG
                );

                sig->asyncEmit(m_pickedData->m_groupName, m_pickedData->m_index);
            }

            this->requestRender();
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::mouseMoveEvent(MouseButton, Modifier, int _x, int _y, int, int)
{
    if(m_pickedData != nullptr)
    {
        Ogre::Vector3 newPos;

        // Discard the current distance to launch the ray over the scene without picking this one.
        m_pickedData->m_object->setQueryFlags(0x0);

        // Check if something is picked.
        std::optional<Ogre::Vector3> pickedPos = this->getNearestPickedPosition(_x, _y);
        if(pickedPos.has_value())
        {
            newPos = pickedPos.value();
        }
        // Else we move the distance along a plane.
        else
        {
            const auto layer = this->getLayer();

            const Ogre::Camera* const cam = layer->getDefaultCamera();
            const auto* const vp          = cam->getViewport();

            const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
            const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

            const Ogre::Ray ray           = cam->getCameraToViewportRay(vpX, vpY);
            const Ogre::Vector3 direction = this->getCamDirection(cam);

            const Ogre::Vector3 position = m_pickedData->m_node->getPosition();

            const Ogre::Plane plane(direction, position);

            const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

            if(!hit.first)
            {
                SIGHT_ERROR("The ray must intersect the plane")
                return;
            }

            newPos = ray.getPoint(hit.second);
        }

        // Reset the query flag.
        m_pickedData->m_object->setQueryFlags(m_landmarksQueryFlag);

        // Update the data, the autoconnection will call modifyPoint.
        auto landmarks                   = m_landmarks.lock();
        data::Landmarks::PointType point = landmarks->getPoint(m_pickedData->m_groupName, m_pickedData->m_index);
        point[0] = newPos[0];
        point[1] = newPos[1];
        point[2] = newPos[2];

        const auto& sig = landmarks->signal<data::Landmarks::PointModifiedSigType>(
            data::Landmarks::s_POINT_MODIFIED_SIG
        );
        sig->asyncEmit(m_pickedData->m_groupName, m_pickedData->m_index);

        this->requestRender();
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::buttonReleaseEvent(MouseButton, Modifier, int, int)
{
    if(m_pickedData != nullptr)
    {
        const Ogre::Real scale = 1.f;
        m_pickedData->m_node->setScale(scale, scale, scale);
        m_pickedData = nullptr;

        this->getLayer()->requestRender();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
