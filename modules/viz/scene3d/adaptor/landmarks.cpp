/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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
#include <core/compare.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static constexpr Ogre::Real SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real DEFAULT_SCALE  = 1.F;

//------------------------------------------------------------------------------

Ogre::Vector3 landmarks::get_cam_direction(const Ogre::Camera* const _cam)
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

    new_slot(slots::REMOVE_GROUP, &landmarks::remove_group, this);
    new_slot(slots::MODIFY_GROUP, &landmarks::modify_group, this);
    new_slot(slots::MODIFY_POINT, &landmarks::modify_point, this);
    new_slot(slots::ADD_POINT, &landmarks::add_point, this);
    new_slot(slots::REMOVE_POINT, &landmarks::remove_point, this);
    new_slot(slots::INSERT_POINT, &landmarks::insert_point, this);
    new_slot(slots::SELECT_POINT, &landmarks::select_point, this);
    new_slot(slots::DESELECT_POINT, &landmarks::deselect_point, this);
    new_slot(slots::INITIALIZE_IMAGE, &landmarks::initialize_image, this);
    new_slot(slots::SLICE_TYPE, &landmarks::change_slice_type, this);
    new_slot(slots::SLICE_INDEX, &landmarks::change_slice_index, this);
    new_slot(slots::RENAME_GROUP, &landmarks::rename_group, this);
    new_slot(slots::TOGGLE_ADD_LANDMARKS, &landmarks::toggle_add_landmarks, this);
    new_slot(slots::TOGGLE_REMOVE_LANDMARKS, &landmarks::toggle_remove_landmarks, this);
    new_slot(slots::REMOVE_LANDMARKS, &landmarks::remove_landmarks, this);
    new_slot(slots::CREATE_LANDMARK, &landmarks::create_landmark, this);
    new_slot(slots::CONFIGURE_LANDMARKS, &landmarks::configure_landmarks, this);
}

//-----------------------------------------------------------------------------

void landmarks::configuring()
{
    configure_params();

    const auto& config = this->get_config();

    set_transform_id(
        config.get<std::string>(
            sight::viz::scene3d::transformable::TRANSFORM_CONFIG,
            get_id() + "_transform"
        )
    );

    static const std::string s_FONT_SIZE_CONFIG       = CONFIG + "fontSize";
    static const std::string s_LABEL_CONFIG           = CONFIG + "label";
    static const std::string s_ORIENTATION_CONFIG     = CONFIG + "orientation";
    static const std::string s_LANDMARKS_FLAGS_CONFIG = CONFIG + "landmarksQueryFlags";
    static const std::string s_INTERACTIVE_CONFIG     = CONFIG + "interactive";
    static const std::string s_PRIORITY_CONFIG        = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG      = CONFIG + "queryMask";
    static const std::string s_VIEW_DISTANCE          = CONFIG + "viewDistance";
    static const std::string s_INITIAL_GROUP          = CONFIG + "initialGroup";
    static const std::string s_INITIAL_COLOR          = CONFIG + "initialColor";
    static const std::string s_INITIAL_SIZE           = CONFIG + "initialSize";
    static const std::string s_INITIAL_SHAPE          = CONFIG + "initialShape";

    m_font_size     = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);
    m_enable_labels = config.get<bool>(s_LABEL_CONFIG, m_enable_labels);
    m_interactive   = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);
    m_priority      = config.get<int>(s_PRIORITY_CONFIG, m_priority);

    const std::string orientation = config.get<std::string>(s_ORIENTATION_CONFIG, "axial");
    if(orientation == "axial")
    {
        m_orientation = orientation_mode::z_axis;
    }
    else if(orientation == "frontal")
    {
        m_orientation = orientation_mode::y_axis;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = orientation_mode::x_axis;
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
        m_query_mask = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    hexa_mask = config.get<std::string>(s_LANDMARKS_FLAGS_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_LANDMARKS_FLAGS_CONFIG + ") should start with '0x'. Given value : " + hexa_mask,
            hexa_mask.length() > 2 && hexa_mask.substr(0, 2) == "0x"
        );
        m_landmarks_query_flag = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }

    // View distance
    const std::string& view_distance = config.get<std::string>(s_VIEW_DISTANCE, "slicesInRange");
    if(view_distance == "slicesInRange")
    {
        m_view_distance = view_distance::slices_in_range;
    }
    else if(view_distance == "currentSlice")
    {
        m_view_distance = view_distance::current_slice;
    }
    else if(view_distance == "allSlices")
    {
        m_view_distance = view_distance::all_slices;
    }
    else
    {
        SIGHT_ERROR("Unknown view distance, allowed values are `slicesInRange`, `currentSlice` and `allSlices`");
    }

    // Initial group
    m_current_group = config.get<std::string>(s_INITIAL_GROUP, m_current_group);

    // Initial color
    auto color = std::make_shared<sight::data::color>();
    color->from_string(config.get<std::string>(s_INITIAL_COLOR, "#FFFF00FF"));
    m_current_color = {color->red(), color->green(), color->blue(), color->alpha()};

    // Initial size
    m_current_size = config.get<float>(s_INITIAL_SIZE, m_current_size);

    // Initial shape
    const std::string& initial_shape = config.get<std::string>(s_INITIAL_SHAPE, "sphere");
    if(initial_shape == "sphere")
    {
        m_current_shape = sight::data::landmarks::shape::sphere;
    }
    else if(initial_shape == "cube")
    {
        m_current_shape = sight::data::landmarks::shape::cube;
    }
    else
    {
        SIGHT_ERROR("Unknown shape, allowed values are `sphere` and `cube`");
    }
}

