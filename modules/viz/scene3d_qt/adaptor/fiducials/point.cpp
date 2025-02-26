/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include "point.hpp"

#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/uuid.hpp>

#include <data/dicom/attribute.hpp>
#include <data/fiducials_series.hpp>
#include <data/helper/fiducials_series.hpp>
#include <data/image_series.hpp>

#include <geometry/data/image.hpp>

#include <ui/__/cursor.hpp>

#include <viz/scene3d/helper/manual_object.hpp>
#include <viz/scene3d/helper/scene.hpp>
#include <viz/scene3d/utils.hpp>

#include <modules/viz/scene3d_qt/window_interactor.hpp>

#include <QHBoxLayout>
#include <QPushButton>

#include <ranges>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

static constexpr Ogre::Real SELECTED_SCALE = 1.35F;
static constexpr Ogre::Real DEFAULT_SCALE  = 1.F;

namespace
{

struct private_slots final
{
    using key_t = sight::core::com::slots::key_t;

    inline static const key_t REMOVE_GROUP = "remove_group";
    inline static const key_t MODIFY_GROUP = "modify_group";
    inline static const key_t RENAME_GROUP = "rename_group";

    inline static const key_t INSERT_POINT   = "insert_point";
    inline static const key_t MODIFY_POINT   = "modify_point";
    inline static const key_t ADD_POINT      = "add_point";
    inline static const key_t REMOVE_POINT   = "remove_point";
    inline static const key_t SELECT_POINT   = "select_point";
    inline static const key_t DESELECT_POINT = "deselect_point";

    inline static const key_t SLICE_TYPE  = "slice_type";
    inline static const key_t SLICE_INDEX = "slice_index";
};

class hide_contextual_menu_when_focus_out final : public QObject
{
public:

    explicit hide_contextual_menu_when_focus_out(QObject* _filtered, QWidget* _contextual_menu) :
        m_filtered(_filtered),
        m_contextual_menu(_contextual_menu)
    {
        _filtered->installEventFilter(this);
    }

    ~hide_contextual_menu_when_focus_out() final
    {
        if(!m_filtered.isNull())
        {
            m_filtered->removeEventFilter(this);
        }
    }

protected:

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* /*_o*/, QEvent* _e) final
    {
        if(!m_contextual_menu.isNull()
           && (_e->type() == QEvent::FocusOut || _e->type() == QEvent::Resize)
           && !m_contextual_menu->findChild<QPushButton*>()->hasFocus())

        {
            m_contextual_menu->hide();
        }

        return false;
    }

private:

    const QPointer<QObject> m_filtered;
    const QPointer<QWidget> m_contextual_menu;
};

/// @return true if the fiducial_query need to be added to the list
inline bool fiducial_query_predicate(const data::fiducials_series::fiducial_query& _result)
{
    return _result.m_contour_data && _result.m_contour_data->size() == 3;
}

} // namespace

//------------------------------------------------------------------------------

Ogre::Vector3 point::get_cam_direction(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

point::point() noexcept
{
    new_slot(private_slots::RENAME_GROUP, &point::rename_group, this);
    new_slot(private_slots::REMOVE_GROUP, &point::remove_group, this);
    new_slot(private_slots::MODIFY_GROUP, &point::modify_group, this);

    new_slot(private_slots::ADD_POINT, &point::add_point, this);
    new_slot(private_slots::MODIFY_POINT, &point::modify_point, this);
    new_slot(private_slots::REMOVE_POINT, &point::remove_point, this);
    new_slot(private_slots::INSERT_POINT, &point::insert_point, this);
    new_slot(private_slots::SELECT_POINT, &point::select_point, this);
    new_slot(private_slots::DESELECT_POINT, &point::deselect_point, this);

    new_slot(private_slots::SLICE_TYPE, &point::change_slice_type, this);
    new_slot(private_slots::SLICE_INDEX, &point::change_slice_index, this);

    new_slot(slots::REMOVE_FIDUCIALS, &point::remove_fiducials, this);
    new_slot(slots::REMOVE_VISIBLE_FIDUCIALS, &point::remove_visible_fiducials, this);
    new_slot(slots::SET_CURRENT_GROUP, &point::set_current_group, this);
    new_slot(slots::CONFIGURE_FIDUCIALS, &point::configure_fiducials, this);
    new_slot(slots::ENABLE_EDIT_MODE, &point::enable_edit_mode, this);
    new_slot(slots::DISABLE_EDIT_MODE, &point::disable_edit_mode, this);
    new_slot(
        slots::TOGGLE_EDIT_MODE,
        [this]
        {
            (m_edit_mode& edit_mode::edit) == edit_mode::edit ? disable_edit_mode() : enable_edit_mode();
        });
    new_slot(slots::CHANGE_EDIT_MODE, [this](bool _edit_mode){_edit_mode ? enable_edit_mode() : disable_edit_mode();});
    new_slot(slots::ENABLE_MOVE_MODE, &point::enable_move_mode, this);
    new_slot(slots::DISABLE_MOVE_MODE, &point::disable_move_mode, this);
    new_slot(
        slots::TOGGLE_MOVE_MODE,
        [this]
        {
            (m_edit_mode& edit_mode::move) == edit_mode::move ? disable_move_mode() : enable_move_mode();
        });
    new_slot(slots::CHANGE_MOVE_MODE, [this](bool _edit_mode){_edit_mode ? enable_move_mode() : disable_move_mode();});

    new_signal<signals::send_world_coordinates_t>(signals::SEND_WORLD_COORD);
    new_signal<signals::edit_mode_changed_t>(signals::EDIT_MODE_CHANGED);
}

//-----------------------------------------------------------------------------

void point::configuring()
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
    static const std::string s_FIDUCIALS_FLAGS_CONFIG = CONFIG + "landmarksQueryFlags";
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
        m_axis = axis_t::z_axis;
    }
    else if(orientation == "frontal")
    {
        m_axis = axis_t::y_axis;
    }
    else if(orientation == "sagittal")
    {
        m_axis = axis_t::x_axis;
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

    hexa_mask = config.get<std::string>(s_FIDUCIALS_FLAGS_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values (" + s_FIDUCIALS_FLAGS_CONFIG + ") should start with '0x'. Given value : " + hexa_mask,
            hexa_mask.length() > 2 && hexa_mask.substr(0, 2) == "0x"
        );
        m_fiducials_query_flag = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
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
    m_initial_group = m_current_group;

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
        m_current_shape = sight::data::fiducials_series::private_shape::sphere;
    }
    else if(initial_shape == "cube")
    {
        m_current_shape = sight::data::fiducials_series::private_shape::cube;
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
        "'imageSeries' must be present as parameter",
        m_image_series.const_lock()
    );
}

