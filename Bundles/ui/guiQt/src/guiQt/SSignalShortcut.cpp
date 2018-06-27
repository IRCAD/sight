/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/SSignalShortcut.hpp"

#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>
#include <fwGui/GuiRegistry.hpp>
#include <fwGui/IGuiContainerSrv.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <QKeySequence>
#include <QWidget>

#include <memory>

namespace guiQt
{

static const ::fwCom::Signals::SignalKeyType s_ACTIVATED_SIG = "activated";

fwServicesRegisterMacro( ::fwServices::IService, ::guiQt::SSignalShortcut );

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
    OSLM_ASSERT("Shortcut must not be empty", m_shortcut != "");

    m_wid = configShortcut.get<std::string>("wid", m_wid);
    m_sid = configShortcut.get<std::string>("sid", m_sid);
    OSLM_ASSERT("Either The wid or sid attribute must be specified for SSignalShortcut", m_wid != "" || m_sid != "");
}

//-----------------------------------------------------------------------------

void SSignalShortcut::starting()
{
    ::fwGui::container::fwContainer::sptr fwc = nullptr;

    // Either get the container via a service id
    if(m_sid != "")
    {
        bool sidExists = ::fwTools::fwID::exist(m_sid);

        if(sidExists)
        {
            ::fwServices::IService::sptr service         = ::fwServices::get( m_sid );
            ::fwGui::IGuiContainerSrv::sptr containerSrv = ::fwGui::IGuiContainerSrv::dynamicCast(service);
            fwc                                          = containerSrv->getContainer();
        }
        else
        {
            OSLM_ERROR("Invalid service id " << m_sid);
        }
    }
    // or a window id
    else if(m_wid != "")
    {
        fwc = ::fwGui::GuiRegistry::getWIDContainer(m_wid);
        if(!fwc)
        {
            OSLM_ERROR("Invalid window id " << m_wid);
        }
    }

    if(fwc != nullptr)
    {
        ::fwGuiQt::container::QtContainer::sptr qtc =
            std::dynamic_pointer_cast< ::fwGuiQt::container::QtContainer >(fwc);
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
        OSLM_ERROR("Cannot setup shortcut " << m_shortcut << " on invalid "
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

} // namespace gui
