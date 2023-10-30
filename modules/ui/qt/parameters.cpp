/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

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
#include <boost/tokenizer.hpp>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QColorDialog>
#include <QEvent>
#include <QFormLayout>
#include <QGraphicsColorizeEffect>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QString>
#include <QStyle>
#include <QToolButton>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

// Internal static function to split a string using separator (usually =), mainly used to split enum into value, data
static std::pair<std::string, std::string> split_string(const std::string& _str, const std::string& _sep = "=")
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

parameters::~parameters() noexcept =
    default;

//-----------------------------------------------------------------------------

void parameters::configuring()
{
    this->initialize();
}

//-----------------------------------------------------------------------------

static QWidget* recursively_find_widget_in_layout_by_key(const QLayout* _layout, const std::string& _key)
{
    for(int i = 0 ; i < _layout->count() ; ++i)
    {
        auto* item = _layout->itemAt(i);
        if(QWidget* widget = item->widget(); widget != nullptr)
        {
            if(_key == widget->property("key").toString().toStdString())
            {
                return widget;
            }
        }
        else if(QLayout* sub_layout = item->layout(); sub_layout != nullptr)
        {
            if(QWidget* sub_widget = recursively_find_widget_in_layout_by_key(sub_layout, _key); sub_widget != nullptr)
            {
                return sub_widget;
            }
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

static QVector<QWidget*> recursively_find_widgets_in_layout_by_key(const QLayout* _layout, const std::string& _key)
{
    QVector<QWidget*> res;
    for(int i = 0 ; i < _layout->count() ; ++i)
    {
        auto* item = _layout->itemAt(i);
        if(QWidget* widget = item->widget(); widget != nullptr)
        {
            if(_key == widget->property("key").toString().toStdString())
            {
                res.append(widget);
            }
        }
        else if(QLayout* sub_layout = item->layout(); sub_layout != nullptr)
        {
            res.append(recursively_find_widgets_in_layout_by_key(sub_layout, _key));
        }
    }

    return res;
}

//------------------------------------------------------------------------------

void parameters::starting()
{
    this->create();

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(QString::fromStdString(service_id));

    auto* layout             = new QGridLayout();
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

    int row = 0;

    this->blockSignals(true);

    // Create widgets
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& param, parameters_cfg.equal_range("param"))
    {
        const service::config_t& cfg = param.second;

        const auto name          = cfg.get<std::string>("<xmlattr>.name");
        const auto key           = cfg.get<std::string>("<xmlattr>.key");
        const auto type          = cfg.get<std::string>("<xmlattr>.type");
        const auto default_value = cfg.get<std::string>("<xmlattr>.defaultValue");
        const bool reset_button  = cfg.get<bool>("<xmlattr>.reset", true);

        // If orientation is true => horizontal
        // If orientation is false => vertical
        const bool orientation = cfg.get<std::string>("<xmlattr>.orientation", "horizontal") != "vertical";

        const bool hide_min_max = cfg.get<bool>("<xmlattr>.hideMinMax", false);

        if(!name.empty())
        {
            auto* parameter_label = new QLabel(QString::fromStdString(name));
            parameter_label->setWordWrap(true);
            parameter_label->setStyleSheet("margin: -0.1em;");
            layout->addWidget(parameter_label, row, 0);
        }

        if(type == "bool")
        {
            this->create_bool_widget(*layout, row, key, default_value, reset_button);
        }
        else if(type == "color")
        {
            this->create_color_widget(*layout, row, key, default_value, reset_button);
        }
        else if(type == "double" || type == "double2" || type == "double3")
        {
            const std::string widget = cfg.get<std::string>("<xmlattr>.widget", "spin");

            const double min                  = cfg.get<double>("<xmlattr>.min", 0.);
            const double max                  = cfg.get<double>("<xmlattr>.max", 1.);
            const double default_value_double = cfg.get<double>("<xmlattr>.defaultValue", 0.5);

            const int count = (type == "double3") ? 3 : (type == "double2" ? 2 : 1);

            if(widget == "spin")
            {
                this->create_double_widget(*layout, row, key, default_value_double, min, max, count, reset_button);
            }
            else if(widget == "slider")
            {
                // We don't support multiple sliders because we will not have the room in a single row
                SIGHT_ASSERT("Count > 1 is not supported with sliders", count == 1);

                const std::uint8_t decimals = cfg.get<std::uint8_t>("<xmlattr>.decimals", 2);
                const bool on_release       = cfg.get<bool>("<xmlattr>.emitOnRelease", false);
                this->create_double_slider_widget(
                    *layout,
                    row,
                    key,
                    default_value_double,
                    min,
                    max,
                    decimals,
                    reset_button,
                    on_release,
                    orientation,
                    hide_min_max
                );
            }
            else
            {
                SIGHT_ERROR(
                    std::string("Unknown widget type : '") + widget + "' for " + name
                    + ". Must be 'spin' or 'slider'."
                );
            }
        }
        else if(type == "int" || type == "int2" || type == "int3")
        {
            const std::string widget = cfg.get<std::string>("<xmlattr>.widget", "slider");

            const int min                  = cfg.get<int>("<xmlattr>.min", 0);
            const int max                  = cfg.get<int>("<xmlattr>.max", 100);
            const int default_value_double = cfg.get<int>("<xmlattr>.defaultValue", 50);

            const int count = (type == "int3") ? 3 : (type == "int2" ? 2 : 1);

            if(widget == "spin")
            {
                this->create_integer_spin_widget(
                    *layout,
                    row,
                    key,
                    default_value_double,
                    min,
                    max,
                    count,
                    reset_button
                );
            }
            else if(widget == "slider")
            {
                // We don't support multiple sliders because we will not have the room in a single row
                SIGHT_ASSERT("Count > 1 is not supported with sliders", count == 1);

                const bool on_release = cfg.get<bool>("<xmlattr>.emitOnRelease", false);
                this->create_integer_slider_widget(
                    *layout,
                    row,
                    key,
                    default_value_double,
                    min,
                    max,
                    reset_button,
                    on_release,
                    orientation,
                    hide_min_max
                );
            }
            else
            {
                SIGHT_ERROR(
                    std::string("Unknown widget type : '") + widget + "' for " + name
                    + ". Must be 'spin' or 'slider'."
                );
            }
        }
        else if(type == "enum")
        {
            const auto widget = cfg.get<std::string>("<xmlattr>.widget", "combobox");

            if(widget == "combobox")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                //split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::parameters::parse_enum_string(options, values, data);
                this->create_enum_widget(*layout, row, key, default_value, values, data);
            }
            else if(widget == "slider")
            {
                const auto options = cfg.get<std::string>("<xmlattr>.values");
                //split values separated by ',', ' ', ';'
                std::vector<std::string> values;
                std::vector<std::string> data;

                sight::module::ui::qt::parameters::parse_enum_string(options, values, data);
                const bool on_release = cfg.get<bool>("<xmlattr>.emitOnRelease", false);
                this->create_slider_enum_widget(
                    *layout,
                    row,
                    key,
                    default_value,
                    values,
                    on_release,
                    orientation,
                    hide_min_max
                );
            }
            else if(widget == "buttonBar")
            {
                const int h_offset      = cfg.get<int>("<xmlattr>.hOffset", 0);
                const int width         = cfg.get<int>("<xmlattr>.width", 0);
                const int height        = cfg.get<int>("<xmlattr>.height", 0);
                const std::string style = cfg.get<std::string>("<xmlattr>.style", "iconOnly");

                const auto value_config = cfg.equal_range("item");
                std::vector<enum_button_param> button_list;
                for(auto value_config_it = value_config.first ;
                    value_config_it != value_config.second ;
                    ++value_config_it)
                {
                    const auto value = value_config_it->second.get<std::string>(
                        "<xmlattr>.value"
                    );

                    const auto label = value_config_it->second.get<std::string>(
                        "<xmlattr>.label",
                        ""
                    );

                    const auto icon_path_relative = value_config_it->second.get<std::string>(
                        "<xmlattr>.icon",
                        ""
                    );

                    const std::string icon_path =
                        core::runtime::get_module_resource_file_path(icon_path_relative).generic_string();

                    button_list.push_back(
                        enum_button_param(
                        {
                            value,
                            label,
                            icon_path
                        })
                    );
                }

                this->create_button_bar_enum_widget(
                    *layout,
                    row,
                    key,
                    default_value,
                    button_list,
                    width,
                    height,
                    h_offset,
                    style
                );
            }
        }

        ++row;
    }

    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& param, parameters_cfg.equal_range("param"))
    {
        const service::config_t& cfg = param.second;

        const auto key                  = cfg.get<std::string>("<xmlattr>.key");
        const std::string depends       = cfg.get<std::string>("<xmlattr>.depends", "");
        const std::string depends_value = cfg.get<std::string>("<xmlattr>.dependsValue", "");
        const bool dependsreverse       = cfg.get<bool>("<xmlattr>.dependsReverse", false);

        if(!depends.empty())
        {
            QVector<QWidget*> widgets = recursively_find_widgets_in_layout_by_key(layout, key);

            SIGHT_ASSERT("there are no widgets", !widgets.isEmpty());

            QWidget* depends_widget = recursively_find_widget_in_layout_by_key(layout, depends);

            for(QWidget* widget : widgets)
            {
                widget->installEventFilter(this);
                auto* check_box = qobject_cast<QCheckBox*>(depends_widget);
                if(check_box != nullptr)
                {
                    QObject::connect(
                        check_box,
                        &QCheckBox::stateChanged,
                        this,
                        [ = ]
                        {
                            on_depends_changed(check_box, widget, dependsreverse);
                        });
                    on_depends_changed(check_box, widget, dependsreverse);
                }
                else
                {
                    auto* combo_box = qobject_cast<QComboBox*>(depends_widget);
                    if(combo_box != nullptr)
                    {
                        QObject::connect(
                            combo_box,
                            static_cast<void (QComboBox::*)(
                                            int
                                        )>(&QComboBox::currentIndexChanged),
                            this,
                            [ = ]
                            {
                                on_depends_changed(combo_box, widget, depends_value, dependsreverse);
                            });
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
    QWidget* widget   = qt_container->get_qt_container();

    service::config_t config                = this->get_config();
    const service::config_t& parameters_cfg = config.get_child("parameters");

    // emit signal for each widget
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_FOREACH(const auto& param, parameters_cfg.equal_range("param"))
    {
        const service::config_t& cfg = param.second;

        const auto key  = cfg.get<std::string>("<xmlattr>.key");
        const auto type = cfg.get<std::string>("<xmlattr>.type");

        auto* child = widget->findChild<QObject*>(QString::fromStdString(key));
        if(child != nullptr)
        {
            if(type == "bool")
            {
                const QCheckBox* box = dynamic_cast<QCheckBox*>(child);
                SIGHT_ASSERT("Widget must be a QCheckBox", box);

                const bool state = (box->checkState() == Qt::Checked);

                if(!m_block_signals)
                {
                    this->signal<signals::boolean_changed_signal_t>(signals::BOOLEAN_CHANGED_SIG)->async_emit(
                        state,
                        key
                    );
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(state, key);
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (state ? "true" : "false") << ", " << key
                        << ")"
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
                        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)->async_emit(
                            data.toStdString(),
                            key
                        );
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                            data.toStdString(),
                            key
                        );
                        SIGHT_DEBUG(
                            "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key << ")"
                        );
                        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)->async_emit(
                            box->currentIndex(),
                            key
                        );
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                            box->currentIndex(),
                            key
                        );
                        SIGHT_DEBUG(
                            "[EMIT] " << signals::ENUM_INDEX_CHANGED_SIG << "(" << box->currentIndex() << ", " << key
                            << ")"
                        );
                    }
                }
                else if(auto* slider = qobject_cast<sight::ui::qt::widget::non_linear_slider*>(child);
                        slider != nullptr)
                {
                    if(!m_block_signals)
                    {
                        int value = slider->value();
                        this->signal<signals::integer_changed_signal_t>(signals::INTEGER_CHANGED_SIG)->async_emit(
                            value,
                            key
                        );
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                            value,
                            key
                        );
                        SIGHT_DEBUG("[EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << value << ", " << key << ")");
                        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)->async_emit(
                            std::to_string(
                                value
                            ),
                            key
                        );
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                            std::to_string(
                                value
                            ),
                            key
                        );
                        SIGHT_DEBUG("[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << value << ", " << key << ")");
                        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)->async_emit(
                            int(slider->index()),
                            key
                        );
                    }
                }
                else if(auto* button_group = qobject_cast<QButtonGroup*>(child);
                        button_group != nullptr)
                {
                    auto* checked_button = button_group->checkedButton();

                    // Find the currently checked button
                    int button_index = button_group->buttons().indexOf(checked_button);

                    if(checked_button != nullptr)
                    {
                        const std::string value = checked_button->property("value").toString().toStdString();
                        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
                        ->async_emit(value, key);
                        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                        ->async_emit(value, key);
                        SIGHT_DEBUG(
                            "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << value << ", " << key << ")"
                        );
                        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                        ->async_emit(button_index, key);
                    }
                }
                else
                {
                    SIGHT_ASSERT("Widget must either be a ComboBox or non_linear_slider", false);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::stopping()
{
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

void parameters::on_change_enum(int _value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();

    const auto* box = dynamic_cast<const QComboBox*>(sender);

    SIGHT_ASSERT("Wrong widget type", box);

    const QString data = box->itemData(_value).toString();

    if(!m_block_signals)
    {
        this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)->async_emit(
            data.toStdString(),
            key.toStdString()
        );
        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
            data.toStdString(),
            key.toStdString()
        );
        SIGHT_DEBUG(
            "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << data.toStdString() << ", " << key.toStdString()
            << ")"
        );
        this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)->async_emit(
            _value,
            key.toStdString()
        );
        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
            box->currentIndex(),
            key.toStdString()
        );
        SIGHT_DEBUG("[EMIT] " << signals::ENUM_INDEX_CHANGED_SIG << "(" << _value << ", " << key.toStdString() << ")");
    }
}

