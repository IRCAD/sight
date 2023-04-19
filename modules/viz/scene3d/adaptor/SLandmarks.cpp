/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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
#include <core/tools/compare.hpp>

#include <viz/scene3d/helper/ManualObject.hpp>
#include <viz/scene3d/helper/Scene.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Slots::SlotKeyType s_REMOVE_GROUP_SLOT       = "removeGroup";
static const core::com::Slots::SlotKeyType s_MODIFY_GROUP_SLOT       = "modifyGroup";
static const core::com::Slots::SlotKeyType s_MODIFY_POINT_SLOT       = "modifyPoint";
static const core::com::Slots::SlotKeyType s_ADD_POINT_SLOT          = "addPoint";
static const core::com::Slots::SlotKeyType s_REMOVE_POINT_SLOT       = "removePoint";
static const core::com::Slots::SlotKeyType s_INSERT_POINT_SLOT       = "insertPoint";
static const core::com::Slots::SlotKeyType s_SELECT_POINT_SLOT       = "selectPoint";
static const core::com::Slots::SlotKeyType s_DESELECT_POINT_SLOT     = "deselectPoint";
static const core::com::Slots::SlotKeyType s_INITIALIZE_IMAGE_SLOT   = "initializeImage";
static const core::com::Slots::SlotKeyType s_SLICE_TYPE_SLOT         = "sliceType";
static const core::com::Slots::SlotKeyType s_SLICE_INDEX_SLOT        = "sliceIndex";
static const core::com::Slots::SlotKeyType s_RENAME_GROUP_SLOT       = "renameGroup";
static const core::com::Slots::SlotKeyType s_TOGGLE_ADD_LANDMARKS    = "toggleAddLandmarks";
static const core::com::Slots::SlotKeyType s_TOGGLE_REMOVE_LANDMARKS = "toggleRemoveLandmarks";
static const core::com::Slots::SlotKeyType s_REMOVE_LANDMARKS        = "removeLandmarks";
static const core::com::Slots::SlotKeyType s_CREATE_LANDMARK         = "createLandmark";

const core::com::Signals::SignalKeyType SLandmarks::s_SEND_WORLD_COORD = "sendWorldCoord";

static constexpr Ogre::Real s_SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real s_DEFAULT_SCALE  = 1.F;

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
    newSlot(s_RENAME_GROUP_SLOT, &SLandmarks::renameGroup, this);
    newSlot(s_TOGGLE_ADD_LANDMARKS, &SLandmarks::toggleAddLandmarks, this);
    newSlot(s_TOGGLE_REMOVE_LANDMARKS, &SLandmarks::toggleRemoveLandmarks, this);
    newSlot(s_REMOVE_LANDMARKS, &SLandmarks::removeLandmarks, this);
    newSlot(s_CREATE_LANDMARK, &SLandmarks::createLandmark, this);

    newSignal<world_coordinates_signal_t>(s_SEND_WORLD_COORD);
}

//-----------------------------------------------------------------------------

