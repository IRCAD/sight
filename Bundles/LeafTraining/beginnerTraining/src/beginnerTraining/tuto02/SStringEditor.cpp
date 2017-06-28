/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "beginnerTraining/tuto02/SStringEditor.hpp"

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <qboxlayout.h>
#include <qpalette.h>
#include <qwidget.h>

fwServicesRegisterMacro( ::gui::editor::IEditor, ::beginnerTraining::tuto02::SStringEditor );

namespace beginnerTraining
{
namespace tuto02
{

//-----------------------------------------------------------------------------

SStringEditor::SStringEditor()
{
}

//-----------------------------------------------------------------------------

SStringEditor::~SStringEditor() throw()
{
}

//-----------------------------------------------------------------------------

void SStringEditor::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize(); // start with this inherited function
}

//-----------------------------------------------------------------------------

void SStringEditor::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create(); // start with this inherited function

    // Retrieve Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QWidget* const container = qtContainer->getQtContainer();

    // Change background color in red
    QPalette p( container->palette() );
    p.setColor( QPalette::Background, Qt::red );
    container->setPalette(p);
    container->setAutoFillBackground( true );

    // Create textEditor
    QVBoxLayout* layout = new QVBoxLayout( container );
    m_textEditor = new QTextEdit( container );
    m_textEditor->setPlainText( "Edit text !" );
    layout->addWidget( m_textEditor );
    qtContainer->setLayout( layout );
}

//-----------------------------------------------------------------------------

void SStringEditor::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->destroy();
}

//-----------------------------------------------------------------------------

void SStringEditor::updating() throw ( ::fwTools::Failed )
{
    // Update your textEditor from your associated object
    ::fwData::String::sptr myAssociatedData = this->getInOut< ::fwData::String >("editString");
    m_textEditor->setPlainText( myAssociatedData->getValue().c_str() );
}

//-----------------------------------------------------------------------------

} // namespace tuto02
} // namespace beginnerTraining

