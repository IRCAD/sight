/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <guiQt/container/QtContainer.hpp>
#include <guiQt/highlighter/CppHighlighter.hpp>
#include <guiQt/highlighter/PythonHighlighter.hpp>

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/String.hpp>

#include <services/macros.hpp>

#include <QHBoxLayout>

namespace sight::modules::guiQt
{

namespace editor
{

fwServicesRegisterMacro( ::sight::gui::editor::IEditor, ::sight::modules::guiQt::editor::Code, ::sight::data::String )

//------------------------------------------------------------------------------

const std::string Code::s_PYTHON = "Python";
const std::string Code::s_CPP = "Cpp";

static const services::IService::KeyType s_STRING_INOUT = "string";

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
    this->gui::IGuiContainerSrv::create();

    auto qtContainer = ::sight::guiQt::container::QtContainer::dynamicCast(this->getContainer() );

    data::String::sptr stringObj = this->getInOut< data::String >(s_STRING_INOUT);

    QHBoxLayout* layout = new QHBoxLayout();
    m_valueCtrl = new QTextEdit( );
    layout->addWidget( m_valueCtrl, 1);

    if(m_language == s_PYTHON )
    {
        m_highlighter = new ::sight::guiQt::highlighter::PythonHighlighter(m_valueCtrl->document());
    }
    else if(m_language == s_CPP )
    {
        m_highlighter = new ::sight::guiQt::highlighter::CppHighlighter(m_valueCtrl->document());
    }
    else
    {
        SLM_WARN("Language "<<m_language<<" not yet supported.");
    }

    qtContainer->setLayout( layout );

    QObject::connect(m_valueCtrl, SIGNAL(textChanged()), this, SLOT(onModifyValue()));
    this->updating();
}

//------------------------------------------------------------------------------

void Code::stopping()
{

    QObject::disconnect(m_valueCtrl, SIGNAL(textChanged()), this, SLOT(onModifyValue()));

    this->destroy();
}

//------------------------------------------------------------------------------

void Code::configuring()
{
    this->gui::IGuiContainerSrv::initialize();
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
    data::String::sptr stringObj = this->getInOut< data::String >(s_STRING_INOUT);
    SLM_ASSERT("The given string object is null", stringObj);

    m_valueCtrl->setText(QString::fromStdString(stringObj->value()));
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
    QString value                = m_valueCtrl->toPlainText();
    data::String::sptr stringObj = this->getInOut< data::String >(s_STRING_INOUT);
    data::String::sptr oldValue;
    oldValue = data::Object::copy(stringObj);

    std::string strValue = value.toStdString();
    stringObj->value() = strValue;

    if ( oldValue->value() != stringObj->value() )
    {

        auto sig = stringObj->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

//------------------------------------------------------------------------------

services::IService::KeyConnectionsMap Code::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_STRING_INOUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor

} // namespace sight::modules::guiQt
