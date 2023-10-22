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

#include "modules/viz/scene3d/adaptor/landmarks.hpp"

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slots.hxx>
#include <core/tools/compare.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static constexpr Ogre::Real s_SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real s_DEFAULT_SCALE  = 1.F;

//------------------------------------------------------------------------------

Ogre::Vector3 landmarks::getCamDirection(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

landmarks::landmarks() noexcept
{
    SIGHT_WARN(
        "'sight::module::viz::scene3d::adaptor::landmarks' is deprecated, please use"
        " 'sight::module::viz::scene3d_qt::adaptor::landmarks' instead."
    );

    new_slot(Slots::REMOVE_GROUP, &landmarks::removeGroup, this);
    new_slot(Slots::MODIFY_GROUP, &landmarks::modifyGroup, this);
    new_slot(Slots::MODIFY_POINT, &landmarks::modifyPoint, this);
    new_slot(Slots::ADD_POINT, &landmarks::addPoint, this);
    new_slot(Slots::REMOVE_POINT, &landmarks::removePoint, this);
    new_slot(Slots::INSERT_POINT, &landmarks::insertPoint, this);
    new_slot(Slots::SELECT_POINT, &landmarks::selectPoint, this);
    new_slot(Slots::DESELECT_POINT, &landmarks::deselectPoint, this);
    new_slot(Slots::INITIALIZE_IMAGE, &landmarks::initializeImage, this);
    new_slot(Slots::SLICE_TYPE, &landmarks::changeSliceType, this);
    new_slot(Slots::SLICE_INDEX, &landmarks::changeSliceIndex, this);
    new_slot(Slots::RENAME_GROUP, &landmarks::renameGroup, this);
    new_slot(Slots::TOGGLE_ADD_LANDMARKS, &landmarks::toggleAddLandmarks, this);
    new_slot(Slots::TOGGLE_REMOVE_LANDMARKS, &landmarks::toggleRemoveLandmarks, this);
    new_slot(Slots::REMOVE_LANDMARKS, &landmarks::removeLandmarks, this);
    new_slot(Slots::CREATE_LANDMARK, &landmarks::createLandmark, this);
    new_slot(Slots::CONFIGURE_LANDMARKS, &landmarks::configureLandmarks, this);
}

//-----------------------------------------------------------------------------

void landmarks::configuring()
{
    configureParams();

    const auto& config = this->get_config();

    setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            get_id() + "_transform"
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

    std::string hexa_mask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_QUERY_MASK_CONFIG + ") should start with '0x'. Given value : " + hexa_mask,
            hexa_mask.length() > 2 && hexa_mask.substr(0, 2) == "0x"
        );
        m_queryMask = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    hexa_mask = config.get<std::string>(s_LANDMARKS_FLAGS_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_LANDMARKS_FLAGS_CONFIG + ") should start with '0x'. Given value : " + hexa_mask,
            hexa_mask.length() > 2 && hexa_mask.substr(0, 2) == "0x"
        );
        m_landmarksQueryFlag = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
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
    auto color = std::make_shared<sight::data::color>();
    color->setRGBA(config.get<std::string>(s_INITIAL_COLOR, "#FFFF00FF"));
    m_currentColor = {color->red(), color->green(), color->blue(), color->alpha()};

    // Initial size
    m_currentSize = config.get<float>(s_INITIAL_SIZE, m_currentSize);

    // Initial shape
    const std::string& initial_shape = config.get<std::string>(s_INITIAL_SHAPE, "sphere");
    if(initial_shape == "sphere")
    {
        m_currentShape = sight::data::landmarks::Shape::SPHERE;
    }
    else if(initial_shape == "cube")
    {
        m_currentShape = sight::data::landmarks::Shape::CUBE;
    }
    else
    {
        SIGHT_ERROR("Unknown shape, allowed values are `sphere` and `cube`");
    }
}

//-----------------------------------------------------------------------------

void landmarks::starting()
{
    initialize();

    getRenderService()->makeCurrent();

    auto* root_scene_node = getSceneManager()->getRootSceneNode();
    m_transNode = getOrCreateTransformNode(root_scene_node);

    m_material = std::make_shared<data::material>();
    m_material->setDiffuse(std::make_shared<data::color>(1.F, 1.F, 1.F, 1.F));

    // Register the material adaptor.
    m_materialAdaptor = this->registerService<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_materialAdaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
    m_materialAdaptor->configure(
        this->get_id() + m_materialAdaptor->get_id(),
        this->get_id() + m_materialAdaptor->get_id(),
        this->getRenderService(),
        m_layerID
    );
    m_materialAdaptor->start();

    m_materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    m_materialAdaptor->update();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
        getLayer()->addInteractor(interactor, m_priority);
    }

    // Set current slice indexes.
    initializeImage();

    // Draw landmarks.
    updating();
}