//-----------------------------------------------------------------------------

void parameters::on_change_boolean(int _value)
{
    const QObject* sender = this->sender();
    const QString key     = sender->property("key").toString();
    const bool checked    = _value == Qt::Checked;

    if(!m_block_signals)
    {
        this->signal<signals::boolean_changed_signal_t>(signals::BOOLEAN_CHANGED_SIG)->async_emit(
            checked,
            key.toStdString()
        );
        this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
            checked,
            key.toStdString()
        );
        SIGHT_DEBUG(
            "[EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (checked ? "true" : "false") << ", " << key.toStdString()
            << ")"
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
    SIGHT_ASSERT("container not instanced", container);

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

void parameters::on_change_integer(int /*unused*/)
{
    QObject* sender = this->sender();
    this->emit_integer_signal(sender);
}

//------------------------------------------------------------------------------

void parameters::emit_integer_signal(QObject* _widget)
{
    if(!m_block_signals)
    {
        SIGHT_ASSERT("widget is null", _widget != nullptr);

        const QString key = _widget->property("key").toString();
        const int count   = _widget->property("count").toInt();

        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

        const auto* spinbox = dynamic_cast<const QSpinBox*>(_widget);
        const auto* slider  = dynamic_cast<const QSlider*>(_widget);
        SIGHT_ASSERT("Wrong widget type", spinbox || slider);

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

            this->signal<signals::integer_changed_signal_t>(signals::INTEGER_CHANGED_SIG)->async_emit(
                value,
                key.toStdString()
            );
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                value,
                key.toStdString()
            );
            SIGHT_DEBUG("[EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
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
                this->signal<signals::integer2_changed_signal_t>(signals::INTEGER2_CHANGED_SIG)->async_emit(
                    value1,
                    value2,
                    key.toStdString()
                );
                const sight::ui::int2_t values = {value1, value2};
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                    values,
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << signals::INTEGER2_CHANGED_SIG << "(" << value1 << ", " << value2 << ", "
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

                this->signal<signals::integer3_changed_signal_t>(signals::INTEGER3_CHANGED_SIG)->async_emit(
                    value1,
                    value2,
                    value3,
                    key.toStdString()
                );
                const sight::ui::int3_t values = {value1, value2, value3};
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                    values,
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << signals::INTEGER3_CHANGED_SIG << "(" << value1 << ", " << value2 << ", " << value3
                    << ", " << key.toStdString() << ")"
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::on_change_double(double /*unused*/)
{
    QObject* sender = this->sender();
    this->emit_double_signal(sender);
}

//------------------------------------------------------------------------------

void parameters::emit_double_signal(QObject* _widget)
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
            this->signal<signals::double_changed_signal_t>(signals::DOUBLE_CHANGED_SIG)->async_emit(
                value,
                key.toStdString()
            );
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                value,
                key.toStdString()
            );
            SIGHT_DEBUG("[EMIT] " << signals::DOUBLE_CHANGED_SIG << "(" << value << ", " << key.toStdString() << ")");
        }
        else if(spinbox != nullptr)
        {
            SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

            if(count == 1)
            {
                this->signal<signals::double_changed_signal_t>(signals::DOUBLE_CHANGED_SIG)->async_emit(
                    spinbox->value(),
                    key.toStdString()
                );
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                    spinbox->value(),
                    key.toStdString()
                );
                SIGHT_DEBUG(
                    "[EMIT] " << signals::DOUBLE_CHANGED_SIG << "(" << spinbox->value() << ", "
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
                    this->signal<signals::double2_changed_signal_t>(signals::DOUBLE2_CHANGED_SIG)->async_emit(
                        value1,
                        value2,
                        key.toStdString()
                    );
                    const sight::ui::double2_t values = {value1, value2};
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                        values,
                        key.toStdString()
                    );
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::DOUBLE2_CHANGED_SIG << "(" << value1 << ", " << value2
                        << ", " << key.toStdString() << ")"
                    );
                }
                else
                {
                    const QDoubleSpinBox* spin3 = spinbox->property("widget#2").value<QDoubleSpinBox*>();
                    const double value3         = spin3->value();

                    this->signal<signals::double3_changed_signal_t>(signals::DOUBLE3_CHANGED_SIG)->async_emit(
                        value1,
                        value2,
                        value3,
                        key.toStdString()
                    );
                    const sight::ui::double3_t values = {value1, value2, value3};
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                        values,
                        key.toStdString()
                    );
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::DOUBLE3_CHANGED_SIG << "(" << value1 << ", " << value2
                        << ", " << value3 << ", " << key.toStdString() << ")"
                    );
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::on_change_double_slider(int /*unused*/)
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

