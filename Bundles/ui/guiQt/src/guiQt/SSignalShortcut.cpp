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

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QWidget>

#include <memory>

namespace guiQt
{

static const ::fwCom::Signals::SignalKeyType s_TRIGGERED_SIG = "triggered";

fwServicesRegisterMacro( ::fwServices::IService, ::guiQt::SSignalShortcut );

//-----------------------------------------------------------------------------

SSignalShortcut::SSignalShortcut() noexcept :
    m_shortcut(""),
    m_wid("")
{
    newSignal< TriggeredShortcutSignalType >(s_TRIGGERED_SIG);
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
    SLM_ASSERT("Shortcut must not be empty", shortcut != "");

    m_wid = configShortcut.get<std::string>("wid", m_wid);
    SLM_ASSERT("The wid attribute must be specified for SSignalShortcut", m_wid != "");
}

//-----------------------------------------------------------------------------

void SSignalShortcut::starting()
{
    // Attempt to setup the event filter on the registered window
    ::fwGui::container::fwContainer::sptr fwc = ::fwGui::GuiRegistry::getWIDContainer(m_wid);
    if(fwc != nullptr)
    {
        ::fwGuiQt::container::QtContainer::sptr qtc =
            std::dynamic_pointer_cast< ::fwGuiQt::container::QtContainer >(fwc);
        if(qtc != nullptr)
        {
            QWidget* widget = qtc->getQtContainer();
            if(widget != nullptr)
            {
                widget->installEventFilter(this);
            }
        }
    }
    else
    {
        SLM_WARN("Cannot setup shortcut " << m_shortcut << " on invalid wid " << m_wid);
    }
}

//-----------------------------------------------------------------------------

void SSignalShortcut::stopping()
{
    // Delete event filter
    ::fwGui::container::fwContainer::sptr fwc = ::fwGui::GuiRegistry::getWIDContainer(m_wid);
    if(fwc != nullptr)
    {
        ::fwGuiQt::container::QtContainer::sptr qtc =
            std::dynamic_pointer_cast< ::fwGuiQt::container::QtContainer >(fwc);
        if(qtc != nullptr)
        {
            QWidget* widget = qtc->getQtContainer();
            if(widget != nullptr)
            {
                this->removeEventFilter( qApp->activeWindow());
            }
        }
    }
}

//-----------------------------------------------------------------------------
void SSignalShortcut::updating()
{

}

//-----------------------------------------------------------------------------
bool SSignalShortcut::eventFilter(QObject* obj, QEvent* event)
{
    // Update the widget position when the parent is moved or resized
    if (event->type() == QEvent::KeyPress)
    {
        QKeySequence shortcutSequence = QKeySequence(QString::fromStdString(m_shortcut));
        QKeyEvent* keyEvent           = static_cast<QKeyEvent*>(event);
        QKeySequence seq              = QKeySequence(keyEvent->key() | keyEvent->modifiers());
        if(seq.matches(shortcutSequence))
        {
            this->signal<TriggeredShortcutSignalType>(s_TRIGGERED_SIG)->asyncEmit();
        }
    }
    return QObject::eventFilter(obj, event);
}

} // namespace gui
