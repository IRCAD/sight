/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// Qt objects
#include <qwidget.h>
#include <qpalette.h>
#include <qboxlayout.h>

// Service associated data
#include <fwData/String.hpp>

// Services tools
#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

// To manipulate QtContainer
#include <fwGuiQt/container/QtContainer.hpp>

#include "beginnerTraining/tuto03/SStringEditor.hpp"

REGISTER_SERVICE( ::gui::editor::IEditor, ::beginnerTraining::tuto03::SStringEditor, ::fwData::String );


namespace beginnerTraining
{
namespace tuto03
{

SStringEditor::SStringEditor()
{
    addNewHandledEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT );
}

SStringEditor::~SStringEditor() throw()
{}

void SStringEditor::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize(); // start with this inherited function
}

void SStringEditor::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create(); // start with this inherited function

    // Retreive Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
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

void SStringEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    m_textEditor->blockSignals(true);
    // If event is UPDATED_OBJECT
    if ( _msg->hasEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT  ) )
    {
        this->updating();
    }
    m_textEditor->blockSignals(false);
}

void SStringEditor::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

void SStringEditor::onTextChanged()
{
    // Set new string value in your associated object
    ::fwData::String::sptr myAssociatedData = this->getObject< ::fwData::String >();
    myAssociatedData->setValue( m_textEditor->toPlainText().toStdString() );

    // Then, notifies listerners that the image has been modified
    notifyMessage();
}

void SStringEditor::notifyMessage()
{
    SLM_TRACE_FUNC();
    ::fwData::String::sptr associatedObj = this->getObject< ::fwData::String >();

    // Creation of an object message to say that data is modified
    ::fwServices::ObjectMsg::NewSptr msg;
    msg->addEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT ) ;

    // Notifies message to all service listeners
    ::fwServices::IEditionService::notify( this->getSptr(), associatedObj, msg );
}
} // namespace tuto03
} // namespace beginnerTraining


