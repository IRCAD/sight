/************************************************************************
 *
 * Copyright (C) 2015-2025 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "signal_button.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/ptree.hpp>
#include <core/runtime/path.hpp>

#include <io/joystick/interactor.hpp>

#include <ui/qt/container/widget.hpp>

#include <QVariant>
#include <QVBoxLayout>

namespace sight::module::ui::qt::com
{

static const core::com::slots::key_t SET_CHECKED_SLOT = "set_checked";
static const core::com::slots::key_t CHECK_SLOT       = "check";
static const core::com::slots::key_t UNCHECK_SLOT     = "uncheck";

static const core::com::slots::key_t SET_ENABLED_SLOT = "set_enabled";
static const core::com::slots::key_t ENABLE_SLOT      = "enable";
static const core::com::slots::key_t DISABLE_SLOT     = "disable";
static const core::com::slots::key_t SET_VISIBLE_SLOT = "set_visible";
static const core::com::slots::key_t SHOW_SLOT        = "show";
static const core::com::slots::key_t HIDE_SLOT        = "hide";

//-----------------------------------------------------------------------------

signal_button::signal_button() noexcept
{
    new_signal<signals::void_t>(signals::CLICKED);
    new_signal<signals::bool_t>(signals::IS_CHECKED);
    new_signal<signals::bool_t>(signals::TOGGLED);
    new_signal<signals::void_t>(signals::CHECKED);
    new_signal<signals::void_t>(signals::UNCHECKED);

    new_slot(SET_CHECKED_SLOT, &signal_button::set_checked, this);
    new_slot(CHECK_SLOT, &signal_button::check, this);
    new_slot(UNCHECK_SLOT, &signal_button::uncheck, this);
    new_slot(SET_ENABLED_SLOT, &signal_button::set_enabled, this);
    new_slot(ENABLE_SLOT, &signal_button::enable, this);
    new_slot(DISABLE_SLOT, &signal_button::disable, this);
    new_slot(SET_VISIBLE_SLOT, &signal_button::set_visible, this);
    new_slot(SHOW_SLOT, &signal_button::show, this);
    new_slot(HIDE_SLOT, &signal_button::hide, this);
}

//-----------------------------------------------------------------------------

signal_button::~signal_button() noexcept =
    default;

//-----------------------------------------------------------------------------

void signal_button::configuring()
{
    this->initialize();

    const auto configuration = this->get_config();

    const auto cfg = configuration.get_child_optional("config");

    if(cfg.has_value())
    {
        m_checkable      = cfg->get<bool>("checkable", m_checkable);
        m_check_at_start = cfg->get<bool>("checked", m_check_at_start);
        m_enable         = cfg->get<bool>("enable", m_enable);

        m_text     = cfg->get<std::string>("text", m_text);
        m_text2    = cfg->get<std::string>("text2", m_text2);
        m_tool_tip = core::ptree::get_and_deprecate(*cfg, "tool_tip", "toolTip", "26.0", m_tool_tip);

        if(const auto icon = cfg->get_optional<std::string>("icon"); icon.has_value())
        {
            m_icon = core::runtime::get_module_resource_file_path(icon.value());
        }

        if(const auto icon = cfg->get_optional<std::string>("icon2"); icon.has_value())
        {
            m_icon2 = core::runtime::get_module_resource_file_path(icon.value());
        }

        m_joystick_alias = sight::io::joystick::interactor::to_joystick(cfg->get<std::string>("joystick", ""));

        m_icon_width  = core::ptree::get_and_deprecate(*cfg, "icon_width", "iconWidth", "26.0", m_icon_width);
        m_icon_height = core::ptree::get_and_deprecate(*cfg, "icon_height", "iconHeight", "26.0", m_icon_height);
    }
}

//-----------------------------------------------------------------------------

void signal_button::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    auto* layout = new QVBoxLayout();
    m_button = new QPushButton(QString::fromStdString(m_text));
    m_button->setEnabled(m_enable);

    const QString service_id = QString::fromStdString(base_id());
    m_button->setObjectName(service_id + "/signal_button");

    m_button->setProperty("class", "signal-button");
    layout->addWidget(m_button);
    qt_container->set_layout(layout);

    if(!m_tool_tip.empty())
    {
        m_button->setToolTip(QString::fromStdString(m_tool_tip));
    }

    if(!m_icon.empty())
    {
        m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
    }

    if(m_icon_width > 0 && m_icon_height > 0)
    {
        m_button->setIconSize(QSize(int(m_icon_width), int(m_icon_height)));
    }

    if(m_checkable)
    {
        m_button->setCheckable(true);

        if(m_check_at_start)
        {
            m_button->setChecked(true);
            if(!m_text2.empty())
            {
                m_button->setText(QString::fromStdString(m_text2));
            }

            if(!m_icon2.empty())
            {
                m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
            }
        }
    }

    QObject::connect(m_button.data(), &QPushButton::clicked, this, &signal_button::on_clicked);
    QObject::connect(m_button.data(), &QPushButton::toggled, this, &signal_button::on_toggled);

    if(m_joystick_alias != sight::io::joystick::joystick_t::unknown)
    {
        this->start_listening_joystick();
    }
}

//-----------------------------------------------------------------------------

void signal_button::updating()
{
}

//-----------------------------------------------------------------------------

void signal_button::stopping()
{
    this->destroy();

    if(m_joystick_alias != sight::io::joystick::joystick_t::unknown)
    {
        this->stop_listening_joystick();
    }
}

//-----------------------------------------------------------------------------

void signal_button::joystick_axis_direction_event(const sight::io::joystick::axis_direction_event& _event)
{
    if(_event.device->alias == m_joystick_alias)
    {
        if(_event.axis_alias == sight::io::joystick::axis_t::tz and _event.value
           == sight::io::joystick::axis_direction_event::direction_t::backward)
        {
            m_button->animateClick();
        }
    }
}

//-----------------------------------------------------------------------------

void signal_button::on_clicked()
{
    const auto sig = this->signal<signals::void_t>(signals::CLICKED);
    sig->async_emit();
}

//-----------------------------------------------------------------------------

void signal_button::on_toggled(bool _toggled)
{
    this->set_checked(_toggled);

    // legacy
    {
        const auto sig = this->signal<signals::bool_t>(signals::TOGGLED);
        sig->async_emit(_toggled);
    }

    // current signal
    {
        const auto sig = this->signal<signals::bool_t>(signals::IS_CHECKED);
        sig->async_emit(_toggled);
    }

    // checked/unchecked signals
    if(_toggled)
    {
        const auto sig = this->signal<signals::void_t>(signals::CHECKED);
        sig->async_emit();
    }
    else
    {
        const auto sig = this->signal<signals::void_t>(signals::UNCHECKED);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

void signal_button::set_checked(bool _checked)
{
    if(_checked)
    {
        if(!m_text2.empty())
        {
            m_button->setText(QString::fromStdString(m_text2));
        }

        if(!m_icon2.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
        }
    }
    else
    {
        if(!m_text.empty())
        {
            m_button->setText(QString::fromStdString(m_text));
        }

        if(!m_icon.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
        }
    }

    // properly check/uncheck the button when this method is called from the sight slot.
    this->blockSignals(true);
    m_button->setChecked(_checked);
    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void signal_button::check()
{
    this->set_checked(true);
}

//-----------------------------------------------------------------------------

void signal_button::uncheck()
{
    this->set_checked(false);
}

//-----------------------------------------------------------------------------

void signal_button::set_enabled(bool _is_enabled)
{
    editor::set_enabled(_is_enabled);
    // Keep this in case of signal_button is used outside a view container
    m_button->setEnabled(_is_enabled);
}

//-----------------------------------------------------------------------------

void signal_button::enable()
{
    this->set_enabled(true);
}

//-----------------------------------------------------------------------------

void signal_button::disable()
{
    this->set_enabled(false);
}

//-----------------------------------------------------------------------------

void signal_button::set_visible(bool _is_visible)
{
    editor::set_visible(_is_visible);
    // Keep this in case of signal_button is used outside a view container
    m_button->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void signal_button::show()
{
    this->set_visible(true);
}

//-----------------------------------------------------------------------------

void signal_button::hide()
{
    this->set_visible(false);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::com
