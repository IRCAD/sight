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

#include "settings.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/object.hpp>
#include <core/runtime/path.hpp>

#include <data/boolean.hpp>
#include <data/color.hpp>
#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/map.hpp>
#include <data/real.hpp>
#include <data/string.hpp>
#include <data/tools/color.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/qt/container/widget.hpp>
#include <ui/qt/widget/non_linear_slider.hpp>
#include <ui/qt/widget/tickmarks_slider.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/join.hpp>
#include <boost/tokenizer.hpp>

#include <QAbstractButton>
#include <QApplication>
#include <QButtonGroup>
#include <QColorDialog>
#include <QEvent>
#include <QFormLayout>
#include <QGraphicsColorizeEffect>
#include <QGroupBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QString>
#include <QStyle>
#include <QToolButton>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

inline static void set_minimum_size(QWidget* _widget, const settings::param_widget& _params)
{
    if(_params.min_width.has_value())
    {
        _widget->setMinimumWidth(std::max(_widget->minimumWidth(), *_params.min_width));
    }

    if(_params.min_height.has_value())
    {
        _widget->setMinimumHeight(std::max(_widget->minimumHeight(), *_params.min_height));
    }
}

//-----------------------------------------------------------------------------

settings::settings() noexcept
{
    new_slot(slots::UPDATE_ENUM_RANGE_SLOT, &settings::update_enum_range, this);
    new_slot(slots::UPDATE_INT_MIN_PARAMETER_SLOT, &settings::update_int_min_parameter, this);
    new_slot(slots::UPDATE_INT_MAX_PARAMETER_SLOT, &settings::update_int_max_parameter, this);
    new_slot(slots::UPDATE_DOUBLE_MIN_PARAMETER_SLOT, &settings::update_double_min_parameter, this);
    new_slot(slots::UPDATE_DOUBLE_MAX_PARAMETER_SLOT, &settings::update_double_max_parameter, this);
}

//-----------------------------------------------------------------------------

void settings::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void settings::starting()
{
    this->block_signals(true);
    this->create();

    const std::set<std::string> is_text_widget {
        "text", "file", "dir", "file_read", "file_write", "dir_write", "dir_read"
    };

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(QString::fromStdString(base_id()));

    QScrollArea* scroll_area = nullptr;

    service::config_t config = this->get_config();

    const auto& ui_cfg    = config.get_child("ui");
    const bool scrollable = ui_cfg.get<bool>("<xmlattr>.scrollable", false);

    auto* layout = new QFormLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);
    layout->setFormAlignment(Qt::AlignCenter);
    layout->setLabelAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    if(scrollable)
    {
        scroll_area = new QScrollArea(qt_container->get_qt_container()->parentWidget());
        scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        auto* const viewport = new QWidget(qt_container->get_qt_container());
        viewport->setLayout(layout);
        scroll_area->setWidgetResizable(true);
        scroll_area->setWidget(viewport);
    }

    // We don't support having multiple properties with the same key (this isn't a good idea any way).
    // This set keeps tracks of the ones we add, and triggers and assert if it already exists.
    [[maybe_unused]] std::set<std::string> keys;

    const bool use_map = m_settings_map.const_lock() != nullptr;

    // Create widgets
    for(std::size_t data_index = 0 ; const auto& param : boost::make_iterator_range(ui_cfg.equal_range("item")))
    {
        const service::config_t& cfg = param.second;

        const auto orientation = cfg.get<std::string>("<xmlattr>.orientation", "horizontal") == "horizontal"
                                 ? Qt::Horizontal
                                 : Qt::Vertical;

        const auto param_box_direction = orientation
                                         == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

        param_widget widget
        {
            .name         = cfg.get<std::string>("<xmlattr>.name"),
            .reset_button = cfg.get<bool>("<xmlattr>.reset", true),
            .hide_min_max = cfg.get<bool>("<xmlattr>.hide_min_max", false),
            .min_width    = cfg.get_optional<int>("<xmlattr>.min_width"),
            .min_height   = cfg.get_optional<int>("<xmlattr>.min_height")
        };

        widget.data_index = data_index++;

        const std::string type =
            [&, this]()
            {
                sight::data::mt::locked_ptr<const sight::data::map> map_lock;
                sight::data::mt::locked_ptr<const sight::data::object> lock;
                sight::data::object::csptr obj;
                if(use_map)
                {
                    widget.key = cfg.get<std::string>("<xmlattr>.key");
                    map_lock   = m_settings_map.const_lock();
                    obj        = map_lock->at(widget.key);
                }
                else
                {
                    lock       = m_settings[widget.data_index].const_lock();
                    obj        = lock.get_shared();
                    widget.key = obj->base_id();
                }

                auto serializable_data = std::dynamic_pointer_cast<const sight::data::string_serializable>(obj);
                widget.default_value = serializable_data->to_string();

                SIGHT_ERROR_IF(
                    "No type should be defined for " << std::quoted(widget.key)
                    << " when passing a data object. It will be ignored.",
                    cfg.get_optional<std::string>("<xmlattr>.type").has_value()
                );
                return obj->get_classname();
            }();

        SIGHT_ASSERT(
            get_id() << ": Key " << std::quoted(widget.key) << " already exists.",
            keys.insert(widget.key).second
        );

        auto* const param_box = new QWidget;
        const auto qt_key     = QString::fromStdString(widget.key) + "_box";
        param_box->setProperty(qt_property::key, qt_key);
        param_box->setObjectName(qt_key);
        param_box->setContentsMargins(0, 0, 0, 0);

        if(orientation == Qt::Vertical)
        {
            param_box->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
        }
        else
        {
            param_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        }

        m_param_boxes.emplace_back(param_box);

        auto* const param_box_layout = new QBoxLayout {param_box_direction, param_box};

        auto* const row_layout = new QHBoxLayout;
        row_layout->setContentsMargins(0, 0, 0, 0);

        if(orientation == Qt::Vertical)
        {
            row_layout->addWidget(param_box, 0, Qt::AlignCenter);
        }
        else
        {
            row_layout->addWidget(param_box, 0, Qt::AlignVCenter);
        }

        // Label
        if(!widget.name.empty())
        {
            auto* const parameter_label = new QLabel(QString::fromStdString(widget.name));
            parameter_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
            parameter_label->setWordWrap(true);
            parameter_label->setAlignment(Qt::AlignCenter);

            // When horizontal, the label is added to the "main" widget layout, otherwise, at the top of the box
            if(orientation == Qt::Vertical)
            {
                param_box_layout->addWidget(parameter_label, 0, Qt::AlignCenter);
                layout->addRow(row_layout);
            }
            else
            {
                layout->addRow(parameter_label, row_layout);
            }
        }
        else
        {
            layout->addRow(row_layout);
        }

        // If we have a reset button, we need to add it to the row layout
        QPushButton* reset = nullptr;

        bool created = false; // Just needed for integer type

        // Widget who is able to be controlled by a joystick
        if(const auto& joystick = to_joystick(cfg.get<std::string>("<xmlattr>.joystick", ""));
           joystick != sight::io::joystick::joystick_t::unknown)
        {
            std::vector<std::string> axes;
            boost::split(
                axes,
                cfg.get<std::string>("<xmlattr>.joystick_axis", ""),
                boost::is_any_of(",; \t"),
                boost::token_compress_on
            );

            m_widget_joysticks.insert_or_assign(
                widget.key,
                widget_joystick {
                    .alias  = joystick,
                    .axis_1 = !axes.empty() ? to_axis(axes[0]) : sight::io::joystick::axis_t::unknown,
                    .axis_2 = axes.size() > 1 ? to_axis(axes[1]) : sight::io::joystick::axis_t::unknown,
                    .axis_3 = axes.size() > 2 ? to_axis(axes[2]) : sight::io::joystick::axis_t::unknown
                });
        }

        if(type == "sight::data::boolean")
        {
            reset = this->create_bool_widget(param_box_layout, widget, orientation);
        }
        else if(type == "sight::data::color")
        {
            reset = this->create_color_widget(param_box_layout, widget);
        }
        else if(type == "sight::data::real" || type == "sight::data::dvec2" || type == "sight::data::dvec3")
        {
            const std::string widget_type = cfg.get<std::string>("<xmlattr>.widget", "spin");

            const double min = cfg.get<double>("<xmlattr>.min", 0.);
            const double max = cfg.get<double>("<xmlattr>.max", 1.);

            const double_widget widget_double = {widget, min, max};

            const int count = (type == "sight::data::dvec3") ? 3 : (type == "sight::data::dvec2" ? 2 : 1);

            if(widget_type == "spin")
            {
                reset = this->create_double_spin_widget(param_box_layout, widget_double, count, orientation);
            }
            else if(widget_type == "slider")
            {
                // TODO: this could be supported now
                SIGHT_ASSERT(get_id() << ": Count > 1 is not supported with sliders", count == 1);

                const std::uint8_t decimals = cfg.get<std::uint8_t>("<xmlattr>.decimals", 2);
                const bool on_release       = cfg.get<bool>("<xmlattr>.emit_on_release", false);

                reset = this->create_double_slider_widget(
                    param_box_layout,
                    widget_double,
                    decimals,
                    orientation,
                    on_release
                );

                if(orientation == Qt::Vertical)
                {
                    param_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
                }
            }
            else
            {
                SIGHT_ERROR(
                    get_id() << ": Unknown widget type : " << std::quoted(widget_type) << " for "
                    << std::quoted(widget.name)
                    << ". Must be 'spin' or 'slider'."
                );
            }
        }
        else if(type == "sight::data::integer" || type == "sight::data::ivec2" || type == "sight::data::ivec3")
        {
            const auto widget_type = cfg.get<std::string>("<xmlattr>.widget");

            const int min               = cfg.get<int>("<xmlattr>.min", 0);
            const int max               = cfg.get<int>("<xmlattr>.max", 100);
            const int_widget widget_int = {widget, min, max};

            const int count = (type == "sight::data::ivec3") ? 3 : (type == "sight::data::ivec2" ? 2 : 1);

            if(widget_type == "spin")
            {
                reset   = this->create_integer_spin_widget(param_box_layout, widget_int, count, orientation);
                created = true;
            }
            else if(widget_type == "slider")
            {
                // TODO: this could be supported now
                SIGHT_ASSERT(get_id() << ": Count > 1 is not supported with sliders", count == 1);

                const bool on_release = cfg.get<bool>("<xmlattr>.emit_on_release", false);
                reset = this->create_integer_slider_widget(param_box_layout, widget_int, orientation, on_release);

                if(orientation == Qt::Vertical)
                {
                    param_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
                }

                created = true;
            }
            else
            {
                SIGHT_ERROR_IF(
                    get_id() << ": Unknown widget type : " << std::quoted(widget_type) << " for " << widget.name
                    << ". Must be 'spin' or 'slider'.",
                    type != "sight::data::integer"
                );
            }
        }

        if(!created and (type == "sight::data::integer" or type == "sight::data::string"))
        {
            const auto widget_type = cfg.get<std::string>("<xmlattr>.widget", "");

            if(widget_type == "combobox")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                // split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::settings::parse_enum_string(options, values, data);
                this->create_enum_combobox_widget(param_box_layout, widget, values, data);
            }
            else if(widget_type == "comboslider")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                // split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::settings::parse_enum_string(options, values, data);
                const bool on_release = cfg.get<bool>("<xmlattr>.emit_on_release", false);

                this->create_enum_slider_widget(param_box_layout, widget, values, orientation, on_release);

                if(orientation == Qt::Vertical)
                {
                    param_box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
                }
            }
            else if(widget_type == "buttonBar")
            {
                const int h_offset      = cfg.get<int>("<xmlattr>.hOffset", 0);
                const int width         = cfg.get<int>("<xmlattr>.width", 0);
                const int height        = cfg.get<int>("<xmlattr>.height", 0);
                const std::string style = cfg.get<std::string>("<xmlattr>.style", "iconOnly");

                const auto value_config = cfg.equal_range("item");
                std::vector<enum_button_param> button_list;
                for(auto value_config_it = value_config.first ; value_config_it != value_config.second ;
                    ++value_config_it)
                {
                    const auto value              = value_config_it->second.get<std::string>("<xmlattr>.value");
                    const auto label              = value_config_it->second.get<std::string>("<xmlattr>.label", "");
                    const auto icon_path_relative = value_config_it->second.get<std::string>("<xmlattr>.icon", "");

                    const std::string icon_path =
                        core::runtime::get_module_resource_file_path(icon_path_relative).generic_string();

                    button_list.push_back(enum_button_param({value, label, icon_path}));
                }

                this->create_enum_button_bar_widget(
                    param_box_layout,
                    widget,
                    button_list,
                    width,
                    height,
                    h_offset,
                    style,
                    orientation
                );
            }
            else if(widget_type == "tickmarks")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values", "");

                std::vector<std::string> labels;
                std::vector<std::string> data;
                sight::module::ui::qt::settings::parse_enum_string(options, labels, data);

                this->create_tickmarks_widget(param_box_layout, widget, labels);
            }
            else if(is_text_widget.contains(widget_type))
            {
                reset = this->create_text_widget(param_box_layout, widget, widget_type);
            }
            else
            {
                SIGHT_ERROR("Unknown widget type for key: " + widget.key);
            }
        }

        if(reset != nullptr)
        {
            // Looks better under the rest
            if(orientation == Qt::Vertical)
            {
                param_box_layout->addWidget(reset, /*stretch = */ 0, Qt::AlignCenter);
            }
            else
            {
                row_layout->addWidget(reset);
            }
        }
    }

    for(std::size_t data_index = 0 ; const auto& param : boost::make_iterator_range(ui_cfg.equal_range("item")))
    {
        const service::config_t& cfg = param.second;

        std::string key;

        sight::data::mt::locked_ptr<const sight::data::map> map_lock;
        sight::data::mt::locked_ptr<sight::data::object> lock;
        sight::data::object::sptr obj;
        if(use_map)
        {
            key      = cfg.get<std::string>("<xmlattr>.key");
            map_lock = m_settings_map.const_lock();
            obj      = map_lock->at(key);
        }
        else
        {
            lock = m_settings[data_index].lock();
            obj  = lock.get_shared();
            key  = obj->base_id();
        }

        const std::string depends       = cfg.get<std::string>("<xmlattr>.depends", "");
        const std::string depends_value = cfg.get<std::string>("<xmlattr>.depends_value", "");
        const bool depends_reverse      = cfg.get<bool>("<xmlattr>.depends_reverse", false);

        if(!depends.empty())
        {
            auto* const depends_widget = get_param_widget(depends);

            // Note: we disable the QGroupBox containing the widget**s** instead of individual ones, because
            // some parameter create several widgets (3 spinboxes, etc.)
            const auto widget_container_it = std::ranges::find_if(
                m_param_boxes,
                [qt_key = QString::fromStdString(key) + "_box"](const QWidget* const _w)
                {
                    return _w->objectName() == qt_key;
                });

            SIGHT_ASSERT(
                get_id() << ": unknown parameter " << std::quoted(key) << ".",
                widget_container_it != m_param_boxes.cend()
            );

            if(auto* const widget = qobject_cast<QWidget*>((*widget_container_it)))
            {
                widget->installEventFilter(this);
                auto* check_box = qobject_cast<QCheckBox*>(depends_widget);
                if(check_box != nullptr)
                {
                    QObject::connect(
                        check_box,
                        &QCheckBox::stateChanged,
                        this,
                        [ = ]{on_depends_changed(check_box, widget, depends_reverse);});
                    on_depends_changed(check_box, widget, depends_reverse);
                }
                else
                {
                    auto* combo_box = qobject_cast<QComboBox*>(depends_widget);
                    if(combo_box != nullptr)
                    {
                        QObject::connect(
                            combo_box,
                            QOverload<int>::of(&QComboBox::currentIndexChanged),
                            this,
                            [ = ]{on_depends_changed(combo_box, widget, depends_value, depends_reverse);});
                        on_depends_changed(combo_box, widget, depends_value, depends_reverse);
                    }
                }
            }
        }

        ++data_index;
    }

    if(scroll_area != nullptr)
    {
        auto* main_layout = new QHBoxLayout();
        main_layout->addWidget(scroll_area);
        main_layout->setContentsMargins(0, 0, 0, 0);
        qt_container->set_layout(main_layout);
    }
    else
    {
        qt_container->set_layout(layout);
    }

    this->start_listening_joystick();

    this->block_signals(false);
}

