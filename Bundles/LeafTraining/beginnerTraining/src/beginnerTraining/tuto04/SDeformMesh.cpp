/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "beginnerTraining/tuto04/SDeformMesh.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <qboxlayout.h>
#include <qpalette.h>
#include <qwidget.h>

fwServicesRegisterMacro( ::gui::editor::IEditor, ::beginnerTraining::tuto04::SDeformMesh );

namespace beginnerTraining
{
namespace tuto04
{

static const std::string s_EDIT_STRING_KEY = "editString";

//-----------------------------------------------------------------------------

SDeformMesh::SDeformMesh()
{
}

//-----------------------------------------------------------------------------

SDeformMesh::~SDeformMesh() throw()
{
}

//-----------------------------------------------------------------------------

void SDeformMesh::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize(); // start with this inherited function
}

//-----------------------------------------------------------------------------

void SDeformMesh::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create(); // start with this inherited function

    // Retrieve Qt container
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    // Change background color in red
    QWidget* const container = qtContainer->getQtContainer();
    QPalette p( container->palette() );
    p.setColor( QPalette::Background, Qt::red );
    container->setPalette(p);
    container->setAutoFillBackground( true );

    // Create textEditor
    QVBoxLayout* layout = new QVBoxLayout( );
    m_textEditor = new QTextEdit( );
    m_textEditor->setPlainText( "Edit text !" );
    layout->addWidget( m_textEditor );
    qtContainer->setLayout( layout );

    // Connect m_textEditor
    QObject::connect(m_textEditor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
}

//-----------------------------------------------------------------------------

void SDeformMesh::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Disconnect m_textEditor
    QObject::disconnect(m_textEditor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    // Destroy the container and all its children
    this->destroy();
}

//-----------------------------------------------------------------------------

void SDeformMesh::updating() throw ( ::fwTools::Failed )
{
    // Update your textEditor from your associated object
    ::fwData::String::sptr myAssociatedData = this->getInOut< ::fwData::String >(s_EDIT_STRING_KEY);
    m_textEditor->setPlainText( myAssociatedData->getValue().c_str() );
}

//-----------------------------------------------------------------------------

void SDeformMesh::onTextChanged()
{
    // Set new string value in your associated object
    ::fwData::String::sptr myAssociatedData = this->getInOut< ::fwData::String >(s_EDIT_STRING_KEY);
    myAssociatedData->setValue( m_textEditor->toPlainText().toStdString() );

    // Then, notifies listeners that the image has been modified
    this->notifyMessage();
}

//-----------------------------------------------------------------------------

void SDeformMesh::notifyMessage()
{
    SLM_TRACE_FUNC();
    auto associatedObj = this->getInOut< ::fwData::String >(s_EDIT_STRING_KEY);

    auto sig = associatedObj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection( this->slot(s_UPDATE_SLOT) ));
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

} // namespace tuto04
} // namespace beginnerTraining

