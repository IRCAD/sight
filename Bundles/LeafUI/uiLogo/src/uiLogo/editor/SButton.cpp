/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiLogo/editor/SButton.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>
#include <fwTools/Object.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>
#include <QString>

namespace uiLogo
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiLogo::editor::SButton, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType TRIGGERED_SIG = "triggered";

//-----------------------------------------------------------------------------

SButton::SButton() throw ()
{
    m_sigTriggered = newSignal<TrigerredSignalType>(TRIGGERED_SIG);
}

//-----------------------------------------------------------------------------

SButton::~SButton() throw ()
{
}

//-----------------------------------------------------------------------------

void SButton::starting() throw (::fwTools::Failed)
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight, container);
    container->setLayout(layout);

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

    QObject::connect(button, SIGNAL(clicked()), this, SLOT(onClick()));
}

//-----------------------------------------------------------------------------

void SButton::stopping() throw (::fwTools::Failed)
{
    this->getContainer()->clean();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SButton::updating() throw (::fwTools::Failed)
{
    m_sigTriggered->asyncEmit();
}

//-----------------------------------------------------------------------------

void SButton::onClick()
{
    this->updating();
}

//-----------------------------------------------------------------------------

void SButton::configuring() throw (::fwTools::Failed)
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
