/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "guiQt/SSignalShortcut.hpp"

#include <guiQt/container/QtContainer.hpp>

#include <core/base.hpp>
#include <core/com/Signal.hxx>

#include <gui/container/fwContainer.hpp>
#include <gui/GuiRegistry.hpp>
#include <gui/IGuiContainerSrv.hpp>

#include <services/macros.hpp>
#include <services/op/Get.hpp>

#include <QKeySequence>
#include <QWidget>

#include <memory>

namespace sight::modules::guiQt
{

static const core::com::Signals::SignalKeyType s_ACTIVATED_SIG = "activated";

fwServicesRegisterMacro( ::sight::services::IService, ::sight::modules::guiQt::SSignalShortcut )

//-----------------------------------------------------------------------------

SSignalShortcut::SSignalShortcut() noexcept :
    m_shortcut(""),
    m_sid(""),
    m_wid(""),
    m_shortcutObject(nullptr)
{
    newSignal< ActivatedShortcutSignalType >(s_ACTIVATED_SIG);
}

//-----------------------------------------------------------------------------

SSignalShortcut::~SSignalShortcut() noexcept
{
}

//-----------------------------------------------------------------------------

void SSignalShortcut::configuring()
{
    const auto configTree     = this->getConfigTree();
    const auto configShortcut = configTree.get_child("config.<xmlattr>");
    m_shortcut = configShortcut.get<std::string>("shortcut", m_shortcut);
    SLM_ASSERT("Shortcut must not be empty", m_shortcut != "");

    m_wid = configShortcut.get<std::string>("wid", m_wid);
    m_sid = configShortcut.get<std::string>("sid", m_sid);
    SLM_ASSERT("Either The wid or sid attribute must be specified for SSignalShortcut", m_wid != "" || m_sid != "");
}

//-----------------------------------------------------------------------------

void SSignalShortcut::starting()
{
    gui::container::fwContainer::sptr fwc = nullptr;

    // Either get the container via a service id
    if(m_sid != "")
    {
        bool sidExists = core::tools::fwID::exist(m_sid);

        if(sidExists)
        {
            services::IService::sptr service         = services::get( m_sid );
            gui::IGuiContainerSrv::sptr containerSrv = gui::IGuiContainerSrv::dynamicCast(service);
            fwc = containerSrv->getContainer();
        }
        else
        {
            SLM_ERROR("Invalid service id " << m_sid);
        }
    }
    // or a window id
    else if(m_wid != "")
    {
        fwc = gui::GuiRegistry::getWIDContainer(m_wid);
        if(!fwc)
        {
            SLM_ERROR("Invalid window id " << m_wid);
        }
    }

    if(fwc != nullptr)
    {
        auto qtc = std::dynamic_pointer_cast< ::sight::guiQt::container::QtContainer >(fwc);
        if(qtc != nullptr)
        {

            if(!m_shortcutObject)
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
        SLM_ERROR("Cannot setup shortcut " << m_shortcut << " on invalid "
                                           << (m_wid != "" ? "wid " + m_wid : "sid " + m_sid));
    }
}

//-----------------------------------------------------------------------------

void SSignalShortcut::stopping()
{
    if(m_shortcutObject)
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
    this->signal<ActivatedShortcutSignalType>(s_ACTIVATED_SIG)->asyncEmit();
}

} // namespace sight::modules::gui
