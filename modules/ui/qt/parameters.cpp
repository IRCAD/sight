/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "parameters.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/object.hpp>

#include <data/tools/color.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>
#include <ui/qt/widget/non_linear_slider.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tokenizer.hpp>

#include <QAbstractButton>
#include <QApplication>
#include <QButtonGroup>
#include <QColorDialog>
#include <QEvent>
#include <QFormLayout>
#include <QGraphicsColorizeEffect>
#include <QGroupBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QString>
#include <QStyle>
#include <QToolButton>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

namespace
{

// Internal static function to split a string using separator (usually =), mainly used to split enum into value, data
std::pair<std::string, std::string> split_string(const std::string& _str, const std::string& _sep = "=")
{
    std::string left;
    std::string right;

    const boost::char_separator<char> subsep(_sep.c_str());
    const boost::tokenizer<boost::char_separator<char> > subtokens {_str, subsep};
    auto it = subtokens.begin();

    if(it != subtokens.end())
    {
        left = *it;
        ++it;
        // check if we have another part
        if(it != subtokens.end())
        {
            right = *it;
        }
    }

    return {left, right};
}

} // anonymous namespace

//-----------------------------------------------------------------------------

parameters::parameters() noexcept
{
    new_signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG);
    new_signal<signals::boolean_changed_signal_t>(signals::BOOLEAN_CHANGED_SIG);
    new_signal<signals::color_changed_signal_t>(signals::COLOR_CHANGED_SIG);
    new_signal<signals::double_changed_signal_t>(signals::DOUBLE_CHANGED_SIG);
    new_signal<signals::double2_changed_signal_t>(signals::DOUBLE2_CHANGED_SIG);
    new_signal<signals::double3_changed_signal_t>(signals::DOUBLE3_CHANGED_SIG);
    new_signal<signals::integer_changed_signal_t>(signals::INTEGER_CHANGED_SIG);
    new_signal<signals::integer2_changed_signal_t>(signals::INTEGER2_CHANGED_SIG);
    new_signal<signals::integer3_changed_signal_t>(signals::INTEGER3_CHANGED_SIG);
    new_signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG);
    new_signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG);

    new_slot(slots::SET_PARAMETER_SLOT, &parameters::set_parameter, this);
    new_slot(slots::SET_BOOL_PARAMETER_SLOT, &parameters::set_bool_parameter, this);
    new_slot(slots::SET_COLOR_PARAMETER_SLOT, &parameters::set_color_parameter, this);
    new_slot(slots::SET_DOUBLE_PARAMETER_SLOT, &parameters::set_double_parameter, this);
    new_slot(slots::SET_DOUBLE2_PARAMETER_SLOT, &parameters::set_double2_parameter, this);
    new_slot(slots::SET_DOUBLE3_PARAMETER_SLOT, &parameters::set_double3_parameter, this);
    new_slot(slots::SET_INT_PARAMETER_SLOT, &parameters::set_int_parameter, this);
    new_slot(slots::SET_INT2_PARAMETER_SLOT, &parameters::set_int2_parameter, this);
    new_slot(slots::SET_INT3_PARAMETER_SLOT, &parameters::set_int3_parameter, this);
    new_slot(slots::SET_ENUM_PARAMETER_SLOT, &parameters::set_enum_parameter, this);
    new_slot(slots::SET_ENUM_INDEX_PARAMETER_SLOT, &parameters::set_enum_index_parameter, this);
    new_slot(slots::UPDATE_ENUM_RANGE_SLOT, &parameters::update_enum_range, this);
    new_slot(slots::UPDATE_INT_MIN_PARAMETER_SLOT, &parameters::update_int_min_parameter, this);
    new_slot(slots::UPDATE_INT_MAX_PARAMETER_SLOT, &parameters::update_int_max_parameter, this);
    new_slot(slots::UPDATE_DOUBLE_MIN_PARAMETER_SLOT, &parameters::update_double_min_parameter, this);
    new_slot(slots::UPDATE_DOUBLE_MAX_PARAMETER_SLOT, &parameters::update_double_max_parameter, this);
}

//-----------------------------------------------------------------------------

void parameters::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

