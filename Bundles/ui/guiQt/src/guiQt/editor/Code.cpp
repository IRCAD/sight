/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::guiQt::editor::Code, ::fwData::String );

//------------------------------------------------------------------------------

const std::string Code::s_PYTHON = "Python";
const std::string Code::s_CPP    = "Cpp";

static const ::fwServices::IService::KeyType s_STRING_INOUT = "string";

//------------------------------------------------------------------------------

Code::Code() noexcept :
    m_language(s_PYTHON)
{
}

//------------------------------------------------------------------------------

Code::~Code() noexcept
{
}

//------------------------------------------------------------------------------

void Code::starting()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    ::fwData::String::sptr stringObj = this->getInOut< ::fwData::String >(s_STRING_INOUT);

    QHBoxLayout* layout = new QHBoxLayout();
    m_valueCtrl = new QTextEdit( );
    layout->addWidget( m_valueCtrl, 1);

    if(m_language == s_PYTHON )
    {
        m_highlighter = new ::fwGuiQt::highlighter::PythonHighlighter(m_valueCtrl->document());
    }
    else if(m_language == s_CPP )
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

void Code::stopping()
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_valueCtrl, SIGNAL(textChanged()), this, SLOT(onModifyValue()));

    this->destroy();
}

//------------------------------------------------------------------------------

void Code::configuring()
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

void Code::updating()
{
    ::fwData::String::sptr stringObj = this->getInOut< ::fwData::String >(s_STRING_INOUT);
    SLM_ASSERT("The given string object is null", stringObj);

    m_valueCtrl->setText(QString::fromStdString(stringObj->value()));
    OSLM_TRACE(stringObj->getID() << " updated value : " << stringObj->value());
}

//------------------------------------------------------------------------------

void Code::swapping()
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
    ::fwData::String::sptr stringObj = this->getInOut< ::fwData::String >(s_STRING_INOUT);
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

::fwServices::IService::KeyConnectionsMap Code::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_STRING_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor

} // namespace guiQt