void parameters::on_reset_boolean_mapped(QWidget* _widget)
{
    auto* checkbox = qobject_cast<QCheckBox*>(_widget);
    if(checkbox != nullptr)
    {
        int value = checkbox->property("defaultValue").toInt();
        checkbox->setCheckState(Qt::CheckState(value));

        const QString key = checkbox->property("key").toString();
        if(!m_block_signals)
        {
            this->signal<signals::boolean_changed_signal_t>(signals::BOOLEAN_CHANGED_SIG)->async_emit(
                value != 0,
                key.toStdString()
            );
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                value != 0,
                key.toStdString()
            );
            SIGHT_DEBUG(
                "[EMIT] " << signals::BOOLEAN_CHANGED_SIG << "(" << (value ? "true" : "false") << ", "
                << key.toStdString() << ")"
            );
        }
    }
}

//-----------------------------------------------------------------------------

void parameters::on_reset_color_mapped(QWidget* _widget)
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

        const std::array<std::uint8_t, 4> new_color = {{static_cast<std::uint8_t>(color.red()),
            static_cast<std::uint8_t>(color.green()),
            static_cast<std::uint8_t>(color.blue()),
            static_cast<std::uint8_t>(color.alpha())
        }
        };
        if(!m_block_signals)
        {
            this->signal<signals::color_changed_signal_t>(signals::COLOR_CHANGED_SIG)->async_emit(
                new_color,
                key.toStdString()
            );
            this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(
                new_color,
                key.toStdString()
            );
            SIGHT_DEBUG(
                "[EMIT] " << signals::COLOR_CHANGED_SIG << "(" << int(new_color[0]) << ", "
                << int(new_color[1]) << ", " << int(new_color[2]) << ", " << int(new_color[3]) << ", "
                << key.toStdString() << ")"
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
        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

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
        SIGHT_ASSERT("Invalid widgets count, must be <= 3", count <= 3);

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

QPushButton* parameters::create_reset_button(const std::string& _key)
{
    std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);
    auto* reset_button     = new QPushButton("R");
    reset_button->setObjectName(QString::fromStdString(service_id + "/Reset " + _key));
    reset_button->setFocusPolicy(Qt::NoFocus);
    reset_button->setToolTip("Reset to the default value.");
    reset_button->setMaximumWidth(20);

    return reset_button;
}

//-----------------------------------------------------------------------------

void parameters::create_bool_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    const std::string& _default_value,
    bool _add_reset_button
)
{
    auto* checkbox = new QCheckBox();
    checkbox->setTristate(false);
    checkbox->setObjectName(QString::fromStdString(_key));

    if(_default_value == "true")
    {
        checkbox->setCheckState(Qt::Checked);
    }

    checkbox->setProperty("key", QString(_key.c_str()));
    checkbox->setProperty("defaultValue", checkbox->checkState());
    _layout.addWidget(checkbox, _row, 1);
    QObject::connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(on_change_boolean(int)));

    // Reset button
    if(_add_reset_button)
    {
        QPushButton* reset_button = this->create_reset_button(_key);

        _layout.addWidget(reset_button, _row, 2);

        // Connect reset button to the slider
        QObject::connect(
            reset_button,
            &QPushButton::clicked,
            this,
            [this, checkbox]{on_reset_boolean_mapped(checkbox);});
    }
}