//-----------------------------------------------------------------------------

void landmarks::starting()
{
    adaptor::init();

    render_service()->make_current();

    auto* root_scene_node = get_scene_manager()->getRootSceneNode();
    m_trans_node = get_or_create_transform_node(root_scene_node);

    m_material = std::make_shared<data::material>();
    m_material->set_diffuse(std::make_shared<data::color>(1.F, 1.F, 1.F, 1.F));

    // Register the material adaptor.
    m_material_adaptor = this->register_service<module::viz::scene3d::adaptor::material>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_material_adaptor->set_inout(m_material, module::viz::scene3d::adaptor::material::MATERIAL_INOUT, true);
    m_material_adaptor->configure(
        this->get_id() + m_material_adaptor->get_id(),
        this->get_id() + m_material_adaptor->get_id(),
        this->render_service(),
        m_layer_id
    );
    m_material_adaptor->start();

    m_material_adaptor->get_material_fw()->set_has_vertex_color(true);
    m_material_adaptor->update();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
        layer()->add_interactor(interactor, m_priority);
    }

    // Set current slice indexes.
    initialize_image();

    // Draw landmarks.
    updating();
}

//-----------------------------------------------------------------------------

service::connections_t landmarks::auto_connections() const
{
    service::connections_t connections = adaptor::auto_connections();

    connections.push(TRANSFORM_CONFIG, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);

    connections.push(LANDMARKS_INOUT, data::landmarks::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(LANDMARKS_INOUT, data::landmarks::GROUP_REMOVED_SIG, slots::REMOVE_GROUP);
    connections.push(LANDMARKS_INOUT, data::landmarks::GROUP_MODIFIED_SIG, slots::MODIFY_GROUP);
    connections.push(LANDMARKS_INOUT, data::landmarks::GROUP_RENAMED_SIG, slots::RENAME_GROUP);
    connections.push(LANDMARKS_INOUT, data::landmarks::POINT_MODIFIED_SIG, slots::MODIFY_POINT);
    connections.push(LANDMARKS_INOUT, data::landmarks::POINT_ADDED_SIG, slots::ADD_POINT);
    connections.push(LANDMARKS_INOUT, data::landmarks::POINT_REMOVED_SIG, slots::REMOVE_POINT);
    connections.push(LANDMARKS_INOUT, data::landmarks::POINT_INSERTED_SIG, slots::INSERT_POINT);
    connections.push(LANDMARKS_INOUT, data::landmarks::POINT_SELECTED_SIG, slots::SELECT_POINT);
    connections.push(LANDMARKS_INOUT, data::landmarks::POINT_DESELECTED_SIG, slots::DESELECT_POINT);
    connections.push(IMAGE_INPUT, data::image::MODIFIED_SIG, slots::INITIALIZE_IMAGE);
    connections.push(IMAGE_INPUT, data::image::SLICE_TYPE_MODIFIED_SIG, slots::SLICE_TYPE);
    connections.push(IMAGE_INPUT, data::image::SLICE_INDEX_MODIFIED_SIG, slots::SLICE_INDEX);

    return connections;
}

//-----------------------------------------------------------------------------

void landmarks::updating()
{
    // Delete all groups.
    remove_all();

    const auto& landmarks = m_landmarks.const_lock();

    // Create all point.
    for(const auto& group_name : landmarks->get_group_names())
    {
        const auto& group = landmarks->get_group(group_name);
        for(std::size_t index = 0 ; index < group.m_points.size() ; ++index)
        {
            this->insert_my_point(group_name, index, landmarks.get_shared());
        }
    }
}

//-----------------------------------------------------------------------------

void landmarks::stopping()
{
    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
        layer()->remove_interactor(interactor);
    }

    // Stop all threads.
    for(const auto& selected_landmark : m_selected_landmarks)
    {
        selected_landmark->m_timer->stop();
    }

    remove_all();

    // Unregister the material adaptor.
    unregister_services();

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void landmarks::remove_all()
{
    // Make the context as current.
    render_service()->make_current();

    auto* scene_mgr = get_scene_manager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto it = m_manual_objects.begin() ; it != m_manual_objects.end() ; )
    {
        const auto& landmark = *it;
        const auto& name     = landmark->m_group_name;

        deselect_point(name, landmark->m_index);

        if(m_enable_labels)
        {
            landmark->m_label->detach_from_node();
        }

        m_trans_node->removeAndDestroyChild(landmark->m_node);
        scene_mgr->destroyManualObject(landmark->m_object);

        it = m_manual_objects.erase(it);
    }

    // Request the rendering.
    request_render();
}