//-----------------------------------------------------------------------------

void settings::updating()
{
}

//-----------------------------------------------------------------------------

void settings::stopping()
{
    this->stop_listening_joystick();

    m_settings_slots.clear();
    m_param_boxes.clear(); // Avoid keeping dangling pointers
    this->destroy();
}

//-----------------------------------------------------------------------------

bool settings::eventFilter(QObject* _watched, QEvent* _event)
{
    if(_event->type() == ::QEvent::EnabledChange)
    {
        auto* check_box = qobject_cast<QCheckBox*>(_watched);
        if(check_box != nullptr)
        {
            check_box->stateChanged(check_box->isChecked() ? Qt::Checked : Qt::Unchecked);
        }
        else
        {
            auto* combo_box = qobject_cast<QComboBox*>(_watched);
            if(combo_box != nullptr)
            {
                combo_box->currentIndexChanged(combo_box->currentIndex());
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------

void settings::on_depends_changed(QCheckBox* _check_box, QWidget* _widget, bool _reverse)
{
    if(!_check_box->isEnabled())
    {
        _widget->setDisabled(true);
    }
    else if(_reverse)
    {
        _widget->setDisabled(_check_box->checkState() != 0U);
    }
    else
    {
        _widget->setEnabled(_check_box->checkState() != 0U);
    }
}

//------------------------------------------------------------------------------

void settings::on_depends_changed(QComboBox* _combo_box, QWidget* _widget, const std::string& _value, bool _reverse)
{
    if(!_combo_box->isEnabled())
    {
        _widget->setDisabled(true);
    }
    else if(_reverse)
    {
        _widget->setDisabled(_combo_box->currentText().toStdString() == _value);
    }
    else
    {
        _widget->setEnabled(_combo_box->currentText().toStdString() == _value);
    }
}

//------------------------------------------------------------------------------

void settings::on_color_button()
{
    QObject* sender = this->sender();

    // Create Color choice dialog.
    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QWidget* const container = qt_container->get_qt_container();
    SIGHT_ASSERT(get_id() << ": Container not instantiated yet.", container);

    const auto old_color  = sender->property("color").value<QColor>();
    const QColor color_qt = QColorDialog::getColor(
        old_color,
        container,
        "pick a color",
        QColorDialog::ShowAlphaChannel
    );

    if(color_qt.isValid())
    {
        auto* colour_button = dynamic_cast<QPushButton*>(sender);
        colour_button->setProperty("color", color_qt);

        int icon_size = colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(icon_size, icon_size);
        pix.fill(color_qt);

        colour_button->setIcon(QIcon(pix));

        sight::data::color::array_t color_array(
            {color_qt.redF(), color_qt.greenF(), color_qt.blueF(), color_qt.alphaF()});
        update_data<sight::data::color>(sender, color_array);
    }
}

//-----------------------------------------------------------------------------

void settings::on_change_integer(int _val)
{
    QObject* sender = this->sender();

    auto* slider  = dynamic_cast<QSlider*>(sender);
    auto* spinbox = dynamic_cast<QSpinBox*>(sender);
    if(slider != nullptr)
    {
        update_data<sight::data::integer, std::int64_t>(sender, _val);
    }
    else if(spinbox != nullptr)
    {
        const int count = spinbox->property(qt_property::count).toInt();
        SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

        if(count == 1)
        {
            update_data<sight::data::integer, std::int64_t>(sender, _val);
        }
        else
        {
            const auto index = spinbox->property(qt_property::index).toUInt();
            SIGHT_ASSERT(get_id() << ": Invalid widgets index, must be <= 3", index <= 3);

            if(count <= 2)
            {
                const auto obj = data<sight::data::ivec2>(sender);
                auto value     = obj->value();
                value[index] = _val;
                update_data<sight::data::ivec2>(sender, value);
            }
            else
            {
                const auto obj = data<sight::data::ivec3>(sender);
                auto value     = obj->value();
                value[index] = _val;
                update_data<sight::data::ivec3>(sender, value);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void settings::on_change_double(double _val)
{
    QObject* sender = this->sender();

    auto* slider  = dynamic_cast<QSlider*>(sender);
    auto* spinbox = dynamic_cast<QDoubleSpinBox*>(sender);
    if(slider != nullptr)
    {
        update_data<sight::data::real>(sender, _val);
    }
    else if(spinbox != nullptr)
    {
        const int count = spinbox->property(qt_property::count).toInt();
        SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

        if(count == 1)
        {
            update_data<sight::data::real>(sender, _val);
        }
        else
        {
            const auto index = spinbox->property(qt_property::index).toUInt();
            SIGHT_ASSERT(get_id() << ": Invalid widgets index, must be <= 3", index <= 3);

            if(count <= 2)
            {
                const auto obj = data<sight::data::dvec2>(sender);
                auto value     = obj->value();
                value[index] = _val;
                update_data<sight::data::dvec2>(sender, value);
            }
            else
            {
                const auto obj = data<sight::data::dvec3>(sender);
                auto value     = obj->value();
                value[index] = _val;
                update_data<sight::data::dvec3>(sender, value);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void settings::on_change_double_slider(int /*unused*/)
{
    auto* slider       = qobject_cast<QSlider*>(this->sender());
    const double value = sight::module::ui::qt::settings::get_double_slider_value(slider);
    update_data<sight::data::real>(slider, static_cast<double>(value));
}

//-----------------------------------------------------------------------------

void settings::on_slider_mapped(QLabel* _label, QSlider* _slider)
{
    _label->setText(QString::number(_slider->value()));
}

//-----------------------------------------------------------------------------

void settings::on_slider_range_mapped(QLabel* _min_label, QLabel* _max_label, QSlider* _slider)
{
    const int min = _slider->minimum();
    const int max = _slider->maximum();

    _min_label->setText(QString::number(min));
    _max_label->setText(QString::number(max));
}

//-----------------------------------------------------------------------------

void settings::on_double_slider_mapped(QLabel* _label, QSlider* _slider)
{
    const double new_value = get_double_slider_value(_slider);
    const int decimals     = _slider->property("decimals").toInt();

    _label->setText(QString::number(new_value, 'f', decimals));
}

//-----------------------------------------------------------------------------

void settings::on_double_slider_range_mapped(QLabel* _min_label, QLabel* _max_label, QSlider* _slider)
{
    const double min   = _slider->property("min").toDouble();
    const double max   = _slider->property("max").toDouble();
    const int decimals = _slider->property("decimals").toInt();

    _min_label->setText(QString::number(min, 'g', decimals));
    _max_label->setText(QString::number(max, 'g', decimals));
}

//-----------------------------------------------------------------------------

void settings::on_reset_boolean(QWidget* _widget)
{
    auto* checkbox = qobject_cast<QCheckBox*>(_widget);
    if(checkbox != nullptr)
    {
        const auto value = data<sight::data::boolean>(checkbox)->default_value();
        checkbox->setCheckState(value ? Qt::Checked : Qt::Unchecked);

        update_data<sight::data::boolean>(_widget, value);
    }
}

//-----------------------------------------------------------------------------

void settings::on_reset_color(QWidget* _widget)
{
    auto* colour_button = qobject_cast<QPushButton*>(_widget);
    if(colour_button != nullptr)
    {
        const auto color = data<sight::data::color>(colour_button)->default_value();

        QColor color_qt;
        color_qt.setRgbF(color[0], color[1], color[2], color[3]);

        int icon_size = colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(icon_size, icon_size);
        pix.fill(color_qt);

        colour_button->setIcon(QIcon(pix));
        colour_button->setProperty("color", color_qt);

        update_data<sight::data::color>(_widget, color);
    }
}

//-----------------------------------------------------------------------------

void settings::on_reset_integer(QWidget* _widget)
{
    auto* slider  = qobject_cast<QSlider*>(_widget);
    auto* spinbox = qobject_cast<QSpinBox*>(_widget);

    if(slider != nullptr)
    {
        const auto value = data<sight::data::integer, std::int64_t>(slider)->default_value();
        slider->setValue(static_cast<int>(value));
        update_data<sight::data::integer, std::int64_t>(_widget, value);
    }
    else if(spinbox != nullptr)
    {
        const int count = spinbox->property(qt_property::count).toInt();
        SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

        auto* spin1 = spinbox->property("widget#0").value<QSpinBox*>();
        if(count == 1)
        {
            const auto value1 = data<sight::data::integer, std::int64_t>(spin1)->default_value();
            spin1->setValue(static_cast<int>(value1));
            update_data<sight::data::integer, std::int64_t>(_widget, spin1->value());
        }
        else
        {
            auto* spin2 = spinbox->property("widget#1").value<QSpinBox*>();
            if(count <= 2)
            {
                const auto value = data<sight::data::ivec2>(spin2)->default_value();
                spin1->setValue(static_cast<int>(value[0]));
                spin2->setValue(static_cast<int>(value[1]));
                update_data<sight::data::ivec2>(_widget, value);
            }
            else
            {
                auto* spin3      = spinbox->property("widget#2").value<QSpinBox*>();
                const auto value = data<sight::data::ivec3>(spin3)->default_value();
                spin1->setValue(static_cast<int>(value[0]));
                spin2->setValue(static_cast<int>(value[1]));
                spin3->setValue(static_cast<int>(value[2]));
                update_data<sight::data::ivec3>(_widget, value);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void settings::on_reset_double(QWidget* _widget)
{
    auto* spinbox = qobject_cast<QDoubleSpinBox*>(_widget);
    auto* slider  = qobject_cast<QSlider*>(_widget);

    if(slider != nullptr)
    {
        const double value       = data<sight::data::real>(slider)->default_value();
        const double min         = slider->property("min").toDouble();
        const double max         = slider->property("max").toDouble();
        const double value_range = max - min;
        const int slider_val     = int(std::round(((value - min) / value_range) * double(slider->maximum())));
        slider->setValue(slider_val);
        update_data<sight::data::real>(_widget, value);
    }
    else if(spinbox != nullptr)
    {
        const unsigned int count = spinbox->property(qt_property::count).toUInt();
        SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

        auto* spin1 = spinbox->property("widget#0").value<QDoubleSpinBox*>();

        if(count == 1)
        {
            const double value = data<sight::data::real>(spin1)->default_value();
            spin1->setValue(value);
            update_data<sight::data::real>(_widget, value);
        }
        else
        {
            auto* spin2 = spinbox->property("widget#1").value<QDoubleSpinBox*>();
            if(count <= 2)
            {
                const auto value = data<sight::data::dvec2>(spin2)->default_value();
                spin1->setValue(value[0]);
                spin2->setValue(value[1]);
                update_data<sight::data::dvec2>(_widget, value);
            }
            else
            {
                auto* spin3      = spinbox->property("widget#2").value<QDoubleSpinBox*>();
                const auto value = data<sight::data::dvec3>(spin3)->default_value();
                spin1->setValue(value[0]);
                spin2->setValue(value[1]);
                spin3->setValue(value[2]);
                update_data<sight::data::dvec3>(_widget, value);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void settings::on_reset_string(QWidget* _widget)
{
    auto* edit = qobject_cast<QLineEdit*>(_widget);
    if(edit != nullptr)
    {
        const auto value = data<sight::data::string>(edit)->default_value();
        edit->setText(QString::fromStdString(value));

        update_data<sight::data::string>(_widget, value);
    }
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_reset_button(const std::string& _key, std::function<void()> _on_click) const
{
    std::string service_id = base_id();
    auto* reset_button     = new QPushButton("R");
    reset_button->setObjectName(QString::fromStdString(service_id + "/Reset " + _key));
    reset_button->setFocusPolicy(Qt::NoFocus);
    reset_button->setToolTip("Reset to the default value.");
    reset_button->setMaximumWidth(20);
    reset_button->setStyleSheet(qApp->styleSheet());

    QObject::connect(reset_button, &QPushButton::clicked, this, _on_click);

    return reset_button;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_bool_widget(QBoxLayout* _layout, const param_widget& _setup, Qt::Orientation _orientation)
{
    auto* checkbox = new QCheckBox();

    // Base properties
    const auto key = QString::fromStdString(_setup.key);
    checkbox->setObjectName(key);
    checkbox->setProperty(qt_property::key, key);
    checkbox->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

    // Data
    const auto obj        = data<sight::data::boolean>(checkbox);
    const auto init_value = obj->value();
    checkbox->setCheckState(init_value ? Qt::Checked : Qt::Unchecked);
    connect_data(obj, _setup.key);

    // Style
    checkbox->setTristate(false);
    checkbox->setStyleSheet(qApp->styleSheet());

    if(_orientation == Qt::Vertical)
    {
        _layout->addWidget(checkbox, 0, Qt::AlignCenter);
    }
    else
    {
        _layout->addWidget(checkbox, 0, Qt::AlignLeft | Qt::AlignVCenter);
    }

    // Forward to the Sight signal
    QObject::connect(
        checkbox,
        &QCheckBox::stateChanged,
        [this, key = _setup.key, checkbox](int _value)
        {
            const bool checked = _value == Qt::Checked;

            update_data<sight::data::boolean>(checkbox, checked);
        });

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, checkbox](){on_reset_boolean(checkbox);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_color_widget(QBoxLayout* _layout, const param_widget& _setup)
{
    auto* colour_button = new QPushButton();
    {
        // Base properties
        colour_button->setObjectName(QString::fromStdString(_setup.key));
        colour_button->setProperty(qt_property::key, QString::fromStdString(_setup.key));
        colour_button->setToolTip(tr("Selected color"));
        colour_button->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

        // Data
        const auto obj        = data<sight::data::color>(colour_button);
        const auto init_value = obj->value();
        QColor color_qt;
        color_qt.setRgbF(init_value[0], init_value[1], init_value[2], init_value[3]);
        connect_data(obj, _setup.key);

        // Style
        const int icon_size = colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(icon_size, icon_size);
        pix.fill(color_qt);

        colour_button->setIcon(QIcon(pix));
        colour_button->setProperty("color", color_qt);

        colour_button->setStyleSheet(qApp->styleSheet());
        set_minimum_size(colour_button, _setup);
    }

    _layout->addWidget(colour_button);

    QObject::connect(colour_button, &QPushButton::clicked, this, &settings::on_color_button);

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, colour_button]{on_reset_color(colour_button);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_double_spin_widget(
    QBoxLayout* _layout,
    const double_widget& _setup,
    int _count,
    Qt::Orientation _orientation
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* const sub_layout = new QBoxLayout {layout_direction};
    sub_layout->setContentsMargins(0, 0, 0, 0);
    _layout->addLayout(sub_layout);
    _layout->setProperty(qt_property::key, QString::fromStdString(_setup.key));
    _layout->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

    std::array<QDoubleSpinBox*, 3> spinboxes {};
    std::array<double, 3> init_values {0., 0., 0.};

    if(_count == 1)
    {
        const auto obj = data<sight::data::real>(_layout);
        init_values[0] = obj->value();
        connect_data(obj, _setup.key);
    }
    else if(_count == 2)
    {
        const auto obj   = data<sight::data::dvec2>(_layout);
        const auto value = obj->value();
        std::ranges::copy(value, init_values.begin());
        connect_data(obj, _setup.key);
    }
    else if(_count == 3)
    {
        const auto obj   = data<sight::data::dvec3>(_layout);
        const auto value = obj->value();
        std::ranges::copy(value, init_values.begin());
        connect_data(obj, _setup.key);
    }

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        auto* spinbox = new QDoubleSpinBox();

        // Base properties
        spinbox->setObjectName(QString::fromStdString(_setup.key + "/" + std::to_string(i)));
        spinbox->setProperty(qt_property::key, QString::fromStdString(_setup.key));
        spinbox->setProperty(qt_property::count, _count);
        spinbox->setProperty(qt_property::index, static_cast<unsigned int>(i));
        spinbox->setProperty(qt_property::data_index, static_cast<unsigned int>(_setup.data_index));
        spinboxes[i] = spinbox;

        // Data
        auto count_decimals = [](double _num) -> int
                              {
                                  std::stringstream out;
                                  out << _num;
                                  const std::string s = out.str();
                                  const std::string t = s.substr(s.find('.') + 1);
                                  return static_cast<int>(t.length());
                              };

        spinbox->setDecimals(std::max(std::max(count_decimals(_setup.min), count_decimals(_setup.max)), 2));
        spinbox->setRange(_setup.min, _setup.max);

        // Beware, set setSingleStep after setRange() and setDecimals() otherwise it may fail
        spinbox->setSingleStep(std::abs(spinbox->maximum() - spinbox->minimum()) / 100.);

        // Set value last only after setting range and decimals, otherwise the value may be truncated

        spinbox->setValue(init_values[i]);

        // Style
        spinbox->setStyleSheet(qApp->styleSheet());
        set_minimum_size(spinbox, _setup);

        sub_layout->addWidget(spinbox);

        QObject::connect(
            spinbox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &settings::on_change_double
        );
    }

    QDoubleSpinBox* spinbox = spinboxes[0];
    spinbox->setObjectName(QString::fromStdString(_setup.key));

    // Set a property with a pointer on each member of the group
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        for(std::size_t j = 0 ; j < std::size_t(_count) ; ++j)
        {
            const std::string prop_name = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(prop_name.c_str(), QVariant::fromValue<QDoubleSpinBox*>(spinboxes[j]));
        }
    }

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, spinbox]{on_reset_double(spinbox);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_double_slider_widget(
    QBoxLayout* _layout,
    const double_widget& _setup,
    std::uint8_t _decimals,
    Qt::Orientation _orientation,
    bool _on_release
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;
    auto* const sub_layout      = new QBoxLayout {layout_direction};
    sub_layout->setContentsMargins(0, 0, 0, 0);
    _layout->addLayout(sub_layout);

    auto* const slider = new QSlider();

    // Base properties
    const double value_range = _setup.max - _setup.min;
    slider->setOrientation(_orientation);
    slider->setObjectName(QString::fromStdString(_setup.key));
    slider->setProperty(qt_property::key, slider->objectName());
    slider->setProperty(qt_property::count, 1);
    slider->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

    // Data
    const auto obj        = data<sight::data::real>(slider);
    const auto init_value = obj->value();
    connect_data(obj, _setup.key);

    slider->setProperty("decimals", _decimals);
    slider->setProperty("min", _setup.min);
    slider->setProperty("max", _setup.max);

    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!_on_release);

    set_double_slider_range(slider, init_value);

    const int default_slider_value =
        int(std::round(((init_value - _setup.min) / value_range) * double(slider->maximum())));
    slider->setValue(default_slider_value);

    // Compute a "usable" page step
    slider->setPageStep(int(std::round(value_range * 10)));

    // Style
    slider->setProperty("widget#0", QVariant::fromValue<QSlider*>(slider));

    slider->setStyleSheet(qApp->styleSheet());
    set_minimum_size(slider, _setup);

    QFont min_max_labels_font;
    min_max_labels_font.setPointSize(7);
    min_max_labels_font.setItalic(true);

    auto* const min_value_label = new QLabel();
    min_value_label->setFont(min_max_labels_font);
    min_value_label->setText(QString::number(_setup.min, 'f', _decimals));
    min_value_label->setToolTip("Minimum value.");
    min_value_label->setObjectName(QString::fromStdString(_setup.key + "/minValueLabel"));
    min_value_label->setAlignment(Qt::AlignCenter);
    min_value_label->setStyleSheet(qApp->styleSheet());

    auto* const max_value_label = new QLabel();
    max_value_label->setFont(min_max_labels_font);
    max_value_label->setText(QString::number(_setup.max, 'f', _decimals));
    max_value_label->setToolTip("Maximum value.");
    max_value_label->setObjectName(QString::fromStdString(_setup.key + "/maxValueLabel"));
    max_value_label->setAlignment(Qt::AlignCenter);
    max_value_label->setStyleSheet(qApp->styleSheet());

    auto* const value_label = new QLabel();
    value_label->setStyleSheet("QLabel { font: bold; }");
    value_label->setText(QString::number(init_value, 'f', _decimals));
    value_label->setToolTip("Current value.");
    sight::module::ui::qt::settings::set_label_minimum_size(value_label, _setup.min, _setup.max, _decimals);
    value_label->setObjectName(QString::fromStdString(_setup.key + "/valueLabel"));
    value_label->setAlignment(Qt::AlignCenter);
    value_label->setStyleSheet(qApp->styleSheet());

    // Slots
    {
        // Forward to the corresponding Sight signal
        QObject::connect(slider, &QSlider::valueChanged, this, &settings::on_change_double_slider);

        // Update the labels when the value or the range changes
        QObject::connect(
            slider,
            &QSlider::valueChanged,
            this,
            [value_label, slider]
            {
                sight::module::ui::qt::settings::on_double_slider_mapped(value_label, slider);
            });

        QObject::connect(
            slider,
            &QSlider::rangeChanged,
            this,
            [ = ]{on_double_slider_range_mapped(min_value_label, max_value_label, slider);});
    }

    // Sub layout setup
    {
        const auto alignment = _orientation == Qt::Vertical ? Qt::AlignCenter : Qt::Alignment {};

        auto* const first_label  = _orientation == Qt::Vertical ? max_value_label : min_value_label;
        auto* const second_label = _orientation == Qt::Vertical ? min_value_label : max_value_label;

        sub_layout->addWidget(first_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(slider, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(second_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(value_label, /*scretch=*/ 0, alignment);
        sub_layout->setAlignment(alignment);

        if(_setup.hide_min_max)
        {
            min_value_label->hide();
            max_value_label->hide();
        }
    }

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, slider]{on_reset_double(slider);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_integer_slider_widget(
    QBoxLayout* _layout,
    const int_widget& _setup,
    Qt::Orientation _orientation,
    bool _on_release
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;
    auto* sub_layout            = new QBoxLayout {layout_direction};
    _layout->addLayout(sub_layout);
    sub_layout->setContentsMargins(0, 0, 0, 0);

    auto* slider = new QSlider();

    // Base properties
    slider->setObjectName(QString::fromStdString(_setup.key));
    slider->setProperty(qt_property::key, QString::fromStdString(_setup.key));
    slider->setProperty(qt_property::count, 1);
    slider->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));
    slider->setProperty("widget#0", QVariant::fromValue<QSlider*>(slider));
    // Data
    const auto obj        = data<sight::data::integer>(slider);
    const auto init_value = obj->value();

    slider->setMinimum(_setup.min);
    slider->setMaximum(_setup.max);
    slider->setValue(static_cast<int>(init_value));
    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!_on_release);
    connect_data(obj, _setup.key);

    // Style
    slider->setOrientation(_orientation);
    slider->setStyleSheet(qApp->styleSheet());
    set_minimum_size(slider, _setup);

    QFont min_max_labels_font;
    min_max_labels_font.setPointSize(7);
    min_max_labels_font.setItalic(true);

    auto* min_value_label = new QLabel();
    min_value_label->setFont(min_max_labels_font);
    min_value_label->setText(QString::number(slider->minimum()));
    min_value_label->setToolTip("Minimum value.");
    min_value_label->setObjectName(QString::fromStdString(_setup.key + "/minValueLabel"));
    min_value_label->setAlignment(Qt::AlignCenter);
    min_value_label->setStyleSheet(qApp->styleSheet());

    auto* max_value_label = new QLabel();
    max_value_label->setFont(min_max_labels_font);
    max_value_label->setText(QString::number(slider->maximum()));
    max_value_label->setToolTip("Maximum value.");
    max_value_label->setObjectName(QString::fromStdString(_setup.key + "/maxValueLabel"));
    max_value_label->setAlignment(Qt::AlignCenter);
    max_value_label->setStyleSheet(qApp->styleSheet());

    auto* value_label = new QLabel();
    value_label->setStyleSheet("QLabel { font: bold; }");
    value_label->setText(QString("%1").arg(slider->value()));
    value_label->setToolTip("Current value.");
    sight::module::ui::qt::settings::set_label_minimum_size(value_label, _setup.min, _setup.max);
    value_label->setObjectName(QString::fromStdString(_setup.key + "/valueLabel"));
    value_label->setAlignment(Qt::AlignCenter);
    value_label->setStyleSheet(qApp->styleSheet());

    // Sub layout setup
    {
        const auto alignment = _orientation == Qt::Vertical ? Qt::AlignCenter : Qt::Alignment {};

        auto* const first_label  = _orientation == Qt::Vertical ? max_value_label : min_value_label;
        auto* const second_label = _orientation == Qt::Vertical ? min_value_label : max_value_label;

        sub_layout->addWidget(first_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(slider, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(second_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(value_label, /*scretch=*/ 0, alignment);
        sub_layout->setAlignment(alignment);

        if(_setup.hide_min_max)
        {
            min_value_label->hide();
            max_value_label->hide();
        }
    }

    // Connections
    {
        // Forward to the corresponding Sight signal
        QObject::connect(slider, &QSlider::valueChanged, this, &settings::on_change_integer);

        // Update the labels when the value or the range changes
        QObject::connect(
            slider,
            &QSlider::valueChanged,
            this,
            [value_label, slider]{sight::module::ui::qt::settings::on_slider_mapped(value_label, slider);});

        QObject::connect(
            slider,
            &QSlider::rangeChanged,
            this,
            [ = ]{on_slider_range_mapped(min_value_label, max_value_label, slider);});
    }

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, slider]{on_reset_integer(slider);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_integer_spin_widget(
    QBoxLayout* _layout,
    const int_widget& _setup,
    int _count,
    Qt::Orientation _orientation
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;
    auto* sub_layout            = new QBoxLayout {layout_direction};
    sub_layout->setContentsMargins(0, 0, 0, 0);
    _layout->addLayout(sub_layout);
    _layout->setProperty(qt_property::key, QString::fromStdString(_setup.key));
    _layout->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

    std::array<QSpinBox*, 3> spinboxes {};
    std::array<std::int64_t, 3> init_values {0, 0, 0};

    if(_count == 1)
    {
        const auto obj = data<sight::data::integer>(_layout);
        init_values[0] = obj->value();
        connect_data(obj, _setup.key);
    }
    else if(_count == 2)
    {
        const auto obj   = data<sight::data::ivec2>(_layout);
        const auto value = obj->value();
        std::ranges::copy(value, init_values.begin());
        connect_data(obj, _setup.key);
    }
    else if(_count == 3)
    {
        const auto obj   = data<sight::data::ivec3>(_layout);
        const auto value = obj->value();
        std::ranges::copy(value, init_values.begin());
        connect_data(obj, _setup.key);
    }

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        auto* spinbox = new QSpinBox();
        spinboxes[i] = spinbox;

        // Base properties
        spinbox->setObjectName(QString::fromStdString(_setup.key + "/" + std::to_string(i)));
        spinbox->setProperty(qt_property::key, QString::fromStdString(_setup.key));
        spinbox->setProperty(qt_property::count, _count);
        spinbox->setProperty(qt_property::index, static_cast<unsigned int>(i));
        spinbox->setProperty(qt_property::data_index, static_cast<unsigned int>(_setup.data_index));

        spinbox->setMinimum(_setup.min);
        spinbox->setMaximum(_setup.max);
        spinbox->setValue(static_cast<int>(init_values[i]));

        // Style
        spinbox->setStyleSheet(qApp->styleSheet());
        set_minimum_size(spinbox, _setup);

        sub_layout->addWidget(spinbox);

        // Connect spinbox value with our editor
        QObject::connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &settings::on_change_integer);
    }

    QSpinBox* first_spinbox = spinboxes[0];
    first_spinbox->setObjectName(QString::fromStdString(_setup.key));

    // Set a property with a pointer on each member of the group
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        for(std::size_t j = 0 ; j < std::size_t(_count) ; ++j)
        {
            const std::string prop_name = std::string("widget#") + std::to_string(j);
            spinboxes[i]->setProperty(prop_name.c_str(), QVariant::fromValue<QSpinBox*>(spinboxes[j]));
        }
    }

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(
            _setup.key,
            [this, first_spinbox](){on_reset_integer(first_spinbox);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void settings::update_enum_list(const std::vector<std::string>& _list, const std::string _key)
{
    std::vector<std::string> labels;
    std::vector<std::string> keys;
    for(const auto& s : _list)
    {
        std::vector<std::string> parts;
        boost::split(parts, s, boost::is_any_of("="));
        labels.push_back(parts[0]);
        if(!parts[1].empty())
        {
            keys.push_back(parts[1]);
        }
        else
        {
            keys.push_back(parts[0]);
        }
    }

    this->block_signals(true);

    QObject* widget = this->get_param_widget(_key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
    {
        combobox->clear();

        int idx = 0;
        for(const auto& l : labels)
        {
            combobox->insertItem(idx, QString::fromStdString(l));
            ++idx;
        }

        // Add optional data
        idx = 0;
        for(const auto& k : keys)
        {
            combobox->setItemData(idx, QString::fromStdString(k));
            ++idx;
        }
    }
    else if(auto* non_linear_slider = qobject_cast<sight::ui::qt::widget::non_linear_slider*>(widget);
            non_linear_slider != nullptr)
    {
        std::vector<int> values;
        values.reserve(labels.size());
        std::ranges::transform(labels, std::back_inserter(values), [](const std::string& _s){return std::stoi(_s);});
        non_linear_slider->set_values(values);
        non_linear_slider->set_value(values[0]);
    }

    this->block_signals(false);
}

//-----------------------------------------------------------------------------

void settings::parse_enum_string(
    const std::string& _options,
    std::vector<std::string>& _labels,
    std::vector<std::string>& _keys,
    std::string _separators
)
{
    const boost::char_separator<char> sep(_separators.c_str());
    const boost::tokenizer<boost::char_separator<char> > tokens {_options, sep};

    for(const auto& token : tokens)
    {
        std::vector<std::string> parts;
        boost::split(parts, token, boost::is_any_of("="));

        _labels.push_back(parts[0]);
        if(parts.size() > 1)
        {
            _keys.push_back(parts[1]);
        }
        else
        {
            _keys.push_back(parts[0]);
        }
    }
}

//------------------------------------------------------------------------------

void settings::create_enum_combobox_widget(
    QBoxLayout* _layout,
    const param_widget& _setup,
    const std::vector<std::string>& _values,
    const std::vector<std::string>& _data
)
{
    auto* combo_box = new QComboBox();
    combo_box->setObjectName(QString::fromStdString(_setup.key));
    combo_box->setStyleSheet(qApp->styleSheet());

    combo_box->setProperty(qt_property::key, QString::fromStdString(_setup.key));
    combo_box->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

    for(int idx = 0 ; const auto& value : _values)
    {
        combo_box->insertItem(idx, QString::fromStdString(value));
        ++idx;
    }

    // Add optional data
    for(int idx = 0 ; const auto& choice : _data)
    {
        combo_box->setItemData(idx, QString::fromStdString(choice));
        ++idx;
    }

    _layout->addWidget(combo_box);

    const bool is_string = data<sight::data::string>(combo_box) != nullptr;

    // Set the comboBox to the default value
    if(is_string)
    {
        const auto obj        = data<sight::data::string>(combo_box);
        const auto init_value = obj->value();

        for(std::size_t idx = 0 ; const auto& choice : _data)
        {
            if(choice == init_value)
            {
                combo_box->setCurrentText(QString::fromStdString(_values[idx]));
            }

            ++idx;
        }

        connect_data(obj, _setup.key);

        QObject::connect(
            combo_box,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, combo_box](int _value)
            {
                const std::string data = combo_box->itemData(_value).toString().toStdString();
                update_data<sight::data::string>(combo_box, data);
            });
    }
    else
    {
        const auto obj        = data<sight::data::integer>(combo_box);
        const auto init_value = obj->value();
        combo_box->setCurrentIndex(static_cast<int>(init_value));
        connect_data(obj, _setup.key);

        QObject::connect(
            combo_box,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this, combo_box](int _value)
            {
                update_data<sight::data::integer, std::int64_t>(combo_box, _value);
            });
    }

    // Style
    set_minimum_size(combo_box, _setup);
}

//------------------------------------------------------------------------------

void settings::create_enum_slider_widget(
    QBoxLayout* _layout,
    const param_widget& _setup,
    const std::vector<std::string>& _values,
    Qt::Orientation _orientation,
    bool _on_release
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* sub_layout = new QBoxLayout {layout_direction};
    sub_layout->setContentsMargins(0, 0, 0, 0);
    _layout->addLayout(sub_layout);

    std::vector<int> int_values;
    std::ranges::transform(
        _values,
        std::back_inserter(int_values),
        [](const std::string& _s){return std::stoi(_s);});

    auto* const slider = new sight::ui::qt::widget::non_linear_slider();
    slider->set_orientation(_orientation);
    set_minimum_size(slider, _setup);
    slider->setObjectName(QString::fromStdString(_setup.key));
    slider->setProperty(qt_property::key, QString::fromStdString(_setup.key));
    slider->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

    slider->set_values(int_values);
    slider->set_tracking(!_on_release);

    const bool is_string = data<sight::data::string>(slider) != nullptr;

    // Set the slider to the default value
    if(is_string)
    {
        const auto obj        = data<sight::data::string>(slider);
        const auto init_value = obj->value();
        slider->set_value(std::stoi(init_value));
        connect_data(obj, _setup.key);
    }
    else
    {
        const auto obj        = data<sight::data::integer>(slider);
        const auto init_index = obj->value();
        slider->set_value(int_values[static_cast<std::size_t>(init_index)]);
        connect_data(obj, _setup.key);
    }

    slider->setStyleSheet(qApp->styleSheet());

    QFont min_max_labels_font;
    min_max_labels_font.setPointSize(7);
    min_max_labels_font.setItalic(true);

    auto* const min_value_label = new QLabel();
    min_value_label->setFont(min_max_labels_font);
    min_value_label->setText(QString::fromStdString(_values.front()));
    min_value_label->setToolTip("Minimum value.");
    min_value_label->setObjectName(QString::fromStdString(_setup.key + "/minValueLabel"));
    min_value_label->setAlignment(Qt::AlignCenter);
    min_value_label->setStyleSheet(qApp->styleSheet());

    auto* const max_value_label = new QLabel();
    max_value_label->setFont(min_max_labels_font);
    max_value_label->setText(QString::fromStdString(_values.back()));
    max_value_label->setToolTip("Maximum value.");
    max_value_label->setObjectName(QString::fromStdString(_setup.key + "/maxValueLabel"));
    max_value_label->setAlignment(Qt::AlignCenter);
    max_value_label->setStyleSheet(qApp->styleSheet());

    auto* value_label = new QLabel();

    value_label->setStyleSheet("QLabel { font: bold; }");
    value_label->setText(QString::number(slider->value()));
    value_label->setToolTip("Current value.");
    set_label_minimum_size(value_label, int_values.front(), int_values.back());
    value_label->setObjectName(QString::fromStdString(_setup.key + "/valueLabel"));
    value_label->setAlignment(Qt::AlignCenter);

    // Sub layout setup
    {
        const auto alignment = _orientation == Qt::Vertical ? Qt::AlignCenter : Qt::Alignment {};

        auto* const first_label  = _orientation == Qt::Vertical ? max_value_label : min_value_label;
        auto* const second_label = _orientation == Qt::Vertical ? min_value_label : max_value_label;

        sub_layout->addWidget(first_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(slider, /*stretch=*/ 0, alignment);
        sub_layout->addWidget(second_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(value_label, /*scretch=*/ 0, alignment);

        if(_setup.hide_min_max)
        {
            min_value_label->hide();
            max_value_label->hide();
        }
    }

    // Connections
    {
        // Forward to the Sight signal
        QObject::connect(
            slider,
            &sight::ui::qt::widget::non_linear_slider::value_changed,
            [this, key = _setup.key, slider, value_label](int _value)
            {
                if(const auto string_obj = data<sight::data::string>(slider); string_obj == nullptr)
                {
                    update_data<sight::data::integer>(slider, static_cast<std::int64_t>(slider->index()));
                }
                else
                {
                    update_data<sight::data::string>(slider, std::to_string(_value));
                }

                value_label->setText(QString::number(_value));
            });

        // Update the labels
        QObject::connect(
            slider,
            &sight::ui::qt::widget::non_linear_slider::range_changed,
            [ = ](int _min, int _max)
            {
                min_value_label->setText(QString::number(_min));
                max_value_label->setText(QString::number(_max));
            });
    }
}

//-----------------------------------------------------------------------------
void settings::create_tickmarks_widget(
    QBoxLayout* _layout,
    const param_widget& _setup,
    const std::vector<std::string>& _values
)
{
    auto* tick_widget = new sight::ui::qt::widget::tickmarks_slider();
    tick_widget->setProperty(
        qt_property::key,
        QString::fromStdString(_setup.key)
    );
    tick_widget->setProperty(
        qt_property::data_index,
        static_cast<uint>(_setup.data_index)
    );

    tick_widget->setObjectName(QString::fromStdString(_setup.key));
    tick_widget->set_range(0, static_cast<int>(_values.size()) - 1);

    tick_widget->set_tick_labels(_values);
    QStringList q_labels;
    for(const auto& s : _values)
    {
        q_labels << QString::fromStdString(s);
    }

    tick_widget->setProperty("tickLabels", q_labels);
    tick_widget->set_tick_interval(1);
    tick_widget->set_current_tick(0);
    tick_widget->setMinimumSize(100, 50);

    auto* value_label = new QLabel();

    value_label->setObjectName(QString::fromStdString(_setup.key + "/valueLabel"));
    value_label->setVisible(false);
    const bool is_string = data<sight::data::string>(tick_widget) != nullptr;

    if(is_string)
    {
        const auto obj        = data<sight::data::string>(tick_widget);
        const auto init_value = obj->value();
        tick_widget->set_tick_labels({init_value});
        connect_data(obj, _setup.key);
    }
    else
    {
        auto obj        = data<sight::data::integer>(tick_widget);
        int  init_index = static_cast<int>(obj->value());
        init_index = std::clamp(init_index, 0, int(_values.size()) - 1);
        tick_widget->set_current_tick(init_index);
        value_label->setText(
            QString::fromStdString(
                _values[static_cast<std::size_t>(init_index)]
            )
        );

        connect_data(obj, _setup.key);
    }

    connect(
        tick_widget,
        &sight::ui::qt::widget::tickmarks_slider::value_changed,
        this,
        [this, tick_widget](int _index)
        {
            const auto& labels = tick_widget->tick_labels();
            if(_index < 0 || _index >= static_cast<int>(labels.size()))
            {
                return;
            }

            const std::string& tick_value = labels[static_cast<size_t>(_index)];
            if(auto str = data<sight::data::string>(tick_widget))
            {
                update_data<sight::data::string>(tick_widget, tick_value);
            }
            else
            {
                update_data<sight::data::integer>(
                    tick_widget,
                    static_cast<std::int64_t>(_index)
                );
            }
        });

    _layout->addWidget(tick_widget);
    std::string options = boost::algorithm::join(_values, ",");

    _layout->addWidget(value_label);
    this->update_tickmarks(options, _setup.key);
}

//-----------------------------------------------------------------------------

void settings::create_enum_button_bar_widget(
    QBoxLayout* _layout,
    const param_widget& _setup,
    const std::vector<enum_button_param>& _button_list,
    const int _width,
    const int _height,
    const int _spacing,
    const std::string& _style,
    Qt::Orientation _orientation
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* sub_layout = new QBoxLayout {layout_direction};
    sub_layout->setContentsMargins(0, 0, 0, 0);

    if(_spacing != 0)
    {
        sub_layout->setSpacing(_spacing);
    }

    _layout->addLayout(sub_layout);

    // create a button group to deactivate the buttons on selection
    auto* button_bar_group = new QButtonGroup(sub_layout);
    button_bar_group->setObjectName(QString::fromStdString(_setup.key));

    // create the buttons from the provided list
    int button_index         = 0;
    const auto is_button_bar = _width != 0 || _height != 0;
    for(const auto& button_param : _button_list)
    {
        auto* enum_button = new QToolButton();
        button_bar_group->addButton(enum_button);

        // The name needs to be the key_value, to find it when the service is updated through a slot
        enum_button->setObjectName((QString::fromStdString(_setup.key + "_" + button_param.value)));
        enum_button->setProperty(qt_property::key, QString::fromStdString(_setup.key));
        enum_button->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

        enum_button->setIcon(QIcon(QString::fromStdString(button_param.icon_path)));
        enum_button->setToolTip(QString::fromStdString(button_param.label));
        enum_button->setCheckable(true);
        enum_button->setProperty("class", "buttonBar");
        enum_button->setProperty("value", QString::fromStdString(button_param.value));
        enum_button->setProperty("button_index", button_index);

        enum_button->setText(QString::fromStdString(button_param.label));

        enum_button->setStyleSheet("border : 1px solid transparent;");

        if(_orientation == Qt::Vertical)
        {
            enum_button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        }
        else
        {
            enum_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
        }

        if(_style == "ToolButtonTextOnly")
        {
            enum_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
        }
        else if(_style == "ToolButtonTextBesideIcon")
        {
            enum_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        }
        else if(_style == "ToolButtonTextUnderIcon")
        {
            enum_button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        }
        else if(_style == "ToolButtonFollowStyle")
        {
            enum_button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        }
        else
        {
            enum_button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }

        // create an effect to make it gray when not selected, and full color when selected
        auto* effect = new QGraphicsColorizeEffect;
        effect->setColor(QColor(10, 10, 10));
        effect->setStrength(0.7);
        enum_button->setGraphicsEffect(effect);

        // the size depends on the configuration. xml > qss
        if(is_button_bar)
        {
            // the size is provided through the xml config. Don't use the qss and ignore _setup.min_size
            const int width  = _width == 0 ? _height : _width;
            const int height = _height == 0 ? _width : _height;
            enum_button->setIconSize({width, height});
        }
        else
        {
            // the size is not provided through the xml. Use the qss style.
            enum_button->setProperty("class", "buttonBarTouchFriendly");
        }

        // add the button in the grid at its place
        sub_layout->addWidget(enum_button, 0, Qt::AlignCenter);

        // create the connection to fire signals when the button is clicked
        QObject::connect(
            enum_button,
            &QToolButton::clicked,
            [this, button_param, enum_button, button_index]
            {
                if(const auto string_obj = data<sight::data::string>(enum_button); string_obj == nullptr)
                {
                    update_data<sight::data::integer>(enum_button, static_cast<std::int64_t>(button_index));
                }
                else
                {
                    update_data<sight::data::string>(enum_button, button_param.value);
                }
            });

        // create connection to change the display (gray/full color) when the selection state changes
        QObject::connect(
            enum_button,
            &QAbstractButton::toggled,
            [ = ](bool _checked)
            {
                QGraphicsEffect* effect = enum_button->graphicsEffect();
                effect->setEnabled(!_checked);
            });

        const bool is_string = data<sight::data::string>(enum_button) != nullptr;

        // Set the slider to the default value
        if(is_string)
        {
            // set the default value
            const auto obj        = data<sight::data::string>(enum_button);
            const auto init_value = obj->value();
            connect_data(obj, _setup.key);

            if(button_param.value == init_value)
            {
                enum_button->toggle();
            }
        }
        else
        {
            const auto obj        = data<sight::data::integer>(enum_button);
            const auto init_value = obj->value();
            connect_data(obj, _setup.key);
            if(button_index == init_value)
            {
                enum_button->toggle();
            }
        }

        ++button_index;
    }

    // create the connection to fire signals when the button is clicked
    connect(
        button_bar_group,
        &QButtonGroup::buttonClicked,
        [button_bar_group, is_button_bar](QAbstractButton* _selected_button)
        {
            if(is_button_bar)
            {
                for(const auto& button : button_bar_group->buttons())
                {
                    button->setStyleSheet("border: 1px solid transparent;");
                }

                _selected_button->setStyleSheet("border: 1px solid white;");
            }
        });
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* settings::create_text_widget(QBoxLayout* _layout, const param_widget& _setup, const std::string& _type)
{
    auto* edit     = new QLineEdit();
    const auto key = QString::fromStdString(_setup.key);
    edit->setObjectName(key);

    // Base properties
    edit->setProperty(qt_property::key, key);
    edit->setProperty(qt_property::data_index, static_cast<uint>(_setup.data_index));

    // Data
    const auto obj        = data<sight::data::string>(edit);
    const auto init_value = obj->value();
    edit->setText(QString::fromStdString(init_value));
    connect_data(obj, _setup.key);

    edit->setStyleSheet(qApp->styleSheet());
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    set_minimum_size(edit, _setup);

    _layout->addWidget(edit);

    // Forward to the Sight signal
    QObject::connect(
        edit,
        &QLineEdit::textChanged,
        [this, key = _setup.key, edit](QString _value)
        {
            update_data<sight::data::string>(edit, _value.toStdString());
        });

    QPushButton* dir_selector = nullptr;

    if(_type == "file" or _type == "file_read" or _type == "file_write")
    {
        dir_selector = new QPushButton("...");

        QObject::connect(
            dir_selector,
            &QPushButton::clicked,
            [edit, _type]()
            {
                static auto default_directory = std::make_shared<core::location::single_folder>();

                sight::ui::dialog::location dialog_file;
                dialog_file.set_title("Select file");
                dialog_file.set_default_location(default_directory);

                using sight::ui::dialog::location;
                dialog_file.set_option(_type == "file_write" ? location::write : location::read);

                dialog_file.set_type(sight::ui::dialog::location::single_file);

                auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
                if(result)
                {
                    default_directory->set_folder(result->get_file().parent_path());
                    edit->setText(QString::fromStdString(result->get_file().string()));
                    dialog_file.save_default_location(default_directory);
                }
            });
    }
    else if(_type == "dir" or _type == "dir_read" or _type == "dir_write")
    {
        dir_selector = new QPushButton("...");

        QObject::connect(
            dir_selector,
            &QPushButton::clicked,
            [edit, _type]()
            {
                static auto default_directory = std::make_shared<core::location::single_folder>();

                sight::ui::dialog::location dialog_file;
                dialog_file.set_title("Select directory");
                dialog_file.set_default_location(default_directory);

                using sight::ui::dialog::location;
                dialog_file.set_option(_type == "dir_read" ? location::read : location::write);

                dialog_file.set_type(sight::ui::dialog::location::folder);

                const auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
                if(result)
                {
                    default_directory->set_folder(result->get_folder());
                    edit->setText(QString::fromStdString(result->get_folder().string()));
                    dialog_file.save_default_location(default_directory);
                }
            });
    }

    if(dir_selector != nullptr)
    {
        dir_selector->setMaximumWidth(20);
        dir_selector->setStyleSheet(qApp->styleSheet());
        _layout->addWidget(dir_selector, 0, Qt::AlignCenter);
    }

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, edit](){on_reset_string(edit);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

double settings::get_double_slider_value(const QSlider* _slider)
{
    const double min = _slider->property("min").toDouble();
    const double max = _slider->property("max").toDouble();

    const double value_range = max - min;
    double double_value      = min;
    if(_slider->maximum() != 0)
    {
        double_value = (double(_slider->value()) / _slider->maximum()) * value_range + min;
    }

    return double_value;
}

//------------------------------------------------------------------------------

void settings::update_enum_range(std::string _options, std::string _key)
{
    this->block_signals(true);

    QObject* widget = this->get_param_widget(_key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
    {
        combobox->blockSignals(true);
        combobox->clear();

        std::vector<std::string> values;
        std::vector<std::string> data;

        sight::module::ui::qt::settings::parse_enum_string(_options, values, data);

        int idx = 0;
        for(const auto& value : values)
        {
            combobox->insertItem(idx, QString::fromStdString(value));
            ++idx;
        }

        // Add optional data
        idx = 0;
        for(const auto& choice : data)
        {
            combobox->setItemData(idx, QString::fromStdString(choice));
            ++idx;
        }

        const bool is_string = settings::data<sight::data::string>(combobox) != nullptr;
        if(is_string)
        {
            const auto obj        = settings::data<sight::data::string>(combobox);
            const auto init_value = obj->value();
            for(std::size_t index = 0 ; const auto& choice : data)
            {
                if(choice == init_value)
                {
                    combobox->setCurrentIndex(static_cast<int>(index));
                }

                ++index;
            }
        }
        else
        {
            const auto obj        = settings::data<sight::data::integer>(combobox);
            const auto init_value = obj->value();
            combobox->setCurrentIndex(static_cast<int>(init_value));
        }

        combobox->blockSignals(false);
    }
    else
    {
        this->update_tickmarks(_options, _key);
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------
void settings::update_tickmarks(const std::string _options, const std::string _key)
{
    QObject* widget_ptr = this->get_param_widget(_key);

    auto* tickmarks_widget = qobject_cast<sight::ui::qt::widget::tickmarks_slider*>(widget_ptr);
    if(tickmarks_widget != nullptr)
    {
        QSignalBlocker guard(tickmarks_widget);

        std::vector<std::string> tick_labels;
        std::vector<std::string> tick_data;
        sight::module::ui::qt::settings::parse_enum_string(_options, tick_labels, tick_data);

        if(!tick_labels.empty())
        {
            const int max_index = static_cast<int>(tick_labels.size()) - 1;
            tickmarks_widget->set_range(0, max_index);
            tickmarks_widget->set_tick_interval(1);
            tickmarks_widget->set_tick_labels(tick_labels);

            int current_index = 0;
            if(auto string_data = settings::data<sight::data::string>(tickmarks_widget))
            {
                const std::string& current_value = string_data->value();
                auto it                          = std::find(tick_data.begin(), tick_data.end(), current_value);
                if(it != tick_data.end())
                {
                    current_index = static_cast<int>(std::distance(tick_data.begin(), it));
                }
            }
            else if(auto integer_data = settings::data<sight::data::integer>(tickmarks_widget))
            {
                current_index = static_cast<int>(std::clamp<std::int64_t>(integer_data->value(), 0, max_index));
            }

            tickmarks_widget->set_current_tick(current_index);
        }
    }
}

//-----------------------------------------------------------------------------

void settings::block_signals(bool _block)
{
    m_block_signals = _block;
}

//------------------------------------------------------------------------------

void settings::update_int_min_parameter(int _min, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property(qt_property::count).toInt();
        auto* spin0     = child->property("widget#0").value<QSpinBox*>();
        spin0->setMinimum(_min);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QSpinBox*>();
            spin1->setMinimum(_min);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QSpinBox*>();
            spin2->setMinimum(_min);
        }
    }
    else if(slider != nullptr)
    {
        slider->setMinimum(_min);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void settings::update_int_max_parameter(int _max, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property(qt_property::count).toInt();

        auto* spin0 = child->property("widget#0").value<QSpinBox*>();
        spin0->setMaximum(_max);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QSpinBox*>();
            spin1->setMaximum(_max);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QSpinBox*>();
            spin2->setMaximum(_max);
        }
    }
    else if(slider != nullptr)
    {
        slider->setMaximum(_max);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void settings::update_double_min_parameter(double _min, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property(qt_property::count).toInt();

        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        spin0->setMinimum(_min);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
            spin1->setMinimum(_min);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();
            spin2->setMinimum(_min);
        }
    }
    else if(slider != nullptr)
    {
        const auto value = data<sight::data::real>(slider)->value();
        slider->setProperty("min", _min);
        set_double_slider_range(slider, value);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void settings::update_double_max_parameter(double _max, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property(qt_property::count).toInt();

        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        spin0->setMaximum(_max);

        if(count >= 2)
        {
            auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
            spin1->setMaximum(_max);
        }

        if(count >= 3)
        {
            auto* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();
            spin2->setMaximum(_max);
        }
    }
    else if(slider != nullptr)
    {
        const auto value = data<sight::data::real>(slider)->value();
        slider->setProperty("max", _max);
        set_double_slider_range(slider, value);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }
}

//-----------------------------------------------------------------------------

void settings::set_double_slider_range(QSlider* _slider, double _current_value)
{
    const double min            = _slider->property("min").toDouble();
    const double max            = _slider->property("max").toDouble();
    const std::uint8_t decimals = static_cast<std::uint8_t>(_slider->property("decimals").toUInt());
    int max_slider_value        = 1;
    for(std::uint8_t i = 0 ; i < decimals ; ++i)
    {
        max_slider_value *= 10;
    }

    const double value_range = max - min;
    max_slider_value = int(max_slider_value * value_range);

    // The slider's maximum internal range is [0; 2 147 483 647]
    // We could technically extend this range by setting the minimum to std::numeric_limits<int>::min()
    // but it would be ridiculous to use a slider handling so many values.
    _slider->setMinimum(0);

    const std::string key = _slider->property(qt_property::key).toString().toStdString();
    SIGHT_ERROR_IF(
        ": The requested value range for " << std::quoted(
            key
        ) << " is too large to be handled by a double slider. "
             "Please reduce your range, the number of decimals or use a 'spin' widget.",
        max_slider_value < std::numeric_limits<double>::epsilon()
    );
    if(max_slider_value < std::numeric_limits<double>::epsilon())
    {
        max_slider_value = 1.;
    }

    _slider->setMaximum(max_slider_value);

    // Update the slider integer value according to the new mix/max
    if(_current_value <= min)
    {
        _slider->setValue(0);

        // qt does not emit the signal if the value does not change, we have to force qt signal to update the displayed
        // value and emit 'doubleChanged' signal
        Q_EMIT _slider->valueChanged(0);
    }
    else if(_current_value > max)
    {
        _slider->setValue(max_slider_value);
    }
    else
    {
        const int slider_val = int(std::round(((_current_value - min) / value_range) * double(_slider->maximum())));
        _slider->setValue(slider_val);
    }
}

//-----------------------------------------------------------------------------

QObject* settings::get_param_widget(const std::string& _key)
{
    const auto key = QString::fromStdString(_key);

    const auto it = std::ranges::find_if(
        m_param_boxes,
        [box_key = key + "_box"](const QWidget* const _w){return _w->objectName() == box_key;});

    if(it == m_param_boxes.cend())
    {
        SIGHT_ERROR(get_id() << ": No param with '" + _key + "' found");
        return nullptr;
    }

    auto* widget = (*it)->findChild<QObject*>(key);

    // The child widget *MUST* exist if a parameter exists with this name, if it does not exist, this is a regression
    // caused by modifications in create_xxx_widgets.
    SIGHT_ASSERT(get_id() << ": Widget " << std::quoted(_key) << " not found in its parent box.", widget != nullptr);

    return widget;
}

//------------------------------------------------------------------------------

template<>
inline QString settings::value_to_string_label<int>(int _value, std::uint8_t /*unused*/)
{
    return QString("%1").arg(_value);
}

//------------------------------------------------------------------------------

template<>
inline QString settings::value_to_string_label<double>(double _value, std::uint8_t _decimals)
{
    return QString::number(_value, 'f', _decimals);
}

//------------------------------------------------------------------------------

template<typename T>
void settings::set_label_minimum_size(QLabel* _label, T _min, T _max, std::uint8_t _decimals)
{
    const auto min_string = value_to_string_label(_min, _decimals);
    const auto max_string = value_to_string_label(_max, _decimals);

    // Create a dummy label with same properties
    QLabel dummy_label;
    dummy_label.setFont(_label->font());
    dummy_label.setStyleSheet(_label->styleSheet());

    // Fill it with the string of the max value and request the size from Qt
    dummy_label.setText(max_string);
    const QSize size_with_max_value = dummy_label.sizeHint();

    // Fill it with the string of the min value and request the size from Qt
    dummy_label.setText(min_string);
    const QSize size_with_min_value = dummy_label.sizeHint();

    // Compute the maximum size and set it to our label
    const QSize max_size = size_with_max_value.expandedTo(size_with_min_value);
    _label->setMinimumSize(max_size);
}

//------------------------------------------------------------------------------
template<>
void settings::set_parameter<sight::data::boolean>(const bool& _val, std::string _key)
{
    this->block_signals(true);
    QObject* child = this->get_param_widget(_key);
    auto* checkbox = qobject_cast<QCheckBox*>(child);

    if(checkbox != nullptr)
    {
        checkbox->setCheckState(_val ? Qt::Checked : Qt::Unchecked);
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::color>(const sight::data::color::array_t& _val, std::string _key)
{
    this->block_signals(true);
    QObject* child     = this->get_param_widget(_key);
    auto* color_button = qobject_cast<QPushButton*>(child);

    if(color_button != nullptr)
    {
        const int icon_size = color_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(icon_size, icon_size);
        QColor color_qt;
        color_qt.setRgbF(_val[0], _val[1], _val[2], _val[3]);
        pix.fill(color_qt);

        color_button->setIcon(QIcon(pix));
        color_button->setProperty("color", color_qt);
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::string>(const std::string& _val, std::string _key)
{
    this->block_signals(true);

    QObject* widget = this->get_param_widget(_key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
    {
        // Find first in text
        auto res = combobox->findText(QString::fromStdString(_val));
        if(res == -1)
        {
            // fallback, try to find in optional data.
            res = combobox->findData(QString::fromStdString(_val));
        }

        if(res >= 0)
        {
            combobox->setCurrentIndex(res);
        }
        else
        {
            SIGHT_WARN(
                get_id() << "value " << std::quoted(_val) << "doesn't exist for parameter " << std::quoted(
                    _key
                ) << "."
            );
        }
    }
    else if(auto* non_linear_slider = qobject_cast<sight::ui::qt::widget::non_linear_slider*>(widget);
            non_linear_slider != nullptr)
    {
        non_linear_slider->set_value(std::stoi(_val));
    }
    else if(auto* button_group = qobject_cast<QButtonGroup*>(widget);
            button_group != nullptr)
    {
        const auto enum_key = QString::fromStdString(_key + "_" + _val);

        const auto& buttons = button_group->buttons();
        auto button_it      = std::ranges::find_if(
            buttons,
            [&enum_key](const auto* const _b)
            {
                return _b->objectName() == enum_key;
            });

        SIGHT_ASSERT(
            get_id() << ": Value " << std::quoted(_val) << " does not appear in parameter widget "
            << std::quoted(_key),
            button_it != buttons.cend()
        );

        (*button_it)->toggle();
    }
    else if(auto* line_edit = qobject_cast<QLineEdit*>(widget);
            line_edit != nullptr)
    {
        line_edit->setText(QString::fromStdString(_val));
    }
    else if(auto* tickmarks_widget = qobject_cast<sight::ui::qt::widget::tickmarks_slider*>(widget);
            tickmarks_widget != nullptr)
    {
        auto  q_labels  = tickmarks_widget->property("tickLabels").toStringList();
        qsizetype idx_q = q_labels.indexOf(QString::fromStdString(_val));
        int idx         = static_cast<int>(idx_q >= 0 ? idx_q : 0);
        tickmarks_widget->set_current_tick(idx >= 0 ? idx : 0);
    }
    else
    {
        SIGHT_ERROR("Unknown widget type in set_parameter callback");
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::integer>(const std::int64_t& _val, std::string _key)
{
    this->block_signals(true);
    QObject* widget = this->get_param_widget(_key);

    auto* spinbox  = qobject_cast<QSpinBox*>(widget);
    auto* slider   = qobject_cast<QSlider*>(widget);
    auto* combobox = qobject_cast<QComboBox*>(widget);

    const auto val = static_cast<int>(_val);
    if(spinbox != nullptr)
    {
        spinbox->setValue(val);
    }
    else if(slider != nullptr)
    {
        slider->setValue(val);
    }
    else if(auto* non_linear_slider = qobject_cast<sight::ui::qt::widget::non_linear_slider*>(widget);
            non_linear_slider != nullptr)
    {
        non_linear_slider->set_index(std::size_t(val));
    }
    else if(auto* tickmarks_widget = qobject_cast<sight::ui::qt::widget::tickmarks_slider*>(widget);
            tickmarks_widget != nullptr)
    {
        tickmarks_widget->set_current_tick(val);
    }
    else if(combobox != nullptr)
    {
        combobox->setCurrentIndex(val);
    }
    else if(auto* button_group = qobject_cast<QButtonGroup*>(widget); button_group != nullptr)
    {
        const auto button_index = static_cast<int>(_val);

        const auto& buttons  = button_group->buttons();
        const auto button_it = std::ranges::find_if(
            buttons,
            [&button_index](const auto* const _b)
            {
                return _b->property("button_index").toInt() == button_index;
            });

        SIGHT_ASSERT(
            get_id() << ": Index " << std::quoted(std::to_string(_val)) << " does not appear in parameter widget "
            << std::quoted(_key),
            button_it != buttons.cend()
        );

        (*button_it)->toggle();
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::ivec2>(const sight::data::ivec2::array_t& _val, std::string _key)
{
    this->block_signals(true);
    QObject* widget = this->get_param_widget(_key);

    if(widget != nullptr)
    {
        auto* spin0 = widget->property("widget#0").value<QSpinBox*>();
        auto* spin1 = widget->property("widget#1").value<QSpinBox*>();

        spin0->setValue(static_cast<int>(_val[0]));
        spin1->setValue(static_cast<int>(_val[1]));
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::ivec3>(const sight::data::ivec3::array_t& _val, std::string _key)
{
    this->block_signals(true);
    QObject* widget = this->get_param_widget(_key);

    if(widget != nullptr)
    {
        auto* spin0 = widget->property("widget#0").value<QSpinBox*>();
        auto* spin1 = widget->property("widget#1").value<QSpinBox*>();
        auto* spin2 = widget->property("widget#2").value<QSpinBox*>();

        spin0->setValue(static_cast<int>(_val[0]));
        spin1->setValue(static_cast<int>(_val[1]));
        spin2->setValue(static_cast<int>(_val[2]));
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::real>(const double& _val, std::string _key)
{
    this->block_signals(true);
    QObject* child = this->get_param_widget(_key);

    if(auto* spinbox = qobject_cast<QDoubleSpinBox*>(child); spinbox != nullptr)
    {
        spinbox->setValue(_val);
        update_data<sight::data::real>(spinbox, _val);
    }
    else if(auto* slider = qobject_cast<QSlider*>(child); slider != nullptr)
    {
        const double min         = slider->property("min").toDouble();
        const double max         = slider->property("max").toDouble();
        const double value_range = max - min;
        const int slider_val     = int(std::round(
                                           ((std::max(
                                                 _val,
                                                 min
                                             ) - min) / value_range) * double(slider->maximum())
        ));
        slider->setValue(slider_val);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::dvec2>(const sight::data::dvec2::array_t& _val, std::string _key)
{
    this->block_signals(true);
    QObject* widget = this->get_param_widget(_key);

    if(widget != nullptr)
    {
        auto* spin0 = widget->property("widget#0").value<QDoubleSpinBox*>();
        auto* spin1 = widget->property("widget#1").value<QDoubleSpinBox*>();

        spin0->setValue(_val[0]);
        spin1->setValue(_val[1]);
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<>
void settings::set_parameter<sight::data::dvec3>(const sight::data::dvec3::array_t& _val, std::string _key)
{
    this->block_signals(true);
    QObject* widget = this->get_param_widget(_key);

    if(widget != nullptr)
    {
        auto* spin0 = widget->property("widget#0").value<QDoubleSpinBox*>();
        auto* spin1 = widget->property("widget#1").value<QDoubleSpinBox*>();
        auto* spin2 = widget->property("widget#2").value<QDoubleSpinBox*>();

        spin0->setValue(_val[0]);
        spin1->setValue(_val[1]);
        spin2->setValue(_val[2]);
    }

    this->block_signals(false);
}

//------------------------------------------------------------------------------

template<class DATATYPE, class SUBTYPE>
requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
std::shared_ptr<const DATATYPE> settings::data(const QObject* _widget)
{
    if(const auto data_index = _widget->property(qt_property::data_index); data_index.isValid())
    {
        sight::data::mt::locked_ptr<const sight::data::object> lock;
        sight::data::object::csptr obj;
        const std::string key = _widget->property(qt_property::key).toString().toStdString();
        const auto map        = m_settings_map.lock();
        if(map)
        {
            obj = map->at(key);
        }
        else
        {
            lock = m_settings[static_cast<std::size_t>(data_index.toUInt())].const_lock();
            obj  = lock.get_shared();
        }

        const auto typed_obj = std::dynamic_pointer_cast<const DATATYPE>(obj);
        return typed_obj;
    }

    return nullptr;
}

//------------------------------------------------------------------------------

template<class DATATYPE, class SUBTYPE>
requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
void settings::connect_data(const CSPTR(DATATYPE)& _obj, const std::string& _key)
{
    const auto sig  = _obj->template signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    const auto slot = core::com::new_slot(
        [_obj, _key, this]()
        {
            this->set_parameter<DATATYPE, SUBTYPE>(_obj->value(), _key);
        });
    slot->set_worker(this->worker());

    m_settings_slots[_key] = slot;
    sig->connect(slot);
}

//------------------------------------------------------------------------------

template<class DATATYPE, class SUBTYPE>
requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
void settings::update_data(const QObject* _widget, const SUBTYPE& _val)
{
    if(const auto data_index = _widget->property(qt_property::data_index); data_index.isValid())
    {
        sight::data::mt::locked_ptr<sight::data::object> lock;
        sight::data::object::sptr obj;
        const std::string key = _widget->property(qt_property::key).toString().toStdString();
        const auto map        = m_settings_map.lock();
        if(map)
        {
            obj = map->at(key);
        }
        else
        {
            lock = m_settings[static_cast<std::size_t>(data_index.toUInt())].lock();
            obj  = lock.get_shared();
        }

        const auto typed_obj = std::dynamic_pointer_cast<DATATYPE>(obj);
        typed_obj->set_value(_val);

        if(not m_block_signals)
        {
            const auto sig = obj->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            core::com::connection::blocker block(sig->get_connection(m_settings_slots[key]));
            sig->async_emit();
        }
    }
}

//-----------------------------------------------------------------------------

void settings::joystick_axis_direction_event(const sight::io::joystick::axis_direction_event& _event)
{
    using direction_t = sight::io::joystick::axis_direction_event::direction_t;

    SIGHT_DEBUG("Joystick alias: " << this->to_string(_event.device->alias));
    SIGHT_DEBUG("Joystick axis alias: " << this->to_string(_event.axis_alias));
    SIGHT_DEBUG("Joystick axis: " << int(_event.axis));
    SIGHT_DEBUG("Joystick direction: " << this->to_string(_event.value));

    for(const auto& [widget_key, widget_joystick] : m_widget_joysticks)
    {
        auto* const widget = get_param_widget(widget_key);

        if(auto* const button_group = dynamic_cast<QButtonGroup*>(widget); button_group != nullptr)
        {
            // Filter the joystick event to only handle the right joystick
            if((_event.axis_alias != widget_joystick.axis_1
                && _event.axis_alias != widget_joystick.axis_2)
               || _event.device->alias != widget_joystick.alias)
            {
                continue;
            }

            // Get the list of buttons
            const auto& buttons = button_group->buttons();

            // Get the checked button
            auto* const checked_button = button_group->checkedButton();

            // Find the current index of the checked button
            const auto checked_index = buttons.indexOf(checked_button);

            // Increment / decrement the index according to the joystick event
            if(_event.axis_alias == widget_joystick.axis_1 && _event.value == direction_t::left)
            {
                const auto go_left_or_down =
                    [&buttons](auto _next_index) -> auto
                    {
                        while(--_next_index >= 0)
                        {
                            if(auto* const next_button = buttons[_next_index];
                               next_button->isEnabled() && next_button->isVisible())
                            {
                                next_button->toggle();
                                return _next_index;
                            }
                        }

                        return _next_index;
                    };

                auto next_index = go_left_or_down(checked_index);

                if(next_index < 0)
                {
                    go_left_or_down(buttons.size());
                }
            }
            else if(_event.axis_alias == widget_joystick.axis_1 && _event.value == direction_t::right)
            {
                const auto go_right_or_up =
                    [&buttons](auto _next_index) -> auto
                    {
                        while(++_next_index < buttons.size())
                        {
                            if(auto* const next_button = buttons[_next_index];
                               next_button->isEnabled() && next_button->isVisible())
                            {
                                next_button->toggle();
                                return _next_index;
                            }
                        }

                        return _next_index;
                    };

                auto next_index = go_right_or_up(checked_index);

                if(next_index >= buttons.size())
                {
                    go_right_or_up(-1);
                }
            }
            else if(_event.axis_alias == widget_joystick.axis_2 && _event.value == direction_t::backward
                    && checked_button != nullptr)
            {
                checked_button->animateClick();
                button_group->idClicked(static_cast<int>(checked_index));
            }
        }
        else if(auto* const non_linear_slider = dynamic_cast<sight::ui::qt::widget::non_linear_slider*>(widget);
                non_linear_slider != nullptr)
        {
            // Filter the joystick event to only handle the right joystick
            if(_event.axis_alias != widget_joystick.axis_1
               || _event.device->alias != widget_joystick.alias)
            {
                continue;
            }

            if(_event.value == direction_t::left)
            {
                const int new_index = int(non_linear_slider->index()) - 1;
                non_linear_slider->set_index(new_index < 0 ? std::size_t(0) : std::size_t(new_index));
            }
            else if(_event.value == direction_t::right)
            {
                const int new_index  = int(non_linear_slider->index()) + 1;
                const int last_index = std::max(0, int(non_linear_slider->num_values()) - 1);
                non_linear_slider->set_index(new_index > last_index ? std::size_t(last_index) : std::size_t(new_index));
            }
        }
        else if(auto* const slider = dynamic_cast<QSlider*>(widget); slider != nullptr)
        {
            // Filter the joystick event to only handle the right joystick
            if((_event.axis_alias != widget_joystick.axis_1
                && _event.axis_alias != widget_joystick.axis_2)
               || _event.device->alias != widget_joystick.alias)
            {
                continue;
            }

            const auto get_step =
                [&_event, &slider]
                {
                    if(_event.value == direction_t::centered)
                    {
                        return 0;
                    }

                    if(_event.axis == 0)
                    {
                        return slider->singleStep();
                    }

                    if(_event.axis == 4)
                    {
                        if(slider->property("decimals").isNull() || _event.count < 10)
                        {
                            return slider->singleStep();
                        }

                        if(_event.count < 20)
                        {
                            return std::max(slider->singleStep(), slider->pageStep() / 4);
                        }

                        if(_event.count < 40)
                        {
                            return std::max(slider->singleStep(), slider->pageStep() / 3);
                        }

                        if(_event.count < 80)
                        {
                            return std::max(slider->singleStep(), slider->pageStep() / 2);
                        }

                        if(_event.count < 160)
                        {
                            return std::max(slider->singleStep(), slider->pageStep());
                        }
                    }

                    return slider->singleStep();
                };

            if(_event.axis_alias == widget_joystick.axis_2)
            {
                if(_event.value == direction_t::left)
                {
                    slider->setValue(slider->value() - get_step());
                }
                else if(_event.value == direction_t::right)
                {
                    slider->setValue(slider->value() + get_step());
                }
            }
            else if(_event.axis_alias == widget_joystick.axis_1)
            {
                if(_event.value == direction_t::right)
                {
                    slider->setValue(slider->value() + get_step());
                }
                else if(_event.value == direction_t::left)
                {
                    slider->setValue(slider->value() - get_step());
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui::qt