//-----------------------------------------------------------------------------

void point::starting()
{
    adaptor::init();

    render_service()->make_current();

    auto* root_scene_node = get_scene_manager()->getRootSceneNode();
    m_transform_node = get_or_create_transform_node(root_scene_node);

    const auto mtl_name = gen_id("fiducial_point_material");
    m_material = std::make_unique<sight::viz::scene3d::material::standard>(mtl_name);
    m_material->set_layout(data::mesh::attribute::point_normals | data::mesh::attribute::point_colors);
    const sight::viz::scene3d::layer::sptr layer = this->layer();
    m_material->set_shading(sight::data::material::shading_t::phong, layer->num_lights(), false, false);

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
        layer->add_interactor(interactor, m_priority);
    }

    auto interactor     = layer->render_service()->get_interactor_manager();
    auto qt_interactor  = std::dynamic_pointer_cast<window_interactor>(interactor);
    auto* parent_widget = qt_interactor->get_qt_widget();
    m_contextual_menu = new QWidget(parent_widget);
    m_contextual_menu->setStyleSheet(".QWidget {background-color: none;}");
    m_contextual_menu->hide();
    auto* layout = new QHBoxLayout;
    QIcon trash_bin_icon(QString::fromStdString(
                             core::runtime::get_module_resource_file_path(
                                 "sight::module::ui::icons/trash.svg"
                             ).string()
    ));
    auto* bin_button             = new QPushButton(trash_bin_icon, "");
    const std::string service_id = base_id();
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
                               "sight::module::ui::icons/text.svg"
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

    // Draw all fiducials.
    updating();
}

//-----------------------------------------------------------------------------

service::connections_t point::auto_connections() const
{
    return sight::service::connections_t {
        {m_image_series, data::image_series::MODIFIED_SIG, adaptor::slots::LAZY_UPDATE},

        {m_image_series, data::has_fiducials::signals::GROUP_RENAMED, private_slots::RENAME_GROUP},
        {m_image_series, data::has_fiducials::signals::GROUP_REMOVED, private_slots::REMOVE_GROUP},
        {m_image_series, data::has_fiducials::signals::GROUP_MODIFIED, private_slots::MODIFY_GROUP},

        {m_image_series, data::has_fiducials::signals::POINT_ADDED, private_slots::ADD_POINT},
        {m_image_series, data::has_fiducials::signals::POINT_MODIFIED, private_slots::MODIFY_POINT},
        {m_image_series, data::has_fiducials::signals::POINT_REMOVED, private_slots::REMOVE_POINT},
        {m_image_series, data::has_fiducials::signals::POINT_INSERTED, private_slots::INSERT_POINT},
        {m_image_series, data::has_fiducials::signals::POINT_SELECTED, private_slots::SELECT_POINT},
        {m_image_series, data::has_fiducials::signals::POINT_DESELECTED, private_slots::DESELECT_POINT},

        {m_image_series, data::image::SLICE_TYPE_MODIFIED_SIG, private_slots::SLICE_TYPE},
        {m_image_series, data::image::SLICE_INDEX_MODIFIED_SIG, private_slots::SLICE_INDEX}
    } + adaptor::auto_connections();
}

//-----------------------------------------------------------------------------

void point::updating()
{
    render_service()->make_current();

    // Delete all groups.
    remove_ogre_fiducials();

    const auto& locked_image = m_image_series.const_lock();
    const auto& fiducials    = locked_image->get_fiducials();

    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        data::fiducials_series::shape::point
    );

    // Create all points
    for(const auto& query_result : query_results)
    {
        create_ogre_fiducial(query_result, *locked_image);
    }

    update_done();

    request_render();
}

//-----------------------------------------------------------------------------

void point::stopping()
{
    // Make the context as current.
    render_service()->make_current();

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(get_sptr());
        layer()->remove_interactor(interactor);
    }

    // Stop all timers.
    std::ranges::for_each(
        m_selected_ogre_fiducials,
        [](const auto& _selected_ogre_fiducial)
        {
            _selected_ogre_fiducial->m_timer->stop();
        });

    remove_ogre_fiducials();

    // Unregister the material adaptor.
    unregister_services();

    m_event_filter.reset();
    m_material.reset();

    adaptor::deinit();
}

//------------------------------------------------------------------------------

void point::remove_ogre_fiducials()
{
    render_service()->make_current();

    auto* const scene_manager = get_scene_manager();

    std::erase_if(
        m_ogre_fiducials,
        [this, &scene_manager](const auto& _ogre_fiducial)
        {
            const auto& group_name = _ogre_fiducial->m_group_name;

            deselect_point(group_name, _ogre_fiducial->m_index);

            if(_ogre_fiducial->m_label)
            {
                _ogre_fiducial->m_label->detach_from_node();
                _ogre_fiducial->m_label->set_visible(false);
                _ogre_fiducial->m_label.reset();
            }

            m_transform_node->removeAndDestroyChild(_ogre_fiducial->m_node);
            scene_manager->destroyManualObject(_ogre_fiducial->m_manual_object);

            return true;
        });
}

//------------------------------------------------------------------------------

void point::emit_removed_signals(
    const std::vector<data::fiducials_series::fiducial_query>& _removed_results,
    const std::set<std::string>& _removed_fiducial_sets,
    const data::image_series& _image_series,
    bool _remove_ogre_fiducials
)
{
    // The point_removed signal
    const auto& point_removed_sig = _image_series.signal<data::has_fiducials::signals::point_removed>(
        data::has_fiducials::signals::POINT_REMOVED
    );

    // Block the signal to avoid being called back.
    core::com::connection::blocker point_removed_blocker(
        point_removed_sig->get_connection(slot(private_slots::REMOVE_POINT))
    );

    // Do it in reverse order to avoid index shifting.
    for(const auto& remove_result : std::views::reverse(_removed_results))
    {
        // Remove also the ogre fiducial if needed.
        if(_remove_ogre_fiducials && remove_result.m_group_name)
        {
            destroy_ogre_fiducials(*remove_result.m_group_name, remove_result.m_shape_index, true);
        }

        // Emit the point_removed signal for each removed fiducial.
        point_removed_sig->async_emit(remove_result.m_group_name.value_or(""), remove_result.m_shape_index);
    }

    // The group_removed signal.
    const auto& group_removed_sig = _image_series.signal<data::has_fiducials::signals::group_removed>(
        data::has_fiducials::signals::GROUP_REMOVED
    );

    // Block the signal to avoid being called back.
    core::com::connection::blocker group_removed_blocker(
        group_removed_sig->get_connection(slot(private_slots::REMOVE_GROUP))
    );

    for(const auto& removed_fiducial_set : _removed_fiducial_sets)
    {
        // Emit the group_removed signal for each removed fiducial set.
        group_removed_sig->async_emit(removed_fiducial_set);
    }
}

