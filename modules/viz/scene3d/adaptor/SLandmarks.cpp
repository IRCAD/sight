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
static const core::com::Slots::SlotKeyType s_RENAME_GROUP_SLOT     = "renameGroup";

const core::com::Signals::SignalKeyType SLandmarks::s_SEND_WORLD_COORD = "sendWorldCoord";

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
    static const std::string s_FONT_SOURCE_CONFIG     = s_CONFIG + "fontSource";
    static const std::string s_LABEL_CONFIG           = s_CONFIG + "label";
    static const std::string s_ORIENTATION_CONFIG     = s_CONFIG + "orientation";
    static const std::string s_LANDMARKS_FLAGS_CONFIG = s_CONFIG + "landmarksQueryFlags";
    static const std::string s_INTERACTIVE_CONFIG     = s_CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG        = s_CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG      = s_CONFIG + "queryMask";
    static const std::string s_VIEW_DISTANCE          = s_CONFIG + "viewDistance";

    m_fontSource   = config.get(s_FONT_SOURCE_CONFIG, m_fontSource);
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
        SIGHT_ERROR("Unknown view distance, allow values are `slicesInRange`, `currentSlice` and `allSlices`");
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
    // Make the context as current.
    this->getRenderService()->makeCurrent();

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

void SLandmarks::insertMyPoint(std::string _groupName, std::size_t _index, const data::Landmarks::csptr& _landmarks)
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
    this->hideMyLandmark(*m_manualObjects.back(), *_landmarks);

    // Request the rendering.
    this->requestRender();
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
    // Retrieve group.
    const data::Landmarks::LandmarksGroup& group = landmarks.getGroup(landmark.m_groupName);

    bool show = group.m_visibility && m_isVisible;

    if(show)
    {
        const auto& image = m_image.const_lock();

        if(image)
        {
            // Show the landmark only if the slice is inside it.
            const float position       = landmark.m_node->getPosition()[m_orientation];
            const float slice_position = m_currentSlicePos[m_orientation];
            const auto spacing         = float(image->getSpacing()[m_orientation]);

            switch(m_viewDistance)
            {
                // Use the group size to show the landmark.
                case ViewDistance::SLICES_IN_RANGE:
                {
                    const float group_size = group.m_size * 0.5F;
                    const float max_size   = std::max(group_size, spacing);

                    show = core::tools::is_greater(position, (slice_position - group_size))
                           && core::tools::is_less(position, (slice_position + max_size));

                    break;
                }

                case ViewDistance::CURRENT_SLICE:
                {
                    // For pickers that return the exact slice position like VoxelPicker
                    show = core::tools::is_equal(position, slice_position)
                           // For picker that returns position in between two slices like SPicker
                           || (core::tools::is_greater(position, slice_position)
                               && core::tools::is_less(position, (slice_position + spacing)))
                           // Special case for SPicker which returns the last slice position at end
                           || (core::tools::is_equal(float(image->getSize()[m_orientation] - 1), position - spacing)
                               && core::tools::is_equal(slice_position, position - spacing));

                    break;
                }

                default:
                    break;
            }
        }
    }

    // Show or hide the landmark.
    landmark.m_object->setVisible(show);
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
    if(_button == LEFT)
    {
        const auto layer = this->getLayer();

        Ogre::SceneManager* const sceneMgr = layer->getSceneManager();

        const Ogre::Camera* const cam = layer->getDefaultCamera();
        const auto* const vp          = cam->getViewport();

        const float vpX = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vpY = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = cam->getCameraToViewportRay(vpX, vpY);

        Ogre::RaySceneQuery* const raySceneQuery = sceneMgr->createRayQuery(ray, m_landmarksQueryFlag);
        raySceneQuery->setSortByDistance(false);
        if(!raySceneQuery->execute().empty())
        {
            bool found             = false;
            const Ogre::Real scale = 1.15F;

            const Ogre::RaySceneQueryResult& queryResult = raySceneQuery->getLastResults();
            for(std::size_t qrIdx = 0 ; qrIdx < queryResult.size() && !found ; qrIdx++)
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
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SLandmarks::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_pickedData != nullptr)
    {
        const Ogre::Real scale = 1.F;
        m_pickedData->m_node->setScale(scale, scale, scale);
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

    bool found                               = false;
    Ogre::RaySceneQuery* const raySceneQuery = sceneMgr->createRayQuery(ray, m_landmarksQueryFlag);
    raySceneQuery->setSortByDistance(false);
    if(!raySceneQuery->execute().empty())
    {
        const Ogre::Real scale = 1.15F;

        const Ogre::RaySceneQueryResult& queryResult = raySceneQuery->getLastResults();
        for(std::size_t qrIdx = 0 ; qrIdx < queryResult.size() && !found ; qrIdx++)
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
