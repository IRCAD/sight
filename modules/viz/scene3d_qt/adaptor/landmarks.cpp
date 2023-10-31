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

#include "data/landmarks.hpp"

#include "landmarks.hpp"

#include "core/thread/worker.hpp"

#include "data/dicom/attribute.hpp"
#include "data/fiducials_series.hpp"
#include "data/image_series.hpp"

#include "viz/scene3d/material_adaptor.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/uuid.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>

#include <modules/viz/scene3d_qt/window_interactor.hpp>

#include <QHBoxLayout>

namespace sight::module::viz::scene3d_qt::adaptor
{

static constexpr Ogre::Real SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real DEFAULT_SCALE  = 1.F;

namespace
{

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::fiducials_series::fiducial_set> get_fiducial_set(
    const std::string& _group_name,
    data::fiducials_series::csptr _fiducials_series
)
{
    SIGHT_ASSERT("Fiducials series mustn't be null", _fiducials_series != nullptr);
    std::optional<std::pair<data::fiducials_series::fiducial_set, std::size_t> > res =
        _fiducials_series->get_fiducial_set_and_index(_group_name);
    return res.has_value() ? std::optional(res->first) : std::optional<data::fiducials_series::fiducial_set>();
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<data::fiducials_series::fiducial*> get_point_fiducials_ptr(
    data::fiducials_series::fiducial_set& _fiducial_set
)
{
    std::vector<data::fiducials_series::fiducial*> point_fiducials;
    std::ranges::for_each(
        _fiducial_set.fiducial_sequence,
        [&point_fiducials](data::fiducials_series::fiducial& _fiducial)
            {
                if(_fiducial.shape_type == data::fiducials_series::shape::point)
                {
                    point_fiducials.push_back(&_fiducial);
                }
            });
    return point_fiducials;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::vector<std::string> get_group_names(const landmarks_or_image_series_const_lock& _lock)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->get_group_names();
    }

    if(_lock.image_series != nullptr)
    {
        return _lock.image_series->get_fiducials()->get_point_fiducials_group_names();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::size_t> get_number_of_points_in_group(
    const std::string& _group_name,
    const landmarks_or_image_series_const_lock& _lock
)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->get_group(_group_name).m_points.size();
    }

    if(_lock.image_series != nullptr)
    {
        return _lock.image_series->get_fiducials()->get_number_of_points_in_group(_group_name);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

void set_point(
    data::fiducials_series::fiducial_set& _fiducial_set,
    data::fiducials_series::fiducial& _fiducial,
    const data::landmarks::point_t& _point,
    data::image_series::sptr _image_series
)
{
    SIGHT_ASSERT("Only point fiducials are supported", _fiducial.shape_type == data::fiducials_series::shape::point);
    if(!_fiducial_set.frame_of_reference_uid.has_value())
    {
        std::string frame_of_reference_uid = _image_series->get_string_value(
            data::dicom::attribute::Keyword::FrameOfReferenceUID
        );
        if(frame_of_reference_uid.empty())
        {
            // Generate a frame of reference UID if the image doesn't have one. It is supposed to be mandatory according
            // to the DICOM standard anyway.
            frame_of_reference_uid = core::tools::uuid::generate();
            _image_series->set_string_value(
                data::dicom::attribute::Keyword::FrameOfReferenceUID,
                frame_of_reference_uid
            );
        }

        _fiducial_set.frame_of_reference_uid = frame_of_reference_uid;
    }

    _fiducial.contour_data = {data::fiducials_series::point3 {.x = _point[0], .y = _point[1], .z = _point[2]}};
    // If both ContourData and GraphicCoordinatesDataSequence are set, they must be synchronized, but I'm too lazy
    // to do that, so I simply get rid of GraphicCoordinatesDataSequence.
    _fiducial.graphic_coordinates_data_sequence = std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > get_point(
    data::fiducials_series::fiducial _fiducial,
    data::fiducials_series::fiducial_set _fiducial_set
)
{
    if(_fiducial_set.frame_of_reference_uid.has_value() && !_fiducial.contour_data.empty())
    {
        data::fiducials_series::point3 point = _fiducial.contour_data[0];
        return {{point.x, point.y, point.z}};
    }

    // Position with Graphic Coordinates Data Sequence isn't supported
    return std::nullopt;
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > get_point(
    const std::string& _group_name,
    std::size_t _index,
    data::fiducials_series::csptr _fiducials_series
)
{
    std::optional<data::fiducials_series::fiducial_set> fiducial_set = get_fiducial_set(_group_name, _fiducials_series);
    if(!fiducial_set.has_value())
    {
        return std::nullopt;
    }

    std::vector<data::fiducials_series::fiducial> point_fiducials =
        data::fiducials_series::get_point_fiducials(*fiducial_set);
    if(point_fiducials.size() <= _index)
    {
        return std::nullopt;
    }

    return get_point(point_fiducials[_index], *fiducial_set);
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<std::array<double, 3> > get_point(
    const std::string& _group_name,
    std::size_t _index,
    const landmarks_or_image_series_const_lock& _lock
)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->get_point(_group_name, _index);
    }

    if(_lock.image_series != nullptr)
    {
        return get_point(_group_name, _index, _lock.image_series->get_fiducials());
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] std::optional<data::landmarks::landmarks_group> get_group(
    const std::string& _group_name,
    const landmarks_or_image_series_const_ptr& _lock
)
{
    if(_lock.landmarks != nullptr)
    {
        if(!_lock.landmarks->has_group(_group_name))
        {
            return std::nullopt;
        }

        return _lock.landmarks->get_group(_group_name);
    }

    if(_lock.image_series != nullptr)
    {
        return _lock.image_series->get_fiducials()->get_group(_group_name);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

[[nodiscard]] bool has_group(const std::string& _group_name, const landmarks_or_image_series_const_lock& _lock)
{
    if(_lock.landmarks != nullptr)
    {
        return _lock.landmarks->has_group(_group_name);
    }

    if(_lock.image_series != nullptr)
    {
        return get_fiducial_set(_group_name, _lock.image_series->get_fiducials()).has_value();
    }

    SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
    return {};
}

//------------------------------------------------------------------------------

std::array<float, 3> get_current_slice_pos(const data::image& _image)
{
    namespace Helper = sight::data::helper::medical_image;

    const auto axial_index = std::max(
        std::int64_t(0),
        Helper::get_slice_index(_image, Helper::orientation_t::axial).value_or(0)
    );

    const auto frontal_index = std::max(
        std::int64_t(0),
        Helper::get_slice_index(_image, Helper::orientation_t::frontal).value_or(0)
    );

    const auto sagittal_index = std::max(
        std::int64_t(0),
        Helper::get_slice_index(_image, Helper::orientation_t::sagittal).value_or(0)
    );

    const auto& img_spacing = _image.spacing();
    const auto& img_origin  = _image.origin();

    return {
        static_cast<float>(sagittal_index) * static_cast<float>(img_spacing[0])
        + static_cast<float>(img_origin[0]),
        static_cast<float>(frontal_index) * static_cast<float>(img_spacing[1])
        + static_cast<float>(img_origin[1]),
        static_cast<float>(axial_index) * static_cast<float>(img_spacing[2])
        + static_cast<float>(img_origin[2])
    };
}

} // namespace

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
    new_slot(slots::REMOVE_ALL, &landmarks::remove_all, this);
    new_slot(slots::REMOVE_GROUP, &landmarks::remove_group, this);
    new_slot(slots::MODIFY_GROUP, &landmarks::modify_group, this);
    new_slot(slots::MODIFY_POINT, &landmarks::modify_point, this);
    new_slot(slots::ADD_POINT, &landmarks::add_point, this);
    new_slot(slots::REMOVE_POINT, &landmarks::remove_point, this);
    new_slot(slots::INSERT_POINT, &landmarks::insert_point, this);
    new_slot(slots::SELECT_POINT, &landmarks::select_point, this);
    new_slot(slots::DESELECT_POINT, &landmarks::deselect_point, this);
    new_slot(slots::SLICE_TYPE, &landmarks::change_slice_type, this);
    new_slot(slots::SLICE_INDEX, &landmarks::change_slice_index, this);
    new_slot(slots::RENAME_GROUP, &landmarks::rename_group, this);
    new_slot(slots::SET_CURRENT_GROUP, &landmarks::set_current_group, this);
    new_slot(slots::REMOVE_LANDMARKS, &landmarks::remove_landmarks, this);
    new_slot(slots::CREATE_LANDMARK, &landmarks::create_landmark, this);
    new_slot(slots::CONFIGURE_LANDMARKS, &landmarks::configure_landmarks, this);
    new_slot(slots::ENABLE_EDIT_MODE, &landmarks::enable_edit_mode, this);
    new_slot(slots::DISABLE_EDIT_MODE, &landmarks::disable_edit_mode, this);
    new_slot(
        slots::TOGGLE_EDIT_MODE,
        [this]
        {
            (m_edit_mode& edit_mode::edit) == edit_mode::edit ? disable_edit_mode() : enable_edit_mode();
        });
    new_slot(slots::CHANGE_EDIT_MODE, [this](bool _edit_mode){_edit_mode ? enable_edit_mode() : disable_edit_mode();});
    new_slot(slots::ENABLE_MOVE_MODE, &landmarks::enable_move_mode, this);
    new_slot(slots::DISABLE_MOVE_MODE, &landmarks::disable_move_mode, this);
    new_slot(
        slots::TOGGLE_MOVE_MODE,
        [this]
        {
            (m_edit_mode& edit_mode::move) == edit_mode::move ? disable_move_mode() : enable_move_mode();
        });
    new_slot(slots::CHANGE_MOVE_MODE, [this](bool _edit_mode){_edit_mode ? enable_move_mode() : disable_move_mode();});
}

//-----------------------------------------------------------------------------

void landmarks::configuring()
{
    configure_params();

    const auto& config = get_config();

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
    static const std::string s_ALLOW_RENAME           = CONFIG + "allowRename";
    static const std::string s_MODIFY                 = CONFIG + "modify";

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
    color->set_rgba(config.get<std::string>(s_INITIAL_COLOR, "#FFFF00FF"));
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

    m_renaming_allowed = config.get<bool>(s_ALLOW_RENAME, m_enable_labels);
    SIGHT_ASSERT(
        "Renaming labels is allowed yet the labels are disabled, this is forbidden.",
        m_enable_labels || !m_renaming_allowed
    );

    std::string modify = config.get(s_MODIFY, "all");
    SIGHT_ERROR_IF(
        "'modify' config has wrong value '" << modify << "', the only authorized values are 'all' and 'group'",
        modify != "all" && modify != "group"
    );
    m_can_only_modify_current = (modify == "group");

    SIGHT_ASSERT(
        "Only one of 'landmarks' or 'imageSeries' must be present as inout",
        (m_landmarks.const_lock() != nullptr) + (m_image_series.const_lock() != nullptr) == 1
    );
    SIGHT_ASSERT(
        "Either 'image' or 'imageSeries' must be present as parameter",
        (m_image.const_lock() != nullptr) + (m_image_series.const_lock() != nullptr) <= 1
    );
}

//-----------------------------------------------------------------------------

void landmarks::starting()
{
    initialize();

    render_service()->make_current();

    auto* root_scene_node = get_scene_manager()->getRootSceneNode();
    m_trans_node = get_or_create_transform_node(root_scene_node);

    m_material = std::make_shared<data::material>();
    m_material->set_diffuse(std::make_shared<data::color>(1.F, 1.F, 1.F, 1.F));

    // Register the material adaptor.
    m_material_adaptor = this->register_service<sight::viz::scene3d::material_adaptor>(
        "sight::module::viz::scene3d::adaptor::material"
    );
    m_material_adaptor->set_inout(m_material, sight::viz::scene3d::material_adaptor::MATERIAL_INOUT, true);
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

    // Draw landmarks.
    updating();

    auto interactor     = layer()->render_service()->get_interactor_manager();
    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parent_widget = qt_interactor->get_qt_widget();
    m_contextual_menu = new QWidget(parent_widget);
    m_contextual_menu->setStyleSheet(".QWidget {background-color: none;}");
    m_contextual_menu->hide();
    auto* layout = new QHBoxLayout;
    QIcon trash_bin_icon(QString::fromStdString(
                             core::runtime::get_module_resource_file_path(
                                 "sight::module::ui::flaticons/RedTrashBin.svg"
                             ).string()
    ));
    auto* bin_button             = new QPushButton(trash_bin_icon, "");
    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);
    bin_button->setObjectName(QString::fromStdString(service_id) + "/binButton");
    bin_button->setCursor(Qt::ArrowCursor);
    bin_button->adjustSize();
    if(bin_button->width() < 40)
    {
        bin_button->setMinimumWidth(40);
    }

    if(bin_button->height() < 40)
    {
        bin_button->setMinimumHeight(40);
    }

    bin_button->adjustSize();
    bin_button->setIconSize(bin_button->size());
    layout->addWidget(bin_button);
    if(m_renaming_allowed)
    {
        QIcon pen_icon(QString::fromStdString(
                           core::runtime::get_module_resource_file_path(
                               "sight::module::ui::flaticons/YellowPen.svg"
                           ).string()
        ));
        auto* pen_button = new QPushButton(pen_icon, "");
        pen_button->setObjectName(QString::fromStdString(service_id) + "/penButton");
        pen_button->setCursor(Qt::ArrowCursor);
        pen_button->adjustSize();
        if(pen_button->width() < 40)
        {
            pen_button->setMinimumWidth(40);
        }

        if(pen_button->height() < 40)
        {
            pen_button->setMinimumHeight(40);
        }

        pen_button->adjustSize();
        pen_button->setIconSize(pen_button->size());
        layout->addWidget(pen_button);
    }

    m_contextual_menu->setLayout(layout);
    m_contextual_menu->adjustSize();
    m_contextual_menu->raise();

    if(auto image_series = m_image_series.lock())
    {
        image_series->get_fiducials()->set_group_names_for_point_fiducials();
    }
}

//-----------------------------------------------------------------------------

service::connections_t landmarks::auto_connections() const
{
    service::connections_t connections;

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

    connections.push(IMAGE_SERIES_INOUT, data::image_series::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_REMOVED, slots::REMOVE_GROUP);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_MODIFIED, slots::MODIFY_GROUP);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_RENAMED, slots::RENAME_GROUP);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_MODIFIED, slots::MODIFY_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_ADDED, slots::ADD_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_REMOVED, slots::REMOVE_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_INSERTED, slots::INSERT_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_SELECTED, slots::SELECT_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_DESELECTED, slots::DESELECT_POINT);

    connections.push(IMAGE_INPUT, data::image::SLICE_TYPE_MODIFIED_SIG, slots::SLICE_TYPE);
    connections.push(IMAGE_INPUT, data::image::SLICE_INDEX_MODIFIED_SIG, slots::SLICE_INDEX);

    connections.push(IMAGE_SERIES_INOUT, data::image::SLICE_TYPE_MODIFIED_SIG, slots::SLICE_TYPE);
    connections.push(IMAGE_SERIES_INOUT, data::image::SLICE_INDEX_MODIFIED_SIG, slots::SLICE_INDEX);

    return connections;
}

//-----------------------------------------------------------------------------

void landmarks::updating()
{
    // Delete all groups.
    remove_all_manual_objects();

    landmarks_or_image_series_const_lock lock = const_lock_landmarks();

    // Create all point.
    for(const auto& group_name : get_group_names(lock))
    {
        std::optional<std::size_t> nb_of_points_in_group = get_number_of_points_in_group(group_name, lock);
        for(std::size_t index = 0 ; index < nb_of_points_in_group ; ++index)
        {
            this->create_manual_object(group_name, index, lock);
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

    remove_all_manual_objects();

    // Unregister the material adaptor.
    unregister_services();

    m_event_filter = nullptr;
}

//------------------------------------------------------------------------------

void landmarks::remove_all_manual_objects()
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

void landmarks::remove_all()
{
    m_contextual_menu->hide();

    landmarks_or_image_series_lock lf_lock = lock_landmarks();
    if(lf_lock.landmarks != nullptr)
    {
        for(const std::string& grp : lf_lock.landmarks->get_group_names())
        {
            lf_lock.landmarks->remove_group(grp);
            lf_lock.landmarks->signal<data::landmarks::group_removed_signal_t>(data::landmarks::GROUP_REMOVED_SIG)->
            async_emit(grp);
        }
    }
    else if(lf_lock.image_series != nullptr)
    {
        std::vector<std::string> deleted_group_names;
        std::vector<data::fiducials_series::fiducial_set> fiducial_sets =
            lf_lock.image_series->get_fiducials()->get_fiducial_sets();
        for(auto it_fiducial_set = fiducial_sets.begin() ; it_fiducial_set != fiducial_sets.end() ; )
        {
            if(!it_fiducial_set->group_name.has_value())
            {
                ++it_fiducial_set;
                continue;
            }

            bool has_deleted = false;
            for(auto it_fiducial = it_fiducial_set->fiducial_sequence.begin() ;
                it_fiducial != it_fiducial_set->fiducial_sequence.end() ; )
            {
                if(it_fiducial->shape_type == data::fiducials_series::shape::point)
                {
                    it_fiducial = it_fiducial_set->fiducial_sequence.erase(it_fiducial);
                    has_deleted = true;
                }
                else
                {
                    ++it_fiducial;
                }
            }

            if(has_deleted)
            {
                deleted_group_names.push_back(*it_fiducial_set->group_name);
            }

            if(it_fiducial_set->fiducial_sequence.empty())
            {
                it_fiducial_set = fiducial_sets.erase(it_fiducial_set);
            }
            else
            {
                ++it_fiducial_set;
            }
        }

        lf_lock.image_series->get_fiducials()->set_fiducial_sets(fiducial_sets);
        for(const std::string& group : deleted_group_names)
        {
            lf_lock.image_series->signal<data::has_fiducials::signals::group_removed>(
                data::has_fiducials::signals::GROUP_REMOVED
            )->async_emit(group);
        }
    }
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
                (*object_it)->m_label->set_visible(false);
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
    landmarks_or_image_series_const_lock lock = const_lock_landmarks();

    std::optional<std::size_t> group_size = get_number_of_points_in_group(_group_name, lock);
    if(!group_size.has_value())
    {
        return;
    }

    // Re-create the group.
    for(std::size_t index = 0 ; index < group_size ; ++index)
    {
        create_manual_object(_group_name, index, lock);
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
    for(const std::shared_ptr<landmark>& landmark : m_manual_objects)
    {
        if(landmark->m_group_name == _old_group_name)
        {
            landmark->m_group_name = _new_group_name;
            if(m_enable_labels)
            {
                landmark->m_label->set_text(_new_group_name + '_' + std::to_string(landmark->m_index));
            }
        }
    }

    if(m_current_group == _old_group_name)
    {
        m_current_group = _new_group_name;
    }
}

//------------------------------------------------------------------------------

void landmarks::set_current_group(std::string _new_current_group_name)
{
    m_current_group = _new_current_group_name;
}

//------------------------------------------------------------------------------

void landmarks::modify_point(std::string _group_name, std::size_t _index)
{
    std::optional<std::array<double, 3> > maybe_point = get_point(_group_name, _index, const_lock_landmarks());

    if(!maybe_point.has_value())
    {
        return;
    }

    std::array<double, 3> point = *maybe_point;

    for(auto& m_manual_object : m_manual_objects)
    {
        const auto& name = m_manual_object->m_group_name;
        if(name.find(_group_name) != std::string::npos && m_manual_object->m_index == _index)
        {
            const Ogre::Vector3 position(static_cast<float>(point[0]),
                                         static_cast<float>(point[1]),
                                         static_cast<float>(point[2]));
            m_manual_object->m_node->setPosition(position);
            update_landmark_visibility(
                *m_manual_object,
                get_group(m_manual_object->m_group_name, const_lock_landmarks())
            );
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
    landmarks_or_image_series_const_lock lock = const_lock_landmarks();

    // Get the last index.
    std::optional<std::size_t> number_of_points = get_number_of_points_in_group(_group_name, lock);

    if(!number_of_points.has_value() || number_of_points == 0)
    {
        return;
    }

    // Add the new point.
    this->create_manual_object(_group_name, *number_of_points - 1, lock);
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
                (*object_it)->m_label->set_visible(false);
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

    create_manual_object(_group_name, _index, const_lock_landmarks());
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::landmark> landmarks::create_manual_object(
    const std::string& _group_name,
    std::size_t _index,
    data::landmarks::landmarks_group _group_data,
    data::landmarks::point_t _point_pos
)
{
    // Create the point name.
    const std::string point_name = _group_name + "_" + std::to_string(_index);

    // Create the manual object.
    const Ogre::ColourValue color =
        Ogre::ColourValue(
            _group_data.m_color[0],
            _group_data.m_color[1],
            _group_data.m_color[2],
            _group_data.m_color[3]
        );

    Ogre::SceneManager* scene_mgr = this->get_scene_manager();
    Ogre::ManualObject* object    = scene_mgr->createManualObject(this->get_id() + "_" + point_name + "_object");
    switch(_group_data.m_shape)
    {
        case data::landmarks::shape::sphere:
            sight::viz::scene3d::helper::manual_object::create_sphere(
                object,
                m_material_adaptor->get_material_name(),
                color,
                _group_data.m_size * 0.5F
            );
            break;

        case data::landmarks::shape::cube:
            sight::viz::scene3d::helper::manual_object::create_cube(
                object,
                m_material_adaptor->get_material_name(),
                color,
                _group_data.m_size
            );
            break;
    }

    object->setQueryFlags(m_landmarks_query_flag);

    Ogre::SceneNode* node = m_trans_node->createChildSceneNode(this->get_id() + "_" + point_name + "_node");

    // Set the point to the right position.
    node->setPosition(Ogre::Real(_point_pos[0]), Ogre::Real(_point_pos[1]), Ogre::Real(_point_pos[2]));

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
        text->set_visible(_group_data.m_visibility && m_visible);
        text->set_text_alignment("center", "");

        // Attach data.
        text->attach_to_node(node, this->layer()->get_default_camera());
    }

    // Store the created data.
    auto new_landmark = std::make_shared<landmark>(node, object, _group_name, _index, text);
    m_manual_objects.push_back(new_landmark);

    if(m_enable_labels)
    {
        // The landmark should be in a weak ptr, else there is a circular reference (the slot refers to the landmark via
        // newLandmark, and the landmark refers to the slot via m_slots).
        auto slot_text_edited = core::com::new_slot(
            [this, weak_landmark = std::weak_ptr(new_landmark)](std::string _edited_text)
            {
                auto landmark               = weak_landmark.lock();
                std::string new_group_name  = _edited_text;
                std::string index_as_string = std::to_string(landmark->m_index);
                // Remove the index suffix if present
                if(new_group_name.ends_with('_' + index_as_string))
                {
                    new_group_name = new_group_name.substr(0, new_group_name.size() - (index_as_string.size() + 1));
                }

                landmarks_or_image_series_lock lock = lock_landmarks();
                std::string old_group_name          = landmark->m_group_name;
                if(lock.landmarks != nullptr)
                {
                    lock.landmarks->rename_group(old_group_name, new_group_name);
                    rename_group(old_group_name, new_group_name);
                    auto signal =
                        lock.landmarks->signal<data::landmarks::group_renamed_signal_t>(
                            data::landmarks::GROUP_RENAMED_SIG
                        );
                    signal->async_emit(
                        old_group_name,
                        new_group_name
                    );
                }
                else if(lock.image_series != nullptr)
                {
                    std::optional<std::pair<data::fiducials_series::fiducial_set, std::size_t> > fiducial_set =
                        lock.image_series->get_fiducials()->get_fiducial_set_and_index(old_group_name);
                    if(!fiducial_set.has_value())
                    {
                        SIGHT_WARN("Couldn't rename group '" << old_group_name << "', it doesn't exist");
                        return;
                    }

                    lock.image_series->get_fiducials()->set_group_name(fiducial_set->second, new_group_name);
                    rename_group(old_group_name, new_group_name);
                    lock.image_series->signal<data::has_fiducials::signals::group_renamed>(
                        data::has_fiducials::signals::GROUP_RENAMED
                    )->async_emit(old_group_name, new_group_name);
                }
            });
        slot_text_edited->set_worker(core::thread::get_default_worker());
        new_landmark->m_label->signal(sight::viz::scene3d::text::TEXT_EDITED_SIGNAL)->connect(slot_text_edited);
        new_landmark->m_slots.push_back(slot_text_edited);

        auto slot_editing_finish = core::com::new_slot(
            [label = std::weak_ptr(new_landmark->m_label)]
            {
                label.lock()->set_edit_mode(false);
            });
        slot_editing_finish->set_worker(core::thread::get_default_worker());
        new_landmark->m_label->signal(sight::viz::scene3d::text::EDITING_FINISHED_SIGNAL)->connect(slot_editing_finish);
        new_landmark->m_slots.push_back(slot_editing_finish);
    }

    update_landmark_visibility(*m_manual_objects.back(), _group_data);

    // Request the rendering.
    this->request_render();

    return new_landmark;
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::landmark> landmarks::create_manual_object(
    const std::string& _group_name,
    std::size_t _index,
    data::landmarks::csptr _landmarks
)
{
    const data::landmarks::landmarks_group& group = _landmarks->get_group(_group_name);
    return create_manual_object(_group_name, _index, group, group.m_points[_index]);
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::landmark> landmarks::create_manual_object(
    const std::string& _group_name,
    std::size_t _index,
    landmarks_or_image_series_const_ptr _lf
)
{
    if(_lf.landmarks != nullptr)
    {
        return create_manual_object(_group_name, _index, _lf.landmarks);
    }

    if(_lf.image_series != nullptr)
    {
        std::optional<data::landmarks::landmarks_group> group =
            _lf.image_series->get_fiducials()->get_group(_group_name);
        if(!group.has_value())
        {
            SIGHT_ASSERT("The group '" << _group_name << "' doesn't exist", false);
            return nullptr;
        }

        if(group->m_points.size() <= _index)
        {
            SIGHT_ASSERT("The point of '" << _group_name << '_' << _index << "' doesn't exist", false);
            return nullptr;
        }

        return create_manual_object(_group_name, _index, *group, group->m_points[_index]);
    }

    SIGHT_ASSERT("Either 'landmarks' or 'fiducialsSeries' must be configured as inout", false);
    return nullptr;
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
                    return _landmark->m_landmark->m_group_name == _group_name && _landmark->m_landmark->m_index == _index;
                });

            if(it == m_selected_landmarks.end())
            {
                // This method must be synchronized with deselectPoint(std::string, std::size_t).
                std::lock_guard guard(m_selected_mutex);

                // Create thread data.
                std::shared_ptr<selected_landmark> selected_landmark =
                    std::make_shared<landmarks::selected_landmark>(this->worker()->create_timer(), m_manual_object);
                m_selected_landmarks.push_back(selected_landmark);

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
            update_landmark_visibility((*landmark_it)->m_landmark);

            // Request the rendering.
            request_render();

            m_selected_landmarks.erase(landmark_it);
            break;
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::hightlight(std::shared_ptr<selected_landmark> _selected_landmark)
{
    // Make the context as current.
    this->render_service()->make_current();

    // Hightlight the selected landmark.
    this->update_landmark_visibility(_selected_landmark->m_landmark);
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

    this->update_landmarks_visibility();

    // Request the rendering.
    this->request_render();
}

//------------------------------------------------------------------------------

void landmarks::change_slice_index(int /*_axialIndex*/, int /*_frontalIndex*/, int /*_sagittalIndex*/)
{
    // Make the context as current.
    this->render_service()->make_current();

    bool image_exists = false;
    {
        image_or_image_series_const_lock lock = const_lock_image();

        if(lock.image || lock.image_series)
        {
            image_exists = true;

            this->render_service()->make_current();
        }
    }

    if(image_exists)
    {
        this->update_landmarks_visibility();

        // Request the rendering.
        this->request_render();
    }
}

//------------------------------------------------------------------------------

void landmarks::remove_landmarks()
{
    std::optional<std::int64_t> slice_index;
    {
        image_or_image_series_const_lock lock = const_lock_image();
        data::image::csptr image              = lock.image ? lock.image.get_shared() : lock.image_series.get_shared();
        SIGHT_THROW_IF("Data is null.", !image);

        slice_index = sight::data::helper::medical_image::get_slice_index(
            *image,
            m_orientation
        );
    }

    if(!slice_index)
    {
        // No slice selected, early return
        return;
    }

    landmarks_or_image_series_lock fl_lock = lock_landmarks();

    if(fl_lock.landmarks == nullptr && fl_lock.image_series == nullptr)
    {
        // No need to continue if there is no landmarks
        return;
    }

    bool has_deleted = false;

    if(fl_lock.landmarks != nullptr)
    {
        for(const auto& name : fl_lock.landmarks->get_group_names())
        {
            auto& group = fl_lock.landmarks->get_group(name);

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
    }
    else if(fl_lock.image_series != nullptr)
    {
        data::fiducials_series::sptr fiducials_series = fl_lock.image_series.get_shared()->get_fiducials();
        for(const std::string& name : fiducials_series->get_point_fiducials_group_names())
        {
            std::optional<std::pair<data::fiducials_series::fiducial_set, std::size_t> > fiducial_set =
                fiducials_series->get_fiducial_set_and_index(name);
            if(!fiducial_set.has_value())
            {
                continue;
            }

            std::optional<data::landmarks::size_t> size = fiducial_set->first.size;
            if(!size.has_value())
            {
                size                     = m_current_size;
                fiducial_set->first.size = size;
            }

            for(auto it = fiducial_set->first.fiducial_sequence.begin() ;
                it != fiducial_set->first.fiducial_sequence.end() ; )
            {
                std::optional<std::array<double, 3> > point = get_point(*it, fiducial_set->first);
                if(!point.has_value())
                {
                    continue;
                }

                if(is_landmark_visible(*point, *size))
                {
                    it          = fiducial_set->first.fiducial_sequence.erase(it);
                    has_deleted = true;
                }
            }

            fiducials_series->set_fiducial_set(fiducial_set->second, fiducial_set->first);
        }
    }

    if(has_deleted)
    {
        data::object::sptr object;
        if(fl_lock.landmarks != nullptr)
        {
            object = fl_lock.landmarks.get_shared();
        }
        else if(fl_lock.image_series != nullptr)
        {
            object = fl_lock.image_series->get_fiducials();
        }

        object->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG)->async_emit();
    }
}

//------------------------------------------------------------------------------

void landmarks::configure_landmarks(sight::viz::scene3d::landmarks_configuration _configuration)
{
    if(_configuration.group)
    {
        m_current_group = *_configuration.group;
    }

    if(_configuration.color)
    {
        m_current_color = *_configuration.color;
    }

    if(_configuration.size)
    {
        m_current_size = *_configuration.size;
    }

    if(_configuration.shape)
    {
        m_current_shape = *_configuration.shape;
    }

    if(_configuration.group_max)
    {
        if(*_configuration.group_max >= 0)
        {
            m_group_max[m_current_group] = std::size_t(*_configuration.group_max);
        }
        else
        {
            m_group_max.erase(m_current_group);
        }
    }

    if(_configuration.visible_max)
    {
        if(*_configuration.visible_max >= 0)
        {
            m_visible_max = *_configuration.visible_max;
        }
        else
        {
            m_visible_max = std::nullopt;
        }
    }

    if(_configuration.total_max)
    {
        if(*_configuration.total_max >= 0)
        {
            m_total_max = *_configuration.total_max;
        }
        else
        {
            m_total_max = std::nullopt;
        }
    }

    if(_configuration.movable_groups)
    {
        m_movable_groups = *_configuration.movable_groups;
    }
}

//------------------------------------------------------------------------------

void landmarks::enable_edit_mode()
{
    if((m_edit_mode& edit_mode::edit) == edit_mode::edit)
    {
        return;
    }

    m_edit_mode |= edit_mode::edit;

    set_cursor(Qt::CrossCursor);

    if(m_event_filter == nullptr)
    {
        auto interactor     = layer()->render_service()->get_interactor_manager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->get_qt_widget();
        m_event_filter = std::make_unique<delete_contextual_menu_when_focus_out>(this);
        parent_widget->installEventFilter(m_event_filter.get());
    }

    m_edit_mode_changed->async_emit(true);
}

//------------------------------------------------------------------------------

void landmarks::disable_edit_mode()
{
    if((m_edit_mode& edit_mode::edit) != edit_mode::edit)
    {
        return;
    }

    m_edit_mode &= ~edit_mode::edit;

    if(m_edit_mode == edit_mode::display)
    {
        auto interactor     = layer()->render_service()->get_interactor_manager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->get_qt_widget();
        parent_widget->unsetCursor();
    }

    m_contextual_menu->hide();

    if(m_enable_labels && m_renaming_allowed)
    {
        std::ranges::for_each(m_manual_objects, [](std::shared_ptr<landmark> _l){_l->m_label->set_edit_mode(false);});
    }

    m_edit_mode_changed->async_emit(false);
}

//------------------------------------------------------------------------------

void landmarks::enable_move_mode()
{
    if((m_edit_mode& edit_mode::move) == edit_mode::move)
    {
        return;
    }

    m_edit_mode |= edit_mode::move;

    set_cursor(Qt::CrossCursor);
}

//------------------------------------------------------------------------------

void landmarks::disable_move_mode()
{
    if((m_edit_mode& edit_mode::move) != edit_mode::move)
    {
        return;
    }

    m_edit_mode &= ~edit_mode::move;

    if(m_edit_mode == edit_mode::display)
    {
        auto interactor     = layer()->render_service()->get_interactor_manager();
        auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
        auto* parent_widget = qt_interactor->get_qt_widget();
        parent_widget->unsetCursor();
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

    data::landmarks::landmarks_group group {{}, {}, {}, {}};
    std::size_t index {};

    {
        landmarks_or_image_series_lock lf_lock = lock_landmarks();

        if(lf_lock.landmarks != nullptr)
        {
            // If the group does not exist, we create it.
            if(!lf_lock.landmarks->has_group(m_current_group))
            {
                lf_lock.landmarks->add_group(m_current_group, m_current_color, m_current_size, m_current_shape);
                const auto& sig = lf_lock.landmarks->signal<sight::data::landmarks::group_added_signal_t>(
                    sight::data::landmarks::GROUP_ADDED_SIG
                );

                sig->async_emit(m_current_group);
            }

            lf_lock.landmarks->add_point(m_current_group, _point);

            // Get the last index.
            group = lf_lock.landmarks->get_group(m_current_group);
            index = group.m_points.size() - 1;

            // Block the signal to avoid being called back.
            const auto& sig = lf_lock.landmarks->signal<sight::data::landmarks::point_added_signal_t>(
                sight::data::landmarks::POINT_ADDED_SIG
            );

            sight::core::com::connection::blocker blocker(sig->get_connection(slot(slots::ADD_POINT)));
            sig->async_emit(m_current_group);
        }
        else if(lf_lock.image_series != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::fiducial_set, std::size_t> > res =
                lf_lock.image_series->get_fiducials()->get_fiducial_set_and_index(m_current_group);
            data::fiducials_series::fiducial_set fiducial_set;
            std::size_t index_of_group {};
            // If the group does not exist, we create it.
            if(!res.has_value())
            {
                fiducial_set.group_name = m_current_group;
                fiducial_set.color      = m_current_color;
                fiducial_set.size       = m_current_size;
                switch(m_current_shape)
                {
                    case data::landmarks::shape::sphere:
                        fiducial_set.shape = data::fiducials_series::private_shape::sphere;
                        break;

                    case data::landmarks::shape::cube:
                        fiducial_set.shape = data::fiducials_series::private_shape::cube;
                }

                index_of_group = lf_lock.image_series->get_fiducials()->get_fiducial_sets().size();
                lf_lock.image_series->signal<data::has_fiducials::signals::group_added>(
                    data::has_fiducials::signals::GROUP_ADDED
                )->async_emit(m_current_group);
            }
            else
            {
                std::tie(fiducial_set, index_of_group) = *res;
            }

            data::fiducials_series::fiducial fiducial;
            fiducial.shape_type = data::fiducials_series::shape::point;
            index               = data::fiducials_series::get_point_fiducials(fiducial_set).size();
            std::string fiducial_name = m_current_group + '_' + std::to_string(index);
            fiducial.fiducial_description = fiducial_name; // TODO: Add a more appropriate description?
            fiducial.fiducial_identifier  = fiducial_name;
            fiducial.fiducial_uid         = core::tools::uuid::generate();
            set_point(fiducial_set, fiducial, _point, lf_lock.image_series.get_shared());
            fiducial_set.fiducial_sequence.push_back(fiducial);
            lf_lock.image_series->get_fiducials()->set_fiducial_set(index_of_group, fiducial_set);

            group = *get_group(m_current_group, lf_lock);

            // Block the signal to avoid being called back.
            const auto& sig =
                lf_lock.image_series->signal<data::has_fiducials::signals::point_added>(
                    data::has_fiducials::signals::POINT_ADDED
                );

            sight::core::com::connection::blocker blocker(sig->get_connection(slot(slots::ADD_POINT)));
            sig->async_emit(m_current_group);
        }
        else
        {
            SIGHT_ASSERT("Either 'landmarks' or 'imageSeries' must be configured as inout", false);
            return;
        }
    }

    // Add the new point.
    std::shared_ptr<landmark> new_landmark = create_manual_object(m_current_group, index, group, _point);

    if(_pick)
    {
        m_picked_data = new_landmark;
        m_picked_data->m_node->setScale(SELECTED_SCALE, SELECTED_SCALE, SELECTED_SCALE);
    }
    else
    {
        m_picked_data = nullptr;
    }
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
    landmarks_or_image_series_const_lock lock = const_lock_landmarks();

    if(m_group_max.contains(m_current_group)
       && (m_group_max[m_current_group] == 0
           || (has_group(m_current_group, lock)
               && get_number_of_points_in_group(m_current_group, lock) >= m_group_max[m_current_group])))
    {
        return true;
    }

    if(m_total_max || m_visible_max)
    {
        const auto& names = get_group_names(lock);

        std::size_t max         = 0;
        std::size_t max_visible = 0;

        for(const auto& name : names)
        {
            std::optional<data::landmarks::landmarks_group> maybe_group = get_group(name, lock);
            if(!maybe_group.has_value())
            {
                continue;
            }

            data::landmarks::landmarks_group group = *maybe_group;

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

void landmarks::update_landmarks_visibility()
{
    // Make the context as current.
    render_service()->make_current();

    bool image_exists = false;
    {
        image_or_image_series_const_lock iis_lock = const_lock_image();
        image_exists = iis_lock.image || iis_lock.image_series;
    }

    // Hide landmarks only if there is an image.
    if(image_exists)
    {
        for(const auto& landmark : m_manual_objects)
        {
            update_landmark_visibility(landmark);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::update_landmark_visibility(std::shared_ptr<landmark> _landmark)
{
    // Make the context as current.
    render_service()->make_current();

    update_landmark_visibility(*_landmark, const_lock_landmarks());
}

//------------------------------------------------------------------------------

void landmarks::update_landmark_visibility(landmark& _landmark, std::optional<data::landmarks::landmarks_group> _group)
{
    const bool must_show =
        [&]
        {
            // Retrieve group
            if(!_group.has_value())
            {
                return false;
            }

            if(!_group->m_visibility)
            {
                return false;
            }

            const auto& position = _landmark.m_node->getPosition();
            return is_landmark_visible({position[0], position[1], position[2]}, _group->m_size);
        }();

    // Show or hide the landmark.
    _landmark.m_object->setVisible(must_show);
}

//------------------------------------------------------------------------------

void landmarks::update_landmark_visibility(landmark& _landmark, const landmarks_or_image_series_const_lock& _lock)
{
    update_landmark_visibility(_landmark, get_group(_landmark.m_group_name, _lock));
}

//------------------------------------------------------------------------------

bool landmarks::is_landmark_visible(
    const data::landmarks::point_t& _point,
    data::landmarks::size_t _group_size
) const
{
    if(const auto& iis_lock = const_lock_image(); iis_lock.image || iis_lock.image_series)
    {
        const auto position       = _point[m_orientation];
        data::image::csptr image  = iis_lock.image ? iis_lock.image.get_shared() : iis_lock.image_series.get_shared();
        const auto slice_position = get_current_slice_pos(*image)[m_orientation];
        const auto spacing        = image->spacing()[m_orientation];

        switch(m_view_distance)
        {
            case view_distance::slices_in_range:
            {
                // Check if the position is the same than slice position
                const auto group_half_size = _group_size * 0.5;
                const auto max_size        = std::max(group_half_size, spacing);

                return core::tools::is_greater(position, (slice_position - _group_size))
                       && core::tools::is_less(position, (slice_position + max_size));
            }

            case view_distance::current_slice:
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

void landmarks::set_visible(bool _visible)
{
    landmarks_or_image_series_const_lock lock = const_lock_landmarks();
    for(const auto& landmark : m_manual_objects)
    {
        std::optional<data::landmarks::landmarks_group> maybe_group = get_group(landmark->m_group_name, lock);
        if(!maybe_group.has_value())
        {
            continue;
        }

        data::landmarks::landmarks_group group = *maybe_group;
        landmark->m_object->setVisible(_visible && group.m_visibility);
        if(m_enable_labels)
        {
            landmark->m_label->set_visible(_visible && group.m_visibility);
        }
    }

    request_render();
}

//------------------------------------------------------------------------------

std::optional<Ogre::Vector3> landmarks::get_nearest_picked_position(int _x, int _y)
{
    Ogre::SceneManager* sm = this->get_scene_manager();
    const auto result      = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *sm);

    if(result.has_value())
    {
        const auto* const camera = sm->getCamera(sight::viz::scene3d::layer::DEFAULT_CAMERA_NAME);
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

void landmarks::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    m_contextual_menu->hide();

    if(_button != left)
    {
        // We only manage left click.
        return;
    }

    if(m_edit_mode == edit_mode::display)
    {
        // If the EDIT mode is off, the landmarks are read-only.
        return;
    }

    const bool must_edit = (m_edit_mode& edit_mode::edit) == edit_mode::edit;

    // Try to pick a landmark.
    if(const auto& picked_data = try_pick(_x, _y); picked_data)
    {
        // If something is picked, we will select it.
        set_cursor(Qt::ClosedHandCursor);
        m_picked_data = picked_data;
        m_picked_data->m_node->setScale(SELECTED_SCALE, SELECTED_SCALE, SELECTED_SCALE);

        // Only show contextual menu if we are in "true" EDIT mode
        m_must_show_contextual_menu = must_edit;
    }
    else if(must_edit)
    {
        // Nothing is picked, we will create a new landmark if we are in EDIT mode.
        m_must_show_contextual_menu = false;

        // If nothing is picked, we will create a new landmark.
        if(auto new_pos = this->get_nearest_picked_position(_x, _y); new_pos)
        {
            set_cursor(Qt::ClosedHandCursor);
            create_and_pick_landmark({(*new_pos)[0], (*new_pos)[1], (*new_pos)[2]});
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::mouse_move_event(mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked_data != nullptr)
    {
        m_must_show_contextual_menu = false;

        set_cursor(Qt::ClosedHandCursor);

        // Discard the current landmark to launch the ray over the scene without picking this one.
        m_picked_data->m_object->setQueryFlags(0x0);

        const auto layer              = this->layer();
        const Ogre::Camera* const cam = layer->get_default_camera();
        SIGHT_ASSERT("No camera found", cam);

        bool move_in_camera_plane = true;

        Ogre::Vector3 new_pos;
        if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
        {
            // If something is picked, we will snap the landmark to it
            std::optional<Ogre::Vector3> picked_pos = this->get_nearest_picked_position(_x, _y);
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
            const Ogre::Vector3 direction = this->get_cam_direction(cam);

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
        landmarks_or_image_series_lock lf_lock = lock_landmarks();
        if(lf_lock.landmarks != nullptr)
        {
            data::landmarks::point_t& point = lf_lock.landmarks->get_point(
                m_picked_data->m_group_name,
                m_picked_data->m_index
            );
            point[0] = new_pos[0];
            point[1] = new_pos[1];
            point[2] = new_pos[2];

            const auto& sig = lf_lock.landmarks->signal<data::landmarks::point_modified_sig_t>(
                data::landmarks::POINT_MODIFIED_SIG
            );
            sig->async_emit(m_picked_data->m_group_name, m_picked_data->m_index);
        }
        else if(lf_lock.image_series != nullptr)
        {
            std::optional<std::pair<data::fiducials_series::fiducial_set, std::size_t> > fiducial_set =
                lf_lock.image_series->get_fiducials()->get_fiducial_set_and_index(m_picked_data->m_group_name);
            if(!fiducial_set.has_value())
            {
                return;
            }

            std::vector<data::fiducials_series::fiducial*> point_fiducials =
                get_point_fiducials_ptr(fiducial_set->first);
            if(m_picked_data->m_index >= point_fiducials.size())
            {
                return;
            }

            data::fiducials_series::fiducial& fiducial = *point_fiducials[m_picked_data->m_index];
            set_point(
                fiducial_set->first,
                fiducial,
                {new_pos.x, new_pos.y, new_pos.z},
                lf_lock.image_series.get_shared()
            );
            lf_lock.image_series->get_fiducials()->set_fiducial_set(fiducial_set->second, fiducial_set->first);

            lf_lock.image_series->signal<data::has_fiducials::signals::point_modified>(
                data::has_fiducials::signals::POINT_MODIFIED
            )->async_emit(m_picked_data->m_group_name, m_picked_data->m_index);
        }

        this->request_render();
    }
    else if(m_edit_mode != edit_mode::display)
    {
        if(try_pick(_x, _y) != nullptr)
        {
            set_cursor(Qt::OpenHandCursor);
        }
        else
        {
            set_cursor(Qt::CrossCursor);
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::button_release_event(mouse_button _button, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_picked_data != nullptr)
    {
        m_picked_data->m_node->setScale(DEFAULT_SCALE, DEFAULT_SCALE, DEFAULT_SCALE);

        set_cursor(Qt::OpenHandCursor);

        // If we are in edit mode, we show the bin button to remove it.
        if((m_edit_mode& edit_mode::edit) == edit_mode::edit && m_must_show_contextual_menu)
        {
            std::pair<Ogre::Vector2,
                      Ogre::Vector2> screen_pos = sight::viz::scene3d::helper::scene::compute_bounding_rect(
                *layer()->get_default_camera(),
                m_picked_data->m_node
                      );
            const auto ratio    = m_contextual_menu->devicePixelRatioF();
            auto interactor     = layer()->render_service()->get_interactor_manager();
            auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
            auto* parent_widget = qt_interactor->get_qt_widget();
            const int x         = std::clamp(
                int(((screen_pos.first.x + screen_pos.second.x) / 2) / ratio),
                0,
                parent_widget->width() - m_contextual_menu->width()
            );

            int y = int((screen_pos.first.y / ratio) - m_contextual_menu->height());
            if(y < 0)
            {
                // If there isn't enough place upward the landmark, place the menu downward.
                y = int(screen_pos.second.y / ratio);
            }

            m_contextual_menu->move(x, y);
            m_contextual_menu->raise();
            const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));
            auto* bin_button         = m_contextual_menu->findChild<QPushButton*>(service_id + "/binButton");
            SIGHT_ASSERT("The contextual menu should have the bin button", bin_button);
            bin_button->disconnect();
            QObject::connect(
                bin_button,
                &QPushButton::clicked,
                [this, group_name = m_picked_data->m_group_name, index = m_picked_data->m_index]
                {
                    m_contextual_menu->hide();
                    landmarks_or_image_series_lock lf_lock = lock_landmarks();
                    if(lf_lock.landmarks != nullptr)
                    {
                        lf_lock.landmarks->remove_point(group_name, index);
                        lf_lock.landmarks->signal<sight::data::landmarks::point_removed_signal_t>(
                            sight::data::landmarks::POINT_REMOVED_SIG
                        )->async_emit(
                            group_name,
                            index
                        );
                    }
                    else if(lf_lock.image_series != nullptr)
                    {
                        lf_lock.image_series->get_fiducials()->remove_point(group_name, index);
                        lf_lock.image_series->signal<data::has_fiducials::signals::point_removed>(
                            data::has_fiducials::signals::POINT_REMOVED
                        )->async_emit(group_name, index);
                    }
                });
            if(m_renaming_allowed)
            {
                auto* pen_button = m_contextual_menu->findChild<QPushButton*>(service_id + "/penButton");
                SIGHT_ASSERT("The contextual menu should have the pen button", pen_button);
                pen_button->disconnect();
                QObject::connect(
                    pen_button,
                    &QPushButton::clicked,
                    [this, label = std::weak_ptr(m_picked_data->m_label)]
                    {
                        m_contextual_menu->hide();
                        for(const std::shared_ptr<landmark>& landmark : m_manual_objects)
                        {
                            landmark->m_label->set_edit_mode(false);
                        }

                        label.lock()->set_edit_mode(true);
                    });
            }

            m_contextual_menu->show();
        }

        m_picked_data = nullptr;

        this->layer()->request_render();
    }
    else if(_button == right && m_edit_mode != edit_mode::display)
    {
        disable_edit_mode();
        disable_move_mode();
    }
}

//------------------------------------------------------------------------------

void landmarks::button_double_press_event(mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y)
{
    m_contextual_menu->hide();

    std::shared_ptr<landmark> picked_data = try_pick(_x, _y, false);

    if(picked_data != nullptr)
    {
        layer()->cancel_further_interaction();

        // Check if something is picked to update the position of the distance.
        const auto picked_pos = get_nearest_picked_position(_x, _y);
        if(picked_pos.has_value())
        {
            landmarks_or_image_series_const_lock lock           = const_lock_landmarks();
            std::optional<data::landmarks::point_t> maybe_point = get_point(
                picked_data->m_group_name,
                picked_data->m_index,
                lock
            );
            if(!maybe_point.has_value())
            {
                return;
            }

            data::landmarks::point_t point = *maybe_point;

            // Send signal with world coordinates of the landmarks
            m_send_world_coord->async_emit(
                point[0],
                point[1],
                point[2]
            );
        }
    }
}

//------------------------------------------------------------------------------

void landmarks::wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    m_contextual_menu->hide();
}

//------------------------------------------------------------------------------

void landmarks::key_press_event(int _key, modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    if(m_edit_mode != edit_mode::display && _key == Qt::Key_Escape)
    {
        disable_edit_mode();
        disable_move_mode();
    }
}

//------------------------------------------------------------------------------

std::shared_ptr<landmarks::landmark> landmarks::try_pick(int _x, int _y, bool _for_modification) const
{
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
        return nullptr;
    }

    // Find the ogre landmark and apply a scale.
    bool found = false;

    // The plane is always intercepted, if a landmark is found
    for(std::size_t index = 0 ; !found && index < query_result.size() ; ++index)
    {
        // The landmark should be the nearest object found.
        const auto* const object = query_result[index].movable;

        landmarks_or_image_series_const_lock lock = const_lock_landmarks();

        for(const auto& landmark : m_manual_objects)
        {
            if(landmark->m_object == object)
            {
                try
                {
                    if(auto group = get_group(landmark->m_group_name, lock);
                       group.has_value() && group->m_visibility
                       && is_landmark_visible(group->m_points[landmark->m_index], group->m_size)
                       && (!_for_modification
                           || !m_can_only_modify_current
                           || landmark->m_group_name == m_current_group
                           || m_movable_groups.contains(landmark->m_group_name)))
                    {
                        return landmark;
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

    return nullptr;
}

//------------------------------------------------------------------------------

void landmarks::set_cursor(QCursor _cursor)
{
    auto interactor     = layer()->render_service()->get_interactor_manager();
    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parent_widget = qt_interactor->get_qt_widget();
    parent_widget->setCursor(_cursor);
}

//------------------------------------------------------------------------------

landmarks_or_image_series_lock landmarks::lock_landmarks()
{
    return {.landmarks = m_landmarks.lock(), .image_series = m_image_series.lock()};
}

//------------------------------------------------------------------------------

landmarks_or_image_series_const_lock landmarks::const_lock_landmarks() const
{
    return {.landmarks = m_landmarks.const_lock(), .image_series = m_image_series.const_lock()};
}

//------------------------------------------------------------------------------

image_or_image_series_lock landmarks::lock_image()
{
    return {.image = m_image.lock(), .image_series = m_image_series.lock()};
}

//------------------------------------------------------------------------------

image_or_image_series_const_lock landmarks::const_lock_image() const
{
    return {.image = m_image.const_lock(), .image_series = m_image_series.const_lock()};
}

landmarks::delete_contextual_menu_when_focus_out::delete_contextual_menu_when_focus_out(landmarks* _s_landmarks) :
    m_s_landmarks(_s_landmarks)
{
}

//------------------------------------------------------------------------------

bool landmarks::delete_contextual_menu_when_focus_out::eventFilter(QObject* /*o*/, QEvent* _e)
{
    if(m_s_landmarks->m_contextual_menu != nullptr
       && !m_s_landmarks->m_contextual_menu->findChild<QPushButton*>()->hasFocus()
       && (_e->type() == QEvent::FocusOut || _e->type() == QEvent::Resize))
    {
        m_s_landmarks->m_contextual_menu->hide();
    }

    return false;
}

} // namespace sight::module::viz::scene3d_qt::adaptor.