//------------------------------------------------------------------------------

void point::remove_fiducials()
{
    render_service()->make_current();

    m_contextual_menu->hide();

    // First we remove all ogre fiducials.
    remove_ogre_fiducials();

    // Then we remove all fiducials from the image series.
    auto locked_image = m_image_series.lock();
    auto fiducials    = locked_image->get_fiducials();
    const auto& [remove_results, removed_fiducial_sets] = fiducials->remove_fiducials(
        fiducial_query_predicate,
        data::fiducials_series::shape::point
    );

    emit_removed_signals(remove_results, removed_fiducial_sets, *locked_image, true);

    if(!remove_results.empty())
    {
        request_render();
    }
}

//------------------------------------------------------------------------------

void point::remove_visible_fiducials()
{
    render_service()->make_current();

    m_contextual_menu->hide();

    const auto& locked_image = m_image_series.lock();
    const auto& fiducials    = locked_image->get_fiducials();

    const auto visible_predicate =
        [this, &locked_image](const data::fiducials_series::fiducial_query& _result)
        {
            if(!_result.m_contour_data || _result.m_contour_data->size() != 3)
            {
                return false;
            }

            return check_fiducial_visibility(_result, *locked_image);
        };

    const auto& [remove_results, removed_fiducial_sets] = fiducials->remove_fiducials(
        visible_predicate,
        data::fiducials_series::shape::point
    );

    emit_removed_signals(remove_results, removed_fiducial_sets, *locked_image, true);

    if(!remove_results.empty())
    {
        request_render();
    }
}

//------------------------------------------------------------------------------

void point::remove_group(std::string _group_name)
{
    render_service()->make_current();

    if(m_current_group == _group_name)
    {
        m_current_group = m_initial_group;
    }

    // Find object where name match _groupName and delete Ogre's resources.
    if(destroy_ogre_fiducials(_group_name) > 0)
    {
        request_render();
    }
}

//------------------------------------------------------------------------------

void point::modify_group(std::string _group_name)
{
    render_service()->make_current();

    // Get all selected point.
    std::set<std::size_t> selected_indexes;

    std::ranges::for_each(
        m_selected_ogre_fiducials,
        [&selected_indexes, &_group_name](const auto& _selected_ogre_fiducial)
        {
            if(_selected_ogre_fiducial->m_ogre_fiducial->m_group_name == _group_name)
            {
                selected_indexes.insert(_selected_ogre_fiducial->m_ogre_fiducial->m_index);
            }
        });

    // Remove the group.
    remove_group(_group_name);

    // Query all fiducials of the group.
    const auto& locked_image  = m_image_series.const_lock();
    const auto& fiducials     = locked_image->get_fiducials();
    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        sight::data::fiducials_series::shape::point,
        _group_name
    );

    // Create again all points
    for(const auto& query_result : query_results)
    {
        create_ogre_fiducial(query_result, *locked_image);
    }

    // Re-run selected fiducial timers
    for(const auto& index : selected_indexes)
    {
        select_point(_group_name, index);
    }

    // Request the rendering.
    request_render();
}

//------------------------------------------------------------------------------

void point::rename_group(std::string _old_group_name, std::string _new_group_name)
{
    render_service()->make_current();

    // Change the group name of all fiducials.
    for(const auto& ogre_fiducial : m_ogre_fiducials)
    {
        if(ogre_fiducial->m_group_name == _old_group_name)
        {
            ogre_fiducial->m_group_name = _new_group_name;

            if(ogre_fiducial->m_label)
            {
                ogre_fiducial->m_label->set_text(core::id::join(_new_group_name, ogre_fiducial->m_index));
            }
        }
    }

    // Do not forget to update the current group name.
    if(m_current_group == _old_group_name)
    {
        m_current_group = _new_group_name;
    }

    request_render();
}

//------------------------------------------------------------------------------

void point::set_current_group(std::string _group_name)
{
    m_current_group = _group_name.empty() ? INITIAL_GROUP_NAME : _group_name;
}

//------------------------------------------------------------------------------

void point::modify_point(std::string _group_name, std::size_t _index)
{
    render_service()->make_current();

    // Get the selected point.
    std::optional<std::size_t> selected_index;

    if(std::ranges::find_if(
           m_selected_ogre_fiducials,
           [&_group_name, &_index](const auto& _selected_ogre_fiducial)
        {
            return _selected_ogre_fiducial->m_ogre_fiducial->m_group_name == _group_name
                   && _selected_ogre_fiducial->m_ogre_fiducial->m_index == _index;
        }) != m_selected_ogre_fiducials.end())
    {
        selected_index = _index;
    }

    // Remove the ogre fiducial.
    // We normally would have to update it, but it is simpler to simply destroy it and recreate it.
    if(destroy_ogre_fiducials(_group_name, _index) == 0)
    {
        return;
    }

    // ...and recreate it.
    // Query fiducial
    const auto& locked_image  = m_image_series.const_lock();
    const auto& fiducials     = locked_image->get_fiducials();
    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        sight::data::fiducials_series::shape::point,
        _group_name,
        _index
    );

    if(query_results.empty())
    {
        return;
    }

    create_ogre_fiducial(query_results.front(), *locked_image);

    // Re-run selected fiducial timers
    if(selected_index)
    {
        select_point(_group_name, *selected_index);
    }

    request_render();
}

//------------------------------------------------------------------------------

void point::add_point(std::string _group_name)
{
    render_service()->make_current();

    // Query fiducial
    const auto& locked_image  = m_image_series.const_lock();
    const auto& fiducials     = locked_image->get_fiducials();
    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        sight::data::fiducials_series::shape::point,
        _group_name
    );

    if(query_results.empty())
    {
        return;
    }

    // Create the fiducial (should be the last one).
    create_ogre_fiducial(query_results.back(), *locked_image);

    request_render();
}

//------------------------------------------------------------------------------

void point::remove_point(std::string _group_name, std::size_t _index)
{
    // Make the context as current.
    render_service()->make_current();

    if(destroy_ogre_fiducials(_group_name, _index, true) == 0)
    {
        return;
    }

    request_render();
}

//------------------------------------------------------------------------------

