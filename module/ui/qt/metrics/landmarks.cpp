/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "module/ui/qt/metrics/landmarks.hpp"

#include <data/fiducials_series.hpp>
#include <data/image_series.hpp>

#include <ui/qt/container/widget.hpp>

#include <QColorDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <cmath>
#include <random>

namespace sight::module::ui::qt::metrics
{

namespace
{

using point_container_t = std::vector<sight::vec3d_t>;
using size_t            = float;

enum class shape : std::uint8_t
{
    sphere,
    cube
};

struct fiducials_group
{
    fiducials_group(sight::vec4f_t _color, size_t _size, shape _shape, bool _visibility) :
        m_color(_color),
        m_size(_size),
        m_shape(_shape),
        m_visibility(_visibility)
    {
    }

    sight::vec4f_t m_color;
    size_t m_size;
    shape m_shape;
    bool m_visibility;
    point_container_t m_points;
};

} // namespace

/**
 * Get a fiducial set as a structure
 * @param _group_name The name of the group to fetch
 * @return The fiducial set as a structure
 */
[[nodiscard]] static std::optional<fiducials_group> get_group(
    const data::image_series& _image_series,
    const std::string& _group_name
)
{
    const auto fiducials    = _image_series.get_fiducials();
    const auto fiducial_set = fiducials->get_fiducial_set_and_index(_group_name);
    if(!fiducial_set.has_value())
    {
        return std::nullopt;
    }

    sight::vec4f_t color = fiducial_set->first.color.value_or(std::array {1.F, 1.F, 1.F, 1.F});
    auto size            = fiducial_set->first.size.value_or(10);

    shape shape = shape::sphere;

    switch(fiducial_set->first.shape.value_or(data::fiducials_series::private_shape::sphere))
    {
        case data::fiducials_series::private_shape::sphere:
            shape = shape::sphere;
            break;

        case data::fiducials_series::private_shape::cube:
            shape = shape::cube;

        default:
            break;
    }

    bool visibility = fiducial_set->first.visibility.value_or(true);

    fiducials_group group(color, size, shape, visibility);

    auto query_results = fiducials->query_fiducials(
        std::nullopt,
        sight::data::fiducials_series::shape::point,
        _group_name
    );

    for(auto q : query_results)
    {
        if(!q.m_contour_data.has_value())
        {
            group.m_points.push_back(
                {{q.m_contour_data.value()[0], q.m_contour_data.value()[1], q.m_contour_data.value()[2]
                }
                });
        }
    }

    return group;
}

//------------------------------------------------------------------------------

static const char* s_group_property_name = "group";
static const int GROUP_NAME_ROLE         = Qt::UserRole + 1;

static const std::string SIZE_CONFIG     = "size";
static const std::string OPACITY_CONFIG  = "opacity";
static const std::string ADVANCED_CONFIG = "advanced";
static const std::string TEXT_CONFIG     = "text";

//------------------------------------------------------------------------------

landmarks::landmarks() noexcept
{
    new_slot(slots::ADD_POINT, &landmarks::add_point, this);
    new_slot(slots::MODIFY_POINT, &landmarks::modify_point, this);
    new_slot(slots::SELECT_POINT, &landmarks::select_point, this);
    new_slot(slots::DESELECT_POINT, &landmarks::deselect_point, this);
    new_slot(slots::ADD_GROUP, &landmarks::add_group, this);
    new_slot(slots::REMOVE_POINT, &landmarks::remove_point, this);
    new_slot(slots::REMOVE_GROUP, &landmarks::remove_group, this);
    new_slot(slots::MODIFY_GROUP, &landmarks::modify_group, this);
    new_slot(slots::RENAME_GROUP, &landmarks::rename_group, this);

    new_signal<signals::world_coordinates_t>(signals::SEND_WORLD_COORD);
    new_signal<signals::group_selected_t>(signals::GROUP_SELECTED);
}

//------------------------------------------------------------------------------

void landmarks::configuring()
{
    this->sight::ui::service::initialize();

    const service::config_t config = this->get_config();

    m_default_landmark_size = config.get<float>(SIZE_CONFIG, m_default_landmark_size);
    SIGHT_FATAL_IF(
        "'size' value must be a positive number greater than 0 (current value: " << m_default_landmark_size << ")",
        m_default_landmark_size <= 0.F
    );

    m_default_landmark_opacity = config.get<float>(OPACITY_CONFIG, m_default_landmark_opacity);
    SIGHT_FATAL_IF(
        "'opacity' value must be a number between 0.0 and 1.0 (current value: " << m_default_landmark_opacity << ")",
        m_default_landmark_opacity<0.F || m_default_landmark_opacity>1.F
    );

    m_advanced_mode = config.get<bool>(ADVANCED_CONFIG, false);

    m_text = config.get<std::string>(TEXT_CONFIG, m_text);
}

//------------------------------------------------------------------------------

void landmarks::starting()
{
    this->sight::ui::service::create();

    const QString service_id = QString::fromStdString(base_id());

    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    qt_container->get_qt_container()->setObjectName(service_id);

    auto* const layout      = new QVBoxLayout();
    auto* const grid_layout = new QGridLayout();

    m_visibility_checkbox = new QCheckBox();
    auto* const visibility_label = new QLabel(QString("Visibility"));
    m_visibility_checkbox->setObjectName(service_id + "/" + visibility_label->text());
    m_size_slider = new QSlider(Qt::Horizontal);
    m_size_slider->setMinimum(1);
    m_size_slider->setMaximum(100);
    auto* const size_label = new QLabel(QString("Size"));
    m_size_slider->setObjectName(service_id + "/" + size_label->text());
    m_opacity_slider = new QSlider(Qt::Horizontal);
    auto* const opacity_label = new QLabel("Opacity");
    m_opacity_slider->setObjectName(service_id + "/" + opacity_label->text());
    m_shape_selector = new QComboBox();
    m_shape_selector->addItem(QString("Cube"));
    m_shape_selector->addItem(QString("Sphere"));
    auto* const shape_label = new QLabel("Shape");
    m_shape_selector->setObjectName(service_id + "/" + shape_label->text());

    if(m_advanced_mode)
    {
        m_new_group_button = new QPushButton("New Group");
        m_new_group_button->setObjectName(service_id + "/" + m_new_group_button->text());
    }

    m_remove_button = new QPushButton("Delete");
    m_remove_button->setObjectName(service_id + "/" + m_remove_button->text());
    m_remove_button->setShortcut(QKeySequence::Delete);

    grid_layout->addWidget(visibility_label, 0, 0);
    grid_layout->addWidget(m_visibility_checkbox, 0, 1);
    grid_layout->addWidget(size_label, 1, 0);
    grid_layout->addWidget(m_size_slider, 1, 1);
    grid_layout->addWidget(opacity_label, 2, 0);
    grid_layout->addWidget(m_opacity_slider, 2, 1);
    grid_layout->addWidget(shape_label, 3, 0);
    grid_layout->addWidget(m_shape_selector, 3, 1);
    grid_layout->addWidget(m_remove_button, 4, 1);

    m_group_editor_widget = new QWidget();
    m_group_editor_widget->setLayout(grid_layout);

    m_tree_widget = new QTreeWidget();
    m_tree_widget->setObjectName(service_id + "/treeWidget");
    if(!m_text.empty())
    {
        auto* helper_text_label = new QLabel(QString::fromStdString(m_text));
        layout->addWidget(helper_text_label);
    }

    if(m_advanced_mode)
    {
        layout->addWidget(m_new_group_button);
    }

    layout->addWidget(m_tree_widget);
    layout->addWidget(m_group_editor_widget);
    m_group_editor_widget->setDisabled(true);

    QStringList headers;
    headers << "Group" << "Color";

    if(m_advanced_mode)
    {
        // Add empty third column to display and edit point coordinates.
        headers << "";
    }

    m_tree_widget->setHeaderLabels(headers);

    qt_container->set_layout(layout);

    this->updating();

    SIGHT_ASSERT("'imageSeries' parameter must be set.", m_image_series.const_lock() != nullptr);
    if(auto image_series = m_image_series.lock())
    {
        image_series->get_fiducials()->set_group_names_for_point_fiducials();
    }
}

//------------------------------------------------------------------------------

service::connections_t landmarks::auto_connections() const
{
    connections_t connections;

    connections.push(IMAGE_SERIES_INOUT, data::signals::MODIFIED, service::slots::UPDATE);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_ADDED, slots::ADD_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_MODIFIED, slots::MODIFY_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_SELECTED, slots::SELECT_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_DESELECTED, slots::DESELECT_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_ADDED, slots::ADD_GROUP);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_REMOVED, slots::REMOVE_GROUP);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::POINT_REMOVED, slots::REMOVE_POINT);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_MODIFIED, slots::MODIFY_GROUP);
    connections.push(IMAGE_SERIES_INOUT, data::has_fiducials::signals::GROUP_RENAMED, slots::RENAME_GROUP);

    return connections;
}

