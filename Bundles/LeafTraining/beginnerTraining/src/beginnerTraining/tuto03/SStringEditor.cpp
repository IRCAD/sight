/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "beginnerTraining/tuto03/SStringEditor.hpp"

// Communication
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

// Service associated data
#include <fwData/String.hpp>

// To manipulate QtContainer
#include <fwGuiQt/container/QtContainer.hpp>

// Services tools
#include <fwServices/macros.hpp>

// Qt objects
#include <qwidget.h>
#include <qpalette.h>
#include <qboxlayout.h>



fwServicesRegisterMacro( ::gui::editor::IEditor, ::beginnerTraining::tuto03::SStringEditor, ::fwData::String );


namespace beginnerTraining
{
namespace tuto03
{

SStringEditor::SStringEditor()
{
}

SStringEditor::~SStringEditor() throw()
{
}

void SStringEditor::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize(); // start with this inherited function
}

void SStringEditor::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create(); // start with this inherited function

    // Retrieve Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    // Change background color in red
    QPalette p ( container->palette() );
    p.setColor( QPalette::Background, Qt::red );
    container->setPalette(p);
    container->setAutoFillBackground( true );

    // Create textEditor
    QVBoxLayout* layout = new QVBoxLayout( container );
    m_textEditor = new QTextEdit( container );
    m_textEditor->setPlainText( "Edit text !" );
    layout->addWidget( m_textEditor );
    container->setLayout( layout );

    // Connect m_textEditor
    QObject::connect(m_textEditor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

void SStringEditor::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Disconnect m_textEditor
    QObject::disconnect(m_textEditor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    this->getContainer()->clean(); // Erase widget content
    this->destroy(); // finish with this inherited function
}

void SStringEditor::updating() throw ( ::fwTools::Failed )
{
    // Update your textEditor from your associated object
    ::fwData::String::sptr myAssociatedData = this->getObject< ::fwData::String >();
    m_textEditor->setPlainText( myAssociatedData->getValue().c_str() );
}

void SStringEditor::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when object change
    this->stopping();
    this->starting();
}

void SStringEditor::onTextChanged()
{
    // Set new string value in your associated object
    ::fwData::String::sptr myAssociatedData = this->getObject< ::fwData::String >();
    myAssociatedData->setValue( m_textEditor->toPlainText().toStdString() );

    // Then, notifies listeners that the image has been modified
    this->notifyMessage();
}

void SStringEditor::notifyMessage()
{
    SLM_TRACE_FUNC();
    ::fwData::String::sptr associatedObj = this->getObject< ::fwData::String >();

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = associatedObj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}
} // namespace tuto03
} // namespace beginnerTraining



