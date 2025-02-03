/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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

#include "signal_shortcut.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/helper.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/__/container/widget.hpp>
#include <ui/__/registry.hpp>
#include <ui/__/service.hpp>
#include <ui/qt/container/widget.hpp>

#include <QKeySequence>
#include <QWidget>

#include <memory>

namespace sight::module::ui::qt::com
{

//-----------------------------------------------------------------------------

signal_shortcut::signal_shortcut() noexcept
{
    new_signal<signals::bool_t>(signals::IS_ENABLED);
    new_signal<signals::void_t>(signals::ENABLED);
    new_signal<signals::void_t>(signals::DISABLED);
    new_signal<signals::void_t>(signals::ACTIVATED);

    new_slot(slots::SET_ENABLED, &signal_shortcut::set_enabled, this);
    new_slot(slots::SET_DISABLED, [this](bool _disabled){this->set_enabled(!_disabled);});
    new_slot(slots::ENABLE, [this](){this->set_enabled(true);});
    new_slot(slots::DISABLE, [this](){this->set_enabled(false);});
    new_slot(slots::APPLY_ENABLED, [this](){this->set_enabled(*m_enabled);});
}

//-----------------------------------------------------------------------------

signal_shortcut::~signal_shortcut() noexcept =
    default;

//-----------------------------------------------------------------------------

void signal_shortcut::configuring()
{
    const auto config_tree = this->get_config();

    if(auto properties = config_tree.get_child_optional("properties"); not properties.has_value())
    {
        const auto enabled = m_enabled.lock();
        *enabled = core::runtime::get_ptree_value(config_tree, "state.<xmlattr>.enabled", true);
    }

    const auto config_shortcut = config_tree.get_child("config.<xmlattr>");
    m_shortcut = config_shortcut.get<std::string>("shortcut", m_shortcut);
    SIGHT_ASSERT("Shortcut must not be empty", !m_shortcut.empty());

    m_wid = config_shortcut.get<std::string>("wid", m_wid);
    m_sid = config_shortcut.get<std::string>("sid", m_sid);
    SIGHT_ASSERT(
        "Either The wid or sid attribute must be specified for signal_shortcut",
        !m_wid.empty() || !m_sid.empty()
    );
}

//-----------------------------------------------------------------------------

void signal_shortcut::set_enabled(bool _enabled)
{
    {
        const auto enabled = m_enabled.lock();
        *enabled = _enabled;

        auto sig = enabled->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        core::com::connection::blocker block(sig->get_connection(slot(slots::APPLY_ENABLED)));
        sig->async_emit();
    }

    if(_enabled)
    {
        auto sig = this->signal<signals::void_t>(signals::ENABLED);
        sig->async_emit();
    }
    else
    {
        auto sig = this->signal<signals::void_t>(signals::DISABLED);
        sig->async_emit();
    }

    auto sig = this->signal<signals::bool_t>(signals::IS_ENABLED);
    sig->async_emit(_enabled);
}

//-----------------------------------------------------------------------------

void signal_shortcut::starting()
{
    sight::ui::container::widget::sptr fwc = nullptr;

    // Either get the container via a service id
    if(!m_sid.empty())
    {
        bool sid_exists = core::id::exist(m_sid);

        if(sid_exists)
        {
            service::base::sptr service = service::get(m_sid);
            auto container_srv          = std::dynamic_pointer_cast<sight::ui::service>(service);
            fwc = container_srv->get_container();
        }
        else
        {
            SIGHT_ERROR("Invalid service id " << m_sid);
        }
    }
    // or a window id
    else if(!m_wid.empty())
    {
        fwc = sight::ui::registry::get_wid_container(m_wid);
        if(!fwc)
        {
            SIGHT_ERROR("Invalid window id " << m_wid);
        }
    }

    if(fwc != nullptr)
    {
        auto qtc = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(fwc);
        if(qtc != nullptr)
        {
            if(m_shortcut_object == nullptr)
            {
                // Get the associated widget to use as parent for the shortcut
                QWidget* widget = qtc->get_qt_container();
                // Create a key sequence from the string and its associated QShortcut
                QKeySequence shortcut_sequence = QKeySequence(QString::fromStdString(m_shortcut));
                m_shortcut_object = new QShortcut(shortcut_sequence, widget);
                m_shortcut_object->setContext(Qt::ApplicationShortcut);
            }

            // Connect the activated signal to the onActivation method of this class
            QObject::connect(m_shortcut_object, &QShortcut::activated, this, &self_t::on_activation);
        }
    }
    else
    {
        SIGHT_ERROR(
            "Cannot setup shortcut " << m_shortcut << " on invalid "
            << (!m_wid.empty() ? "wid " + m_wid : "sid " + m_sid)
        );
    }
}

//-----------------------------------------------------------------------------

void signal_shortcut::stopping()
{
    if(m_shortcut_object != nullptr)
    {
        // Connect the activated signal to the onActivation method of this class
        QObject::disconnect(m_shortcut_object, &QShortcut::activated, this, &self_t::on_activation);
    }
}

//-----------------------------------------------------------------------------
void signal_shortcut::updating()
{
}

//------------------------------------------------------------------------------

service::connections_t signal_shortcut::auto_connections() const
{
    return {{m_enabled, sight::data::object::MODIFIED_SIG, slots::APPLY_ENABLED}};
}

//------------------------------------------------------------------------------

void signal_shortcut::on_activation()
{
    if(*m_enabled)
    {
        this->signal<signals::void_t>(signals::ACTIVATED)->async_emit();
    }
}

} // namespace sight::module::ui::qt::com