//------------------------------------------------------------------------------

void landmarks::remove_group(std::string _group_name)
{
    // Make the context as current.
    render_service()->make_current();

    auto* scene_mgr = get_scene_manager();

    // Find object where name match _groupName and delete Ogre's resources.
    for(auto object_it = m_manual_objects.begin() ; object_it != m_manual_objects.end() ; )
    {
        const std::string& name = (*object_it)->m_group_name;
        if(name.find(_group_name) != std::string::npos)
        {
            // Stop the thread if it already run since we are deleting data.
            deselect_point(_group_name, (*object_it)->m_index);

            if(m_enable_labels)
            {
                (*object_it)->m_label->detach_from_node();
            }

            m_trans_node->removeAndDestroyChild((*object_it)->m_node);
            scene_mgr->destroyManualObject((*object_it)->m_object);

            object_it = m_manual_objects.erase(object_it);
        }
        else
        {
            ++object_it;
        }
    }

    // Request the rendering.
    request_render();
}

//------------------------------------------------------------------------------

void landmarks::modify_group(std::string _group_name)
{
    // Make the context as current.
    render_service()->make_current();

    // Get all selected point.
    std::vector<std::size_t> indexes;
    for(const std::shared_ptr<selected_landmark>& landmark : m_selected_landmarks)
    {
        indexes.push_back(landmark->m_landmark->m_index);
    }

    // Remove the group.
    remove_group(_group_name);

    // Get landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->get_group(_group_name);

    std::size_t group_size = group.m_points.size();

    // Re-create the group.
    for(std::size_t index = 0 ; index < group_size ; ++index)
    {
        this->insert_my_point(_group_name, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->select_point(_group_name, index);
    }
}

//------------------------------------------------------------------------------

void landmarks::rename_group(std::string _old_group_name, std::string _new_group_name)
{
    // Make the context as current.
    this->render_service()->make_current();

    // Get all selected point.
    std::vector<std::size_t> indexes;
    for(const auto& landmark : m_selected_landmarks)
    {
        indexes.push_back(landmark->m_landmark->m_index);
    }

    // Remove the group.
    remove_group(_old_group_name);

    // Get landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->get_group(_new_group_name);

    const std::size_t group_size = group.m_points.size();

    // Re-create the group.
    for(std::size_t index = 0 ; index < group_size ; ++index)
    {
        this->insert_my_point(_new_group_name, index, landmarks.get_shared());
    }

    // Re-run selected landmark threads
    for(std::size_t index : indexes)
    {
        this->select_point(_new_group_name, index);
    }
}

//------------------------------------------------------------------------------

void landmarks::modify_point(std::string _group_name, std::size_t _index)
{
    const auto& landmarks = m_landmarks.const_lock();
    const auto& point     = landmarks->get_point(_group_name, _index);

    for(auto& m_manual_object : m_manual_objects)
    {
        const auto& name = m_manual_object->m_group_name;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            m_manual_object->m_node->setPosition(position);
            break;
        }
    }

    render_service()->request_render();
}

//------------------------------------------------------------------------------