void point::insert_point(std::string _group_name, std::size_t _index)
{
    // Make the context as current
    render_service()->make_current();

    // Adjust index of fiducials in the same group.
    std::ranges::for_each(
        m_ogre_fiducials,
        [&_group_name, &_index](const auto& _ogre_fiducial)
        {
            if(_ogre_fiducial->m_group_name == _group_name
               && _ogre_fiducial->m_index >= _index)
            {
                _ogre_fiducial->m_index++;

                if(_ogre_fiducial->m_label)
                {
                    _ogre_fiducial->m_label->set_text(core::id::join(_group_name, _ogre_fiducial->m_index));
                }
            }
        });

    // Query fiducial
    const auto& locked_image  = m_image_series.const_lock();
    const auto& fiducials     = locked_image->get_fiducials();
    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        sight::data::fiducials_series::shape::point,
        _group_name,
        _index
    );

    if(query_results.empty())
    {
        return;
    }

    // Create the fiducial.
    create_ogre_fiducial(query_results.front(), *locked_image);

    request_render();
}

//------------------------------------------------------------------------------

std::shared_ptr<point::ogre_fiducial> point::create_ogre_fiducial(
    const data::fiducials_series::fiducial_query& _query,
    const data::image_series& _image_series
)
{
    return create_ogre_fiducial(
        _query.m_group_name.value_or(m_current_group),
        _query.m_shape_index,
        _query.m_private_shape.value_or(m_current_shape),
        _query.m_color.value_or(m_current_color),
        _query.m_size.value_or(m_current_size),
        check_fiducial_visibility(_query, _image_series),
        _query.m_contour_data.value_or(std::vector<double> {0, 0, 0})
    );
}

//------------------------------------------------------------------------------

std::shared_ptr<point::ogre_fiducial> point::create_ogre_fiducial(
    const std::string& _group_name,
    std::size_t _index,
    data::fiducials_series::private_shape _private_shape,
    std::array<float, 4> _color,
    float _size,
    bool _visible,
    std::vector<double> _position
)
{
    // Create the point name.
    const std::string point_name = core::id::join(_group_name, _index);
    const auto id                = get_id();

    auto* const manual_object = get_scene_manager()->createManualObject(core::id::join(id, point_name, "object"));

    const Ogre::ColourValue ogre_color(_color[0], _color[1], _color[2], _color[3]);

    if(_private_shape == data::fiducials_series::private_shape::cube)
    {
        sight::viz::scene3d::helper::manual_object::create_cube(
            manual_object,
            m_material->name(),
            ogre_color,
            _size
        );
    }
    else
    {
        sight::viz::scene3d::helper::manual_object::create_sphere(
            manual_object,
            m_material->name(),
            ogre_color,
            _size
        );
    }

    manual_object->setVisible(_visible);
    manual_object->setQueryFlags(m_fiducials_query_flag);

    auto* const node = m_transform_node->createChildSceneNode(core::id::join(id, point_name, "node"));

    // Set the point to the right position.
    node->setPosition(Ogre::Real(_position[0]), Ogre::Real(_position[1]), Ogre::Real(_position[2]));

    // Attach the node to the manual object.
    node->attachObject(manual_object);

    // Create the label.
    sight::viz::scene3d::text::sptr text;

    if(m_enable_labels)
    {
        // Create the label.
        text = sight::viz::scene3d::text::make(this->layer());
        text->set_font_size(m_font_size);
        text->set_text(point_name);
        text->set_text_color(ogre_color);
        text->set_visible(_visible);
        text->set_text_alignment("center", "");

        // Attach data.
        text->attach_to_node(node, this->layer()->get_default_camera());
    }

    // Store the created data.
    auto new_ogre_fiducial = std::make_shared<ogre_fiducial>(node, manual_object, _group_name, _index, text);
    m_ogre_fiducials.insert(new_ogre_fiducial);

    if(m_enable_labels)
    {
        // The ogre_fiducial should be in a weak ptr, else there is a circular reference: the slot refers to the
        // ogre_fiducial via new_ogre_fiducial, and the ogre_fiducial refers to the slot via m_slots.
        auto slot_text_edited = core::com::new_slot(
            [this, weak_ogre_fiducial = std::weak_ptr(new_ogre_fiducial)](std::string _edited_text)
            {
                const auto ogre_fiducial = weak_ogre_fiducial.lock();

                if(!ogre_fiducial)
                {
                    SIGHT_WARN("Couldn't rename group, the ogre fiducial doesn't exist anymore");
                    return;
                }

                std::string new_group_name  = _edited_text;
                std::string index_as_string = std::to_string(ogre_fiducial->m_index);

                // Remove the index suffix if present
                if(new_group_name.ends_with('-' + index_as_string))
                {
                    new_group_name = new_group_name.substr(0, new_group_name.size() - (index_as_string.size() + 1));
                }

                // Not allowed to use an empty name.
                if(new_group_name.empty())
                {
                    SIGHT_WARN("Couldn't rename group, the new name is empty");
                    return;
                }

                // Retrieve the fiducial set and index.
                auto locked_image = m_image_series.lock();
                auto fiducials    = locked_image->get_fiducials();

                // Check if we already have a fiducial_set with the new_group_name.
                if(const auto& duplicate_result = fiducials->query_fiducials(
                       fiducial_query_predicate,
                       sight::data::fiducials_series::shape::point,
                       new_group_name,
                       ogre_fiducial->m_index
                ); !duplicate_result.empty())
                {
                    SIGHT_WARN("Couldn't rename group, the new name is already being used");
                    return;
                }

                const auto& query_results = fiducials->query_fiducials(
                    fiducial_query_predicate,
                    sight::data::fiducials_series::shape::point,
                    ogre_fiducial->m_group_name,
                    ogre_fiducial->m_index
                );

                if(query_results.empty())
                {
                    SIGHT_WARN("Couldn't rename group '" << ogre_fiducial->m_group_name << "', it doesn't exist");
                    return;
                }

                for(const auto& query_result : query_results)
                {
                    // Rename the group in the fiducial_series.
                    fiducials->set_group_name(query_result.m_fiducial_set_index, new_group_name);
                }

                // The signal should call the rename_group slot
                locked_image->signal<data::has_fiducials::signals::group_renamed>(
                    data::has_fiducials::signals::GROUP_RENAMED
                )->async_emit(ogre_fiducial->m_group_name, new_group_name);
            });

        slot_text_edited->set_worker(core::thread::get_default_worker());
        new_ogre_fiducial->m_label->signal(sight::viz::scene3d::text::TEXT_EDITED_SIGNAL)->connect(slot_text_edited);
        new_ogre_fiducial->m_slots.push_back(slot_text_edited);

        auto slot_editing_finished = core::com::new_slot(
            [weak_label = std::weak_ptr(new_ogre_fiducial->m_label)]
            {
                if(const auto label = weak_label.lock(); label)
                {
                    label->set_edit_mode(false);
                }
            });

        slot_editing_finished->set_worker(core::thread::get_default_worker());
        new_ogre_fiducial->m_label->signal(sight::viz::scene3d::text::EDITING_FINISHED_SIGNAL)->connect(
            slot_editing_finished
        );
        new_ogre_fiducial->m_slots.push_back(slot_editing_finished);
    }

    // Request the rendering.
    request_render();

    return new_ogre_fiducial;
}

