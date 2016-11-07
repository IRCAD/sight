/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "beginnerTraining/tuto04/SStringColorEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <qboxlayout.h>
#include <qpalette.h>
#include <qwidget.h>

fwServicesRegisterMacro( ::gui::editor::IEditor, ::beginnerTraining::tuto04::SStringColorEditor );

namespace beginnerTraining
{
namespace tuto04
{
static const std::string s_EDIT_STRING_KEY = "editString";

const ::fwCom::Slots::SlotKeyType SStringColorEditor::s_CHANGE_COLOR_SLOT = "changeColor";

//-----------------------------------------------------------------------------

SStringColorEditor::SStringColorEditor()
{
    newSlot(s_CHANGE_COLOR_SLOT, &SStringColorEditor::changeColor, this);
}

//-----------------------------------------------------------------------------

SStringColorEditor::~SStringColorEditor() throw()
{
}

//-----------------------------------------------------------------------------

void SStringColorEditor::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize(); // start with this inherited function
}

//-----------------------------------------------------------------------------

void SStringColorEditor::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->create(); // start with this inherited function

    // Retrieve Qt container
    auto qtContainer   = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
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

//-----------------------------------------------------------------------------

void SStringColorEditor::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    // Disconnect m_textEditor
    QObject::disconnect(m_textEditor, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    this->getContainer()->clean(); // Erase widget content
    this->destroy(); // finish with this inherited function
}

//-----------------------------------------------------------------------------

void SStringColorEditor::updating() throw ( ::fwTools::Failed )
{
    // Update your textEditor from your associated object
    ::fwData::String::sptr myAssociatedData = this->getInOut< ::fwData::String >(s_EDIT_STRING_KEY);
    m_textEditor->setPlainText( myAssociatedData->getValue().c_str() );
}

//-----------------------------------------------------------------------------

void SStringColorEditor::onTextChanged()
{
    // Set new string value in your associated object
    ::fwData::String::sptr myAssociatedData = this->getInOut< ::fwData::String >(s_EDIT_STRING_KEY);
    myAssociatedData->setValue( m_textEditor->toPlainText().toStdString() );

    // Then, notifies listeners that the image has been modified
    this->notifyMessage();
}

//-----------------------------------------------------------------------------

void SStringColorEditor::notifyMessage()
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

void SStringColorEditor::changeColor(std::array<uint8_t, 4> color, std::string key)
{
    if(key=="borderColor")
    {
        SLM_TRACE_FUNC();
        // Retrieve Qt container
        auto qtContainer   = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer() );
        QWidget* container = qtContainer->getQtContainer();
        SLM_ASSERT("container not instanced", container);

        // Change background color
        QColor qtColor;
        qtColor.setRgb(color[0], color[1], color[2], color[3]);
        QPalette p ( container->palette() );
        p.setColor( QPalette::Background, qtColor );
        container->setPalette(p);
    }
}

//-----------------------------------------------------------------------------

} // namespace tuto04
} // namespace beginnerTraining