void parameters::starting()
{
    this->create();

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(QString::fromStdString(service_id));

    QScrollArea* scroll_area = nullptr;
    QWidget* viewport        = nullptr;

    service::config_t config                = this->get_config();
    const service::config_t& parameters_cfg = config.get_child("parameters");
    const bool scrollable                   = parameters_cfg.get<bool>("<xmlattr>.scrollable", false);

    if(scrollable)
    {
        scroll_area = new QScrollArea(qt_container->get_qt_container()->parentWidget());
        scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        viewport = new QWidget(qt_container->get_qt_container());
    }

    auto* layout = new QFormLayout {};
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    this->blockSignals(true);

    // We don't support having multiple parameters with the same key (this isn't a good idea any way).
    // This set keeps tracks of the ones we add, and triggers and assert if it already existed.
    [[maybe_unused]] std::set<std::string> keys;

    // Create widgets
    for(const auto& param : boost::make_iterator_range(parameters_cfg.equal_range("param")))
    {
        const service::config_t& cfg = param.second;

        const auto orientation = cfg.get<std::string>("<xmlattr>.orientation", "horizontal") == "horizontal"
                                 ? Qt::Horizontal
                                 : Qt::Vertical;

        const auto param_box_direction =
            orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

        const auto type       = cfg.get<std::string>("<xmlattr>.type");
        const auto min_width  = cfg.get<int>("<xmlattr>.min_width", 0);
        const auto min_height = cfg.get<int>("<xmlattr>.min_height", 0);

        const param_widget widget
        {
            .name          = cfg.get<std::string>("<xmlattr>.name"),
            .key           = cfg.get<std::string>("<xmlattr>.key"),
            .default_value = cfg.get<std::string>("<xmlattr>.defaultValue"),
            .reset_button  = cfg.get<bool>("<xmlattr>.reset", true),
            .hide_min_max  = cfg.get<bool>("<xmlattr>.hideMinMax", false),
            .min_size      = {min_width, min_height},
        };

        SIGHT_ASSERT(
            get_id() << ": Key " << std::quoted(widget.key) << " already exists.",
            keys.insert(widget.key).second
        );

        auto* const param_box = new QGroupBox {scroll_area == nullptr ? qt_container->get_qt_container() : scroll_area};
        {
            const auto qt_key = QString::fromStdString(widget.key) + "_box";

            // This intermediate widget shouldn't be visible
            // Note that because we're setting this stylesheet on the fly on this "top-level" widget, we must
            // apply it again on its children: children inherits the stylesheet of their parents

            // Note that these widgets aren't required for display purposes, but having them greatly simplifies the
            // search done in get_param_widget.

            param_box->setStyleSheet("background-color: rgba(0,0,0,0); border: 0px;");
            param_box->setContentsMargins(0, 0, 0, 0);

            param_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            param_box->setProperty("key", qt_key);
            param_box->setObjectName(qt_key);

            m_param_boxes.emplace_back(param_box);
        }

        auto* const param_box_layout = new QBoxLayout {param_box_direction, param_box};

        auto* const row_layout = new QHBoxLayout {};
        row_layout->addWidget(param_box);

        // Label
        {
            QLabel* parameter_label = nullptr;
            if(!widget.name.empty())
            {
                parameter_label = new QLabel(QString::fromStdString(widget.name));
                parameter_label->setWordWrap(true);
                parameter_label->setAlignment(Qt::AlignCenter);
                parameter_label->setStyleSheet("margin: -0.1em;");
            }

            if(parameter_label != nullptr)
            {
                // When horizontal, the label is added to the "main" widget layout, otherwise, at the top of the box
                if(orientation == Qt::Vertical)
                {
                    param_box_layout->addWidget(parameter_label);
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
        }

        if(type == "bool")
        {
            if(auto* reset = this->create_bool_widget(*param_box_layout, widget))
            {
                row_layout->addWidget(reset);
            }
        }
        else if(type == "color")
        {
            if(auto* reset = this->create_color_widget(*param_box_layout, widget))
            {
                row_layout->addWidget(reset);
            }
        }
        else if(type == "double" || type == "double2" || type == "double3")
        {
            const std::string widget_type = cfg.get<std::string>("<xmlattr>.widget", "spin");

            const double min = cfg.get<double>("<xmlattr>.min", 0.);
            const double max = cfg.get<double>("<xmlattr>.max", 1.);

            const double_widget widget_double = {widget, min, max};

            const int count = (type == "double3") ? 3 : (type == "double2" ? 2 : 1);

            if(widget_type == "spin")
            {
                if(auto* reset = this->create_double_widget(*param_box_layout, widget_double, count, orientation))
                {
                    row_layout->addWidget(reset);
                }
            }
            else if(widget_type == "slider")
            {
                // TODO: this could be supported now
                SIGHT_ASSERT(get_id() << ": Count > 1 is not supported with sliders", count == 1);

                const std::uint8_t decimals = cfg.get<std::uint8_t>("<xmlattr>.decimals", 2);
                const bool on_release       = cfg.get<bool>("<xmlattr>.emitOnRelease", false);

                if(auto* reset =
                       this->create_double_slider_widget(
                           *param_box_layout,
                           widget_double,
                           decimals,
                           orientation,
                           on_release
                       )
                )
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
            else
            {
                SIGHT_ERROR(
                    get_id() << ": Unknown widget type : " << std::quoted(widget_type) << " for "
                    << std::quoted(widget.name)
                    << ". Must be 'spin' or 'slider'."
                );
            }
        }
        else if(type == "int" || type == "int2" || type == "int3")
        {
            const std::string widget_type = cfg.get<std::string>("<xmlattr>.widget", "slider");

            const int min               = cfg.get<int>("<xmlattr>.min", 0);
            const int max               = cfg.get<int>("<xmlattr>.max", 100);
            const int_widget widget_int = {widget, min, max};

            const int count = (type == "int3") ? 3 : (type == "int2" ? 2 : 1);

            if(widget_type == "spin")
            {
                if(auto* reset = this->create_integer_spin_widget(*param_box_layout, widget_int, count, orientation))
                {
                    row_layout->addWidget(reset);
                }
            }
            else if(widget_type == "slider")
            {
                // TODO: this could be supported now
                SIGHT_ASSERT(get_id() << ": Count > 1 is not supported with sliders", count == 1);

                const bool on_release = cfg.get<bool>("<xmlattr>.emitOnRelease", false);
                if(auto* reset =
                       this->create_integer_slider_widget(*param_box_layout, widget_int, orientation, on_release))
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
            else
            {
                SIGHT_ERROR(
                    get_id() << ": Unknown widget type : " << std::quoted(widget_type) << " for " << widget.name
                    << ". Must be 'spin' or 'slider'."
                );
            }
        }
        else if(type == "enum")
        {
            const auto widget_type = cfg.get<std::string>("<xmlattr>.widget", "combobox");

            if(widget_type == "combobox")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                // split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::parameters::parse_enum_string(options, values, data);
                this->create_enum_widget(*param_box_layout, widget, values, data);
            }
            else if(widget_type == "slider")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                // split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::parameters::parse_enum_string(options, values, data);
                const bool on_release = cfg.get<bool>("<xmlattr>.emitOnRelease", false);

                this->create_slider_enum_widget(*param_box_layout, widget, values, orientation, on_release);
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

                this->create_button_bar_enum_widget(
                    *param_box_layout,
                    widget,
                    button_list,
                    width,
                    height,
                    h_offset,
                    style,
                    orientation
                );
            }
        }
    }

    for(const auto& param : boost::make_iterator_range(parameters_cfg.equal_range("param")))
    {
        const service::config_t& cfg = param.second;

        const auto key                  = cfg.get<std::string>("<xmlattr>.key");
        const std::string depends       = cfg.get<std::string>("<xmlattr>.depends", "");
        const std::string depends_value = cfg.get<std::string>("<xmlattr>.dependsValue", "");
        const bool dependsreverse       = cfg.get<bool>("<xmlattr>.dependsReverse", false);

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
                        [ = ]{on_depends_changed(check_box, widget, dependsreverse);});
                    on_depends_changed(check_box, widget, dependsreverse);
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
                            [ = ]{on_depends_changed(combo_box, widget, depends_value, dependsreverse);});
                        on_depends_changed(combo_box, widget, depends_value, dependsreverse);
                    }
                }
            }
        }
    }

    if(scroll_area != nullptr)
    {
        viewport->setLayout(layout);
        scroll_area->setWidgetResizable(true);
        scroll_area->setWidget(viewport);
        auto* main_layout = new QGridLayout();
        qt_container->set_layout(main_layout);
        main_layout->addWidget(scroll_area);

        // The size of the vertical scroll bar isn't taken into account when the QGridLayout fill the space, as such
        // some buttons (particularly reset buttons) get hidden. The workaround is to biggen the right margin a little.
        int scroll_bar_width = QScrollBar().sizeHint().width();
        QMargins margins     = layout->contentsMargins();
        margins.setRight(scroll_bar_width);
        layout->setContentsMargins(margins);
    }
    else
    {
        qt_container->set_layout(layout);
    }

    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void parameters::updating()
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    service::config_t config                = this->get_config();
    const service::config_t& parameters_cfg = config.get_child("parameters");

    // emit signal for each widget
    for(const auto& param : boost::make_iterator_range(parameters_cfg.equal_range("param")))
    {
        const service::config_t& cfg = param.second;

        const auto key  = cfg.get<std::string>("<xmlattr>.key");
        const auto type = cfg.get<std::string>("<xmlattr>.type");

        if(auto* const child = get_param_widget(key); child != nullptr)
        {
            if(type == "bool")
            {
                const QCheckBox* box = dynamic_cast<QCheckBox*>(child);
                SIGHT_ASSERT(get_id() << ": Widget must be a QCheckBox", box);

                const bool state = (box->checkState() == Qt::Checked);

                if(!m_block_signals)
                {
                    this->signal<signals::boolean_changed_signal_t>(signals::BOOLEAN_CHANGED_SIG)
                    ->async_emit(state, key);
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(state, key);
                    SIGHT_DEBUG(
                        get_id() << ": [EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << std::boolalpha << state
                        << std::noboolalpha << ", " << key << ")"
                    );
                }
            }
            else if(type == "color")
            {
                const auto color_qt = child->property("color").value<QColor>();
                this->emit_color_signal(color_qt, key);
            }
            else if(type == "double" || type == "double2" || type == "double3")
            {
                this->emit_double_signal(child);
            }
            else if(type == "int" || type == "int2" || type == "int3")
            {
                this->emit_integer_signal(child);
            }
            else if(type == "enum")
            {
                if(auto* box = qobject_cast<QComboBox*>(child); box != nullptr)
                {
                    const QString data = box->itemData(box->currentIndex()).toString();

                    if(!m_block_signals)
                    {
                        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
                        ->async_emit(data.toStdString(), key);
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                        ->async_emit(data.toStdString(), key);
                        SIGHT_DEBUG(
                            get_id() << ": [EMIT] " << signals::ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key
                            << ")"
                        );
                        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                        ->async_emit(box->currentIndex(), key);
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                        ->async_emit(box->currentIndex(), key);
                        SIGHT_DEBUG(
                            get_id() << ": [EMIT] " << signals::ENUM_INDEX_CHANGED_SIG << "(" << box->currentIndex() << ", "
                            << key << ")"
                        );
                    }
                }
                else if(auto* slider = qobject_cast<sight::ui::qt::widget::non_linear_slider*>(child);
                        slider != nullptr)
                {
                    if(!m_block_signals)
                    {
                        int value = slider->value();
                        this->signal<signals::integer_changed_signal_t>(signals::INTEGER_CHANGED_SIG)
                        ->async_emit(value, key);
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(value, key);
                        SIGHT_DEBUG(
                            get_id() << ": [EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << value << ", " << key
                            << ")"
                        );
                        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
                        ->async_emit(std::to_string(value), key);
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                        ->async_emit(std::to_string(value), key);
                        SIGHT_DEBUG(
                            get_id() << ": [EMIT] " << signals::ENUM_CHANGED_SIG << "(" << value << ", " << key
                            << ")"
                        );
                        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                        ->async_emit(int(slider->index()), key);
                    }
                }
                else if(auto* button_group = qobject_cast<QButtonGroup*>(child); button_group != nullptr)
                {
                    if(auto* checked_button = button_group->checkedButton(); checked_button != nullptr)
                    {
                        // Find the currently checked button
                        const int button_index = button_group->buttons().indexOf(checked_button);

                        const std::string value = checked_button->property("value").toString().toStdString();
                        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)->async_emit(value, key);
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(value, key);
                        SIGHT_DEBUG(
                            get_id() << ": [EMIT] " << signals::ENUM_CHANGED_SIG << "(" << value << ", " << key
                            << ")"
                        );
                        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                        ->async_emit(button_index, key);
                    }
                }
                else
                {
                    SIGHT_ASSERT(get_id() << ": Widget must either be a ComboBox or non_linear_slider", false);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::stopping()
{
    m_param_boxes.clear(); // Avoid keeping dangling pointers
    this->destroy();
}

//-----------------------------------------------------------------------------

bool parameters::eventFilter(QObject* _watched, QEvent* _event)
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

void parameters::on_depends_changed(QCheckBox* _check_box, QWidget* _widget, bool _reverse)
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

void parameters::on_depends_changed(QComboBox* _combo_box, QWidget* _widget, const std::string& _value, bool _reverse)
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

//-----------------------------------------------------------------------------

void parameters::on_change_enum(int _value) const
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();

    const auto* box = dynamic_cast<const QComboBox*>(sender);

    SIGHT_ASSERT(get_id() << ": Wrong widget type", box);

    const QString data = box->itemData(_value).toString();

    if(!m_block_signals)
    {
        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
        ->async_emit(data.toStdString(), key.toStdString());
        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
        ->async_emit(data.toStdString(), key.toStdString());
        SIGHT_DEBUG(
            get_id() << ": [EMIT] " << signals::ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key.toStdString()
            << ")"
        );
        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
        ->async_emit(_value, key.toStdString());
        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
        ->async_emit(box->currentIndex(), key.toStdString());
        SIGHT_DEBUG(
            get_id() << ": [EMIT] " << signals::ENUM_INDEX_CHANGED_SIG << "(" << _value << ", " << key.toStdString()
            << ")"
        );
    }
}

//-----------------------------------------------------------------------------

void parameters::on_change_boolean(int _value) const
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();
    const bool checked    = _value == Qt::Checked;

    if(!m_block_signals)
    {
        this->signal<signals::boolean_changed_signal_t>(signals::BOOLEAN_CHANGED_SIG)
        ->async_emit(checked, key.toStdString());
        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(checked, key.toStdString());
        SIGHT_DEBUG(
            get_id() << ": [EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (checked ? "true" : "false") << ", "
            << key.toStdString() << ")"
        );
    }
}

//------------------------------------------------------------------------------

void parameters::on_color_button()
{
    QObject* sender = this->sender();

    // Create Color choice dialog.
    auto qt_container        = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    QWidget* const container = qt_container->get_qt_container();
    SIGHT_ASSERT(get_id() << ": Container not instantiated yet.", container);

    const auto old_color  = sender->property("color").value<QColor>();
    const QColor color_qt = QColorDialog::getColor(old_color, container);
    if(color_qt.isValid())
    {
        const QString key = sender->property("key").toString();

        auto* colour_button = dynamic_cast<QPushButton*>(sender);
        colour_button->setProperty("color", color_qt);

        int icon_size = colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(icon_size, icon_size);
        pix.fill(color_qt);

        colour_button->setIcon(QIcon(pix));

        this->emit_color_signal(color_qt, key.toStdString());
    }
}

//-----------------------------------------------------------------------------

void parameters::on_change_integer(int /*unused*/) const
{
    QObject* sender = this->sender();
    this->emit_integer_signal(sender);
}

//------------------------------------------------------------------------------

void parameters::emit_integer_signal(QObject* _widget) const
{
    if(!m_block_signals)
    {
        SIGHT_ASSERT(get_id() << ": Widget is null", _widget != nullptr);

        const QString key = _widget->property("key").toString();
        const int count   = _widget->property("count").toInt();

        SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

        const auto* spinbox = dynamic_cast<const QSpinBox*>(_widget);
        const auto* slider  = dynamic_cast<const QSlider*>(_widget);
        SIGHT_ASSERT(get_id() << ": Wrong widget type", spinbox || slider);

        if(count == 1)
        {
            int value = 0;
            if(spinbox != nullptr)
            {
                value = spinbox->value();
            }
            else
            {
                value = slider->value();
            }

            this->signal<signals::integer_changed_signal_t>(signals::INTEGER_CHANGED_SIG)
            ->async_emit(value, key.toStdString());
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
            ->async_emit(value, key.toStdString());
            SIGHT_DEBUG(
                get_id() << ": [EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << value << ", " << key.toStdString()
                << ")"
            );
        }
        else
        {
            int value1 = 0;
            int value2 = 0;

            if(spinbox != nullptr)
            {
                const QSpinBox* spin1 = _widget->property("widget#0").value<QSpinBox*>();
                const QSpinBox* spin2 = _widget->property("widget#1").value<QSpinBox*>();

                value1 = spin1->value();
                value2 = spin2->value();
            }
            else
            {
                const QSlider* spin1 = _widget->property("widget#0").value<QSlider*>();
                const QSlider* spin2 = _widget->property("widget#1").value<QSlider*>();

                value1 = spin1->value();
                value2 = spin2->value();
            }

            if(count == 2)
            {
                this->signal<signals::integer2_changed_signal_t>(signals::INTEGER2_CHANGED_SIG)
                ->async_emit(value1, value2, key.toStdString());
                const sight::ui::int2_t values = {value1, value2};
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                ->async_emit(values, key.toStdString());
                SIGHT_DEBUG(
                    get_id() << ": [EMIT] " << signals::INTEGER2_CHANGED_SIG << "(" << value1 << ", " << value2 << ", "
                    << key.toStdString() << ")"
                );
            }
            else
            {
                int value3 = 0;
                if(spinbox != nullptr)
                {
                    const QSpinBox* spin3 = _widget->property("widget#2").value<QSpinBox*>();
                    value3 = spin3->value();
                }
                else
                {
                    const QSlider* spin3 = _widget->property("widget#2").value<QSlider*>();
                    value3 = spin3->value();
                }

                this->signal<signals::integer3_changed_signal_t>(signals::INTEGER3_CHANGED_SIG)
                ->async_emit(value1, value2, value3, key.toStdString());
                const sight::ui::int3_t values = {value1, value2, value3};
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                ->async_emit(values, key.toStdString());
                SIGHT_DEBUG(
                    get_id() << ": [EMIT] " << signals::INTEGER3_CHANGED_SIG << "(" << value1 << ", " << value2 << ", "
                    << value3 << ", " << key.toStdString() << ")"
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::on_change_double(double /*unused*/) const
{
    QObject* sender = this->sender();
    this->emit_double_signal(sender);
}

//------------------------------------------------------------------------------

void parameters::emit_double_signal(QObject* _widget) const
{
    if(!m_block_signals)
    {
        const QString key = _widget->property("key").toString();
        const int count   = _widget->property("count").toInt();

        auto* spinbox = qobject_cast<QDoubleSpinBox*>(_widget);
        auto* slider  = qobject_cast<QSlider*>(_widget);

        if(slider != nullptr)
        {
            const double value = sight::module::ui::qt::parameters::get_double_slider_value(slider);
            this->signal<signals::double_changed_signal_t>(signals::DOUBLE_CHANGED_SIG)
            ->async_emit(value, key.toStdString());
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
            ->async_emit(value, key.toStdString());
            SIGHT_DEBUG(
                get_id() << ": [EMIT] " << signals::DOUBLE_CHANGED_SIG << "(" << value << ", " << key.toStdString()
                << ")"
            );
        }
        else if(spinbox != nullptr)
        {
            SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

            if(count == 1)
            {
                this->signal<signals::double_changed_signal_t>(signals::DOUBLE_CHANGED_SIG)
                ->async_emit(spinbox->value(), key.toStdString());
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                ->async_emit(spinbox->value(), key.toStdString());
                SIGHT_DEBUG(
                    get_id() << ": [EMIT] " << signals::DOUBLE_CHANGED_SIG << "(" << spinbox->value() << ", "
                    << key.toStdString() << ")"
                );
            }
            else
            {
                const QDoubleSpinBox* spin1 = spinbox->property("widget#0").value<QDoubleSpinBox*>();
                const QDoubleSpinBox* spin2 = spinbox->property("widget#1").value<QDoubleSpinBox*>();

                const double value1 = spin1->value();
                const double value2 = spin2->value();

                if(count == 2)
                {
                    this->signal<signals::double2_changed_signal_t>(signals::DOUBLE2_CHANGED_SIG)
                    ->async_emit(value1, value2, key.toStdString());
                    const sight::ui::double2_t values = {value1, value2};
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                    ->async_emit(values, key.toStdString());
                    SIGHT_DEBUG(
                        get_id() << ": [EMIT] " << signals::DOUBLE2_CHANGED_SIG << "(" << value1 << ", " << value2 << ", "
                        << key.toStdString() << ")"
                    );
                }
                else
                {
                    const QDoubleSpinBox* spin3 = spinbox->property("widget#2").value<QDoubleSpinBox*>();
                    const double value3         = spin3->value();

                    this->signal<signals::double3_changed_signal_t>(signals::DOUBLE3_CHANGED_SIG)
                    ->async_emit(value1, value2, value3, key.toStdString());
                    const sight::ui::double3_t values = {value1, value2, value3};
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                    ->async_emit(values, key.toStdString());
                    SIGHT_DEBUG(
                        get_id() << ": [EMIT] " << signals::DOUBLE3_CHANGED_SIG << "(" << value1 << ", " << value2 << ", "
                        << value3 << ", " << key.toStdString() << ")"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::on_change_double_slider(int /*unused*/) const
{
    auto* sender = qobject_cast<QSlider*>(this->sender());
    this->emit_double_signal(sender);
}

//-----------------------------------------------------------------------------

void parameters::on_slider_mapped(QLabel* _label, QSlider* _slider)
{
    _label->setText(QString::number(_slider->value()));
}

//-----------------------------------------------------------------------------

void parameters::on_slider_range_mapped(QLabel* _min_label, QLabel* _max_label, QSlider* _slider)
{
    const int min = _slider->minimum();
    const int max = _slider->maximum();

    _min_label->setText(QString::number(min));
    _max_label->setText(QString::number(max));
}

//-----------------------------------------------------------------------------

void parameters::on_double_slider_mapped(QLabel* _label, QSlider* _slider)
{
    const double new_value = get_double_slider_value(_slider);
    const int decimals     = _slider->property("decimals").toInt();

    _label->setText(QString::number(new_value, 'f', decimals));
}

//-----------------------------------------------------------------------------

void parameters::on_double_slider_range_mapped(QLabel* _min_label, QLabel* _max_label, QSlider* _slider)
{
    const double min   = _slider->property("min").toDouble();
    const double max   = _slider->property("max").toDouble();
    const int decimals = _slider->property("decimals").toInt();

    _min_label->setText(QString::number(min, 'g', decimals));
    _max_label->setText(QString::number(max, 'g', decimals));
}

//-----------------------------------------------------------------------------

void parameters::on_reset_boolean_mapped(QWidget* _widget) const
{
    auto* checkbox = qobject_cast<QCheckBox*>(_widget);
    if(checkbox != nullptr)
    {
        int value = checkbox->property("defaultValue").toInt();
        checkbox->setCheckState(Qt::CheckState(value));

        const QString key = checkbox->property("key").toString();
        if(!m_block_signals)
        {
            this->signal<signals::boolean_changed_signal_t>(signals::BOOLEAN_CHANGED_SIG)
            ->async_emit(value != 0, key.toStdString());
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
            ->async_emit(value != 0, key.toStdString());
            SIGHT_DEBUG(
                get_id() << ": [EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (value ? "true" : "false") << ", "
                << key.toStdString() << ")"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::on_reset_color_mapped(QWidget* _widget) const
{
    auto* colour_button = qobject_cast<QPushButton*>(_widget);
    if(colour_button != nullptr)
    {
        const auto color  = colour_button->property("defaultValue").value<QColor>();
        const QString key = colour_button->property("key").toString();

        int icon_size = colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(icon_size, icon_size);
        pix.fill(color);

        colour_button->setIcon(QIcon(pix));
        colour_button->setProperty("color", color);

        const std::array<std::uint8_t, 4> new_color =
        {
            static_cast<std::uint8_t>(color.red()),
            static_cast<std::uint8_t>(color.green()),
            static_cast<std::uint8_t>(color.blue()),
            static_cast<std::uint8_t>(color.alpha())
        };

        if(!m_block_signals)
        {
            this->signal<signals::color_changed_signal_t>(signals::COLOR_CHANGED_SIG)
            ->async_emit(new_color, key.toStdString());
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
            ->async_emit(new_color, key.toStdString());
            SIGHT_DEBUG(
                get_id() << ": [EMIT] " << signals::COLOR_CHANGED_SIG << "(" << int(new_color[0]) << ", "
                << int(new_color[1])
                << ", " << int(new_color[2]) << ", " << int(new_color[3]) << ", " << key.toStdString()
                << ")"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::on_reset_integer_mapped(QWidget* _widget)
{
    auto* slider  = dynamic_cast<QSlider*>(_widget);
    auto* spinbox = dynamic_cast<QSpinBox*>(_widget);
    this->blockSignals(true);
    if(slider != nullptr)
    {
        const int value = slider->property("defaultValue").toInt();
        slider->setValue(value);
    }
    else if(spinbox != nullptr)
    {
        const QString key = spinbox->property("key").toString();
        const int value   = spinbox->property("defaultValue").toInt();
        const int count   = spinbox->property("count").toInt();
        SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

        auto* spin1 = spinbox->property("widget#0").value<QSpinBox*>();
        spin1->setValue(value);

        if(count > 1)
        {
            auto* spin2 = spinbox->property("widget#1").value<QSpinBox*>();
            spin2->setValue(value);

            if(count == 3)
            {
                auto* spin3 = spinbox->property("widget#2").value<QSpinBox*>();
                spin3->setValue(value);
            }
        }
    }

    this->blockSignals(false);
    this->emit_integer_signal(_widget);
}

//-----------------------------------------------------------------------------

void parameters::on_reset_double_mapped(QWidget* _widget)
{
    auto* spinbox = qobject_cast<QDoubleSpinBox*>(_widget);
    auto* slider  = qobject_cast<QSlider*>(_widget);

    this->blockSignals(true);
    if(slider != nullptr)
    {
        const double value       = slider->property("defaultValue").toDouble();
        const double min         = slider->property("min").toDouble();
        const double max         = slider->property("max").toDouble();
        const double value_range = max - min;
        const int slider_val     = int(std::round(((value - min) / value_range) * double(slider->maximum())));
        slider->setValue(slider_val);
    }
    else if(spinbox != nullptr)
    {
        const QString key  = spinbox->property("key").toString();
        const double value = spinbox->property("defaultValue").toDouble();
        const int count    = spinbox->property("count").toInt();
        SIGHT_ASSERT(get_id() << ": Invalid widgets count, must be <= 3", count <= 3);

        auto* spin1 = spinbox->property("widget#0").value<QDoubleSpinBox*>();
        spin1->setValue(value);

        if(count > 1)
        {
            auto* spin2 = spinbox->property("widget#1").value<QDoubleSpinBox*>();
            spin2->setValue(value);

            if(count == 3)
            {
                auto* spin3 = spinbox->property("widget#2").value<QDoubleSpinBox*>();
                spin3->setValue(value);
            }
        }
    }

    this->blockSignals(false);
    this->emit_double_signal(_widget);
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* parameters::create_reset_button(const std::string& _key, std::function<void()> _on_click) const
{
    std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);
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
QPushButton* parameters::create_bool_widget(QBoxLayout& _layout, const param_widget& _setup) const
{
    auto* checkbox = new QCheckBox();
    {
        const auto key = QString::fromStdString(_setup.key);

        checkbox->setTristate(false);

        if(_setup.default_value == "true")
        {
            checkbox->setCheckState(Qt::Checked);
        }

        checkbox->setObjectName(key);
        checkbox->setProperty("key", key);

        checkbox->setProperty("defaultValue", checkbox->checkState());
        checkbox->setMinimumSize(_setup.min_size);

        checkbox->setStyleSheet(qApp->styleSheet());
        checkbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    _layout.addWidget(checkbox);

    // Forward to the Sight signal
    QObject::connect(checkbox, &QCheckBox::stateChanged, this, &parameters::on_change_boolean);

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, checkbox](){on_reset_boolean_mapped(checkbox);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* parameters::create_color_widget(QBoxLayout& _layout, const param_widget& _setup) const
{
    auto* colour_button = new QPushButton("Color");
    {
        colour_button->setObjectName(QString::fromStdString(_setup.key));
        colour_button->setToolTip(tr("Selected color"));

        std::string color_str = "#ffffffff";
        if(!_setup.default_value.empty())
        {
            std::array<std::uint8_t, 4> color {};

            data::tools::color::hexa_string_to_rgba(_setup.default_value, color);

            color_str = _setup.default_value;
        }

        std::array<std::uint8_t, 4> color {};
        data::tools::color::hexa_string_to_rgba(color_str, color);

        const int icon_size = colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        const QColor color_qt(color[0], color[1], color[2], color[3]);
        QPixmap pix(icon_size, icon_size);

        pix.fill(color_qt);

        colour_button->setIcon(QIcon(pix));

        colour_button->setProperty("key", QString::fromStdString(_setup.key));
        colour_button->setProperty("defaultValue", color_qt);
        colour_button->setProperty("color", color_qt);
        colour_button->setMinimumSize(_setup.min_size);

        colour_button->setStyleSheet(qApp->styleSheet());
        colour_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    _layout.addWidget(colour_button);

    QObject::connect(colour_button, &QPushButton::clicked, this, &parameters::on_color_button);

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, colour_button]{on_reset_color_mapped(colour_button);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* parameters::create_double_widget(
    QBoxLayout& _layout,
    const double_widget& _setup,
    int _count,
    Qt::Orientation _orientation
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* sub_layout = new QBoxLayout {layout_direction};
    {
        _layout.addLayout(sub_layout);
    }

    std::array<QDoubleSpinBox*, 3> spinboxes {};

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        auto* spinbox = new QDoubleSpinBox();
        spinbox->setObjectName(QString::fromStdString(_setup.key + "/" + std::to_string(i)));
        spinboxes[i] = spinbox;

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

        spinbox->setMinimumSize(_setup.min_size);

        // Set value last only after setting range and decimals, otherwise the value may be truncated
        double initial_value = std::midpoint(_setup.min, _setup.max);

        if(not _setup.default_value.empty())
        {
            try
            {
                initial_value = boost::lexical_cast<double>(_setup.default_value);
            }
            catch([[maybe_unused]] const boost::bad_lexical_cast& e)
            {
                SIGHT_ASSERT(
                    "Invalid value " << std::quoted(_setup.default_value) << ": not convertible to 'int/double': "
                    << e.what(),
                    false
                );
            }
        }

        spinbox->setValue(initial_value);

        spinbox->setProperty("key", QString::fromStdString(_setup.key));
        spinbox->setProperty("count", _count);
        spinbox->setProperty("defaultValue", spinbox->value());

        spinbox->setStyleSheet(qApp->styleSheet());
        spinbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        sub_layout->addWidget(spinbox);

        QObject::connect(
            spinbox,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &parameters::on_change_double
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
        return this->create_reset_button(_setup.key, [this, spinbox]{on_reset_double_mapped(spinbox);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* parameters::create_double_slider_widget(
    QBoxLayout& _layout,
    const double_widget& _setup,
    std::uint8_t _decimals,
    Qt::Orientation _orientation,
    bool _on_release
)
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    double initial_value = std::midpoint(_setup.min, _setup.max);

    if(not _setup.default_value.empty())
    {
        try
        {
            initial_value = boost::lexical_cast<double>(_setup.default_value);
        }
        catch([[maybe_unused]] const boost::bad_lexical_cast& e)
        {
            SIGHT_ASSERT(
                "Invalid value " << std::quoted(_setup.default_value) << ": not convertible to 'int/double': "
                << e.what(),
                false
            );
        }
    }

    auto* const sub_layout = new QBoxLayout {layout_direction};
    sub_layout->setSpacing(0);
    sub_layout->setContentsMargins(0, 0, 0, 0);
    _layout.addLayout(sub_layout);

    auto* const slider = new QSlider();
    {
        const double value_range = _setup.max - _setup.min;
        slider->setOrientation(_orientation);
        slider->setObjectName(QString::fromStdString(_setup.key));

        slider->setProperty("key", slider->objectName());
        slider->setProperty("count", 1);
        slider->setProperty("defaultValue", initial_value);
        slider->setProperty("decimals", _decimals);
        slider->setProperty("min", _setup.min);
        slider->setProperty("max", _setup.max);

        // tracking true: emit signal when value change, false: emit signal when slider released.
        slider->setTracking(!_on_release);

        set_double_slider_range(slider, initial_value);

        const int default_slider_value =
            int(std::round(((initial_value - _setup.min) / value_range) * double(slider->maximum())));
        slider->setValue(default_slider_value);
        slider->setMinimumSize(_setup.min_size);

        slider->setProperty("widget#0", QVariant::fromValue<QSlider*>(slider));

        slider->setStyleSheet(qApp->styleSheet());
        slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    QFont min_max_labels_font;
    min_max_labels_font.setPointSize(7);
    min_max_labels_font.setItalic(true);

    auto* const min_value_label = new QLabel();
    min_value_label->setFont(min_max_labels_font);
    min_value_label->setText(QString::number(_setup.min, 'g', _decimals));
    min_value_label->setToolTip("Minimum value.");
    min_value_label->setObjectName(QString::fromStdString(_setup.key + "/minValueLabel"));
    min_value_label->setAlignment(Qt::AlignCenter);
    min_value_label->setStyleSheet(qApp->styleSheet());

    auto* const max_value_label = new QLabel();
    max_value_label->setFont(min_max_labels_font);
    max_value_label->setText(QString::number(_setup.max, 'g', _decimals));
    max_value_label->setToolTip("Maximum value.");
    max_value_label->setObjectName(QString::fromStdString(_setup.key + "/maxValueLabel"));
    max_value_label->setAlignment(Qt::AlignCenter);
    max_value_label->setStyleSheet(qApp->styleSheet());

    auto* const value_label = new QLabel();
    value_label->setStyleSheet("QLabel { font: bold; }");
    value_label->setText(QString::number(initial_value, 'f', _decimals));
    value_label->setToolTip("Current value.");
    sight::module::ui::qt::parameters::set_label_minimum_size(value_label, _setup.min, _setup.max, _decimals);
    value_label->setObjectName(QString::fromStdString(_setup.key + "/valueLabel"));
    value_label->setAlignment(Qt::AlignCenter);
    value_label->setStyleSheet(qApp->styleSheet());

    // Slots
    {
        // Forward to the corresponding Sight signal
        QObject::connect(slider, &QSlider::valueChanged, this, &parameters::on_change_double_slider);

        // Update the labels when the value or the range changes
        QObject::connect(
            slider,
            &QSlider::valueChanged,
            this,
            [this, value_label, slider]{this->on_double_slider_mapped(value_label, slider);});

        QObject::connect(
            slider,
            &QSlider::rangeChanged,
            this,
            [ = ]{on_double_slider_range_mapped(min_value_label, max_value_label, slider);});
    }

    // Sub layout setup
    {
        const auto alignment = _orientation == Qt::Vertical ? Qt::AlignCenter : Qt::Alignment {};

        sub_layout->addWidget(min_value_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(slider, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(max_value_label, /*scretch=*/ 0, alignment);
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
        return this->create_reset_button(_setup.key, [this, slider]{on_reset_double_mapped(slider);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* parameters::create_integer_slider_widget(
    QBoxLayout& _layout,
    const int_widget& _setup,
    Qt::Orientation _orientation,
    bool _on_release
)
{
    int initial_value = std::midpoint(_setup.min, _setup.max);

    if(not _setup.default_value.empty())
    {
        try
        {
            initial_value = boost::lexical_cast<int>(_setup.default_value);
        }
        catch([[maybe_unused]] const boost::bad_lexical_cast& e)
        {
            SIGHT_ASSERT(
                "Invalid value " << std::quoted(_setup.default_value) << ": not convertible to 'int/double': "
                << e.what(),
                false
            );
        }
    }

    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* sub_layout = new QBoxLayout {layout_direction};
    _layout.addLayout(sub_layout);
    sub_layout->setSpacing(0);
    sub_layout->setContentsMargins(0, 0, 0, 0);

    auto* slider = new QSlider();

    slider->setOrientation(_orientation);
    slider->setObjectName(QString::fromStdString(_setup.key));
    slider->setMinimum(_setup.min);
    slider->setMaximum(_setup.max);
    slider->setValue(initial_value);
    slider->setMinimumSize(_setup.min_size);
    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!_on_release);
    slider->setProperty("key", QString::fromStdString(_setup.key));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", slider->value());
    slider->setProperty("widget#0", QVariant::fromValue<QSlider*>(slider));
    slider->setStyleSheet(qApp->styleSheet());
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    sight::module::ui::qt::parameters::set_label_minimum_size(value_label, _setup.min, _setup.max);
    value_label->setObjectName(QString::fromStdString(_setup.key + "/valueLabel"));
    value_label->setAlignment(Qt::AlignCenter);
    value_label->setStyleSheet(qApp->styleSheet());

    // Sub layout setup
    {
        const auto alignment = _orientation == Qt::Vertical ? Qt::AlignCenter : Qt::Alignment {};

        sub_layout->addWidget(min_value_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(slider, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(max_value_label, /*scretch=*/ 0, alignment);
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
        QObject::connect(slider, &QSlider::valueChanged, this, &parameters::on_change_integer);

        // Update the labels when the value or the range changes
        QObject::connect(
            slider,
            &QSlider::valueChanged,
            this,
            [this, value_label, slider]{this->on_slider_mapped(value_label, slider);});

        QObject::connect(
            slider,
            &QSlider::rangeChanged,
            this,
            [ = ]{on_slider_range_mapped(min_value_label, max_value_label, slider);});
    }

    // Reset button
    if(_setup.reset_button)
    {
        return this->create_reset_button(_setup.key, [this, slider]{on_reset_integer_mapped(slider);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

[[nodiscard]]
QPushButton* parameters::create_integer_spin_widget(
    QBoxLayout& _layout,
    const int_widget& _setup,
    int _count,
    Qt::Orientation _orientation
)
{
    int initial_value = std::midpoint(_setup.min, _setup.max);

    if(not _setup.default_value.empty())
    {
        try
        {
            initial_value = boost::lexical_cast<int>(_setup.default_value);
        }
        catch([[maybe_unused]] const boost::bad_lexical_cast& e)
        {
            SIGHT_ASSERT(
                "Invalid value " << std::quoted(_setup.default_value) << ": not convertible to 'int/double': "
                << e.what(),
                false
            );
        }
    }

    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* sub_layout = new QBoxLayout {layout_direction};
    _layout.addLayout(sub_layout);

    std::array<QSpinBox*, 3> spinboxes {};

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        auto* spinbox = new QSpinBox();
        spinboxes[i] = spinbox;
        spinbox->setObjectName(QString::fromStdString(_setup.key + "/" + std::to_string(i)));

        spinbox->setMinimum(_setup.min);
        spinbox->setMaximum(_setup.max);
        spinbox->setValue(initial_value);
        spinbox->setMinimumSize(_setup.min_size);
        spinbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        spinbox->setProperty("key", QString::fromStdString(_setup.key));
        spinbox->setProperty("count", _count);
        spinbox->setProperty("defaultValue", spinbox->value());

        spinbox->setStyleSheet(qApp->styleSheet());
        spinbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        sub_layout->addWidget(spinbox);

        // Connect spinbox value with our editor
        QObject::connect(spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, &parameters::on_change_integer);
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
            [this, first_spinbox](){on_reset_integer_mapped(first_spinbox);});
    }

    return nullptr;
}

//-----------------------------------------------------------------------------

void parameters::update_enum_list(const std::vector<std::string>& _list, const std::string _key)
{
    std::vector<std::string> labels;
    std::vector<std::string> keys;
    for(const auto& s : _list)
    {
        const auto [left_part, right_part] = split_string(s);
        labels.push_back(left_part);
        if(!right_part.empty())
        {
            keys.push_back(right_part);
        }
        else
        {
            keys.push_back(left_part);
        }
    }

    this->blockSignals(true);

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

    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void parameters::parse_enum_string(
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
        const auto [left_part, right_part] = split_string(token);

        _labels.push_back(left_part);
        if(!right_part.empty())
        {
            _keys.push_back(right_part);
        }
        else
        {
            _keys.push_back(left_part);
        }
    }
}

//------------------------------------------------------------------------------

void parameters::create_enum_widget(
    QBoxLayout& _layout,
    const param_widget& _setup,
    const std::vector<std::string>& _values,
    const std::vector<std::string>& _data
) const
{
    auto* menu = new QComboBox();
    menu->setObjectName(QString::fromStdString(_setup.key));
    menu->setStyleSheet(qApp->styleSheet());
    menu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    menu->setProperty("key", QString::fromStdString(_setup.key));
    int idx = 0;
    for(const auto& value : _values)
    {
        menu->insertItem(idx, QString::fromStdString(value));
        ++idx;
    }

    // Add optional data
    idx = 0;
    for(const auto& choice : _data)
    {
        menu->setItemData(idx, QString::fromStdString(choice));
        ++idx;
    }

    _layout.addWidget(menu);

    QObject::connect(menu, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &parameters::on_change_enum);

    // Set the comboBox to the default value
    menu->setCurrentText(QString::fromStdString(_setup.default_value));
    menu->setMinimumSize(_setup.min_size);
}

//------------------------------------------------------------------------------

void parameters::create_slider_enum_widget(
    QBoxLayout& _layout,
    const param_widget& _setup,
    const std::vector<std::string>& _values,
    Qt::Orientation _orientation,
    bool _on_release
) const
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* sub_layout = new QBoxLayout {layout_direction};
    {
        _layout.addLayout(sub_layout);
    }

    std::vector<int> int_values;
    std::ranges::transform(
        _values,
        std::back_inserter(int_values),
        [](const std::string& _s){return std::stoi(_s);});

    auto* const slider = new sight::ui::qt::widget::non_linear_slider();
    slider->set_orientation(_orientation);
    slider->setMinimumSize(_setup.min_size);
    slider->setObjectName(QString::fromStdString(_setup.key));
    slider->setProperty("key", QString::fromStdString(_setup.key));
    slider->set_values(int_values);
    slider->set_value(std::stoi(_setup.default_value));
    slider->set_tracking(!_on_release);
    slider->setStyleSheet(qApp->styleSheet());
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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

        sub_layout->addWidget(min_value_label, /*scretch=*/ 0, alignment);
        sub_layout->addWidget(slider, /*stretch=*/ 0, alignment);
        sub_layout->addWidget(max_value_label, /*scretch=*/ 0, alignment);
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
                if(!m_block_signals)
                {
                    this->signal<signals::integer_changed_signal_t>(signals::INTEGER_CHANGED_SIG)
                    ->async_emit(_value, key);
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(_value, key);
                    SIGHT_DEBUG(
                        get_id() << ": [EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << _value << ", "
                        << key << ")"
                    );
                    this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
                    ->async_emit(std::to_string(_value), key);
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                    ->async_emit(std::to_string(_value), key);
                    SIGHT_DEBUG(
                        get_id() << ": [EMIT] " << signals::ENUM_CHANGED_SIG << "(" << _value << ", " << key
                        << ")"
                    );
                    this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                    ->async_emit(int(slider->index()), key);
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

void parameters::create_button_bar_enum_widget(
    QBoxLayout& _layout,
    const param_widget& _setup,
    const std::vector<enum_button_param>& _button_list,
    const int _width,
    const int _height,
    const int _spacing,
    const std::string& _style,
    Qt::Orientation _orientation
) const
{
    const auto layout_direction = _orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight;

    auto* sub_layout = new QBoxLayout {layout_direction};
    _layout.addLayout(sub_layout);

    if(_spacing != 0)
    {
        sub_layout->setSpacing(_spacing);
    }

    // create a button group to deactivate the buttons on selection
    auto* button_bar_group = new QButtonGroup(sub_layout);
    button_bar_group->setObjectName(QString::fromStdString(_setup.key));

    // create the buttons from the provided list
    int button_index = 0;
    for(const auto& button_param : _button_list)
    {
        auto* enum_button = new QToolButton();
        button_bar_group->addButton(enum_button);

        // The name needs to be the ky_value, to find it when the service is updated through a slot
        enum_button->setObjectName((QString::fromStdString(_setup.key + "_" + button_param.value)));

        enum_button->setIcon(QIcon(QString::fromStdString(button_param.icon_path)));
        enum_button->setToolTip(QString::fromStdString(button_param.label));
        enum_button->setCheckable(true);
        enum_button->setProperty("class", "buttonBar");
        enum_button->setProperty("value", QString::fromStdString(button_param.value));
        enum_button->setText(QString::fromStdString(button_param.label));

        enum_button->setStyleSheet(qApp->styleSheet());
        enum_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
        if(_width != 0 || _height != 0)
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
        sub_layout->addWidget(enum_button);

        // create the connection to fire signals when the button is clicked
        QObject::connect(
            enum_button,
            &QToolButton::clicked,
            [this, button_param, key = _setup.key, button_index]
            {
                if(!m_block_signals)
                {
                    this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
                    ->async_emit(button_param.value, key);
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                    ->async_emit(button_param.value, key);
                    SIGHT_DEBUG(
                        get_id() << ": [EMIT] " << signals::ENUM_CHANGED_SIG << "(" << button_param.value << ", "
                        << key << ")"
                    );
                    this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                    ->async_emit(button_index, key);
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

        // set the default value
        if(button_param.value == _setup.default_value)
        {
            enum_button->toggle();
        }

        ++button_index;
    }
}

//-----------------------------------------------------------------------------

double parameters::get_double_slider_value(const QSlider* _slider)
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

void parameters::set_parameter(sight::ui::parameter_t _val, std::string _key)
{
    this->blockSignals(true);

    if(std::holds_alternative<bool>(_val))
    {
        this->set_bool_parameter(std::get<bool>(_val), _key);
    }
    else if(std::holds_alternative<sight::ui::color_t>(_val))
    {
        this->set_color_parameter(std::get<sight::ui::color_t>(_val), _key);
    }
    else if(std::holds_alternative<double>(_val))
    {
        this->set_double_parameter(std::get<double>(_val), _key);
    }
    else if(std::holds_alternative<sight::ui::double2_t>(_val))
    {
        const auto values = std::get<sight::ui::double2_t>(_val);
        this->set_double2_parameter(values[0], values[1], _key);
    }
    else if(std::holds_alternative<sight::ui::double3_t>(_val))
    {
        const auto values = std::get<sight::ui::double3_t>(_val);
        this->set_double3_parameter(values[0], values[1], values[2], _key);
    }
    else if(std::holds_alternative<sight::ui::int2_t>(_val))
    {
        const auto values = std::get<sight::ui::int2_t>(_val);
        this->set_int2_parameter(values[0], values[1], _key);
    }
    else if(std::holds_alternative<sight::ui::int3_t>(_val))
    {
        const auto values = std::get<sight::ui::int3_t>(_val);
        this->set_int3_parameter(values[0], values[1], values[2], _key);
    }
    else if(std::holds_alternative<std::string>(_val))
    {
        this->set_enum_parameter(std::get<std::string>(_val), _key);
    }
    else if(std::holds_alternative<int>(_val))
    {
        // This can be either an int or a enum index
        // Solve the ambiguity by testing the widget type
        QObject* child = this->get_param_widget(_key);

        auto* spinbox           = qobject_cast<QSpinBox*>(child);
        auto* slider            = qobject_cast<QSlider*>(child);
        auto* non_linear_slider = qobject_cast<sight::ui::qt::widget::non_linear_slider*>(child);

        if(spinbox != nullptr || slider != nullptr || non_linear_slider != nullptr)
        {
            this->set_int_parameter(std::get<int>(_val), _key);
        }
        else
        {
            this->set_enum_index_parameter(std::get<int>(_val), _key);
        }
    }
    else if(std::holds_alternative<sight::ui::enum_list_t>(_val))
    {
        this->update_enum_list(std::get<sight::ui::enum_list_t>(_val), _key);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_bool_parameter(bool _val, std::string _key)
{
    this->blockSignals(true);
    QObject* child = this->get_param_widget(_key);
    auto* checkbox = qobject_cast<QCheckBox*>(child);

    if(checkbox != nullptr)
    {
        checkbox->setCheckState(_val ? Qt::Checked : Qt::Unchecked);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_color_parameter(std::array<std::uint8_t, 4> _color, std::string _key)
{
    this->blockSignals(true);
    QObject* child     = this->get_param_widget(_key);
    auto* color_button = qobject_cast<QPushButton*>(child);

    if(color_button != nullptr)
    {
        const int icon_size = color_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
        QPixmap pix(icon_size, icon_size);
        QColor color_qt(_color[0], _color[1], _color[2], _color[3]);
        pix.fill(color_qt);

        color_button->setIcon(QIcon(pix));
        color_button->setProperty("color", color_qt);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_double_parameter(double _val, std::string _key)
{
    this->blockSignals(true);
    QObject* child = this->get_param_widget(_key);

    if(auto* spinbox = qobject_cast<QDoubleSpinBox*>(child); spinbox != nullptr)
    {
        spinbox->setValue(_val);
    }
    else if(auto* slider = qobject_cast<QSlider*>(child); slider != nullptr)
    {
        const double min         = slider->property("min").toDouble();
        const double max         = slider->property("max").toDouble();
        const double value_range = max - min;
        const int slider_val     = int(std::round(((_val - min) / value_range) * double(slider->maximum())));
        slider->setValue(slider_val);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_double2_parameter(double _val0, double _val1, std::string _key)
{
    this->blockSignals(true);
    QObject* child = this->get_param_widget(_key);

    if(child != nullptr)
    {
        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();

        spin0->setValue(_val0);
        spin1->setValue(_val1);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_double3_parameter(double _val0, double _val1, double _val2, std::string _key)
{
    this->blockSignals(true);
    QObject* child = this->get_param_widget(_key);

    if(child != nullptr)
    {
        auto* spin0 = child->property("widget#0").value<QDoubleSpinBox*>();
        auto* spin1 = child->property("widget#1").value<QDoubleSpinBox*>();
        auto* spin2 = child->property("widget#2").value<QDoubleSpinBox*>();

        spin0->setValue(_val0);
        spin1->setValue(_val1);
        spin2->setValue(_val2);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_int_parameter(int _val, std::string _key)
{
    this->blockSignals(true);
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        spinbox->setValue(_val);
    }
    else if(slider != nullptr)
    {
        slider->setValue(_val);
    }
    else if(auto* non_linear_slider = qobject_cast<sight::ui::qt::widget::non_linear_slider*>(child);
            non_linear_slider != nullptr)
    {
        non_linear_slider->set_value(_val);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_int2_parameter(int _val0, int _val1, std::string _key)
{
    this->blockSignals(true);
    QObject* child = this->get_param_widget(_key);

    if(child != nullptr)
    {
        auto* spin0 = child->property("widget#0").value<QSpinBox*>();
        auto* spin1 = child->property("widget#1").value<QSpinBox*>();

        spin0->setValue(_val0);
        spin1->setValue(_val1);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_int3_parameter(int _val0, int _val1, int _val2, std::string _key)
{
    this->blockSignals(true);
    QObject* widget = this->get_param_widget(_key);

    if(widget != nullptr)
    {
        auto* spin0 = widget->property("widget#0").value<QSpinBox*>();
        auto* spin1 = widget->property("widget#1").value<QSpinBox*>();
        auto* spin2 = widget->property("widget#2").value<QSpinBox*>();

        spin0->setValue(_val0);
        spin1->setValue(_val1);
        spin2->setValue(_val2);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_enum_parameter(std::string _val, std::string _key)
{
    this->blockSignals(true);

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

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::set_enum_index_parameter(int _val, std::string _key)
{
    this->blockSignals(true);

    QObject* widget = this->get_param_widget(_key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
    {
        combobox->setCurrentIndex(_val);
    }

    this->blockSignals(false);
}

//------------------------------------------------------------------------------

void parameters::update_enum_range(std::string _options, std::string _key)
{
    this->blockSignals(true);

    QObject* widget = this->get_param_widget(_key);

    auto* combobox = qobject_cast<QComboBox*>(widget);

    if(combobox != nullptr)
    {
        combobox->clear();

        std::vector<std::string> values;
        std::vector<std::string> data;

        this->parse_enum_string(_options, values, data);

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
    }

    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void parameters::emit_color_signal(const QColor _color, const std::string& _key) const
{
    const std::array<std::uint8_t, 4> new_color = {{static_cast<std::uint8_t>(_color.red()),
        static_cast<std::uint8_t>(_color.green()),
        static_cast<std::uint8_t>(_color.blue()),
        static_cast<std::uint8_t>(_color.alpha())
    }
    };
    if(!m_block_signals)
    {
        this->signal<signals::color_changed_signal_t>(signals::COLOR_CHANGED_SIG)->async_emit(new_color, _key);
        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(new_color, _key);
        SIGHT_DEBUG(
            get_id() << ": [EMIT] " << signals::COLOR_CHANGED_SIG << "(" << int(new_color[0]) << ", "
            << int(new_color[1]) << ", " << int(new_color[2]) << ", " << int(new_color[3]) << ", " << _key << ")"
        );
    }
}

//-----------------------------------------------------------------------------

void parameters::blockSignals(bool _block)
{
    m_block_signals = _block;
}

//------------------------------------------------------------------------------

void parameters::update_int_min_parameter(int _min, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();
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

void parameters::update_int_max_parameter(int _max, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();

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

void parameters::update_double_min_parameter(double _min, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();

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
        const double value = get_double_slider_value(slider);
        slider->setProperty("min", _min);
        set_double_slider_range(slider, value);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }
}

//------------------------------------------------------------------------------

void parameters::update_double_max_parameter(double _max, std::string _key)
{
    QObject* child = this->get_param_widget(_key);

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        const int count = child->property("count").toInt();

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
        const double value = get_double_slider_value(slider);
        slider->setProperty("max", _max);
        set_double_slider_range(slider, value);
    }
    else
    {
        SIGHT_ERROR(get_id() << ": Widget " << std::quoted(_key) << " must be a QSlider or a QDoubleSpinBox");
    }
}

//-----------------------------------------------------------------------------

void parameters::set_double_slider_range(QSlider* _slider, double _current_value)
{
    const std::string key       = _slider->property("key").toString().toStdString();
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

QObject* parameters::get_param_widget(const std::string& _key)
{
    const auto key = QString::fromStdString(_key);

    const auto it = std::ranges::find_if(
        m_param_boxes,
        [box_key = key + "_box"](const QWidget* const _w){return _w->objectName() == box_key;});

    if(it == m_param_boxes.cend())
    {
        SIGHT_ERROR(get_id() << ": No param with '" + _key + "' not found");
        return nullptr;
    }

    auto* widget = (*it)->findChild<QObject*>(key);

    // The child widget *MUST* exist if a parameter exists with this name, if it does not exist, this is a regression
    // caused by modifications in create_xxx_widgets.
    SIGHT_ASSERT(get_id() << ": Widget " << std::quoted(_key) << " not found in its parent box.", widget != nullptr);

    return widget;
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui::qt