//------------------------------------------------------------------------------

std::size_t point::destroy_ogre_fiducials(
    const std::string& _group_name,
    const std::optional<std::size_t>& _index,
    const bool _rebuild_indexes
)
{
    auto* const scene_manager = get_scene_manager();

    const auto count = std::erase_if(
        m_ogre_fiducials,
        [this, &_group_name, &_index, &scene_manager](const auto& _ogre_fiducial)
        {
            if(_ogre_fiducial->m_group_name == _group_name && (!_index || _ogre_fiducial->m_index == *_index))
            {
                // Stop the timer if it already run since we are deleting data.
                deselect_point(_ogre_fiducial->m_group_name, _ogre_fiducial->m_index);

                if(_ogre_fiducial->m_label)
                {
                    _ogre_fiducial->m_label->detach_from_node();
                    _ogre_fiducial->m_label->set_visible(false);
                    _ogre_fiducial->m_label.reset();
                }

                m_transform_node->removeAndDestroyChild(_ogre_fiducial->m_node);
                scene_manager->destroyManualObject(_ogre_fiducial->m_manual_object);

                return true;
            }

            return false;
        });

    if(_index && _rebuild_indexes)
    {
        // Adjust index of fiducials in the same group.
        std::ranges::for_each(
            m_ogre_fiducials,
            [&_group_name, &_index](const auto& _ogre_fiducial)
            {
                if(_ogre_fiducial->m_group_name == _group_name
                   && _ogre_fiducial->m_index > *_index)
                {
                    _ogre_fiducial->m_index--;

                    if(_ogre_fiducial->m_label)
                    {
                        _ogre_fiducial->m_label->set_text(
                            core::id::join(_group_name, _ogre_fiducial->m_index)
                        );
                    }
                }
            });
    }

    return count;
}

//------------------------------------------------------------------------------

void point::select_point(std::string _group_name, std::size_t _index)
{
    // This method must be synchronized with selectPoint(std::string, std::size_t).
    std::lock_guard guard(m_selected_mutex);

    // Check if the point is not already selected.
    if(const auto it = std::ranges::find_if(
           m_selected_ogre_fiducials,
           [&_group_name, &_index](const auto& _selected_ogre_fiducial)
        {
            return _selected_ogre_fiducial->m_ogre_fiducial->m_index == _index
                   && _selected_ogre_fiducial->m_ogre_fiducial->m_group_name == _group_name;
        }); it != m_selected_ogre_fiducials.end())
    {
        return;
    }

    // Find the ogre fiducial.
    if(const auto it = std::ranges::find_if(
           m_ogre_fiducials,
           [&_group_name, &_index](const auto& _ogre_fiducial)
        {
            return _ogre_fiducial->m_index == _index && _ogre_fiducial->m_group_name == _group_name;
        }); it != m_ogre_fiducials.end())
    {
        render_service()->make_current();

        // Create thread data.
        const auto selected_ogre_fiducial = std::make_shared<point::selected_ogre_fiducial>(
            worker()->create_timer(),
            *it
        );

        // Run a thread that change the selected point.
        selected_ogre_fiducial->m_timer->set_function(
            [this, selected_ogre_fiducial](auto&& ...)
            {
                highlight(selected_ogre_fiducial);
            });
        selected_ogre_fiducial->m_timer->set_duration(std::chrono::milliseconds(500));
        selected_ogre_fiducial->m_timer->start();

        m_selected_ogre_fiducials.emplace_back(selected_ogre_fiducial);
    }
}

//------------------------------------------------------------------------------

void point::deselect_point(std::string _group_name, std::size_t _index)
{
    // This method must be synchronized with select_point(std::string, std::size_t).
    std::lock_guard guard(m_selected_mutex);

    render_service()->make_current();

    // Find the thread and stop it.
    if(const auto it = std::ranges::find_if(
           m_selected_ogre_fiducials,
           [&](const std::shared_ptr<selected_ogre_fiducial>& _selected_ogre_fiducial)
        {
            return _selected_ogre_fiducial->m_ogre_fiducial->m_index == _index
                   && _selected_ogre_fiducial->m_ogre_fiducial->m_group_name == _group_name;
        }); it != m_selected_ogre_fiducials.end())
    {
        auto& selected_ogre_fiducial = *it;

        // Stop the timer.
        selected_ogre_fiducial->m_timer->stop();

        // Restore visibility
        const auto& locked_image  = m_image_series.const_lock();
        const auto& fiducials     = locked_image->get_fiducials();
        const auto& query_results = fiducials->query_fiducials(
            fiducial_query_predicate,
            data::fiducials_series::shape::point,
            _group_name,
            _index
        );

        if(!query_results.empty())
        {
            const bool is_visible = check_fiducial_visibility(query_results.front(), *locked_image);

            if(auto* const manual_object = selected_ogre_fiducial->m_ogre_fiducial->m_manual_object;
               manual_object->isVisible() != is_visible)
            {
                manual_object->setVisible(is_visible);

                // Request the rendering.
                request_render();
            }
        }

        // Remove the selected fiducial from selected list
        m_selected_ogre_fiducials.erase(it);
    }
}

//------------------------------------------------------------------------------

void point::highlight(std::shared_ptr<selected_ogre_fiducial> _selected_ogre_fiducial)
{
    render_service()->make_current();

    const auto& locked_image  = m_image_series.const_lock();
    const auto& fiducials     = locked_image->get_fiducials();
    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        data::fiducials_series::shape::point,
        _selected_ogre_fiducial->m_ogre_fiducial->m_group_name,
        _selected_ogre_fiducial->m_ogre_fiducial->m_index
    );

    const auto& manual_object = _selected_ogre_fiducial->m_ogre_fiducial->m_manual_object;

    if(!query_results.empty())
    {
        manual_object->setVisible(check_fiducial_visibility(query_results.front(), *locked_image));
    }

    if(manual_object->isVisible() && !_selected_ogre_fiducial->m_show)
    {
        manual_object->setVisible(false);
    }

    _selected_ogre_fiducial->m_show = !_selected_ogre_fiducial->m_show;

    request_render();
}

//------------------------------------------------------------------------------

void point::change_slice_type(int _from, int _to)
{
    const auto to_orientation   = static_cast<axis_t>(_to);
    const auto from_orientation = static_cast<axis_t>(_from);

    const auto plane_orientation = m_axis;
    const auto new_orientation   = m_axis == to_orientation
                                   ? from_orientation
                                   : m_axis == from_orientation
                                   ? to_orientation
                                   : m_axis;

    if(plane_orientation != new_orientation)
    {
        m_axis = new_orientation;

        apply_visibility();
    }
}

