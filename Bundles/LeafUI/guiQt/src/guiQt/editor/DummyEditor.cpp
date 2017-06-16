/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/editor/DummyEditor.hpp"

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <QVBoxLayout>

#include <string>

namespace gui
{
namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::gui::editor::DummyEditor, ::fwData::Object );

//-----------------------------------------------------------------------------

DummyEditor::DummyEditor() throw() :
    m_text("")
{
}

//-----------------------------------------------------------------------------

DummyEditor::~DummyEditor() throw()
{
}

//-----------------------------------------------------------------------------

void DummyEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();
    std::string text    = m_text.empty() ? this->getID() : m_text;
    m_staticText = new QLabel( QString::fromStdString(text));
    layout->addWidget( m_staticText );
    qtContainer->setLayout( layout );
    QPalette palette;
    QColor color(rand()%256, rand()%256, rand()%256);
    palette.setBrush(QPalette::Window, QBrush(color));
    m_staticText->setPalette(palette);
    m_staticText->setAutoFillBackground(true);
}

//-----------------------------------------------------------------------------

void DummyEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    SLM_ASSERT("container not instanced", qtContainer->getQtContainer());

    this->destroy();
}

//-----------------------------------------------------------------------------

void DummyEditor::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    this->initialize();

    ::fwRuntime::ConfigurationElement::sptr viewsCfgElt = m_configuration->findConfigurationElement("text");
    if(viewsCfgElt)
    {
        m_text = viewsCfgElt->getValue();
    }
}

//-----------------------------------------------------------------------------

void DummyEditor::updating() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    QPalette palette;
    QColor color(rand()%256, rand()%256, rand()%256);
    palette.setBrush(QPalette::Window, QBrush(color));
    m_staticText->setPalette(palette);
    m_staticText->setAutoFillBackground(true);
}

//-----------------------------------------------------------------------------

void DummyEditor::info( std::ostream& _sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

} // namespace editor
} // namespace gui

