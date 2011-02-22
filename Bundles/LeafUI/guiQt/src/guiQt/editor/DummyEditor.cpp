/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <QVBoxLayout>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwTools/fwID.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "guiQt/editor/DummyEditor.hpp"

namespace gui
{
namespace editor
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::gui::editor::DummyEditor , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

DummyEditor::DummyEditor() throw() : m_text("")
{}

//-----------------------------------------------------------------------------

DummyEditor::~DummyEditor() throw()
{}

//-----------------------------------------------------------------------------

void DummyEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    assert( container ) ;

    QVBoxLayout* layout = new QVBoxLayout();
    std::string text = m_text.empty() ? this->getID() : m_text;
    m_staticText = new QLabel( QString::fromStdString(text), container);
    layout->addWidget( m_staticText );
    container->setLayout( layout );
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
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    assert( container );

    m_staticText->deleteLater();
    qtContainer->clean();
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

void DummyEditor::updating(::fwServices::ObjectMsg::csptr _msg) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DummyEditor::info( std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

} // namespace editor
} // namespace gui