void SLandmarks::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    static const std::string s_FONT_SIZE_CONFIG       = s_CONFIG + "fontSize";
    static const std::string s_LABEL_CONFIG           = s_CONFIG + "label";
    static const std::string s_ORIENTATION_CONFIG     = s_CONFIG + "orientation";
    static const std::string s_LANDMARKS_FLAGS_CONFIG = s_CONFIG + "landmarksQueryFlags";
    static const std::string s_INTERACTIVE_CONFIG     = s_CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG        = s_CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG      = s_CONFIG + "queryMask";
    static const std::string s_VIEW_DISTANCE          = s_CONFIG + "viewDistance";
    static const std::string s_INITIAL_GROUP          = s_CONFIG + "initialGroup";
    static const std::string s_INITIAL_COLOR          = s_CONFIG + "initialColor";
    static const std::string s_INITIAL_SIZE           = s_CONFIG + "initialSize";
    static const std::string s_INITIAL_SHAPE          = s_CONFIG + "initialShape";

    m_fontSize     = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_fontSize);
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
            "Hexadecimal values (" + s_QUERY_MASK_CONFIG + ") should start with '0x'. Given value : " + hexaMask,
            hexaMask.length() > 2 && hexaMask.substr(0, 2) == "0x"
        );
        m_queryMask = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    hexaMask = config.get<std::string>(s_LANDMARKS_FLAGS_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_LANDMARKS_FLAGS_CONFIG + ") should start with '0x'. Given value : " + hexaMask,
            hexaMask.length() > 2 && hexaMask.substr(0, 2) == "0x"
        );
        m_landmarksQueryFlag = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    // View distance
    const std::string& view_distance = config.get<std::string>(s_VIEW_DISTANCE, "slicesInRange");
    if(view_distance == "slicesInRange")
    {
        m_viewDistance = ViewDistance::SLICES_IN_RANGE;
    }
    else if(view_distance == "currentSlice")
    {
        m_viewDistance = ViewDistance::CURRENT_SLICE;
    }
    else if(view_distance == "allSlices")
    {
        m_viewDistance = ViewDistance::ALL_SLICES;
    }
    else
    {
        SIGHT_ERROR("Unknown view distance, allowed values are `slicesInRange`, `currentSlice` and `allSlices`");
    }

    // Initial group
    m_currentGroup = config.get<std::string>(s_INITIAL_GROUP, m_currentGroup);

    // Initial color
    auto color = sight::data::Color::New();
    color->setRGBA(config.get<std::string>(s_INITIAL_COLOR, "#FFFF00FF"));
    m_currentColor = {color->red(), color->green(), color->blue(), color->alpha()};

    // Initial size
    m_currentSize = config.get<float>(s_INITIAL_SIZE, m_currentSize);

    // Initial shape
    const std::string& initialShape = config.get<std::string>(s_INITIAL_SHAPE, "sphere");
    if(initialShape == "sphere")
    {
        m_currentShape = sight::data::Landmarks::Shape::SPHERE;
    }
    else if(initialShape == "cube")
    {
        m_currentShape = sight::data::Landmarks::Shape::CUBE;
    }
    else
    {
        SIGHT_ERROR("Unknown shape, allowed values are `sphere` and `cube`");
    }
}

//-----------------------------------------------------------------------------