//------------------------------------------------------------------------------

void point::change_slice_index(int /*_axialIndex*/, int /*_frontalIndex*/, int /*_sagittalIndex*/)
{
    apply_visibility();
}

//------------------------------------------------------------------------------

void point::configure_fiducials(sight::viz::scene3d::fiducials_configuration _configuration)
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

    if(_configuration.slice_max)
    {
        if(*_configuration.slice_max >= 0)
        {
            m_slice_max = *_configuration.slice_max;
        }
        else
        {
            m_slice_max = std::nullopt;
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

void point::enable_edit_mode()
{
    // Make the context as current.
    render_service()->make_current();

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
        m_event_filter = std::make_unique<hide_contextual_menu_when_focus_out>(parent_widget, m_contextual_menu);
    }

    signal<signals::edit_mode_changed_t>(signals::EDIT_MODE_CHANGED)->async_emit(true);
}

//------------------------------------------------------------------------------

void point::disable_edit_mode()
{
    if((m_edit_mode& edit_mode::edit) != edit_mode::edit)
    {
        return;
    }

    // Make the context as current.
    render_service()->make_current();

    m_edit_mode &= ~edit_mode::edit;

    if(m_edit_mode == edit_mode::display)
    {
        this->set_cursor();
    }

    m_contextual_menu->hide();

    if(m_enable_labels && m_renaming_allowed)
    {
        std::ranges::for_each(
            m_ogre_fiducials,
            [](const auto& _ogre_fiducial)
            {
                _ogre_fiducial->m_label->set_edit_mode(false);
            });
    }

    signal<signals::edit_mode_changed_t>(signals::EDIT_MODE_CHANGED)->async_emit(false);
}

//------------------------------------------------------------------------------

void point::enable_move_mode()
{
    if((m_edit_mode& edit_mode::move) == edit_mode::move)
    {
        return;
    }

    // Make the context as current.
    render_service()->make_current();

    m_edit_mode |= edit_mode::move;

    set_cursor(Qt::CrossCursor);
}

//------------------------------------------------------------------------------

void point::disable_move_mode()
{
    if((m_edit_mode& edit_mode::move) != edit_mode::move)
    {
        return;
    }

    // Make the context as current.
    render_service()->make_current();

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

void point::create_and_pick_fiducial(const std::vector<double>& _point, bool _pick)
{
    if(is_max_fiducials_reached())
    {
        // No need to continue if the maximum number of fiducials is reached
        return;
    }

    auto locked_image = m_image_series.lock();
    auto fiducials    = locked_image->get_fiducials();

    // Generate a new frame of reference UID if needed.
    auto frame_of_reference_uid = locked_image->get_frame_of_reference_uid();

    if(!frame_of_reference_uid)
    {
        frame_of_reference_uid = locked_image->generate_uid();
        locked_image->set_frame_of_reference_uid(frame_of_reference_uid);
    }

    // Defines the predicate to add a new fiducial.
    const auto& fiducial_add_predicate =
        [this, &_point, &frame_of_reference_uid](data::fiducials_series::fiducial_query& _result) -> bool
        {
            // Fiducial set part
            if(!_result.m_group_name)
            {
                _result.m_group_name = m_current_group;
            }

            if(!_result.m_color)
            {
                _result.m_color = m_current_color;
            }

            if(!_result.m_size)
            {
                _result.m_size = m_current_size;
            }

            if(!_result.m_private_shape)
            {
                _result.m_private_shape = m_current_shape;
            }

            if(!_result.m_frame_of_reference_uid)
            {
                _result.m_frame_of_reference_uid = frame_of_reference_uid;
            }

            // Fiducial part
            _result.m_contour_data = _point;

            const std::string& fiducial_name = core::id::join(*_result.m_group_name, _result.m_shape_index);
            _result.m_fiducial_description = fiducial_name;
            _result.m_fiducial_identifier  = fiducial_name;
            _result.m_fiducial_uid         = core::tools::uuid::generate();

            return true;
        };

    // Add the new fiducial.
    const auto& [add_result, group_added] = fiducials->add_fiducial(
        fiducial_add_predicate,
        data::fiducials_series::shape::point,
        m_current_group
    );

    if(!add_result)
    {
        // No need to continue if no fiducials could be added
        return;
    }

    // Emit group_added signals..
    if(group_added)
    {
        const auto& sig = locked_image->signal<data::has_fiducials::signals::group_added>(
            data::has_fiducials::signals::GROUP_ADDED
        );

        // No need to block, we don't have connection on GROUP_ADDED
        sig->async_emit(m_current_group);
    }

    // Emit point_added signals..
    {
        const auto& sig = locked_image->signal<data::has_fiducials::signals::point_added>(
            data::has_fiducials::signals::POINT_ADDED
        );

        // Block the signal to avoid being called back.
        core::com::connection::blocker blocker(sig->get_connection(slot(private_slots::ADD_POINT)));
        sig->async_emit(m_current_group);
    }

    // Create the new point. Assume visible by default.
    const auto& ogre_fiducial = create_ogre_fiducial(
        add_result->m_group_name.value_or(m_current_group),
        add_result->m_shape_index,
        add_result->m_private_shape.value_or(m_current_shape),
        add_result->m_color.value_or(m_current_color),
        add_result->m_size.value_or(m_current_size),
        add_result->m_visible.value_or(true),
        _point
    );

    if(_pick)
    {
        m_picked_data = ogre_fiducial;
        m_picked_data->m_node->setScale(SELECTED_SCALE, SELECTED_SCALE, SELECTED_SCALE);
    }
    else
    {
        m_picked_data.reset();
    }

    request_render();
}

//------------------------------------------------------------------------------

bool point::is_max_fiducials_reached()
{
    if(!m_total_max && !m_slice_max && m_group_max.empty())
    {
        // Early return if we must not count anything
        return false;
    }

    // Get fiducials.
    const auto& locked_image  = m_image_series.const_lock();
    const auto& fiducials     = locked_image->get_fiducials();
    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        data::fiducials_series::shape::point
    );

    // If we reached the maximum number of fiducials, we return true.
    if(m_total_max && query_results.size() >= *m_total_max)
    {
        SIGHT_INFO("Maximum number of fiducial reached: " << *m_total_max);
        return true;
    }

    // Get the maximum number of fiducials for the current group.
    const auto& group_max =
        [this]
        {
            if(const auto it = m_group_max.find(m_current_group); it != m_group_max.end())
            {
                return std::make_optional(it->second);
            }

            return std::optional<std::size_t>();
        }();

    // Count fiducials...
    for(std::size_t group = 0, slice = 0 ; const auto& query_result : query_results)
    {
        if(group_max && query_result.m_group_name && *query_result.m_group_name == m_current_group
           && ++group >= *group_max)
        {
            SIGHT_INFO("Maximum number of fiducials in a group reached: " << *group_max);
            return true;
        }

        if(m_slice_max)
        {
            const auto& fiducial_position = query_result.m_contour_data.value_or(std::vector<double> {0, 0, 0});

            if(check_fiducial_visibility(
                   fiducial_position,
                   query_result.m_size.value_or(m_current_size),
                   *m_image_series.const_lock()
               ) && ++slice >= *m_slice_max)
            {
                SIGHT_INFO("Maximum number of fiducials in a slice reached: " << *m_slice_max);
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

bool point::check_fiducial_visibility(
    const std::vector<double>& _fiducial_position,
    float _fiducial_size,
    const data::image_series& _image
) const
{
    if(sight::data::helper::medical_image::check_image_validity(_image))
    {
        const auto axis_spacing = _image.spacing()[m_axis];

        const auto slice_index = sight::data::helper::medical_image::get_slice_index(
            _image,
            m_axis
        ).value_or(0);

        const auto fiducial_axis_position = geometry::data::world_to_image(_image, _fiducial_position, true)[m_axis];

        if(m_view_distance == point::view_distance::slices_in_range)
        {
            // Check if the position is the same than slice position
            const auto fiducial_axis_size = _fiducial_size / axis_spacing;

            return fiducial_axis_position >= std::int64_t(slice_index) - std::int64_t(std::ceil(fiducial_axis_size))
                   && fiducial_axis_position <= std::int64_t(slice_index) + std::int64_t(std::ceil(fiducial_axis_size));
        }

        if(m_view_distance == point::view_distance::current_slice)
        {
            // Check if the position is the same than slice position
            return fiducial_axis_position == slice_index;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

bool point::check_fiducial_visibility(
    const data::fiducials_series::fiducial_query& _query,
    const data::image_series& _image_series,
    const std::optional<bool>& _base_visibility
) const
{
    const auto& position = _query.m_contour_data.value_or(std::vector<double> {0, 0, 0});
    const float size     = _query.m_size.value_or(m_current_size);

    // Check if the fiducial is visible.
    return _base_visibility.value_or(visible())
           && _query.m_visible.value_or(true)
           && check_fiducial_visibility(position, size, _image_series);
}

//------------------------------------------------------------------------------

void point::set_visible(bool _visible)
{
    // Make the context as current.
    render_service()->make_current();

    bool need_render = false;

    const auto& locked_image  = m_image_series.const_lock();
    const auto& fiducials     = locked_image->get_fiducials();
    const auto& query_results = fiducials->query_fiducials(
        fiducial_query_predicate,
        data::fiducials_series::shape::point
    );

    for(const auto& query_result : query_results)
    {
        const bool visible = check_fiducial_visibility(query_result, *locked_image, _visible);

        if(const auto it = std::ranges::find_if(
               m_ogre_fiducials,
               [&query_result, &visible](const auto& _ogre_fiducial)
            {
                return _ogre_fiducial->m_index == query_result.m_shape_index
                       && _ogre_fiducial->m_manual_object->isVisible() != visible
                       && _ogre_fiducial->m_group_name == query_result.m_group_name;
            }); it != m_ogre_fiducials.end())
        {
            const auto& ogre_fiducial = *it;

            ogre_fiducial->m_manual_object->setVisible(visible);

            if(ogre_fiducial->m_label)
            {
                ogre_fiducial->m_label->set_visible(visible);
            }

            need_render = true;
        }
    }

    if(need_render)
    {
        request_render();
    }
}

//------------------------------------------------------------------------------

void point::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    render_service()->make_current();

    m_contextual_menu->hide();

    if(_button != left)
    {
        // We only manage left click.
        return;
    }

    if(m_edit_mode == edit_mode::display)
    {
        // If the EDIT mode is off, the fiducials are read-only.
        return;
    }

    const bool must_edit = (m_edit_mode& edit_mode::edit) == edit_mode::edit;

    // Try to pick a fiducial.
    if(const auto& picked_data = try_pick(_x, _y); picked_data)
    {
        // If something is picked, we will select it.
        set_cursor(Qt::ClosedHandCursor);
        m_picked_data = picked_data;
        m_picked_data->m_node->setScale(SELECTED_SCALE, SELECTED_SCALE, SELECTED_SCALE);

        // Only show contextual menu if we are in "true" EDIT mode
        m_must_show_contextual_menu = must_edit;

        request_render();
    }
    else if(must_edit)
    {
        // Nothing is picked, we will create a new fiducial if we are in EDIT mode.
        m_must_show_contextual_menu = false;

        // Find the place to put the new fiducial.
        if(const auto pos = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *get_scene_manager());
           pos)
        {
            set_cursor(Qt::ClosedHandCursor);
            create_and_pick_fiducial({pos->second.x, pos->second.y, pos->second.z});
        }
    }
}

//------------------------------------------------------------------------------

void point::mouse_move_event(mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    render_service()->make_current();

    if(m_picked_data != nullptr)
    {
        m_must_show_contextual_menu = false;

        set_cursor(Qt::ClosedHandCursor);

        // Discard the current fiducial to launch the ray over the scene without picking this one.
        m_picked_data->m_manual_object->setQueryFlags(0x0);

        const auto layer              = this->layer();
        const Ogre::Camera* const cam = layer->get_default_camera();
        SIGHT_ASSERT("No camera found", cam);

        bool move_in_camera_plane = true;

        Ogre::Vector3 new_pos;
        if(cam->getProjectionType() == Ogre::ProjectionType::PT_PERSPECTIVE)
        {
            Ogre::SceneManager* const scene_mgr = layer->get_scene_manager();
            // If something is picked, we will snap the fiducial to it
            auto picked_pos = sight::viz::scene3d::utils::pick_object(_x, _y, m_query_mask, *scene_mgr);
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
            const Ogre::Vector3 direction = get_cam_direction(cam);

            const Ogre::Vector3 position = m_picked_data->m_node->getPosition();

            const Ogre::Plane plane(direction, position);

            const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, plane);

            if(!hit.first)
            {
                return;
            }

            new_pos = ray.getPoint(hit.second);
        }

        // Move the ogre fiducial.
        m_picked_data->m_node->setPosition(new_pos);

        // Reset the query flag.
        m_picked_data->m_manual_object->setQueryFlags(m_fiducials_query_flag);

        // Update the fiducial.
        auto locked_image = m_image_series.lock();
        auto fiducials    = locked_image->get_fiducials();

        const auto& fiducial_modify_predicate =
            [&new_pos](data::fiducials_series::fiducial_query& _result) -> bool
            {
                // Reset result so only modified fields are updated
                _result                = {};
                _result.m_contour_data = {new_pos.x, new_pos.y, new_pos.z};

                return true;
            };

        const auto& modify_results = fiducials->modify_fiducials(
            fiducial_modify_predicate,
            data::fiducials_series::shape::point,
            m_picked_data->m_group_name,
            m_picked_data->m_index
        );

        if(!modify_results.empty())
        {
            const auto sig = locked_image->signal<data::has_fiducials::signals::point_modified>(
                data::has_fiducials::signals::POINT_MODIFIED
            );

            // Block the signal to avoid being called back.
            core::com::connection::blocker blocker(sig->get_connection(slot(private_slots::MODIFY_POINT)));
            sig->async_emit(m_picked_data->m_group_name, m_picked_data->m_index);
        }

        request_render();
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

void point::button_release_event(mouse_button _button, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    // Make the context as current.
    render_service()->make_current();

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
                // If there isn't enough place upward the fiducial, place the menu downward.
                y = int(screen_pos.second.y / ratio);
            }

            m_contextual_menu->move(x, y);
            m_contextual_menu->raise();
            const QString service_id = QString::fromStdString(base_id());
            auto* bin_button         = m_contextual_menu->findChild<QPushButton*>(service_id + "/binButton");
            SIGHT_ASSERT("The contextual menu should have the bin button", bin_button);
            bin_button->disconnect();
            QObject::connect(
                bin_button,
                &QPushButton::clicked,
                [this, weak_picked_data = std::weak_ptr(m_picked_data)]
                {
                    render_service()->make_current();

                    m_contextual_menu->hide();

                    // Do not do anything if the picked data is already destroyed.
                    if(auto picked_data = weak_picked_data.lock(); picked_data)
                    {
                        auto locked_image = m_image_series.lock();
                        auto fiducials    = locked_image->get_fiducials();

                        const auto& [remove_results, removed_fiducial_sets] = fiducials->remove_fiducials(
                            fiducial_query_predicate,
                            data::fiducials_series::shape::point,
                            picked_data->m_group_name,
                            picked_data->m_index
                        );

                        emit_removed_signals(remove_results, removed_fiducial_sets, *locked_image, true);

                        if(!remove_results.empty())
                        {
                            request_render();
                        }
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
                    [this, weak_label = std::weak_ptr(m_picked_data->m_label)]
                    {
                        m_contextual_menu->hide();

                        if(auto label = weak_label.lock(); label)
                        {
                            std::ranges::for_each(
                                m_ogre_fiducials,
                                [&label](const auto& _ogre_fiducial)
                            {
                                if(label != _ogre_fiducial->m_label)
                                {
                                    _ogre_fiducial->m_label->set_edit_mode(false);
                                }
                            });

                            label->set_visible(true);
                            label->set_edit_mode(true);
                        }
                    });
            }

            m_contextual_menu->show();
        }

        m_picked_data = nullptr;

        request_render();
    }
    else if(_button == right && m_edit_mode != edit_mode::display)
    {
        disable_edit_mode();
        disable_move_mode();
    }
}

//------------------------------------------------------------------------------

void point::button_double_press_event(mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y)
{
    render_service()->make_current();

    m_contextual_menu->hide();

    std::shared_ptr<ogre_fiducial> picked_data = try_pick(_x, _y, false);

    if(picked_data != nullptr)
    {
        layer()->cancel_further_interaction();

        // Check if something is picked to update the position of the distance.
        const auto picked_pos = sight::viz::scene3d::utils::pick_object(
            _x,
            _y,
            m_query_mask,
            *get_scene_manager(),
            false
        );

        if(picked_pos)
        {
            // Send signal with world coordinates of the fiducial.
            const auto& point = picked_data->m_node->getPosition();
            signal<signals::send_world_coordinates_t>(signals::SEND_WORLD_COORD)->async_emit(point.x, point.y, point.z);
        }
    }
}

//------------------------------------------------------------------------------

void point::wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    render_service()->make_current();

    if(m_contextual_menu->isVisible())
    {
        m_contextual_menu->hide();
    }
}

//------------------------------------------------------------------------------

void point::key_press_event(int _key, modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/)
{
    render_service()->make_current();

    if(m_edit_mode != edit_mode::display && _key == Qt::Key_Escape)
    {
        disable_edit_mode();
        disable_move_mode();
    }
}

//------------------------------------------------------------------------------

std::shared_ptr<point::ogre_fiducial> point::try_pick(int _x, int _y, bool _for_modification) const
{
    const auto layer = this->layer();

    Ogre::SceneManager* const scene_mgr = layer->get_scene_manager();

    const Ogre::Camera* const cam = layer->get_default_camera();
    const auto* const vp          = cam->getViewport();

    const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    const Ogre::Ray ray = cam->getCameraToViewportRay(vp_x, vp_y);

    const auto ray_scene_query = std::unique_ptr<Ogre::RaySceneQuery>(
        scene_mgr->createRayQuery(ray, m_fiducials_query_flag)
    );

    ray_scene_query->setSortByDistance(true);
    const auto& ray_results = ray_scene_query->execute();

    // The plane is always intercepted, if a fiducial is found
    for(const auto& ray_result : ray_results)
    {
        // The fiducial should be the nearest object found.
        for(const auto& ogre_fiducial : m_ogre_fiducials)
        {
            if(!ogre_fiducial)
            {
                continue;
            }

            if(ogre_fiducial->m_manual_object != ray_result.movable)
            {
                continue;
            }

            if(!ogre_fiducial->m_manual_object->isVisible())
            {
                continue;
            }

            if(!_for_modification
               || !m_can_only_modify_current
               || ogre_fiducial->m_group_name == m_current_group
               || m_movable_groups.contains(ogre_fiducial->m_group_name))
            {
                return ogre_fiducial;
            }
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

void point::set_cursor(const std::optional<Qt::CursorShape>& _cursor)
{
    auto interactor = layer()->render_service()->get_interactor_manager();
    if(auto qt_interactor = std::dynamic_pointer_cast<window_interactor>(interactor); qt_interactor)
    {
        // Do not change cursor for nothing to avoid sending QEvent::CursorChange
        if(auto* qt_widget = qt_interactor->get_qt_widget(); qt_widget)
        {
            if(_cursor)
            {
                if(qt_widget->cursor().shape() != *_cursor)
                {
                    qt_widget->setCursor(*_cursor);
                }
            }
            else
            {
                qt_widget->unsetCursor();
            }
        }
    }
}

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials.