//------------------------------------------------------------------------------

void landmarks::updating()
{
    m_tree_widget->blockSignals(true);

    {
        const auto image_series = m_image_series.const_lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        m_tree_widget->clear();

        for(const auto& name : image_series->get_fiducials()->get_point_fiducials_group_names())
        {
            this->add_group(name);
            this->add_point(name);
        }
    }

    QObject::connect(m_tree_widget.data(), &QTreeWidget::itemChanged, this, &landmarks::on_group_name_edited);
    QObject::connect(m_tree_widget.data(), &QTreeWidget::currentItemChanged, this, &landmarks::on_selection_changed);
    QObject::connect(
        m_tree_widget.data(),
        &QTreeWidget::itemDoubleClicked,
        this,
        &landmarks::on_landmark_double_clicked
    );
    QObject::connect(m_size_slider.data(), &QSlider::valueChanged, this, &landmarks::on_size_changed);
    QObject::connect(m_opacity_slider.data(), &QSlider::valueChanged, this, &landmarks::on_opacity_changed);
    QObject::connect(m_visibility_checkbox.data(), &QCheckBox::stateChanged, this, &landmarks::on_visibility_changed);
    QObject::connect(m_shape_selector.data(), &QComboBox::currentTextChanged, this, &landmarks::on_shape_changed);
    QObject::connect(m_remove_button.data(), &QPushButton::clicked, this, &landmarks::on_remove_selection);
    QObject::connect(m_new_group_button.data(), &QPushButton::clicked, this, &landmarks::on_add_new_group);

    m_tree_widget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void landmarks::on_color_button()
{
    QObject* const sender = this->sender();

    // Create Color choice dialog.
    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QWidget* const container = qt_container->get_qt_container();
    SIGHT_ASSERT("container not instanced", container);

    const auto old_color  = sender->property("color").value<QColor>();
    const QColor color_qt =
        QColorDialog::getColor(old_color, container, "Select Color", QColorDialog::ShowAlphaChannel);
    if(color_qt.isValid())
    {
        auto* const color_button = dynamic_cast<QPushButton*>(sender);
        color_button->setProperty("color", color_qt);

        set_color_button_icon(color_button, color_qt);

        const std::string group_name = color_button->property(s_group_property_name).value<QString>().toStdString();

        sight::vec4f_t color = {
            static_cast<float>(color_qt.red()) / 255.F, static_cast<float>(color_qt.green()) / 255.F,
            static_cast<float>(color_qt.blue()) / 255.F, static_cast<float>(color_qt.alpha()) / 255.F
        };

        {
            auto image_series = m_image_series.lock();
            SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

            auto fiducial_set = image_series->get_fiducials()->get_fiducial_set_and_index(group_name);
            if(!fiducial_set.has_value())
            {
                return;
            }

            image_series->get_fiducials()->set_color(fiducial_set->second, color);
            image_series->async_emit(this, data::has_fiducials::signals::GROUP_MODIFIED, group_name);
        }
        m_opacity_slider->setValue(static_cast<int>(color[3] * static_cast<float>(m_opacity_slider->maximum())));
    }
}

//------------------------------------------------------------------------------

void landmarks::on_group_name_edited(QTreeWidgetItem* _item, int _column)
{
    SIGHT_ERROR_IF("A column different from the group's name is being edited", _column != 0);
    m_tree_widget->blockSignals(true);

    if(_column == 0)
    {
        const QString old_group_name = _item->data(0, GROUP_NAME_ROLE).toString();
        const QString new_group_name = _item->text(0);

        if(new_group_name.isEmpty())
        {
            const QString msg = "The new group name for '" + old_group_name
                                + "' is empty. Please enter a valid name and try again";
            QMessageBox msg_box(QMessageBox::Warning, "No group name", msg, QMessageBox::Ok);
            msg_box.exec();

            _item->setText(0, old_group_name);
        }
        else if(old_group_name != new_group_name)
        {
            try
            {
                {
                    auto image_series = m_image_series.lock();
                    SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

                    const auto group_names = image_series->get_fiducials()->get_point_fiducials_group_names();

                    std::optional<std::pair<data::fiducials_series::fiducial_set, std::size_t> > fiducial_set =
                        image_series->get_fiducials()->get_fiducial_set_and_index(
                            old_group_name.toStdString()
                        );
                    if(!fiducial_set.has_value())
                    {
                        throw data::exception("'" + old_group_name.toStdString() + "' group doesn't exist");
                    }

                    if(std::ranges::find(group_names, new_group_name.toStdString()) != group_names.end())
                    {
                        throw data::exception("'" + new_group_name.toStdString() + "' group already exists");
                    }

                    image_series->get_fiducials()->set_group_name(
                        fiducial_set->second,
                        new_group_name.toStdString()
                    );

                    image_series->async_emit(
                        this,
                        data::has_fiducials::signals::GROUP_RENAMED,
                        old_group_name.toStdString(),
                        new_group_name.toStdString()
                    );
                }

                _item->setData(0, GROUP_NAME_ROLE, new_group_name);
                QWidget* const widget = m_tree_widget->itemWidget(_item, 1);

                widget->setProperty(s_group_property_name, new_group_name);
            }
            catch(data::exception& e)
            {
                const QString msg = "Can't rename '" + old_group_name + "' as '" + new_group_name + ".\n"
                                    + QString(e.what());
                QMessageBox msg_box(QMessageBox::Warning, "Can't rename" + old_group_name, msg, QMessageBox::Ok);
                msg_box.exec();

                _item->setText(0, old_group_name);
            }
        }
    }

    m_tree_widget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::on_selection_changed(QTreeWidgetItem* _current, QTreeWidgetItem* _previous)
{
    if(_previous != nullptr)
    {
        const auto image_series = m_image_series.const_lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        if(m_advanced_mode)
        {
            const QTreeWidgetItem* const previous_parent = _previous->parent();

            if(previous_parent != nullptr)
            {
                const std::string& group_name = previous_parent->text(0).toStdString();

                const auto index = static_cast<std::size_t>(previous_parent->indexOfChild(_previous));

                SIGHT_ASSERT(
                    "index must be inferior to the number of points in '" + group_name + "'.",
                    index < image_series->get_fiducials()->get_number_of_points_in_group(group_name)
                );

                image_series->async_emit(this, data::has_fiducials::signals::POINT_DESELECTED, group_name, index);
            }
        }
        else
        {
            const std::string& group_name = _previous->text(0).toStdString();

            image_series->async_emit(this, data::has_fiducials::signals::POINT_DESELECTED, group_name, 0);
        }
    }

    if(_current != nullptr)
    {
        int size     = 0;
        bool visible = false;
        QString shape_text;
        float opacity = NAN;
        {
            const auto image_series = m_image_series.const_lock();
            SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

            std::string group_name;

            if(m_advanced_mode)
            {
                const QTreeWidgetItem* const current_parent = _current->parent();

                if(current_parent != nullptr)
                {
                    group_name = current_parent->text(0).toStdString();

                    const auto index = static_cast<std::size_t>(current_parent->indexOfChild(_current));

                    SIGHT_ASSERT(
                        "index must be inferior to the number of points in '" + group_name + "'.",
                        index < image_series->get_fiducials()->get_number_of_points_in_group(group_name)
                    );

                    image_series->async_emit(this, data::has_fiducials::signals::POINT_SELECTED, group_name, index);
                }
                else
                {
                    group_name = _current->text(0).toStdString();
                }
            }
            else
            {
                group_name = _current->text(0).toStdString();

                image_series->async_emit(this, data::has_fiducials::signals::POINT_SELECTED, group_name, 0);
            }

            std::optional<fiducials_group> group = get_group(*image_series, group_name);
            if(!group.has_value())
            {
                return;
            }

            size       = static_cast<int>(group->m_size);
            visible    = group->m_visibility;
            shape_text = group->m_shape == shape::cube ? "Cube" : "Sphere";
            opacity    = group->m_color[3];

            async_emit(signals::GROUP_SELECTED, group_name);
        }

        // Set widget values
        m_size_slider->setValue(size);
        m_visibility_checkbox->setChecked(visible);
        m_shape_selector->setCurrentText(shape_text);
        m_opacity_slider->setValue(static_cast<int>(opacity * static_cast<float>(m_opacity_slider->maximum())));
    }
    else
    {
        async_emit(signals::GROUP_SELECTED, "");
    }

    m_group_editor_widget->setDisabled(_current == nullptr);
}

//------------------------------------------------------------------------------

void landmarks::on_landmark_double_clicked(QTreeWidgetItem* _item, int /*unused*/) const
{
    if(_item != nullptr)
    {
        // Exclude top level item
        if(_item->childCount() != 0)
        {
            return;
        }

        std::array<QString, 3> index {
            _item->text(0),
            _item->text(1),
            _item->text(2)
        };

        if(index[0].isEmpty() || index[1].isEmpty() || index[2].isEmpty())
        {
            // Do nothing if a value is missing.
            return;
        }

        // Convert to double
        std::array check = {false, false, false};
        sight::vec3d_t world_coord {
            index[0].toDouble(check.data()),
            index[1].toDouble(&check[1]),
            index[2].toDouble(&check[2])
        };

        // Check that conversion to double performed well.
        if(!check[0] || !check[1] || !check[2])
        {
            SIGHT_ERROR(
                "Cannot convert landmark position to double ("
                + index[0].toStdString()
                + ", " + index[1].toStdString()
                + ", " + index[2].toStdString() + ")"
            );
            return;
        }

        update_current_landmark(world_coord);
    }
}

//------------------------------------------------------------------------------

void landmarks::update_current_landmark(sight::vec3d_t& _world_coord) const
{
    // Send signal with world coordinates of the landmarks

    SIGHT_DEBUG(
        " Send world coordinates [" << _world_coord[0] << ", " << _world_coord[1]
        << ", " << _world_coord[2] << " ]"
    );
    this->async_emit(
        signals::SEND_WORLD_COORD,
        _world_coord[0],
        _world_coord[1],
        _world_coord[2]
    );

    //update the data with the current point
    auto current_landmark = m_current_landmark.lock();
    if(current_landmark)
    {
        *current_landmark = _world_coord;
        current_landmark->async_emit(sight::data::signals::MODIFIED);
    }
}

//------------------------------------------------------------------------------

void landmarks::on_size_changed(int _new_size)
{
    const auto real_size = static_cast<size_t>(_new_size);

    std::string group_name;
    if(current_selection(group_name))
    {
        auto image_series = m_image_series.lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        std::optional<std::pair<data::fiducials_series::fiducial_set,
                                std::size_t> > fiducial_set =
            image_series->get_fiducials()->get_fiducial_set_and_index(group_name);
        if(!fiducial_set.has_value())
        {
            SIGHT_WARN("Couldn't change size of size of group '" << group_name << "', it doesn't exist");
            return;
        }

        image_series->get_fiducials()->set_size(fiducial_set->second, real_size);

        image_series->async_emit(this, data::has_fiducials::signals::GROUP_MODIFIED, group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::on_opacity_changed(int _new_opacity)
{
    const auto slider_size = static_cast<float>(m_opacity_slider->maximum() - m_opacity_slider->minimum());

    const float real_opacity = static_cast<float>(_new_opacity) / slider_size;

    std::string group_name;
    if(current_selection(group_name))
    {
        auto image_series = m_image_series.lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        auto fs = image_series->get_fiducials()->get_fiducial_set_and_index(group_name);
        if(!fs.has_value())
        {
            SIGHT_WARN("Couldn't modify the color of group '" << group_name << "', it doesn't exist");
            return;
        }

        std::optional<sight::vec4f_t> color = image_series->get_fiducials()->get_color(fs->second);
        SIGHT_ASSERT("The group must have a color", color.has_value());
        (*color)[3] = real_opacity;
        image_series->get_fiducials()->set_color(fs->second, *color);

        QTreeWidgetItem* const item = get_group_item(group_name);
        auto* const color_button    = dynamic_cast<QPushButton*>(m_tree_widget->itemWidget(item, 1));

        auto current_color = color_button->property("color").value<QColor>();
        current_color.setAlphaF(real_opacity);
        color_button->setProperty("color", current_color);

        set_color_button_icon(color_button, current_color);

        image_series->async_emit(this, data::has_fiducials::signals::GROUP_MODIFIED, group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::on_visibility_changed(int _visibility)
{
    std::string group_name;
    if(current_selection(group_name))
    {
        auto image_series = m_image_series.lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        auto fiducial_set = image_series->get_fiducials()->get_fiducial_set_and_index(group_name);
        if(!fiducial_set.has_value())
        {
            SIGHT_WARN("Couldn't change visibility of group '" << group_name << "', it doesn't exist");
            return;
        }

        image_series->get_fiducials()->set_visibility(fiducial_set->second, static_cast<bool>(_visibility));

        image_series->async_emit(this, data::has_fiducials::signals::GROUP_MODIFIED, group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::on_shape_changed(const QString& _shape)
{
    std::string group_name;
    if(current_selection(group_name))
    {
        SIGHT_ASSERT("Shape must be 'Cube' or 'Sphere'.", _shape == "Cube" || _shape == "Sphere");
        const shape s =
            (_shape == "Cube") ? shape::cube : shape::sphere;

        auto image_series = m_image_series.lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        auto fiducial_set = image_series->get_fiducials()->get_fiducial_set_and_index(group_name);
        if(!fiducial_set.has_value())
        {
            SIGHT_WARN("Couldn't change shape of group '" << group_name << "', it doesn't exist");
            return;
        }

        image_series->get_fiducials()->set_shape(
            fiducial_set->second,
            s == shape::cube ? data::fiducials_series::private_shape::cube
                             : data::fiducials_series::private_shape::sphere
        );

        image_series->async_emit(this, data::has_fiducials::signals::GROUP_MODIFIED, group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::on_add_new_group()
{
    const std::string group_name = this->generate_new_group_name();

    auto image_series = m_image_series.lock();
    SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

    if(image_series != nullptr)
    {
        image_series->get_fiducials()->add_group(
            group_name,
            this->generate_new_color(),
            m_default_landmark_size
        );
        image_series->async_emit(data::has_fiducials::signals::GROUP_ADDED, group_name);
    }
}

//------------------------------------------------------------------------------

void landmarks::on_remove_selection()
{
    m_tree_widget->blockSignals(true);

    QTreeWidgetItem* const item = m_tree_widget->currentItem();

    if(item != nullptr)
    {
        const int top_level_index = m_tree_widget->indexOfTopLevelItem(item);

        auto image_series = m_image_series.lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        if(m_advanced_mode && top_level_index == -1) // Delete point
        {
            QTreeWidgetItem* const item_parent = item->parent();

            const auto index              = static_cast<std::size_t>(item_parent->indexOfChild(item));
            const std::string& group_name = item_parent->text(0).toStdString();

            image_series->get_fiducials()->remove_point(group_name, index);

            item_parent->removeChild(item);

            image_series->async_emit(this, data::has_fiducials::signals::POINT_REMOVED, group_name, index);
        }
        else
        {
            const std::string& group_name = item->text(0).toStdString();

            image_series->get_fiducials()->remove_group(group_name);

            delete m_tree_widget->takeTopLevelItem(top_level_index);

            image_series->async_emit(this, data::has_fiducials::signals::GROUP_REMOVED, group_name);
        }

        m_tree_widget->setCurrentItem(nullptr);
        m_group_editor_widget->setDisabled(true);
    }

    m_tree_widget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::add_point(std::string _group_name) const
{
    if(m_advanced_mode)
    {
        m_tree_widget->blockSignals(true);

        const auto image_series = m_image_series.const_lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        QTreeWidgetItem* const item = get_group_item(_group_name);

        const auto nb_childs = static_cast<std::size_t>(item->childCount());
        auto maybe_nb_points = image_series->get_fiducials()->get_number_of_points_in_group(_group_name);
        if(!maybe_nb_points.has_value())
        {
            return;
        }

        std::size_t nb_points = *maybe_nb_points;
        for(std::size_t idx = nb_childs ; idx < nb_points ; ++idx)
        {
            std::optional<sight::vec3d_t> maybe_new_point = image_series->get_fiducials()->get_point(_group_name, idx);
            if(!maybe_new_point.has_value())
            {
                continue;
            }

            sight::vec3d_t new_point = *maybe_new_point;

            auto* const pt = new QTreeWidgetItem();
            for(int i = 0 ; i < 3 ; ++i)
            {
                pt->setText(i, QString::fromStdString(std::to_string(new_point[static_cast<std::size_t>(i)])));
            }

            item->addChild(pt);
        }

        m_tree_widget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void landmarks::add_group(std::string _name) const
{
    QColor color;
    {
        const auto image_series = m_image_series.const_lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        std::optional<fiducials_group> group = get_group(*image_series, _name);
        if(!group.has_value())
        {
            return;
        }

        color = convert_to_q_color(group->m_color);
    }

    auto* const item = new QTreeWidgetItem();
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setData(0, GROUP_NAME_ROLE, QString::fromStdString(_name));

    m_tree_widget->addTopLevelItem(item);

    item->setText(0, QString::fromStdString(_name));
    auto* const button = new QPushButton();

    set_color_button_icon(button, color);
    button->setProperty("color", color);
    button->setProperty(s_group_property_name, QString::fromStdString(_name));

    QObject::connect(button, &QPushButton::clicked, this, &landmarks::on_color_button);

    m_tree_widget->setItemWidget(item, 1, button);
}

//------------------------------------------------------------------------------

void landmarks::remove_group(std::string _name) const
{
    try
    {
        QTreeWidgetItem* const item = get_group_item(_name);

        while(item->childCount() != 0)
        {
            QTreeWidgetItem* const child = item->child(0);
            item->removeChild(child);
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

//------------------------------------------------------------------------------

void landmarks::remove_point(std::string _group_name, std::size_t _index) const
{
    m_tree_widget->blockSignals(true);

    QTreeWidgetItem* const item = get_group_item(_group_name);

    SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

    QTreeWidgetItem* const point_item = item->child(static_cast<int>(_index));
    item->removeChild(point_item);
    m_tree_widget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::rename_group(std::string _old_name, std::string _new_name) const
{
    m_tree_widget->blockSignals(true);

    const QString qt_new_name   = QString::fromStdString(_new_name);
    QTreeWidgetItem* const item = get_group_item(_old_name);
    item->setData(0, GROUP_NAME_ROLE, qt_new_name);
    QWidget* const widget = m_tree_widget->itemWidget(item, 1);
    widget->setProperty(s_group_property_name, qt_new_name);

    item->setText(0, qt_new_name);

    m_tree_widget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::modify_group(std::string _name) const
{
    QTreeWidgetItem* const item = get_group_item(_name);

    item->setText(0, _name.c_str());

    const auto image_series = m_image_series.const_lock();
    SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

    std::optional<fiducials_group> maybe_group = get_group(*image_series, _name);
    if(!maybe_group.has_value())
    {
        return;
    }

    const fiducials_group& group = *maybe_group;

    auto* const color_button = dynamic_cast<QPushButton*>(m_tree_widget->itemWidget(item, 1));

    const QColor color = convert_to_q_color(group.m_color);

    set_color_button_icon(color_button, color);

    bool group_selected = item->isSelected();

    if(m_advanced_mode) // Check if a child is selected.
    {
        for(int i = 0 ; i < item->childCount() && !group_selected ; ++i)
        {
            group_selected = item->child(i)->isSelected();
        }
    }

    if(group_selected)
    {
        // Set widget values
        m_size_slider->setValue(static_cast<int>(group.m_size));
        m_visibility_checkbox->setChecked(group.m_visibility);

        const QString shape_text = group.m_shape == shape::cube ? "Cube" : "Sphere";
        m_shape_selector->setCurrentText(shape_text);

        const float opacity = group.m_color[3];
        m_opacity_slider->setValue(static_cast<int>(opacity * static_cast<float>(m_opacity_slider->maximum())));
    }
}

//------------------------------------------------------------------------------

void landmarks::modify_point(std::string _group_name, std::size_t _index) const
{
    if(m_advanced_mode)
    {
        auto const item_list = m_tree_widget->findItems(QString::fromStdString(_group_name), Qt::MatchExactly);

        SIGHT_ASSERT("Only a single item can be named '" + _group_name + "'", item_list.size() == 1);

        QTreeWidgetItem* const item = item_list.at(0);

        SIGHT_ASSERT("Index must be less than " << item->childCount(), static_cast<int>(_index) < item->childCount());

        QTreeWidgetItem* const point_item = item->child(static_cast<int>(_index));

        const auto image_series = m_image_series.const_lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        std::optional<sight::vec3d_t> maybe_point = image_series->get_fiducials()->get_point(_group_name, _index);
        if(!maybe_point.has_value())
        {
            return;
        }

        const sight::vec3d_t point = *maybe_point;

        m_tree_widget->blockSignals(true);
        for(int i = 0 ; i < 3 ; ++i)
        {
            point_item->setText(i, QString("%1").arg(point[static_cast<std::size_t>(i)]));
        }

        m_tree_widget->blockSignals(false);
    }
}

//------------------------------------------------------------------------------

void landmarks::select_point(std::string _group_name, std::size_t _index) const
{
    m_tree_widget->blockSignals(true);

    QTreeWidgetItem* current_item = get_group_item(_group_name);

    if(m_advanced_mode)
    {
        SIGHT_ASSERT(
            "Index must be less than " << current_item->childCount(),
            static_cast<int>(_index) < current_item->childCount()
        );

        current_item = current_item->child(static_cast<int>(_index));
    }

    m_tree_widget->setCurrentItem(current_item);

    int size     = 0;
    bool visible = false;
    QString shape_text;
    float opacity = NAN;
    {
        const auto image_series = m_image_series.const_lock();
        SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

        std::optional<fiducials_group> maybe_group = get_group(*image_series, _group_name);
        if(!maybe_group.has_value())
        {
            return;
        }

        const fiducials_group& group = *maybe_group;

        size       = static_cast<int>(group.m_size);
        visible    = group.m_visibility;
        shape_text = group.m_shape == shape::cube ? "Cube" : "Sphere";
        opacity    = group.m_color[3];
    }

    // Set widget values
    m_size_slider->setValue(size);
    m_visibility_checkbox->setChecked(visible);
    m_shape_selector->setCurrentText(shape_text);
    m_opacity_slider->setValue(static_cast<int>(opacity * static_cast<float>(m_opacity_slider->maximum())));

    m_group_editor_widget->setEnabled(true);
    m_tree_widget->blockSignals(false);
}

//------------------------------------------------------------------------------

void landmarks::deselect_point(std::string /*unused*/, std::size_t /*unused*/) const
{
    m_tree_widget->blockSignals(true);
    m_tree_widget->setCurrentItem(nullptr);
    m_group_editor_widget->setDisabled(true);
    m_tree_widget->blockSignals(false);
}

//------------------------------------------------------------------------------

std::string landmarks::generate_new_group_name() const
{
    std::size_t group_count = 0;

    const auto image_series = m_image_series.const_lock();
    SIGHT_ASSERT(std::quoted(IMAGE_SERIES_INOUT) << " does not exists.", image_series != nullptr);

    const auto group_names = image_series->get_fiducials()->get_point_fiducials_group_names();

    const std::string new_group_name_prefix = m_advanced_mode ? "Group_" : "Point_";

    while(std::ranges::find(
              group_names,

              new_group_name_prefix + std::to_string(group_count)
          ) != group_names.end())
    {
        ++group_count;
    }

    return new_group_name_prefix + std::to_string(group_count);
}

//------------------------------------------------------------------------------

sight::vec4f_t landmarks::generate_new_color()
{
    const sight::vec4f_t color = {
        m_distributor(m_generator),
        m_distributor(m_generator),
        m_distributor(m_generator),
        m_default_landmark_opacity
    };

    return color;
}

//------------------------------------------------------------------------------

bool landmarks::current_selection(std::string& _selection) const
{
    QTreeWidgetItem* item = m_tree_widget->currentItem();

    const bool selected_group = (item != nullptr);

    if(selected_group)
    {
        const int top_level_index = m_tree_widget->indexOfTopLevelItem(item);

        if(m_advanced_mode && top_level_index == -1)
        {
            item = item->parent();
        }

        _selection = item->text(0).toStdString();
    }

    return selected_group;
}

//------------------------------------------------------------------------------

QColor landmarks::convert_to_q_color(const sight::vec4f_t& _color)
{
    return {
        static_cast<int>(_color[0] * 255),
        static_cast<int>(_color[1] * 255),
        static_cast<int>(_color[2] * 255),
        static_cast<int>(_color[3] * 255)
    };
}

//------------------------------------------------------------------------------

void landmarks::set_color_button_icon(QPushButton* _button, const QColor& _color)
{
    const int icon_size = _button->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(icon_size, icon_size);
    pix.fill(_color);

    _button->setIcon(QIcon(pix));
}

//------------------------------------------------------------------------------

QTreeWidgetItem* landmarks::get_group_item(const std::string& _group_name) const
{
    const auto item_list = m_tree_widget->findItems(QString::fromStdString(_group_name), Qt::MatchExactly);

    SIGHT_ASSERT("Only a single item can be named '" + _group_name + "'", item_list.size() == 1);

    return item_list.at(0);
}

} // namespace sight::module::ui::qt::metrics