//-----------------------------------------------------------------------------

service::connections_t landmarks::auto_connections() const
{
    service::connections_t connections;

    connections.push(s_TRANSFORM_CONFIG, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    connections.push(s_LANDMARKS_INOUT, data::landmarks::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_REMOVED_SIG, Slots::REMOVE_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_MODIFIED_SIG, Slots::MODIFY_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::GROUP_RENAMED_SIG, Slots::RENAME_GROUP);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_MODIFIED_SIG, Slots::MODIFY_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_ADDED_SIG, Slots::ADD_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_REMOVED_SIG, Slots::REMOVE_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_INSERTED_SIG, Slots::INSERT_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_SELECTED_SIG, Slots::SELECT_POINT);
    connections.push(s_LANDMARKS_INOUT, data::landmarks::POINT_DESELECTED_SIG, Slots::DESELECT_POINT);
    connections.push(s_IMAGE_INPUT, data::image::MODIFIED_SIG, Slots::INITIALIZE_IMAGE);
    connections.push(s_IMAGE_INPUT, data::image::SLICE_TYPE_MODIFIED_SIG, Slots::SLICE_TYPE);
    connections.push(s_IMAGE_INPUT, data::image::SLICE_INDEX_MODIFIED_SIG, Slots::SLICE_INDEX);

    return connections;
}

//-----------------------------------------------------------------------------

void landmarks::updating()
{
    // Delete all groups.
    removeAll();

    const auto& landmarks = m_landmarks.const_lock();

    // Create all point.
    for(const auto& group_name : landmarks->getGroupNames())
    {
        const auto& group = landmarks->getGroup(group_name);
        for(std::size_t index = 0 ; index < group.m_points.size() ; ++index)
        {
            this->insertMyPoint(group_name, index, landmarks.get_shared());
        }
    }
}

//-----------------------------------------------------------------------------

void landmarks::stopping()
{
    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
        getLayer()->removeInteractor(interactor);
    }

    // Stop all threads.
    for(const auto& selected_landmark : m_selectedLandmarks)
    {
        selected_landmark->m_timer->stop();
    }

    removeAll();

    // Unregister the material adaptor.
    unregisterServices();
}

//------------------------------------------------------------------------------

void landmarks::removeAll()
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    auto* scene_mgr = getSceneManager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto it = m_manualObjects.begin() ; it != m_manualObjects.end() ; )
    {
        const auto& landmark = *it;
        const auto& name     = landmark->m_groupName;

        deselectPoint(name, landmark->m_index);

        if(m_enableLabels)
        {
            landmark->m_label->detachFromNode();
        }

        m_transNode->removeAndDestroyChild(landmark->m_node);
        scene_mgr->destroyManualObject(landmark->m_object);

        it = m_manualObjects.erase(it);
    }

    // Request the rendering.
    requestRender();
}

//------------------------------------------------------------------------------

void landmarks::removeGroup(std::string _group_name)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    auto* scene_mgr = getSceneManager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto object_it = m_manualObjects.begin() ; object_it != m_manualObjects.end() ; )
    {
        const std::string& name = (*object_it)->m_groupName;
        if(name.find(_group_name) != std::string::npos)
        {
            // Stop the thread if it already run since we are deleting data.
            deselectPoint(_group_name, (*object_it)->m_index);

            if(m_enableLabels)
            {
                (*object_it)->m_label->detachFromNode();
            }

            m_transNode->removeAndDestroyChild((*object_it)->m_node);
            scene_mgr->destroyManualObject((*object_it)->m_object);

            object_it = m_manualObjects.erase(object_it);
        }
        else
        {
            ++object_it;
        }
    }

    // Request the rendering.
    requestRender();
}

//------------------------------------------------------------------------------