//-----------------------------------------------------------------------------

void parameters::create_color_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    const std::string& _default_value,
    bool _add_reset_button
)
{
    auto* colour_button = new QPushButton("Color");
    colour_button->setObjectName(QString::fromStdString(_key));
    colour_button->setToolTip(tr("Selected color"));
    colour_button->setMinimumSize(120, 35);

    std::string color_str = "#ffffffff";
    if(!_default_value.empty())
    {
        std::array<std::uint8_t, 4> color {};

        data::tools::color::hexa_string_to_rgba(_default_value, color);

        color_str = _default_value;
    }

    std::array<std::uint8_t, 4> color {};
    data::tools::color::hexa_string_to_rgba(color_str, color);

    const int icon_size = colour_button->style()->pixelMetric(QStyle::PM_LargeIconSize);
    QPixmap pix(icon_size, icon_size);

    QColor color_qt(color[0], color[1], color[2], color[3]);
    pix.fill(color_qt);

    colour_button->setIcon(QIcon(pix));

    colour_button->setProperty("key", QString(_key.c_str()));
    colour_button->setProperty("defaultValue", color_qt);
    colour_button->setProperty("color", color_qt);

    _layout.addWidget(colour_button, _row, 1);

    QObject::connect(colour_button, &QPushButton::clicked, this, &parameters::on_color_button);

    // Reset button
    if(_add_reset_button)
    {
        QPushButton* reset_button = this->create_reset_button(_key);

        _layout.addWidget(reset_button, _row, 2);

        // Connect reset button to the button
        QObject::connect(
            reset_button,
            &QPushButton::clicked,
            this,
            [this, colour_button]
            {
                on_reset_color_mapped(colour_button);
            });
    }
}