void landmarks::add_point(std::string _group_name)
{
    // Make the context as current.
    render_service()->make_current();

    // Get landmarks.
    const auto landmarks = m_landmarks.const_lock();

    // Retrieve group.
    const auto& group = landmarks->get_group(_group_name);
    SIGHT_ASSERT(
        "They must have at least one point in the group `" << _group_name << "`",
        !group.m_points.empty()
    );

    // Get the last index.
    std::size_t index = group.m_points.size() - 1;

    // Add the new point.
    this->insert_my_point(_group_name, index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

void landmarks::remove_point(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    this->render_service()->make_current();

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();

    // Find object where name match _groupName and the index, and delete Ogre's resources.
    for(auto object_it = m_manual_objects.begin() ; object_it != m_manual_objects.end() ; ++object_it)
    {
        const std::string& name = (*object_it)->m_group_name;
        if(name.find(_group_name) != std::string::npos && (*object_it)->m_index == _index)
        {
            // Stop the thread if it already run since we are deleting data.
            this->deselect_point(_group_name, _index);

            if(m_enable_labels)
            {
                (*object_it)->m_label->detach_from_node();
            }

            m_trans_node->removeAndDestroyChild((*object_it)->m_node);
            scene_mgr->destroyManualObject((*object_it)->m_object);
            object_it = m_manual_objects.erase(object_it);
            break;
        }
    }

    // Re-compute index of landmarks in the same group.
    for(auto& m_manual_object : m_manual_objects)
    {
        const std::string& name = m_manual_object->m_group_name;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index > _index)
        {
            m_manual_object->m_index -= 1;
            if(m_enable_labels)
            {
                m_manual_object->m_label->set_text(_group_name + "_" + std::to_string(m_manual_object->m_index));
            }
        }
    }

    // Request the rendering.
    this->request_render();
}

//------------------------------------------------------------------------------

void landmarks::insert_point(std::string _group_name, std::size_t _index)
{
    // Make the context as current
    render_service()->make_current();

    const auto landmarks = m_landmarks.const_lock();
    insert_my_point(_group_name, _index, landmarks.get_shared());
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::landmark> landmarks::insert_my_point(
    std::string _group_name,
    std::size_t _index,
    const data::landmarks::csptr& _landmarks
)
{
    // Retrieve group.
    const data::landmarks::landmarks_group& group = _landmarks->get_group(_group_name);

    // Create the point name.
    const std::string point_name = _group_name + "_" + std::to_string(_index);

    // Create the manual object.
    const Ogre::ColourValue color =
        Ogre::ColourValue(group.m_color[0], group.m_color[1], group.m_color[2], group.m_color[3]);

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    Ogre::ManualObject* object    = scene_mgr->createManualObject(this->get_id() + "_" + point_name + "_object");
    switch(group.m_shape)
    {
        case data::landmarks::shape::sphere:
            sight::viz::scene3d::helper::manual_object::create_sphere(
                object,
                m_material_adaptor->get_material_name(),
                color,
                group.m_size * 0.5F
            );
            break;

        case data::landmarks::shape::cube:
            sight::viz::scene3d::helper::manual_object::create_cube(
                object,
                m_material_adaptor->get_material_name(),
                color,
                group.m_size
            );
            break;
    }

    object->setQueryFlags(m_landmarks_query_flag);

    Ogre::SceneNode* node = m_trans_node->createChildSceneNode(this->get_id() + "_" + point_name + "_node");

    // Set the point to the right position.
    const data::landmarks::point_t& point = _landmarks->get_point(_group_name, _index);
    node->setPosition(Ogre::Real(point[0]), Ogre::Real(point[1]), Ogre::Real(point[2]));

    // Attach data.
    node->attachObject(object);

    // Create the label.
    sight::viz::scene3d::text::sptr text;
    if(m_enable_labels)
    {
        // Create the label.
        text = sight::viz::scene3d::text::make(this->layer());
        text->set_font_size(m_font_size);
        text->set_text(point_name);
        text->set_text_color(color);
        text->set_visible(group.m_visibility && visible());

        // Attach data.
        text->attach_to_node(node, this->layer()->get_default_camera());
    }

    // Store the created data.
    auto new_landmark = std::make_shared<landmark>(node, object, _group_name, _index, text);
    m_manual_objects.push_back(new_landmark);

    // Hide landmarks if an image is given to the service.
    this->hide_my_landmark(*m_manual_objects.back(), *_landmarks);

    // Request the rendering.
    this->request_render();

    return new_landmark;
}

//------------------------------------------------------------------------------

void landmarks::select_point(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    this->render_service()->make_current();

    for(auto& m_manual_object : m_manual_objects)
    {
        const std::string& name = m_manual_object->m_group_name;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index == _index)
        {
            const auto it = std::find_if(
                m_selected_landmarks.begin(),
                m_selected_landmarks.end(),
                [&](
                    std::shared_ptr<selected_landmark> _landmark)
                {
                    return _landmark->m_landmark->m_group_name == _group_name
                           && _landmark->m_landmark->m_index == _index;
                });

            if(it == m_selected_landmarks.end())
            {
                // This method must be synchronized with deselectPoint(std::string, std::size_t).
                std::lock_guard guard(m_selected_mutex);

                // Create thread data.
                auto selected_landmark =
                    std::make_shared<landmarks::selected_landmark>(this->worker()->create_timer(), m_manual_object);
                m_selected_landmarks.push_back(selected_landmark);

                // Run a thread that change the selected point.
                core::thread::timer::time_duration_t duration = std::chrono::milliseconds(500);
                selected_landmark->m_timer->set_function(
                    [this, selected_landmark](auto&& ...)
                    {
                        highlight(selected_landmark);
                    });
                selected_landmark->m_timer->set_duration(duration);
                selected_landmark->m_timer->start();
            }

            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::deselect_point(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    render_service()->make_current();

    // This method must be synchronized with selectPoint(std::string, std::size_t).
    std::lock_guard guard(m_selected_mutex);

    // Find the thread and stop it.
    for(auto landmark_it = m_selected_landmarks.begin() ; landmark_it != m_selected_landmarks.end() ; ++landmark_it)
    {
        if((*landmark_it)->m_landmark->m_group_name == _group_name && (*landmark_it)->m_landmark->m_index == _index)
        {
            // Stop the timer.
            (*landmark_it)->m_timer->stop();
            (*landmark_it)->m_landmark->m_object->setVisible(true);
            hide_landmark((*landmark_it)->m_landmark);

            // Request the rendering.
            request_render();

            m_selected_landmarks.erase(landmark_it);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::highlight(std::shared_ptr<selected_landmark> _selected_landmark)
{
    // Make the context as current.
    this->render_service()->make_current();

    // Highlight the selected landmark.
    this->hide_landmark(_selected_landmark->m_landmark);
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

        this->request_render();
    }

    _selected_landmark->m_show = !_selected_landmark->m_show;

    // Request the rendering.
    this->request_render();
}

//------------------------------------------------------------------------------

void landmarks::initialize_image()
{
    const auto image = m_image.lock();

    if(image)
    {
        namespace Helper = sight::data::helper::medical_image;

        const auto axial_index = std::max(
            std::int64_t(0),
            Helper::get_slice_index(*image, Helper::axis_t::axial).value_or(0)
        );

        const auto frontal_index = std::max(
            std::int64_t(0),
            Helper::get_slice_index(*image, Helper::axis_t::frontal).value_or(0)
        );

        const auto m_sagittal_index = std::max(
            std::int64_t(0),
            Helper::get_slice_index(*image, Helper::axis_t::sagittal).value_or(0)
        );

        this->change_slice_index(int(axial_index), int(frontal_index), int(m_sagittal_index));
    }
    else
    {
        this->change_slice_index(0, 0, 0);
    }
}

//------------------------------------------------------------------------------

void landmarks::change_slice_type(int _from, int _to)
{
    // Make the context as current.
    this->render_service()->make_current();

    const auto to_orientation   = static_cast<orientation_mode>(_to);
    const auto from_orientation = static_cast<orientation_mode>(_from);

    const auto new_orientation = m_orientation == to_orientation ? from_orientation
                                                                 : m_orientation
                                 == from_orientation ? to_orientation : m_orientation;

    m_orientation = new_orientation;

    this->hide_landmarks();

    // Request the rendering.
    this->request_render();
}

//------------------------------------------------------------------------------

void landmarks::change_slice_index(int _axial_index, int _frontal_index, int _sagittal_index)
{
    // Make the context as current.
    this->render_service()->make_current();

    const auto image_lock = m_image.lock();

    if(image_lock)
    {
        this->render_service()->make_current();

        const auto& img_spacing = image_lock->spacing();
        const auto& img_origin  = image_lock->origin();

        m_current_slice_pos = {
            static_cast<float>(_sagittal_index) * static_cast<float>(img_spacing[0])
            + static_cast<float>(img_origin[0]),
            static_cast<float>(_frontal_index) * static_cast<float>(img_spacing[1])
            + static_cast<float>(img_origin[1]),
            static_cast<float>(_axial_index) * static_cast<float>(img_spacing[2])
            + static_cast<float>(img_origin[2])
        };

        this->hide_landmarks();

        // Request the rendering.
        this->request_render();
    }
}

//------------------------------------------------------------------------------

void landmarks::toggle_add_landmarks(bool _toggle)
{
    if(_toggle)
    {
        m_landmarks_mode = landmarks_mode::add;
    }
    else if(m_landmarks_mode == landmarks_mode::add)
    {
        m_landmarks_mode = landmarks_mode::none;
    }
}

//------------------------------------------------------------------------------

void landmarks::toggle_remove_landmarks(bool _toggle)
{
    if(_toggle)
    {
        m_landmarks_mode = landmarks_mode::remove;
    }
    else if(m_landmarks_mode == landmarks_mode::remove)
    {
        m_landmarks_mode = landmarks_mode::none;
    }
}

//------------------------------------------------------------------------------

void landmarks::remove_landmarks()
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

    for(const auto& name : landmarks->get_group_names())
    {
        auto& group = landmarks->get_group(name);

        for(auto it = group.m_points.begin() ; it < group.m_points.end() ; )
        {
            if(is_landmark_visible(*it, group.m_size))
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

void landmarks::configure_landmarks(
    std::optional<std::string> _group,
    std::optional<sight::data::landmarks::color_t> _color,
    std::optional<sight::data::landmarks::size_t> _size,
    std::optional<sight::data::landmarks::shape> _shape,
    std::optional<int> _group_max,
    std::optional<int> _visible_max,
    std::optional<int> _total_max
)
{
    if(_group)
    {
        m_current_group = *_group;
    }

    if(_color)
    {
        m_current_color = *_color;
    }

    if(_size)
    {
        m_current_size = *_size;
    }

    if(_shape)
    {
        m_current_shape = *_shape;
    }

    if(_group_max)
    {
        if(*_group_max >= 0)
        {
            m_group_max[m_current_group] = std::size_t(*_group_max);
        }
        else
        {
            m_group_max.erase(m_current_group);
        }
    }

    if(_visible_max)
    {
        if(*_visible_max >= 0)
        {
            m_visible_max = *_visible_max;
        }
        else
        {
            m_visible_max = std::nullopt;
        }
    }

    if(_total_max)
    {
        if(*_total_max >= 0)
        {
            m_total_max = *_total_max;
        }
        else
        {
            m_total_max = std::nullopt;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::create_landmark(sight::data::landmarks::point_t _point)
{
    create_and_pick_landmark(_point, false);
}

//------------------------------------------------------------------------------

void landmarks::create_and_pick_landmark(const sight::data::landmarks::point_t& _point, bool _pick)
{
    if(is_max_landmarks_reached())
    {
        // No need to continue if the maximum number of landmarks is reached
        SIGHT_DEBUG("Maximum number of landmarks reached.");
        return;
    }

    auto landmarks = m_landmarks.lock();

    // If the group does not exist, we create it.
    if(!landmarks->has_group(m_current_group))
    {
        landmarks->add_group(m_current_group, m_current_color, m_current_size, m_current_shape);
        const auto& sig = landmarks->signal<sight::data::landmarks::group_added_signal_t>(
            sight::data::landmarks::GROUP_ADDED_SIG
        );

        sig->async_emit(m_current_group);
    }

    landmarks->add_point(m_current_group, _point);

    // Get the last index.
    const auto& group = landmarks->get_group(m_current_group);

    // Add the new point.
    const auto& new_landmark = insert_my_point(m_current_group, group.m_points.size() - 1, landmarks.get_shared());

    if(_pick)
    {
        m_picked_data = new_landmark;
        m_picked_data->m_node->setScale(SELECTED_SCALE, SELECTED_SCALE, SELECTED_SCALE);
    }
    else
    {
        m_picked_data = nullptr;
    }

    // Block the signal to avoid being called back.
    const auto& sig = landmarks->signal<sight::data::landmarks::point_added_signal_t>(
        sight::data::landmarks::POINT_ADDED_SIG
    );

    sight::core::com::connection::blocker blocker(sig->get_connection(slot(slots::ADD_POINT)));
    sig->async_emit(m_current_group);
}

//------------------------------------------------------------------------------

bool landmarks::is_max_landmarks_reached()
{
    if(!m_total_max && !m_visible_max && m_group_max.empty())
    {
        // Early return if we must not count anything
        return false;
    }

    // Count landmarks.
    const auto& landmarks = m_landmarks.const_lock();

    if(m_group_max.contains(m_current_group)
       && (m_group_max[m_current_group] == 0
           || (landmarks->has_group(m_current_group)
               && landmarks->get_group(m_current_group).m_points.size() >= m_group_max[m_current_group])))
    {
        return true;
    }

    if(m_total_max || m_visible_max)
    {
        const auto& names = landmarks->get_group_names();

        std::size_t max         = 0;
        std::size_t max_visible = 0;

        for(const auto& name : names)
        {
            const auto& group = landmarks->get_group(name);

            if(m_total_max)
            {
                max += group.m_points.size();

                if(max >= *m_total_max)
                {
                    return true;
                }
            }

            if(m_visible_max)
            {
                for(const auto& point : group.m_points)
                {
                    if(is_landmark_visible(point, group.m_size))
                    {
                        ++max_visible;

                        if(max_visible >= *m_visible_max)
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

void landmarks::hide_landmarks()
{
    // Make the context as current.
    render_service()->make_current();

    const auto image_lock = m_image.lock();

    // Hide landmarks only if there is an image.
    if(image_lock)
    {
        for(const auto& landmark : m_manual_objects)
        {
            hide_landmark(landmark);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::hide_landmark(std::shared_ptr<landmark> _landmark)
{
    // Make the context as current.
    render_service()->make_current();

    // Get image.
    const auto& landmarks = m_landmarks.const_lock();
    hide_my_landmark(*_landmark, *landmarks);
}

//------------------------------------------------------------------------------

void landmarks::hide_my_landmark(landmark& _landmark, const data::landmarks& _landmarks)
{
    const bool must_show =
        [&]
        {
            // Retrieve group
            if(!_landmarks.has_group(_landmark.m_group_name))
            {
                return false;
            }

            const auto& group = _landmarks.get_group(_landmark.m_group_name);

            if(!group.m_visibility)
            {
                return false;
            }

            if(const auto& image = m_image.const_lock(); image)
            {
                const auto& position = _landmark.m_node->getPosition();
                return is_landmark_visible({position[0], position[1], position[2]}, group.m_size);
            }

            return true;
        }();

    // Show or hide the landmark.
    _landmark.m_object->setVisible(must_show);
}

//------------------------------------------------------------------------------

bool landmarks::is_landmark_visible(
    const data::landmarks::point_t& _point,
    data::landmarks::size_t _group_size
) const
{
    if(const auto& image = m_image.const_lock(); image)
    {
        const auto position       = _point[m_orientation];
        const auto slice_position = m_current_slice_pos[m_orientation];
        const auto spacing        = image->spacing()[m_orientation];

        switch(m_view_distance)
        {
            case view_distance::slices_in_range:
            {
                // Check if the position is the same than slice position
                const auto group_half_size = _group_size * 0.5;
                const auto max_size        = std::max(group_half_size, spacing);

                return core::is_greater(position, (slice_position - _group_size))
                       && core::is_less(position, (slice_position + max_size));
            }

            case view_distance::current_slice:
            {
                // Check if the position is the same than slice position
                const auto rounded_position       = std::round(position / spacing);
                const auto rounded_slice_position = std::round(slice_position / spacing);
                return core::is_equal(rounded_position, rounded_slice_position);
            }

            default:
                break;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

void landmarks::set_visible(bool _visible)
{
    const auto landmarks = m_landmarks.const_lock();
    for(const auto& landmark : m_manual_objects)
    {
        const auto& group = landmarks->get_group(landmark->m_group_name);
        landmark->m_object->setVisible(_visible && group.m_visibility);
        if(m_enable_labels)
        {
            landmark->m_label->set_visible(_visible && group.m_visibility);
        }
    }

    request_render();
}

//------------------------------------------------------------------------------

void landmarks::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    if(_button != left)
    {
        // We only manage left click.
        return;
    }

    const auto layer = this->layer();

    Ogre::SceneManager* const scene_mgr = layer->get_scene_manager();

    const Ogre::Camera* const cam = layer->get_default_camera();
    const auto* const vp          = cam->getViewport();

    const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

    const auto ray_scene_query = std::unique_ptr<Ogre::RaySceneQuery>(
        scene_mgr->createRayQuery(ray, m_landmarks_query_flag)
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

        for(auto& landmark : m_manual_objects)
        {
            if(landmark->m_object == object)
            {
                try
                {
                    if(const auto& group = landmarks->get_group(landmark->m_group_name);
                       group.m_visibility && is_landmark_visible(group.m_points[landmark->m_index], group.m_size))
                    {
                        found         = true;
                        m_picked_data = landmark;
                        m_picked_data->m_node->setScale(SELECTED_SCALE, SELECTED_SCALE, SELECTED_SCALE);

                        // If we are in remove mode, we will remove the picked landmark.
                        if(m_landmarks_mode == landmarks_mode::remove)
                        {
                            // Remove the picked landmark
                            landmarks->remove_point(landmark->m_group_name, landmark->m_index);
                            remove_point(landmark->m_group_name, landmark->m_index);

                            const auto& sig = landmarks->signal<sight::data::landmarks::point_removed_signal_t>(
                                sight::data::landmarks::POINT_REMOVED_SIG
                            );

                            // Block the signal to avoid a being called back.
                            sight::core::com::connection::blocker blocker(
                                sig->get_connection(slot(slots::REMOVE_POINT))
                            );

                            sig->async_emit(m_current_group, landmark->m_index);
                        }

                        break;
                    }
                }
                catch(...)
                {
                    SIGHT_ERROR("Landmark group '" + landmark->m_group_name + "' not found.");

                    // No point found, we can't select it.
                    continue;
                }
            }
        }
    }

    // If nothing is picked, we will create a new landmark.
    if(!found && m_landmarks_mode == landmarks_mode::add)
    {
        if(auto new_pos = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *scene_mgr, true); new_pos)
        {
            create_and_pick_landmark({new_pos->second.x, new_pos->second.y, new_pos->second.z});
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::mouse_move_event(mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked_data != nullptr)
    {
        // Discard the current landmark to launch the ray over the scene without picking this one.
        m_picked_data->m_object->setQueryFlags(0x0);

        const auto layer              = this->layer();
        const Ogre::Camera* const cam = layer->get_default_camera();
        SIGHT_ASSERT("No camera found", cam);

        bool move_in_camera_plane = true;

        Ogre::Vector3 new_pos;
        if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
        {
            Ogre::SceneManager* const scene_mgr = layer->get_scene_manager();
            // If something is picked, we will snap the landmark to it
            auto picked_pos = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *scene_mgr, true);
            if(picked_pos.has_value())
            {
                new_pos              = picked_pos->second;
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
            const Ogre::Vector3 direction = sight::module::viz::scene3d::adaptor::landmarks::get_cam_direction(cam);

            const Ogre::Vector3 position = m_picked_data->m_node->getPosition();

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
        m_picked_data->m_object->setQueryFlags(m_landmarks_query_flag);

        // Update the data, the autoconnection will call modifyPoint.
        auto landmarks                  = m_landmarks.lock();
        data::landmarks::point_t& point = landmarks->get_point(m_picked_data->m_group_name, m_picked_data->m_index);
        point[0] = new_pos[0];
        point[1] = new_pos[1];
        point[2] = new_pos[2];

        const auto& sig = landmarks->signal<data::landmarks::point_modified_sig_t>(
            data::landmarks::POINT_MODIFIED_SIG
        );
        sig->async_emit(m_picked_data->m_group_name, m_picked_data->m_index);

        this->request_render();
    }
}

//------------------------------------------------------------------------------

void landmarks::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_picked_data != nullptr)
    {
        m_picked_data->m_node->setScale(DEFAULT_SCALE, DEFAULT_SCALE, DEFAULT_SCALE);
        m_picked_data = nullptr;

        this->layer()->request_render();
    }
}

//------------------------------------------------------------------------------

void landmarks::button_double_press_event(mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y)
{
    Ogre::SceneManager* const scene_mgr = layer()->get_scene_manager();

    const Ogre::Camera* const cam = layer()->get_default_camera();
    const auto* const vp          = cam->getViewport();

    const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

    const auto ray_scene_query = std::unique_ptr<Ogre::RaySceneQuery>(
        scene_mgr->createRayQuery(ray, m_landmarks_query_flag)
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
        for(auto& landmark : m_manual_objects)
        {
            if(landmark->m_object == object)
            {
                m_picked_data = landmark;
                m_picked_data->m_node->setScale(SELECTED_SCALE, SELECTED_SCALE, SELECTED_SCALE);

                found = true;
                break;
            }
        }
    }

    if(found)
    {
        layer()->cancel_further_interaction();

        // Check if something is picked to update the position of the distance.
        const auto picked_pos = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *scene_mgr, false);
        if(picked_pos.has_value())
        {
            // Update the data, the autoconnection will call modifyPoint.
            const auto& landmarks = m_landmarks.const_lock();
            const auto& point     = landmarks->get_point(
                m_picked_data->m_group_name,
                m_picked_data->m_index
            );

            // Send signal with world coordinates of the landmarks
            m_send_world_coord->async_emit(
                point[0],
                point[1],
                point[2]
            );
        }
    }
}

} // namespace sight::module::viz::scene3d::adaptor.
