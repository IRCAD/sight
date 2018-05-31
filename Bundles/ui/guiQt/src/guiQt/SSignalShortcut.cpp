/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/SSignalShortcut.hpp"

#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>

#include <QEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QWidget>

namespace guiQt
{

static const ::fwCom::Signals::SignalKeyType s_TRIGGERED_SIG = "triggered";

fwServicesRegisterMacro( ::fwServices::IService, ::guiQt::SSignalShortcut );

//-----------------------------------------------------------------------------

SSignalShortcut::SSignalShortcut() noexcept
{
    newSignal< TriggeredShortcutSignalType >(s_TRIGGERED_SIG);

}

//-----------------------------------------------------------------------------

SSignalShortcut::~SSignalShortcut() noexcept
{
}

//-----------------------------------------------------------------------------

void SSignalShortcut::starting()
{
    qApp->activeWindow()->installEventFilter(this);

}

//-----------------------------------------------------------------------------

void SSignalShortcut::stopping()
{
    // delete event filter
    this->removeEventFilter( qApp->activeWindow());
}

//-----------------------------------------------------------------------------

void SSignalShortcut::configuring()
{
    const auto configTree     = this->getConfigTree();
    const auto configShortcut = configTree.get_child("config.<xmlattr>");
    m_shortcut = configShortcut.get<std::string>("shortcut", "");
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