void landmarks::modifyGroup(std::string _group_name)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // Get all selected point.
    std::vector<std::size_t> indexes;
    for(const std::shared_ptr<SelectedLandmark>& landmark : m_selectedLandmarks)
    {
        indexes.push_back(landmark->m_landmark->m_index);
    }

    // Remove the group.
    removeGroup(_group_name);

    // Get landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->getGroup(_group_name);

    std::size_t group_size = group.m_points.size();

    // Re-create the group.
    for(std::size_t index = 0 ; index < group_size ; ++index)
    {
        this->insertMyPoint(_group_name, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->selectPoint(_group_name, index);
    }
}

//------------------------------------------------------------------------------

void landmarks::renameGroup(std::string _old_group_name, std::string _new_group_name)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Get all selected point.
    std::vector<std::size_t> indexes;
    for(const auto& landmark : m_selectedLandmarks)
    {
        indexes.push_back(landmark->m_landmark->m_index);
    }

    // Remove the group.
    removeGroup(_old_group_name);

    // Get landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->getGroup(_new_group_name);

    const std::size_t group_size = group.m_points.size();

    // Re-create the group.
    for(std::size_t index = 0 ; index < group_size ; ++index)
    {
        this->insertMyPoint(_new_group_name, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->selectPoint(_new_group_name, index);
    }
}

//------------------------------------------------------------------------------

