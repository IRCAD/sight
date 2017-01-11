/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiIGTL/SServerConfigEditor.hpp"

#include <fwData/Object.hpp>
#include <fwServices/macros.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include <boost/integer_traits.hpp>
#include <boost/type.hpp>

fwServicesRegisterMacro (::gui::editor::IDialogEditor, ::uiIGTL::SServerConfigEditor, ::fwData::Object);

namespace uiIGTL
{
const ::fwCom::Signals::SignalKeyType SServerConfigEditor::s_CONFIGURATION_UPDATED_SIGNAL = "configurationUpdated";

//-----------------------------------------------------------------------------

SServerConfigEditor::SServerConfigEditor() :
    QObject()
{
    m_configurationUpdatedSignal = ConfigurationUpdatedSignalType::New();

    ::fwCom::HasSignals::m_signals (SServerConfigEditor::s_CONFIGURATION_UPDATED_SIGNAL,  m_configurationUpdatedSignal);
}

//-----------------------------------------------------------------------------

SServerConfigEditor::~SServerConfigEditor() throw()
{
}

//-----------------------------------------------------------------------------

void SServerConfigEditor::configuring() throw (::fwTools::Failed)
{
    QVBoxLayout* mainLayout;
    QPushButton* validateButton;
    QLabel* portLabel;
    QPushButton* cancelButton;
    QHBoxLayout* fieldLayout;
    QHBoxLayout* buttonsLayout;

    mainLayout     = new QVBoxLayout();
    validateButton = new QPushButton("Validate");
    cancelButton   = new QPushButton("Cancel");
    fieldLayout    = new QHBoxLayout();
    portLabel      = new QLabel("Enter port");
    buttonsLayout  = new QHBoxLayout();
    m_port         = new QSpinBox();

    mainLayout->addLayout(fieldLayout);
    mainLayout->addLayout(buttonsLayout);
    fieldLayout->addWidget(portLabel);
    fieldLayout->addWidget(m_port);

    buttonsLayout->addWidget(validateButton);
    buttonsLayout->addWidget(cancelButton);
    m_port->setMinimum(0);
    m_port->setMaximum(::boost::integer_traits< ::boost::uint16_t>::const_max);
    m_configDialog.setModal(true);
    m_configDialog.setLayout(mainLayout);
    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(onValidate()));
    QObject::connect(cancelButton, SIGNAL(clicked()), &m_configDialog, SLOT(close()));
}

//-----------------------------------------------------------------------------

void SServerConfigEditor::starting() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SServerConfigEditor::stopping() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SServerConfigEditor::updating() throw (::fwTools::Failed)
{
    m_configDialog.show();
}

//-----------------------------------------------------------------------------

void SServerConfigEditor::onValidate()
{
    m_configurationUpdatedSignal->asyncEmit((::boost::uint16_t)m_port->value());
    m_configDialog.close();
}

//-----------------------------------------------------------------------------

void SServerConfigEditor::swapping() throw (::fwTools::Failed)
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace uiIGTL