//-----------------------------------------------------------------------------

void parameters::create_double_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    double _default_value,
    double _min,
    double _max,
    int _count,
    bool _add_reset_button
)
{
    auto* sub_layout = new QGridLayout();
    _layout.addLayout(sub_layout, _row, 1);

    std::array<QDoubleSpinBox*, 3> spinboxes {};

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        auto* spinbox = new QDoubleSpinBox();
        spinbox->setObjectName(QString::fromStdString(_key + "/" + std::to_string(i)));
        spinboxes[i] = spinbox;

        auto count_decimals = [](double _num) -> int
                              {
                                  std::stringstream out;
                                  out << _num;
                                  const std::string s = out.str();
                                  const std::string t = s.substr(s.find('.') + 1);
                                  return static_cast<int>(t.length());
                              };

        spinbox->setDecimals(std::max(std::max(count_decimals(_min), count_decimals(_max)), 2));

        spinbox->setRange(_min, _max);

        // Beware, set setSingleStep after setRange() and setDecimals() otherwise it may fail
        spinbox->setSingleStep(std::abs(spinbox->maximum() - spinbox->minimum()) / 100.);

        // Set value last only after setting range and decimals, otherwise the value may be truncated
        spinbox->setValue(_default_value);

        spinbox->setProperty("key", QString(_key.c_str()));
        spinbox->setProperty("count", _count);
        spinbox->setProperty("defaultValue", spinbox->value());

        sub_layout->addWidget(spinbox, 0, int(i));

        QObject::connect(spinbox, SIGNAL(valueChanged(double)), this, SLOT(on_change_double(double)));
    }

    QDoubleSpinBox* spinbox = spinboxes[0];
    spinbox->setObjectName(QString::fromStdString(_key));

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
    if(_add_reset_button)
    {
        QPushButton* reset_button = this->create_reset_button(_key);

        _layout.addWidget(reset_button, _row, 2);

        // Connect reset button to the spinbox
        QObject::connect(reset_button, &QPushButton::clicked, this, [this, spinbox]{on_reset_double_mapped(spinbox);});
    }
}

//-----------------------------------------------------------------------------

void parameters::create_double_slider_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    double _default_value,
    double _min,
    double _max,
    std::uint8_t _decimals,
    bool _add_reset_button,
    bool _on_release,
    bool _orientation,
    bool _hide_min_max
)
{
    auto* sub_layout = new QGridLayout();
    _layout.addLayout(sub_layout, _row, 1);

    const double value_range = _max - _min;

    auto* slider = new QSlider();
    if(_orientation)
    {
        slider->setOrientation(Qt::Horizontal);
    }
    else
    {
        slider->setOrientation(Qt::Vertical);
    }

    slider->setObjectName(QString::fromStdString(_key));

    slider->setProperty("key", QString::fromStdString(_key));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", _default_value);
    slider->setProperty("decimals", _decimals);
    slider->setProperty("min", _min);
    slider->setProperty("max", _max);

    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!_on_release);

    set_double_slider_range(slider, _default_value);

    const int default_slider_value =
        int(std::round(((_default_value - _min) / value_range) * double(slider->maximum())));
    slider->setValue(default_slider_value);

    QFont font;
    font.setPointSize(7);
    font.setItalic(true);

    auto* min_value_label = new QLabel();
    min_value_label->setFont(font);
    min_value_label->setText(QString::number(_min, 'g', _decimals));
    min_value_label->setToolTip("Minimum value.");
    min_value_label->setObjectName(QString::fromStdString(_key + "/minValueLabel"));

    auto* max_value_label = new QLabel();
    max_value_label->setFont(font);
    max_value_label->setText(QString::number(_max, 'g', _decimals));
    max_value_label->setToolTip("Maximum value.");
    max_value_label->setObjectName(QString::fromStdString(_key + "/maxValueLabel"));

    auto* value_label = new QLabel();
    value_label->setStyleSheet("QLabel { font: bold; }");
    value_label->setText(QString::number(_default_value, 'f', _decimals));
    value_label->setToolTip("Current value.");
    sight::module::ui::qt::parameters::set_label_minimum_size(value_label, _min, _max, _decimals);
    value_label->setObjectName(QString::fromStdString(_key + "/valueLabel"));

    if(_orientation)
    {
        sub_layout->addWidget(min_value_label, 0, 0);
        sub_layout->addWidget(slider, 0, 1);
        sub_layout->addWidget(max_value_label, 0, 2);
        sub_layout->addWidget(value_label, 0, 3);
    }
    else
    {
        sub_layout->addWidget(max_value_label, 0, 0, Qt::AlignCenter);
        sub_layout->addWidget(slider, 1, 0);
        sub_layout->addWidget(min_value_label, 2, 0, Qt::AlignCenter);
        sub_layout->addWidget(value_label, 3, 0, Qt::AlignCenter);

        min_value_label->setAlignment(Qt::AlignCenter);
        max_value_label->setAlignment(Qt::AlignCenter);
        value_label->setAlignment(Qt::AlignCenter);
    }

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_change_double_slider(int)));

    // Connect slider value to the label
    QObject::connect(
        slider,
        &QSlider::valueChanged,
        this,
        [this, value_label, slider]
        {
            this->on_double_slider_mapped(value_label, slider);
        });
    QObject::connect(
        slider,
        &QSlider::rangeChanged,
        this,
        [ = ]
        {
            on_double_slider_range_mapped(
                min_value_label,
                max_value_label,
                slider
            );
        });

    const std::string prop_name = std::string("widget#0");
    slider->setProperty(prop_name.c_str(), QVariant::fromValue<QSlider*>(slider));

    // Reset button
    if(_add_reset_button)
    {
        QPushButton* reset_button = this->create_reset_button(_key);

        if(_orientation)
        {
            sub_layout->addWidget(reset_button, 0, 4);
        }
        else
        {
            sub_layout->addWidget(reset_button, 4, 0);
        }

        // Connect reset button to the slider
        QObject::connect(reset_button, &QPushButton::clicked, this, [this, slider]{on_reset_double_mapped(slider);});
    }

    if(_hide_min_max)
    {
        min_value_label->hide();
        max_value_label->hide();
    }
}

