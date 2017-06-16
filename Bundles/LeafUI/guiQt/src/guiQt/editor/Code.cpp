/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/editor/Code.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGuiQt/highlighter/CppHighlighter.hpp>
#include <fwGuiQt/highlighter/PythonHighlighter.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>

namespace guiQt
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::guiQt::editor::Code, ::fwData::String );

//------------------------------------------------------------------------------

const std::string Code::PYTHON = "Python";
const std::string Code::CPP    = "Cpp";

//------------------------------------------------------------------------------

Code::Code() throw() :
    m_language(PYTHON)
{
}

//------------------------------------------------------------------------------

Code::~Code() throw()
{
}

//------------------------------------------------------------------------------

void Code::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    ::fwData::String::sptr stringObj = this->getObject< ::fwData::String >();

    QHBoxLayout* layout = new QHBoxLayout();
    m_valueCtrl = new QTextEdit( );
    layout->addWidget( m_valueCtrl, 1);

    if(m_language == PYTHON )
    {
        m_highlighter = new ::fwGuiQt::highlighter::PythonHighlighter(m_valueCtrl->document());
    }
    else if(m_language == CPP )
    {
        m_highlighter = new ::fwGuiQt::highlighter::CppHighlighter(m_valueCtrl->document());
    }
    else
    {
        OSLM_WARN("Language "<<m_language<<" not yet supported.");
    }

    qtContainer->setLayout( layout );

    QObject::connect(m_valueCtrl, SIGNAL(textChanged()), this, SLOT(onModifyValue()));
    this->updating();
}

//------------------------------------------------------------------------------

void Code::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_valueCtrl, SIGNAL(textChanged(QString)), this, SLOT(onModifyValue(QString)));

    this->destroy();
}

//------------------------------------------------------------------------------

void Code::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    if(!vectConfig.empty())
    {
        std::vector < ConfigurationType > vectLanguage = vectConfig.at(0)->find("language");
        if(!vectLanguage.empty())
        {
            ConfigurationType configLanguage = vectLanguage.at(0);
            SLM_ASSERT("missing 'name' attribute in language tag", configLanguage->hasAttribute("name"));
            m_language = configLanguage->getAttributeValue("name");
        }
    }
}

//------------------------------------------------------------------------------

void Code::updating() throw(::fwTools::Failed)
{
    ::fwData::String::sptr stringObj = this->getObject< ::fwData::String >();
    SLM_ASSERT("The given string object is null", stringObj);

    m_valueCtrl->setText(QString::fromStdString(stringObj->value()));
    OSLM_TRACE(stringObj->getID() << " updated value : " << stringObj->value());
}

//------------------------------------------------------------------------------

void Code::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void Code::info( std::ostream& _sstream )
{
    _sstream << "String Editor";
}

//------------------------------------------------------------------------------

void Code::onModifyValue()
{
    QString value = m_valueCtrl->toPlainText();
    ::fwData::String::sptr stringObj = this->getObject< ::fwData::String >();
    ::fwData::String::sptr oldValue;
    oldValue = ::fwData::Object::copy(stringObj);

    std::string strValue = value.toStdString();
    stringObj->value() = strValue;

    if ( oldValue->value() != stringObj->value() )
    {
        OSLM_TRACE( stringObj->getID() << " modified");

        auto sig = stringObj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Code::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor

} // namespace guiQt
