/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiLogo/editor/SButton.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Object.hpp>

#include <QBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace uiLogo
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiLogo::editor::SButton, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType TRIGGERED_SIG = "triggered";

//-----------------------------------------------------------------------------

SButton::SButton() noexcept
{
    m_sigTriggered = newSignal<TrigerredSignalType>(TRIGGERED_SIG);
}

//-----------------------------------------------------------------------------

SButton::~SButton() noexcept
{
}

//-----------------------------------------------------------------------------

void SButton::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);

    QPushButton* button = new QPushButton();

    if(m_image != "")
    {
        QPixmap pixmap(m_image.c_str());
        button->setIcon(QIcon(pixmap));
        button->setIconSize(pixmap.size());
    }

    if(m_tooltip != "")
    {
        button->setToolTip(m_tooltip.c_str());
    }

    if(m_label != "")
    {
        button->setText(m_label.c_str());
    }

    layout->addWidget(button);

    qtContainer->setLayout(layout);

    QObject::connect(button, SIGNAL(clicked()), this, SLOT(onClick()));
}

//-----------------------------------------------------------------------------

void SButton::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SButton::updating()
{
    m_sigTriggered->asyncEmit();
}

//-----------------------------------------------------------------------------

void SButton::onClick()
{
    this->updating();
}

//-----------------------------------------------------------------------------

void SButton::configuring()
{
    this->initialize();

    if(m_configuration->findConfigurationElement("image"))
    {
        m_image = m_configuration->findConfigurationElement("image")->getValue();
    }
    if(m_configuration->findConfigurationElement("label"))
    {
        m_label = m_configuration->findConfigurationElement("label")->getValue();
    }
    if(m_configuration->findConfigurationElement("tooltip"))
    {
        m_tooltip = m_configuration->findConfigurationElement("tooltip")->getValue();
    }

}

//-----------------------------------------------------------------------------

void SButton::info(std::ostream& _sstream)
{
}

//-----------------------------------------------------------------------------

}   //namespace editor
}   //namespace uiLogo