//-----------------------------------------------------------------------------

void parameters::create_integer_slider_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    int _default_value,
    int _min,
    int _max,
    bool _add_reset_button,
    bool _on_release,
    bool _orientation,
    bool _hide_min_max
)
{
    auto* sub_layout = new QGridLayout();
    _layout.addLayout(sub_layout, _row, 1);

    auto* slider = new QSlider();
    if(_orientation)
    {
        slider->setOrientation(Qt::Horizontal);
    }
    else
    {
        slider->setOrientation(Qt::Vertical);
    }

    slider->setObjectName(QString::fromStdString(_key));
    slider->setMinimum(_min);
    slider->setMaximum(_max);
    slider->setValue(_default_value);

    // tracking true: emit signal when value change, false: emit signal when slider released.
    slider->setTracking(!_on_release);

    QFont font;
    font.setPointSize(7);
    font.setItalic(true);

    auto* min_value_label = new QLabel();
    min_value_label->setFont(font);
    min_value_label->setText(QString::number(slider->minimum()));
    min_value_label->setToolTip("Minimum value.");
    min_value_label->setObjectName(QString::fromStdString(_key + "/minValueLabel"));

    auto* max_value_label = new QLabel();
    max_value_label->setFont(font);
    max_value_label->setText(QString::number(slider->maximum()));
    max_value_label->setToolTip("Maximum value.");
    max_value_label->setObjectName(QString::fromStdString(_key + "/maxValueLabel"));

    auto* value_label = new QLabel();
    value_label->setStyleSheet("QLabel { font: bold; }");
    value_label->setText(QString("%1").arg(slider->value()));
    value_label->setToolTip("Current value.");
    sight::module::ui::qt::parameters::set_label_minimum_size(value_label, _min, _max);
    value_label->setObjectName(QString::fromStdString(_key + "/valueLabel"));

    if(_orientation)
    {
        sub_layout->addWidget(min_value_label, 0, 0);
        sub_layout->addWidget(slider, 0, 1);
        sub_layout->addWidget(max_value_label, 0, 2);
        sub_layout->addWidget(value_label, 0, 3);
    }
    else
    {
        sub_layout->addWidget(max_value_label, 0, 0, Qt::AlignCenter);
        sub_layout->addWidget(slider, 1, 0);
        sub_layout->addWidget(min_value_label, 2, 0, Qt::AlignCenter);
        sub_layout->addWidget(value_label, 3, 0, Qt::AlignCenter);

        min_value_label->setAlignment(Qt::AlignCenter);
        max_value_label->setAlignment(Qt::AlignCenter);
        value_label->setAlignment(Qt::AlignCenter);
    }

    slider->setProperty("key", QString(_key.c_str()));
    slider->setProperty("count", 1);
    slider->setProperty("defaultValue", slider->value());

    // Connect slider value with our editor
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_change_integer(int)));

    // Connect slider value to the label
    QObject::connect(
        slider,
        &QSlider::valueChanged,
        this,
        [this, value_label, slider]
        {
            this->on_slider_mapped(value_label, slider);
        });
    QObject::connect(
        slider,
        &QSlider::rangeChanged,
        this,
        [ = ]
        {
            on_slider_range_mapped(
                min_value_label,
                max_value_label,
                slider
            );
        });

    const std::string prop_name = std::string("widget#0");
    slider->setProperty(prop_name.c_str(), QVariant::fromValue<QSlider*>(slider));

    // Reset button
    if(_add_reset_button)
    {
        QPushButton* reset_button = this->create_reset_button(_key);

        if(_orientation)
        {
            sub_layout->addWidget(reset_button, 0, 4);
        }
        else
        {
            sub_layout->addWidget(reset_button, 4, 0);
        }

        // Connect reset button to the slider
        QObject::connect(reset_button, &QPushButton::clicked, this, [this, slider]{on_reset_integer_mapped(slider);});
    }

    if(_hide_min_max)
    {
        min_value_label->hide();
        max_value_label->hide();
    }
}

//-----------------------------------------------------------------------------

