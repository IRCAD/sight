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
}

void SStringEditor::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
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
    // If event is UPDATED_OBJECT
    if ( _msg->hasEvent( ::fwServices::ObjectMsg::UPDATED_OBJECT  ) )
    {
        this->updating();
    }
}

void SStringEditor::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

} // namespace tuto03
} // namespace beginnerTraining