void SLandmarks::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_transNode = this->getOrCreateTransformNode(rootSceneNode);

    m_material = data::Material::New();
    m_material->setDiffuse(data::Color::New(1.F, 1.F, 1.F, 1.F));

    // Register the material adaptor.
    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::SMaterial>(
        "sight::module::viz::scene3d::adaptor::SMaterial"
    );
    m_materialAdaptor->setInOut(m_material, module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->getID() + m_materialAdaptor->getID(),
        this->getID() + m_materialAdaptor->getID(),
        this->getRenderService(),
        m_layerID
    );
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
        this->getLayer()->addInteractor(interactor, m_priority);
    }

    // Set current slice indexes.
    initializeImage();

    // Draw landmarks.
    this->updating();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SLandmarks::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;

    connections.push(s_TRANSFORM_CONFIG, data::Matrix4::s_MODIFIED_SIG, IService::slots::s_UPDATE);

    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_MODIFIED_SIG, IService::slots::s_UPDATE);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_GROUP_REMOVED_SIG, s_REMOVE_GROUP_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_GROUP_MODIFIED_SIG, s_MODIFY_GROUP_SLOT);
    connections.push(s_LANDMARKS_INPUT, data::Landmarks::s_GROUP_RENAMED_SIG, s_RENAME_GROUP_SLOT);
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
        for(std::size_t index = 0 ; index < group.m_points.size() ; ++index)
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
    for(auto& m_selectedLandmark : m_selectedLandmarks)
    {
        m_selectedLandmark->m_timer->stop();
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

            if(m_enableLabels)
            {
                (*objectIt)->m_label->detachFromNode();
            }

            m_transNode->removeAndDestroyChild((*objectIt)->m_node);
            sceneMgr->destroyManualObject((*objectIt)->m_object);

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
    std::vector<std::size_t> indexes;
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

    std::size_t groupSize = group.m_points.size();

    // Re-create the group.
    for(std::size_t index = 0 ; index < groupSize ; ++index)
    {
        this->insertMyPoint(_groupName, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->selectPoint(_groupName, index);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::renameGroup(std::string _oldGroupName, std::string _newGroupName)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Get all selected point.
    std::vector<std::size_t> indexes;
    for(const std::shared_ptr<SelectedLandmark>& landmark : m_selectedLandmarks)
    {
        indexes.push_back(landmark->m_landmark->m_index);
    }

    // Remove the group.
    this->removeGroup(_oldGroupName);

    // Get landmarks.
    const auto landmarks = m_landmarks.lock();

    // Retrieve group.
    const data::Landmarks::LandmarksGroup& group = landmarks->getGroup(_newGroupName);

    std::size_t groupSize = group.m_points.size();

    // Re-create the group.
    for(std::size_t index = 0 ; index < groupSize ; ++index)
    {
        this->insertMyPoint(_newGroupName, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->selectPoint(_newGroupName, index);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::modifyPoint(std::string _groupName, std::size_t _index)
{
    const auto landmarks                    = m_landmarks.lock();
    const data::Landmarks::PointType& point = landmarks->getPoint(_groupName, _index);

    for(auto& m_manualObject : m_manualObjects)
    {
        const std::string& name = m_manualObject->m_groupName;
        if(name.find(_groupName) != std::string::npos && m_manualObject->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            m_manualObject->m_node->setPosition(position);
            break;
        }
    }

    this->getRenderService()->requestRender();
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
        !group.m_points.empty()
    );

    // Get the last index.
    std::size_t index = group.m_points.size() - 1;

    // Add the new point.
    this->insertMyPoint(_groupName, index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

void SLandmarks::removePoint(std::string _groupName, std::size_t _index)
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

            if(m_enableLabels)
            {
                (*objectIt)->m_label->detachFromNode();
            }

            m_transNode->removeAndDestroyChild((*objectIt)->m_node);
            sceneMgr->destroyManualObject((*objectIt)->m_object);
            objectIt = m_manualObjects.erase(objectIt);
            break;
        }
    }

    // Re-compute index of landmarks in the same group.
    for(auto& m_manualObject : m_manualObjects)
    {
        const std::string& name = m_manualObject->m_groupName;
        if(name.find(_groupName) != std::string::npos && m_manualObject->m_index > _index)
        {
            m_manualObject->m_index -= 1;
            if(m_enableLabels)
            {
                m_manualObject->m_label->setText(_groupName + "_" + std::to_string(m_manualObject->m_index));
            }
        }
    }

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::insertPoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current
    this->getRenderService()->makeCurrent();

    const auto landmarks = m_landmarks.lock();
    insertMyPoint(_groupName, _index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

std::shared_ptr<SLandmarks::Landmark> SLandmarks::insertMyPoint(
    std::string _groupName,
    std::size_t _index,
    const data::Landmarks::csptr& _landmarks
)
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
                group.m_size * 0.5F
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
    sight::viz::scene3d::IText::sptr text;
    if(m_enableLabels)
    {
        // Create the label.
        text = sight::viz::scene3d::IText::New(this->getLayer());
        text->setFontSize(m_fontSize);
        text->setText(pointName);
        text->setTextColor(color);
        text->setVisible(group.m_visibility && m_isVisible);

        // Attach data.
        text->attachToNode(node, this->getLayer()->getDefaultCamera());
    }

    // Store the created data.
    auto newLandmark = std::make_shared<Landmark>(node, object, _groupName, _index, text);
    m_manualObjects.push_back(newLandmark);

    // Hide landmarks if an image is given to the service.
    this->hideMyLandmark(*m_manualObjects.back(), *_landmarks);

    // Request the rendering.
    this->requestRender();

    return newLandmark;
}

//------------------------------------------------------------------------------

void SLandmarks::selectPoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    for(auto& m_manualObject : m_manualObjects)
    {
        const std::string& name = m_manualObject->m_groupName;
        if(name.find(_groupName) != std::string::npos && m_manualObject->m_index == _index)
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
                // This method must be synchronized with deselectPoint(std::string, std::size_t).
                std::lock_guard<std::mutex> guard(m_selectedMutex);

                // Create thread data.
                std::shared_ptr<SelectedLandmark> selectedLandmark =
                    std::make_shared<SelectedLandmark>(this->worker()->createTimer(), m_manualObject);
                m_selectedLandmarks.push_back(selectedLandmark);

                // Run a thread that change the selected point.
                core::thread::Timer::TimeDurationType duration = std::chrono::milliseconds(500);
                selectedLandmark->m_timer->setFunction(
                    [this, selectedLandmark](auto&& ...)
                    {
                        hightlight(selectedLandmark);
                    });
                selectedLandmark->m_timer->setDuration(duration);
                selectedLandmark->m_timer->start();
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::deselectPoint(std::string _groupName, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // This method must be synchronized with selectPoint(std::string, std::size_t).
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
    const auto image = m_image.lock();

    if(image)
    {
        namespace Helper = sight::data::helper::MedicalImage;

        const auto axial_index = std::max(
            std::int64_t(0),
            Helper::getSliceIndex(*image, Helper::orientation_t::AXIAL).value_or(0)
        );

        const auto frontal_index = std::max(
            std::int64_t(0),
            Helper::getSliceIndex(*image, Helper::orientation_t::FRONTAL).value_or(0)
        );

        const auto m_sagittal_index = std::max(
            std::int64_t(0),
            Helper::getSliceIndex(*image, Helper::orientation_t::SAGITTAL).value_or(0)
        );

        this->changeSliceIndex(int(axial_index), int(frontal_index), int(m_sagittal_index));
    }
    else
    {
        this->changeSliceIndex(0, 0, 0);
    }
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

    m_orientation = newOrientation;

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
            static_cast<float>(_sagittalIndex) * static_cast<float>(imgSpacing[0])
            + static_cast<float>(imgOrigin[0]),
            static_cast<float>(_frontalIndex) * static_cast<float>(imgSpacing[1])
            + static_cast<float>(imgOrigin[1]),
            static_cast<float>(_axialIndex) * static_cast<float>(imgSpacing[2])
            + static_cast<float>(imgOrigin[2])
        };

        this->hideLandmarks();

        // Request the rendering.
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::toggleAddLandmarks(bool toggle)
{
    if(toggle)
    {
        m_landmarksMode = LandmarksMode::ADD;
    }
    else if(m_landmarksMode == LandmarksMode::ADD)
    {
        m_landmarksMode = LandmarksMode::NONE;
    }
}

//------------------------------------------------------------------------------

void SLandmarks::toggleRemoveLandmarks(bool toggle)
{
    if(toggle)
    {
        m_landmarksMode = LandmarksMode::REMOVE;
    }
    else if(m_landmarksMode == LandmarksMode::REMOVE)
    {
        m_landmarksMode = LandmarksMode::NONE;
    }
}

//------------------------------------------------------------------------------

void SLandmarks::removeLandmarks()
{
    auto landmarks = m_landmarks.lock();

    if(!landmarks)
    {
        // No need to continue if there is no landmarks
        return;
    }

    const auto& image = m_image.const_lock();
    SIGHT_THROW_IF("Data is null.", !image);

    const auto slice_index = sight::data::helper::MedicalImage::getSliceIndex(
        *image,
        m_orientation
    );

    if(!slice_index)
    {
        // No slice selected, early return
        return;
    }

    for(const auto& name : landmarks->getGroupNames())
    {
        auto& group = landmarks->getGroup(name);

        bool hasDeleted = false;

        for(auto it = group.m_points.begin() ; it < group.m_points.end() ; )
        {
            if(isLandmarkVisible(*it, group.m_size))
            {
                it         = group.m_points.erase(it);
                hasDeleted = true;
            }
            else
            {
                ++it;
            }
        }

        if(hasDeleted)
        {
            const auto& sig = landmarks->signal<sight::data::Landmarks::ModifiedSignalType>(
                sight::data::Landmarks::s_MODIFIED_SIG
            );
            sig->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::configureNewLandmarks(
    std::string group,
    sight::data::Landmarks::ColorType color,
    sight::data::Landmarks::SizeType size,
    sight::data::Landmarks::Shape shape
)
{
    m_currentGroup = group;
    m_currentColor = color;
    m_currentSize  = size;
    m_currentShape = shape;
}

//------------------------------------------------------------------------------

void SLandmarks::createLandmark(sight::data::Landmarks::PointType point)
{
    createAndPickLandmark(point, false);
}

//------------------------------------------------------------------------------

void SLandmarks::createAndPickLandmark(const sight::data::Landmarks::PointType& point, bool pick)
{
    auto landmarks = m_landmarks.lock();

    // If the group does not exist, we create it.
    if(!landmarks->hasGroup(m_currentGroup))
    {
        landmarks->addGroup(m_currentGroup, m_currentColor, m_currentSize, m_currentShape);
        const auto& sig = landmarks->signal<sight::data::Landmarks::GroupAddedSignalType>(
            sight::data::Landmarks::s_GROUP_ADDED_SIG
        );

        sig->asyncEmit(m_currentGroup);
    }

    landmarks->addPoint(m_currentGroup, point);

    // Get the last index.
    const auto& group = landmarks->getGroup(m_currentGroup);

    // Add the new point.
    const auto& new_landmark = insertMyPoint(m_currentGroup, group.m_points.size() - 1, landmarks.get_shared());

    if(pick)
    {
        m_pickedData = new_landmark;
        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);
    }
    else
    {
        m_pickedData = nullptr;
    }

    // Block the signal to avoid being called back.
    const auto& sig = landmarks->signal<sight::data::Landmarks::PointAddedSignalType>(
        sight::data::Landmarks::s_POINT_ADDED_SIG
    );

    sight::core::com::Connection::Blocker blocker(sig->getConnection(this->slot(s_ADD_POINT_SLOT)));
    sig->asyncEmit(m_currentGroup);
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
        for(const std::shared_ptr<Landmark>& landmark : m_manualObjects)
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
    const auto landmarks = m_landmarks.lock();
    hideMyLandmark(*_landmark, *landmarks);
}

//------------------------------------------------------------------------------

void SLandmarks::hideMyLandmark(Landmark& landmark, const data::Landmarks& landmarks)
{
    const bool must_show =
        [&]
        {
            // Retrieve group
            if(!landmarks.hasGroup(landmark.m_groupName))
            {
                return false;
            }

            const auto& group = landmarks.getGroup(landmark.m_groupName);

            if(!group.m_visibility)
            {
                return false;
            }

            if(const auto& image = m_image.const_lock(); image)
            {
                const auto& position = landmark.m_node->getPosition();
                return isLandmarkVisible({position[0], position[1], position[2]}, group.m_size);
            }

            return true;
        }();

    // Show or hide the landmark.
    landmark.m_object->setVisible(must_show);
}

//------------------------------------------------------------------------------

bool SLandmarks::isLandmarkVisible(
    const data::Landmarks::PointType& point,
    data::Landmarks::SizeType group_size
) const
{
    if(const auto& image = m_image.const_lock(); image)
    {
        const auto position       = point[m_orientation];
        const auto slice_position = m_currentSlicePos[m_orientation];
        const auto spacing        = image->getSpacing()[m_orientation];

        switch(m_viewDistance)
        {
            case ViewDistance::SLICES_IN_RANGE:
            {
                // Check if the position is the same than slice position
                const auto group_half_size = group_size * 0.5;
                const auto max_size        = std::max(group_half_size, spacing);

                return core::tools::is_greater(position, (slice_position - group_size))
                       && core::tools::is_less(position, (slice_position + max_size));
            }

            case ViewDistance::CURRENT_SLICE:
            {
                // Check if the position is the same than slice position
                const auto roundedPosition      = std::round(position / spacing);
                const auto roundedSlicePosition = std::round(slice_position / spacing);
                return core::tools::is_equal(roundedPosition, roundedSlicePosition);
            }

            default:
                break;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

void SLandmarks::setVisible(bool _visible)
{
    const auto landmarks = m_landmarks.lock();
    for(const std::shared_ptr<Landmark>& landmark : m_manualObjects)
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
    Ogre::SceneManager* sm = this->getSceneManager();
    const auto result      = sight::viz::scene3d::Utils::pickObject(_x, _y, m_queryMask, *sm);

    if(result.has_value())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = camera->getCameraToViewportRay(vpX, vpY);

        Ogre::Vector3 normal = -ray.getDirection();
        normal.normalise();

        return result->second + normal * 0.01F;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void SLandmarks::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    if(_button != LEFT)
    {
        // We only manage left click.
        return;
    }

    const auto layer = this->getLayer();

    Ogre::SceneManager* const sceneMgr = layer->getSceneManager();

    const Ogre::Camera* const cam = layer->getDefaultCamera();
    const auto* const vp          = cam->getViewport();

    const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

    const auto raySceneQuery = std::unique_ptr<Ogre::RaySceneQuery>(
        sceneMgr->createRayQuery(ray, m_landmarksQueryFlag)
    );

    raySceneQuery->setSortByDistance(true);
    const auto& queryResult = raySceneQuery->execute();

    if(queryResult.empty())
    {
        // Nothing found.
        return;
    }

    // Find the ogre landmark and apply a scale.
    bool found = false;

    // The plane is always intercepted, if a landmark is found
    for(std::size_t index = 0 ; !found && index < queryResult.size() ; ++index)
    {
        // The landmark should be the nearest object found.
        const auto* const object = queryResult[index].movable;

        auto landmarks = m_landmarks.lock();

        for(auto& landmark : m_manualObjects)
        {
            if(landmark->m_object == object)
            {
                try
                {
                    if(const auto& group = landmarks->getGroup(landmark->m_groupName);
                       group.m_visibility && isLandmarkVisible(group.m_points[landmark->m_index], group.m_size))
                    {
                        found        = true;
                        m_pickedData = landmark;
                        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);

                        // If we are in remove mode, we will remove the picked landmark.
                        if(m_landmarksMode == LandmarksMode::REMOVE)
                        {
                            // Remove the picked landmark
                            landmarks->removePoint(landmark->m_groupName, landmark->m_index);
                            removePoint(landmark->m_groupName, landmark->m_index);

                            const auto& sig = landmarks->signal<sight::data::Landmarks::PointRemovedSignalType>(
                                sight::data::Landmarks::s_POINT_REMOVED_SIG
                            );

                            // Block the signal to avoid a being called back.
                            sight::core::com::Connection::Blocker blocker(
                                sig->getConnection(this->slot(s_REMOVE_POINT_SLOT))
                            );

                            sig->asyncEmit(m_currentGroup, landmark->m_index);
                        }

                        break;
                    }
                }
                catch(...)
                {
                    SIGHT_ERROR("Landmark group '" + landmark->m_groupName + "' not found.");

                    // No point found, we can't select it.
                    continue;
                }
            }
        }
    }

    // If nothing is picked, we will create a new landmark.
    if(!found && m_landmarksMode == LandmarksMode::ADD)
    {
        if(auto newPos = this->getNearestPickedPosition(_x, _y); newPos)
        {
            createAndPickLandmark({(*newPos)[0], (*newPos)[1], (*newPos)[2]});
        }
    }
}

//------------------------------------------------------------------------------

void SLandmarks::mouseMoveEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_pickedData != nullptr)
    {
        // Discard the current landmark to launch the ray over the scene without picking this one.
        m_pickedData->m_object->setQueryFlags(0x0);

        const auto layer              = this->getLayer();
        const Ogre::Camera* const cam = layer->getDefaultCamera();
        SIGHT_ASSERT("No camera found", cam);

        bool moveInCameraPlane = true;

        Ogre::Vector3 newPos;
        if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
        {
            // If something is picked, we will snap the landmark to it
            std::optional<Ogre::Vector3> pickedPos = this->getNearestPickedPosition(_x, _y);
            if(pickedPos.has_value())
            {
                newPos            = pickedPos.value();
                moveInCameraPlane = false;
            }
        }

        // Else we move the distance along a plane.
        if(moveInCameraPlane)
        {
            const auto* const vp = cam->getViewport();

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
        auto landmarks                    = m_landmarks.lock();
        data::Landmarks::PointType& point = landmarks->getPoint(m_pickedData->m_groupName, m_pickedData->m_index);
        point[0] = newPos[0];
        point[1] = newPos[1];
        point[2] = newPos[2];

        const auto& sig = landmarks->signal<data::Landmarks::PointModifiedSigType>(
            data::Landmarks::s_POINT_MODIFIED_SIG
        );
        sig->asyncEmit(m_pickedData->m_groupName, m_pickedData->m_index);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_pickedData != nullptr)
    {
        m_pickedData->m_node->setScale(s_DEFAULT_SCALE, s_DEFAULT_SCALE, s_DEFAULT_SCALE);
        m_pickedData = nullptr;

        this->getLayer()->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::buttonDoublePressEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y)
{
    const auto layer = this->getLayer();

    Ogre::SceneManager* const sceneMgr = layer->getSceneManager();

    const Ogre::Camera* const cam = layer->getDefaultCamera();
    const auto* const vp          = cam->getViewport();

    const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

    const auto raySceneQuery = std::unique_ptr<Ogre::RaySceneQuery>(
        sceneMgr->createRayQuery(ray, m_landmarksQueryFlag)
    );

    raySceneQuery->setSortByDistance(true);
    const auto& queryResult = raySceneQuery->execute();

    if(queryResult.empty())
    {
        return;
    }

    bool found = false;

    for(std::size_t qrIdx = 0 ; qrIdx < queryResult.size() && !found ; qrIdx++)
    {
        const Ogre::MovableObject* const object = queryResult[qrIdx].movable;
        for(std::shared_ptr<Landmark>& landmark : m_manualObjects)
        {
            if(landmark->m_object == object)
            {
                const auto landmarks = m_landmarks.lock();

                m_pickedData = landmark;
                m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);

                found = true;
                break;
            }
        }
    }

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

            // Send signal with world coordinates of the landmarks
            this->signal<world_coordinates_signal_t>(s_SEND_WORLD_COORD)->asyncEmit(
                point[0],
                point[1],
                point[2]
            );
        }
    }
}

} // namespace sight::module::viz::scene3d::adaptor.