void parameters::create_integer_spin_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    int _default_value,
    int _min,
    int _max,
    int _count,
    bool _add_reset_button
)
{
    auto* sub_layout = new QGridLayout();
    _layout.addLayout(sub_layout, _row, 1);

    std::array<QSpinBox*, 3> spinboxes {};

    // Spinboxes
    for(std::size_t i = 0 ; i < std::size_t(_count) ; ++i)
    {
        auto* spinbox = new QSpinBox();
        spinbox->setObjectName(QString::fromStdString(_key + "/" + std::to_string(i)));
        spinboxes[i] = spinbox;

        spinbox->setMinimum(_min);
        spinbox->setMaximum(_max);
        spinbox->setValue(_default_value);

        spinbox->setProperty("key", QString(_key.c_str()));
        spinbox->setProperty("count", _count);
        spinbox->setProperty("defaultValue", spinbox->value());

        sub_layout->addWidget(spinbox, 0, int(i));

        // Connect spinbox value with our editor
        QObject::connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(on_change_integer(int)));
    }

    QSpinBox* spinbox = spinboxes[0];
    spinbox->setObjectName(QString::fromStdString(_key));

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
    if(_add_reset_button)
    {
        QPushButton* reset_button = this->create_reset_button(_key);

        _layout.addWidget(reset_button, _row, 2);

        // Connect reset button to the spinbox
        QObject::connect(reset_button, &QPushButton::clicked, this, [this, spinbox]{on_reset_integer_mapped(spinbox);});
    }
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
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    const std::string& _default_value,
    const std::vector<std::string>& _values,
    const std::vector<std::string>& _data
)
{
    auto* menu = new QComboBox();
    menu->setObjectName(QString::fromStdString(_key));

    menu->setProperty("key", QString(_key.c_str()));
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

    _layout.addWidget(menu, _row, 1);

    QObject::connect(menu, SIGNAL(currentIndexChanged(int)), this, SLOT(on_change_enum(int)));

    //Set the comboBox to the default value
    menu->setCurrentText(QString::fromStdString(_default_value));
}

//------------------------------------------------------------------------------

void parameters::create_slider_enum_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    const std::string& _default_value,
    const std::vector<std::string>& _values,
    bool _on_release,
    bool _orientation,
    bool _hide_min_max
)
{
    auto* sub_layout = new QGridLayout();
    _layout.addLayout(sub_layout, _row, 1);

    auto* slider = new sight::ui::qt::widget::non_linear_slider();
    if(_orientation)
    {
        slider->set_orientation(Qt::Horizontal);
    }
    else
    {
        slider->set_orientation(Qt::Vertical);
    }

    slider->setObjectName(QString::fromStdString(_key));
    slider->setProperty("key", QString::fromStdString(_key));
    std::vector<int> int_values;
    std::ranges::transform(_values, std::back_inserter(int_values), [](const std::string& _s){return std::stoi(_s);});
    slider->set_values(int_values);
    slider->set_value(std::stoi(_default_value));

    slider->set_tracking(!_on_release);

    QFont font;
    font.setPointSize(7);
    font.setItalic(true);

    auto* min_value_label = new QLabel();
    min_value_label->setFont(font);
    min_value_label->setText(QString::fromStdString(_values.front()));
    min_value_label->setToolTip("Minimum value.");
    min_value_label->setObjectName(QString::fromStdString(_key + "/minValueLabel"));

    auto* max_value_label = new QLabel();
    max_value_label->setFont(font);
    max_value_label->setText(QString::fromStdString(_values.back()));
    max_value_label->setToolTip("Maximum value.");
    max_value_label->setObjectName(QString::fromStdString(_key + "/maxValueLabel"));

    auto* value_label = new QLabel();
    value_label->setStyleSheet("QLabel { font: bold; }");
    value_label->setText(QString::number(slider->value()));
    value_label->setToolTip("Current value.");
    set_label_minimum_size(value_label, int_values.front(), int_values.back());
    value_label->setObjectName(QString::fromStdString(_key + "/valueLabel"));

    if(_orientation)
    {
        sub_layout->addWidget(min_value_label, 0, 0);
        sub_layout->addWidget(slider, 0, 1);
        sub_layout->addWidget(max_value_label, 0, 2);
        sub_layout->addWidget(value_label, 0, 3);
    }
    else
    {
        sub_layout->addWidget(max_value_label, 0, 0, Qt::AlignCenter);
        sub_layout->addWidget(slider, 1, 0);
        sub_layout->addWidget(min_value_label, 2, 0, Qt::AlignCenter);
        sub_layout->addWidget(value_label, 3, 0, Qt::AlignCenter);

        min_value_label->setAlignment(Qt::AlignCenter);
        max_value_label->setAlignment(Qt::AlignCenter);
        value_label->setAlignment(Qt::AlignCenter);
    }

    QObject::connect(
        slider,
        &sight::ui::qt::widget::non_linear_slider::value_changed,
        [this, _key, slider, value_label](int _value)
        {
            if(!m_block_signals)
            {
                this->signal<signals::integer_changed_signal_t>(signals::INTEGER_CHANGED_SIG)->async_emit(_value, _key);
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)->async_emit(_value, _key);
                SIGHT_DEBUG("[EMIT] " << signals::INTEGER_CHANGED_SIG << "(" << _value << ", " << _key << ")");
                this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
                ->async_emit(std::to_string(_value), _key);
                this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                ->async_emit(std::to_string(_value), _key);
                SIGHT_DEBUG("[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << _value << ", " << _key << ")");
                this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                ->async_emit(int(slider->index()), _key);
            }

            value_label->setText(QString::number(_value));
        });
    QObject::connect(
        slider,
        &sight::ui::qt::widget::non_linear_slider::range_changed,
        [ = ](int _min, int _max)
        {
            min_value_label->setText(QString::number(_min));
            max_value_label->setText(QString::number(_max));
        });

    if(_hide_min_max)
    {
        min_value_label->hide();
        max_value_label->hide();
    }
}