void landmarks::modifyPoint(std::string _group_name, std::size_t _index)
{
    const auto& landmarks = m_landmarks.const_lock();
    const auto& point     = landmarks->getPoint(_group_name, _index);

    for(auto& m_manual_object : m_manualObjects)
    {
        const auto& name = m_manual_object->m_groupName;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            m_manual_object->m_node->setPosition(position);
            break;
        }
    }

    getRenderService()->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::addPoint(std::string _group_name)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // Get landmarks.
    const auto landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->getGroup(_group_name);
    SIGHT_ASSERT(
        "They must have at least one point in the group `" << _group_name << "`",
        !group.m_points.empty()
    );

    // Get the last index.
    std::size_t index = group.m_points.size() - 1;

    // Add the new point.
    this->insertMyPoint(_group_name, index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

void landmarks::removePoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    Ogre::SceneManager* scene_mgr = this->getSceneManager();

    // Find object where name match _groupName and the index, and delete Ogre's resources.
    for(auto object_it = m_manualObjects.begin() ; object_it != m_manualObjects.end() ; ++object_it)
    {
        const std::string& name = (*object_it)->m_groupName;
        if(name.find(_group_name) != std::string::npos && (*object_it)->m_index == _index)
        {
            // Stop the thread if it already run since we are deleting data.
            this->deselectPoint(_group_name, _index);

            if(m_enableLabels)
            {
                (*object_it)->m_label->detachFromNode();
            }

            m_transNode->removeAndDestroyChild((*object_it)->m_node);
            scene_mgr->destroyManualObject((*object_it)->m_object);
            object_it = m_manualObjects.erase(object_it);
            break;
        }
    }

    // Re-compute index of landmarks in the same group.
    for(auto& m_manual_object : m_manualObjects)
    {
        const std::string& name = m_manual_object->m_groupName;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index > _index)
        {
            m_manual_object->m_index -= 1;
            if(m_enableLabels)
            {
                m_manual_object->m_label->setText(_group_name + "_" + std::to_string(m_manual_object->m_index));
            }
        }
    }

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::insertPoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current
    getRenderService()->makeCurrent();

    const auto landmarks = m_landmarks.const_lock();
    insertMyPoint(_group_name, _index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::Landmark> landmarks::insertMyPoint(
    std::string _group_name,
    std::size_t _index,
    const data::landmarks::csptr& _landmarks
)
{
    // Retrieve group.
    const data::landmarks::LandmarksGroup& group = _landmarks->getGroup(_group_name);

    // Create the point name.
    const std::string point_name = _group_name + "_" + std::to_string(_index);

    // Create the manual object.
    const Ogre::ColourValue color =
        Ogre::ColourValue(group.m_color[0], group.m_color[1], group.m_color[2], group.m_color[3]);

    Ogre::SceneManager* scene_mgr = this->getSceneManager();
    Ogre::ManualObject* object    = scene_mgr->createManualObject(this->get_id() + "_" + point_name + "_object");
    switch(group.m_shape)
    {
        case data::landmarks::Shape::SPHERE:
            sight::viz::scene3d::helper::manual_object::createSphere(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                group.m_size * 0.5F
            );
            break;

        case data::landmarks::Shape::CUBE:
            sight::viz::scene3d::helper::manual_object::createCube(
                object,
                m_materialAdaptor->getMaterialName(),
                color,
                group.m_size
            );
            break;
    }

    object->setQueryFlags(m_landmarksQueryFlag);

    Ogre::SceneNode* node = m_transNode->createChildSceneNode(this->get_id() + "_" + point_name + "_node");

    // Set the point to the right position.
    const data::landmarks::point_t& point = _landmarks->getPoint(_group_name, _index);
    node->setPosition(Ogre::Real(point[0]), Ogre::Real(point[1]), Ogre::Real(point[2]));

    // Attach data.
    node->attachObject(object);

    // Create the label.
    sight::viz::scene3d::text::sptr text;
    if(m_enableLabels)
    {
        // Create the label.
        text = sight::viz::scene3d::text::make(this->getLayer());
        text->setFontSize(m_fontSize);
        text->setText(point_name);
        text->setTextColor(color);
        text->setVisible(group.m_visibility && m_isVisible);

        // Attach data.
        text->attachToNode(node, this->getLayer()->getDefaultCamera());
    }

    // Store the created data.
    auto new_landmark = std::make_shared<Landmark>(node, object, _group_name, _index, text);
    m_manualObjects.push_back(new_landmark);

    // Hide landmarks if an image is given to the service.
    this->hideMyLandmark(*m_manualObjects.back(), *_landmarks);

    // Request the rendering.
    this->requestRender();

    return new_landmark;
}

//------------------------------------------------------------------------------

void landmarks::selectPoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    for(auto& m_manual_object : m_manualObjects)
    {
        const std::string& name = m_manual_object->m_groupName;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index == _index)
        {
            const auto it = std::find_if(
                m_selectedLandmarks.begin(),
                m_selectedLandmarks.end(),
                [&](
                    std::shared_ptr<SelectedLandmark> _landmark)
                {
                    return _landmark->m_landmark->m_groupName == _group_name && _landmark->m_landmark->m_index == _index;
                });

            if(it == m_selectedLandmarks.end())
            {
                // This method must be synchronized with deselectPoint(std::string, std::size_t).
                std::lock_guard guard(m_selectedMutex);

                // Create thread data.
                std::shared_ptr<SelectedLandmark> selected_landmark =
                    std::make_shared<SelectedLandmark>(this->worker()->create_timer(), m_manual_object);
                m_selectedLandmarks.push_back(selected_landmark);

                // Run a thread that change the selected point.
                core::thread::timer::time_duration_t duration = std::chrono::milliseconds(500);
                selected_landmark->m_timer->set_function(
                    [this, selected_landmark](auto&& ...)
                    {
                        hightlight(selected_landmark);
                    });
                selected_landmark->m_timer->set_duration(duration);
                selected_landmark->m_timer->start();
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::deselectPoint(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // This method must be synchronized with selectPoint(std::string, std::size_t).
    std::lock_guard guard(m_selectedMutex);

    // Find the thread and stop it.
    for(auto landmark_it = m_selectedLandmarks.begin() ; landmark_it != m_selectedLandmarks.end() ; ++landmark_it)
    {
        if((*landmark_it)->m_landmark->m_groupName == _group_name && (*landmark_it)->m_landmark->m_index == _index)
        {
            // Stop the timer.
            (*landmark_it)->m_timer->stop();
            (*landmark_it)->m_landmark->m_object->setVisible(true);
            hideLandmark((*landmark_it)->m_landmark);

            // Request the rendering.
            requestRender();

            m_selectedLandmarks.erase(landmark_it);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::hightlight(std::shared_ptr<SelectedLandmark> _selected_landmark)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    // Hightlight the selected landmark.
    this->hideLandmark(_selected_landmark->m_landmark);
    if(_selected_landmark->m_landmark->m_object->isVisible())
    {
        if(_selected_landmark->m_show)
        {
            _selected_landmark->m_landmark->m_object->setVisible(true);
        }
        else
        {
            _selected_landmark->m_landmark->m_object->setVisible(false);
        }

        this->requestRender();
    }

    _selected_landmark->m_show = !_selected_landmark->m_show;

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::initializeImage()
{
    const auto image = m_image.lock();

    if(image)
    {
        namespace Helper = sight::data::helper::medical_image;

        const auto axial_index = std::max(
            std::int64_t(0),
            Helper::get_slice_index(*image, Helper::orientation_t::AXIAL).value_or(0)
        );

        const auto frontal_index = std::max(
            std::int64_t(0),
            Helper::get_slice_index(*image, Helper::orientation_t::FRONTAL).value_or(0)
        );

        const auto m_sagittal_index = std::max(
            std::int64_t(0),
            Helper::get_slice_index(*image, Helper::orientation_t::SAGITTAL).value_or(0)
        );

        this->changeSliceIndex(int(axial_index), int(frontal_index), int(m_sagittal_index));
    }
    else
    {
        this->changeSliceIndex(0, 0, 0);
    }
}

//------------------------------------------------------------------------------

void landmarks::changeSliceType(int _from, int _to)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto to_orientation   = static_cast<OrientationMode>(_to);
    const auto from_orientation = static_cast<OrientationMode>(_from);

    const auto new_orientation = m_orientation == to_orientation ? from_orientation
                                                                 : m_orientation
                                 == from_orientation ? to_orientation : m_orientation;

    m_orientation = new_orientation;

    this->hideLandmarks();

    // Request the rendering.
    this->requestRender();
}

//------------------------------------------------------------------------------

void landmarks::changeSliceIndex(int _axial_index, int _frontal_index, int _sagittal_index)
{
    // Make the context as current.
    this->getRenderService()->makeCurrent();

    const auto image_lock = m_image.lock();

    if(image_lock)
    {
        this->getRenderService()->makeCurrent();

        const auto& img_spacing = image_lock->getSpacing();
        const auto& img_origin  = image_lock->getOrigin();

        m_currentSlicePos = {
            static_cast<float>(_sagittal_index) * static_cast<float>(img_spacing[0])
            + static_cast<float>(img_origin[0]),
            static_cast<float>(_frontal_index) * static_cast<float>(img_spacing[1])
            + static_cast<float>(img_origin[1]),
            static_cast<float>(_axial_index) * static_cast<float>(img_spacing[2])
            + static_cast<float>(img_origin[2])
        };

        this->hideLandmarks();

        // Request the rendering.
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void landmarks::toggleAddLandmarks(bool _toggle)
{
    if(_toggle)
    {
        m_landmarksMode = LandmarksMode::ADD;
    }
    else if(m_landmarksMode == LandmarksMode::ADD)
    {
        m_landmarksMode = LandmarksMode::NONE;
    }
}

//------------------------------------------------------------------------------

void landmarks::toggleRemoveLandmarks(bool _toggle)
{
    if(_toggle)
    {
        m_landmarksMode = LandmarksMode::REMOVE;
    }
    else if(m_landmarksMode == LandmarksMode::REMOVE)
    {
        m_landmarksMode = LandmarksMode::NONE;
    }
}

//------------------------------------------------------------------------------

void landmarks::removeLandmarks()
{
    auto landmarks = m_landmarks.lock();

    if(!landmarks)
    {
        // No need to continue if there is no landmarks
        return;
    }

    const auto& image = m_image.const_lock();
    SIGHT_THROW_IF("Data is null.", !image);

    const auto slice_index = sight::data::helper::medical_image::get_slice_index(
        *image,
        m_orientation
    );

    if(!slice_index)
    {
        // No slice selected, early return
        return;
    }

    bool has_deleted = false;

    for(const auto& name : landmarks->getGroupNames())
    {
        auto& group = landmarks->getGroup(name);

        for(auto it = group.m_points.begin() ; it < group.m_points.end() ; )
        {
            if(isLandmarkVisible(*it, group.m_size))
            {
                it          = group.m_points.erase(it);
                has_deleted = true;
            }
            else
            {
                ++it;
            }
        }
    }

    if(has_deleted)
    {
        const auto& sig = landmarks->signal<sight::data::landmarks::modified_signal_t>(
            sight::data::landmarks::MODIFIED_SIG
        );
        sig->async_emit();
    }
}

//------------------------------------------------------------------------------

void landmarks::configureLandmarks(
    std::optional<std::string> _group,
    std::optional<sight::data::landmarks::color_t> _color,
    std::optional<sight::data::landmarks::size_t> _size,
    std::optional<sight::data::landmarks::Shape> _shape,
    std::optional<int> _group_max,
    std::optional<int> _visible_max,
    std::optional<int> _total_max
)
{
    if(_group)
    {
        m_currentGroup = *_group;
    }

    if(_color)
    {
        m_currentColor = *_color;
    }

    if(_size)
    {
        m_currentSize = *_size;
    }

    if(_shape)
    {
        m_currentShape = *_shape;
    }

    if(_group_max)
    {
        if(*_group_max >= 0)
        {
            m_groupMax[m_currentGroup] = std::size_t(*_group_max);
        }
        else
        {
            m_groupMax.erase(m_currentGroup);
        }
    }

    if(_visible_max)
    {
        if(*_visible_max >= 0)
        {
            m_visibleMax = *_visible_max;
        }
        else
        {
            m_visibleMax = std::nullopt;
        }
    }

    if(_total_max)
    {
        if(*_total_max >= 0)
        {
            m_totalMax = *_total_max;
        }
        else
        {
            m_totalMax = std::nullopt;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::createLandmark(sight::data::landmarks::point_t _point)
{
    createAndPickLandmark(_point, false);
}

//------------------------------------------------------------------------------

void landmarks::createAndPickLandmark(const sight::data::landmarks::point_t& _point, bool _pick)
{
    if(isMaxLandmarksReached())
    {
        // No need to continue if the maximum number of landmarks is reached
        SIGHT_DEBUG("Maximum number of landmarks reached.");
        return;
    }

    auto landmarks = m_landmarks.lock();

    // If the group does not exist, we create it.
    if(!landmarks->hasGroup(m_currentGroup))
    {
        landmarks->addGroup(m_currentGroup, m_currentColor, m_currentSize, m_currentShape);
        const auto& sig = landmarks->signal<sight::data::landmarks::group_added_signal_t>(
            sight::data::landmarks::GROUP_ADDED_SIG
        );

        sig->async_emit(m_currentGroup);
    }

    landmarks->addPoint(m_currentGroup, _point);

    // Get the last index.
    const auto& group = landmarks->getGroup(m_currentGroup);

    // Add the new point.
    const auto& new_landmark = insertMyPoint(m_currentGroup, group.m_points.size() - 1, landmarks.get_shared());

    if(_pick)
    {
        m_pickedData = new_landmark;
        m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);
    }
    else
    {
        m_pickedData = nullptr;
    }

    // Block the signal to avoid being called back.
    const auto& sig = landmarks->signal<sight::data::landmarks::point_added_signal_t>(
        sight::data::landmarks::POINT_ADDED_SIG
    );

    sight::core::com::connection::blocker blocker(sig->get_connection(slot(Slots::ADD_POINT)));
    sig->async_emit(m_currentGroup);
}

//------------------------------------------------------------------------------

bool landmarks::isMaxLandmarksReached()
{
    if(!m_totalMax && !m_visibleMax && m_groupMax.empty())
    {
        // Early return if we must not count anything
        return false;
    }

    // Count landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    if(m_groupMax.contains(m_currentGroup)
       && (m_groupMax[m_currentGroup] == 0
           || (landmarks->hasGroup(m_currentGroup)
               && landmarks->getGroup(m_currentGroup).m_points.size() >= m_groupMax[m_currentGroup])))
    {
        return true;
    }

    if(m_totalMax || m_visibleMax)
    {
        const auto& names = landmarks->getGroupNames();

        std::size_t max         = 0;
        std::size_t max_visible = 0;

        for(const auto& name : names)
        {
            const auto& group = landmarks->getGroup(name);

            if(m_totalMax)
            {
                max += group.m_points.size();

                if(max >= *m_totalMax)
                {
                    return true;
                }
            }

            if(m_visibleMax)
            {
                for(const auto& point : group.m_points)
                {
                    if(isLandmarkVisible(point, group.m_size))
                    {
                        ++max_visible;

                        if(max_visible >= *m_visibleMax)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

void landmarks::hideLandmarks()
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    const auto image_lock = m_image.lock();

    // Hide landmarks only if there is an image.
    if(image_lock)
    {
        for(const auto& landmark : m_manualObjects)
        {
            hideLandmark(landmark);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::hideLandmark(std::shared_ptr<Landmark> _landmark)
{
    // Make the context as current.
    getRenderService()->makeCurrent();

    // Get image.
    const auto& landmarks = m_landmarks.const_lock();
    hideMyLandmark(*_landmark, *landmarks);
}

//------------------------------------------------------------------------------

void landmarks::hideMyLandmark(Landmark& _landmark, const data::landmarks& _landmarks)
{
    const bool must_show =
        [&]
        {
            // Retrieve group
            if(!_landmarks.hasGroup(_landmark.m_groupName))
            {
                return false;
            }

            const auto& group = _landmarks.getGroup(_landmark.m_groupName);

            if(!group.m_visibility)
            {
                return false;
            }

            if(const auto& image = m_image.const_lock(); image)
            {
                const auto& position = _landmark.m_node->getPosition();
                return isLandmarkVisible({position[0], position[1], position[2]}, group.m_size);
            }

            return true;
        }();

    // Show or hide the landmark.
    _landmark.m_object->setVisible(must_show);
}

//------------------------------------------------------------------------------

bool landmarks::isLandmarkVisible(
    const data::landmarks::point_t& _point,
    data::landmarks::size_t _group_size
) const
{
    if(const auto& image = m_image.const_lock(); image)
    {
        const auto position       = _point[m_orientation];
        const auto slice_position = m_currentSlicePos[m_orientation];
        const auto spacing        = image->getSpacing()[m_orientation];

        switch(m_viewDistance)
        {
            case ViewDistance::SLICES_IN_RANGE:
            {
                // Check if the position is the same than slice position
                const auto group_half_size = _group_size * 0.5;
                const auto max_size        = std::max(group_half_size, spacing);

                return core::tools::is_greater(position, (slice_position - _group_size))
                       && core::tools::is_less(position, (slice_position + max_size));
            }

            case ViewDistance::CURRENT_SLICE:
            {
                // Check if the position is the same than slice position
                const auto rounded_position       = std::round(position / spacing);
                const auto rounded_slice_position = std::round(slice_position / spacing);
                return core::tools::is_equal(rounded_position, rounded_slice_position);
            }

            default:
                break;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

void landmarks::setVisible(bool _visible)
{
    const auto landmarks = m_landmarks.const_lock();
    for(const auto& landmark : m_manualObjects)
    {
        const auto& group = landmarks->getGroup(landmark->m_groupName);
        landmark->m_object->setVisible(_visible && group.m_visibility);
        if(m_enableLabels)
        {
            landmark->m_label->setVisible(_visible && group.m_visibility);
        }
    }

    requestRender();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> landmarks::getNearestPickedPosition(int _x, int _y)
{
    Ogre::SceneManager* sm = this->getSceneManager();
    const auto result      = sight::viz::scene3d::utils::pickObject(_x, _y, m_queryMask, *sm);

    if(result.has_value())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::layer::s_DEFAULT_CAMERA_NAME);
        const auto* const vp     = camera->getViewport();

        // Screen to viewport space conversion.
        const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
        const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

        const Ogre::Ray ray = camera->getCameraToViewportRay(vp_x, vp_y);

        Ogre::Vector3 normal = -ray.getDirection();
        normal.normalise();

        return result->second + normal * 0.01F;
    }

    return std::nullopt;
}

//------------------------------------------------------------------------------

void landmarks::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    if(_button != LEFT)
    {
        // We only manage left click.
        return;
    }

    const auto layer = this->getLayer();

    Ogre::SceneManager* const scene_mgr = layer->getSceneManager();

    const Ogre::Camera* const cam = layer->getDefaultCamera();
    const auto* const vp          = cam->getViewport();

    const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

    const auto ray_scene_query = std::unique_ptr<Ogre::RaySceneQuery>(
        scene_mgr->createRayQuery(ray, m_landmarksQueryFlag)
    );

    ray_scene_query->setSortByDistance(true);
    const auto& query_result = ray_scene_query->execute();

    if(query_result.empty())
    {
        // Nothing found.
        return;
    }

    // Find the ogre landmark and apply a scale.
    bool found = false;

    // The plane is always intercepted, if a landmark is found
    for(std::size_t index = 0 ; !found && index < query_result.size() ; ++index)
    {
        // The landmark should be the nearest object found.
        const auto* const object = query_result[index].movable;

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

                            const auto& sig = landmarks->signal<sight::data::landmarks::point_removed_signal_t>(
                                sight::data::landmarks::POINT_REMOVED_SIG
                            );

                            // Block the signal to avoid a being called back.
                            sight::core::com::connection::blocker blocker(
                                sig->get_connection(slot(Slots::REMOVE_POINT))
                            );

                            sig->async_emit(m_currentGroup, landmark->m_index);
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
        if(auto new_pos = this->getNearestPickedPosition(_x, _y); new_pos)
        {
            createAndPickLandmark({(*new_pos)[0], (*new_pos)[1], (*new_pos)[2]});
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::mouseMoveEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_pickedData != nullptr)
    {
        // Discard the current landmark to launch the ray over the scene without picking this one.
        m_pickedData->m_object->setQueryFlags(0x0);

        const auto layer              = this->getLayer();
        const Ogre::Camera* const cam = layer->getDefaultCamera();
        SIGHT_ASSERT("No camera found", cam);

        bool move_in_camera_plane = true;

        Ogre::Vector3 new_pos;
        if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
        {
            // If something is picked, we will snap the landmark to it
            std::optional<Ogre::Vector3> picked_pos = this->getNearestPickedPosition(_x, _y);
            if(picked_pos.has_value())
            {
                new_pos              = picked_pos.value();
                move_in_camera_plane = false;
            }
        }

        // Else we move the distance along a plane.
        if(move_in_camera_plane)
        {
            const auto* const vp = cam->getViewport();

            const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
            const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

            const Ogre::Ray ray           = cam->getCameraToViewportRay(vp_x, vp_y);
            const Ogre::Vector3 direction = this->getCamDirection(cam);

            const Ogre::Vector3 position = m_pickedData->m_node->getPosition();

            const Ogre::Plane plane(direction, position);

            const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

            if(!hit.first)
            {
                SIGHT_ERROR("The ray must intersect the plane")
                return;
            }

            new_pos = ray.getPoint(hit.second);
        }

        // Reset the query flag.
        m_pickedData->m_object->setQueryFlags(m_landmarksQueryFlag);

        // Update the data, the autoconnection will call modifyPoint.
        auto landmarks                  = m_landmarks.lock();
        data::landmarks::point_t& point = landmarks->getPoint(m_pickedData->m_groupName, m_pickedData->m_index);
        point[0] = new_pos[0];
        point[1] = new_pos[1];
        point[2] = new_pos[2];

        const auto& sig = landmarks->signal<data::landmarks::point_modified_sig_t>(
            data::landmarks::POINT_MODIFIED_SIG
        );
        sig->async_emit(m_pickedData->m_groupName, m_pickedData->m_index);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void landmarks::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_pickedData != nullptr)
    {
        m_pickedData->m_node->setScale(s_DEFAULT_SCALE, s_DEFAULT_SCALE, s_DEFAULT_SCALE);
        m_pickedData = nullptr;

        this->getLayer()->requestRender();
    }
}

//------------------------------------------------------------------------------

void landmarks::buttonDoublePressEvent(MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y)
{
    const auto& layer = getLayer();

    Ogre::SceneManager* const scene_mgr = layer->getSceneManager();

    const Ogre::Camera* const cam = layer->getDefaultCamera();
    const auto* const vp          = cam->getViewport();

    const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

    const auto ray_scene_query = std::unique_ptr<Ogre::RaySceneQuery>(
        scene_mgr->createRayQuery(ray, m_landmarksQueryFlag)
    );

    ray_scene_query->setSortByDistance(true);
    const auto& query_result = ray_scene_query->execute();

    if(query_result.empty())
    {
        return;
    }

    bool found = false;

    for(std::size_t qr_idx = 0 ; qr_idx < query_result.size() && !found ; qr_idx++)
    {
        const auto* const object = query_result[qr_idx].movable;
        for(auto& landmark : m_manualObjects)
        {
            if(landmark->m_object == object)
            {
                m_pickedData = landmark;
                m_pickedData->m_node->setScale(s_SELECTED_SCALE, s_SELECTED_SCALE, s_SELECTED_SCALE);

                found = true;
                break;
            }
        }
    }

    if(found)
    {
        getLayer()->cancelFurtherInteraction();

        // Check if something is picked to update the position of the distance.
        const auto picked_pos = getNearestPickedPosition(_x, _y);
        if(picked_pos.has_value())
        {
            // Update the data, the autoconnection will call modifyPoint.
            const auto& landmarks = m_landmarks.const_lock();
            const auto& point     = landmarks->getPoint(
                m_pickedData->m_groupName,
                m_pickedData->m_index
            );

            // Send signal with world coordinates of the landmarks
            M_SEND_WORLD_COORD->async_emit(
                point[0],
                point[1],
                point[2]
            );
        }
    }
}

} // namespace sight::module::viz::scene3d::adaptor.
