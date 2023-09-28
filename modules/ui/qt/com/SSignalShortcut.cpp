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

#include "SSignalShortcut.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>

#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <ui/__/container/widget.hpp>
#include <ui/__/registry.hpp>
#include <ui/__/service.hpp>
#include <ui/qt/container/widget.hpp>

#include <QKeySequence>
#include <QWidget>

#include <memory>

namespace sight::module::ui::qt::com
{

static const core::com::signals::key_t ACTIVATED_SIG = "activated";

//-----------------------------------------------------------------------------

SSignalShortcut::SSignalShortcut() noexcept
{
    new_signal<ActivatedShortcutSignalType>(ACTIVATED_SIG);
}

//-----------------------------------------------------------------------------

SSignalShortcut::~SSignalShortcut() noexcept =
    default;

//-----------------------------------------------------------------------------

void SSignalShortcut::configuring()
{
    const auto configTree     = this->getConfiguration();
    const auto configShortcut = configTree.get_child("config.<xmlattr>");
    m_shortcut = configShortcut.get<std::string>("shortcut", m_shortcut);
    SIGHT_ASSERT("Shortcut must not be empty", !m_shortcut.empty());

    m_wid = configShortcut.get<std::string>("wid", m_wid);
    m_sid = configShortcut.get<std::string>("sid", m_sid);
    SIGHT_ASSERT(
        "Either The wid or sid attribute must be specified for SSignalShortcut",
        !m_wid.empty() || !m_sid.empty()
    );
}

//-----------------------------------------------------------------------------

void SSignalShortcut::starting()
{
    sight::ui::container::widget::sptr fwc = nullptr;

    // Either get the container via a service id
    if(!m_sid.empty())
    {
        bool sidExists = core::tools::id::exist(m_sid);

        if(sidExists)
        {
            service::base::sptr service = service::get(m_sid);
            auto containerSrv           = std::dynamic_pointer_cast<sight::ui::service>(service);
            fwc = containerSrv->getContainer();
        }
        else
        {
            SIGHT_ERROR("Invalid service id " << m_sid);
        }
    }
    // or a window id
    else if(!m_wid.empty())
    {
        fwc = sight::ui::registry::getWIDContainer(m_wid);
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
            if(m_shortcutObject == nullptr)
            {
                // Get the associated widget to use as parent for the shortcut
                QWidget* widget = qtc->getQtContainer();
                // Create a key sequence from the string and its associated QShortcut
                QKeySequence shortcutSequence = QKeySequence(QString::fromStdString(m_shortcut));
                m_shortcutObject = new QShortcut(shortcutSequence, widget);
            }

            // Connect the activated signal to the onActivation method of this class
            QObject::connect(m_shortcutObject, SIGNAL(activated()), this, SLOT(onActivation()));
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

void SSignalShortcut::stopping()
{
    if(m_shortcutObject != nullptr)
    {
        // Connect the activated signal to the onActivation method of this class
        QObject::disconnect(m_shortcutObject, SIGNAL(activated()), this, SLOT(onActivation()));
    }
}

//-----------------------------------------------------------------------------
void SSignalShortcut::updating()
{
}

//------------------------------------------------------------------------------

void SSignalShortcut::onActivation()
{
    this->signal<ActivatedShortcutSignalType>(ACTIVATED_SIG)->async_emit();
}

} // namespace sight::module::ui::qt::com