//-----------------------------------------------------------------------------

void parameters::create_button_bar_enum_widget(
    QGridLayout& _layout,
    int _row,
    const std::string& _key,
    const std::string& _default_value,
    const std::vector<enum_button_param>& _button_list,
    const int _requested_width,
    const int _requested_height,
    const int _h_offset,
    const std::string& _style
)
{
    //create the grid to store the buttons
    auto* sub_layout = new QGridLayout();
    if(_h_offset != 0)
    {
        sub_layout->setHorizontalSpacing(_h_offset);
    }

    _layout.addLayout(sub_layout, _row, 1);

    //create a button group to deactivate the buttons on selection
    auto* button_bar_group = new QButtonGroup(sub_layout);
    button_bar_group->setObjectName(QString::fromStdString(_key));

    //create the buttons from the provided list
    int button_index = 0;
    for(const auto& button_param : _button_list)
    {
        auto* enum_button = new QToolButton();
        button_bar_group->addButton(enum_button);

        //The name needs to be the ky_value, to find it when the SParam is updated through a slot
        enum_button->setObjectName((QString::fromStdString(_key + "_" + button_param.value)));

        enum_button->setIcon(QIcon(QString::fromStdString(button_param.icon_path)));
        enum_button->setToolTip(QString::fromStdString(button_param.label));
        enum_button->setMinimumSize(120, 24);
        enum_button->setCheckable(true);
        enum_button->setProperty("class", "buttonBar");
        enum_button->setProperty("value", QString::fromStdString(button_param.value));
        enum_button->setText(QString::fromStdString(button_param.label));

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

        //the size depends on the configuration. xml > qss
        if(_requested_width != 0 || _requested_height != 0)
        {
            //the size is provided through the xml. Don't use the qss.
            const int width  = _requested_width == 0 ? _requested_height : _requested_width;
            const int height = _requested_height == 0 ? _requested_width : _requested_height;
            enum_button->setIconSize(
                QSize(
                    width,
                    height
                )
            );
        }
        else
        {
            //the size is not provided through the xml. Use the qss style.
            enum_button->setProperty("class", "buttonBarTouchFriendly");
        }

        //add the button in the grid at its place
        sub_layout->addWidget(enum_button, 1, button_index);

        //create the connection to fire signals when the button is clicked
        QObject::connect(
            enum_button,
            &QToolButton::clicked,
            [this, button_param, _key, button_index]
            {
                if(!m_block_signals)
                {
                    this->signal<signals::enum_changed_signal_t>(signals::ENUM_CHANGED_SIG)
                    ->async_emit(button_param.value, _key);
                    this->signal<signals::changed_signal_t>(signals::PARAMETER_CHANGED_SIG)
                    ->async_emit(button_param.value, _key);
                    SIGHT_DEBUG(
                        "[EMIT] " << signals::ENUM_CHANGED_SIG << "(" << button_param.value << ", " << _key << ")"
                    );
                    this->signal<signals::enum_changed_index_signal_t>(signals::ENUM_INDEX_CHANGED_SIG)
                    ->async_emit(button_index, _key);
                }
            });

        //create connection to change the display (gray/full color) when the selection state changes
        QObject::connect(
            enum_button,
            &QAbstractButton::toggled,
            [ = ]
                (bool _checked)
            {
                QGraphicsEffect* effect = enum_button->graphicsEffect();
                effect->setEnabled(!_checked);
            });

        //set the default value
        if(button_param.value == _default_value)
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

    auto* spinbox = qobject_cast<QDoubleSpinBox*>(child);
    auto* slider  = qobject_cast<QSlider*>(child);

    if(spinbox != nullptr)
    {
        spinbox->setValue(_val);
    }
    else if(slider != nullptr)
    {
        const double min         = slider->property("min").toDouble();
        const double max         = slider->property("max").toDouble();
        const double value_range = max - min;
        const int slider_val     = int(std::round(((_val - min) / value_range) * double(slider->maximum())));
        slider->setValue(slider_val);
    }
    else
    {
        SIGHT_ERROR("Widget '" + _key + "' must be a QSlider or a QDoubleSpinBox");
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
            SIGHT_WARN(std::string("value '") + _val + "' isn't found in Enum ComboBox '" + _key + "'.");
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
        QObject* button_object = this->get_param_widget(_key + "_" + _val);
        auto* button           = qobject_cast<QToolButton*>(button_object);

        if(button != nullptr)
        {
            button->toggle();
        }
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

void parameters::emit_color_signal(const QColor _color, const std::string& _key)
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
            "[EMIT] " << signals::COLOR_CHANGED_SIG << "(" << int(new_color[0]) << ", "
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
        SIGHT_ERROR("Widget '" + _key + "' must be a QSlider or a QDoubleSpinBox");
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
        SIGHT_ERROR("Widget '" + _key + "' must be a QSlider or a QDoubleSpinBox");
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
        SIGHT_ERROR("Widget '" + _key + "' must be a QSlider or a QDoubleSpinBox");
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
        SIGHT_ERROR("Widget '" + _key + "' must be a QSlider or a QDoubleSpinBox");
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
        "The requested value range for '" + key + "' is too large to be handled by a double slider. "
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
    auto qt_container     = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    const QObject* object = qt_container->get_qt_container();

    auto* child = object->findChild<QObject*>(QString::fromStdString(_key));
    SIGHT_ERROR_IF("object '" + _key + "' is not found", !child);

    return child;
}

//-----------------------------------------------------------------------------

} //namespace sight::module::ui::qt
