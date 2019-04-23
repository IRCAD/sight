/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/dialog/SelectorDialog.hpp"

#include "fwGuiQml/model/RoleListModel.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <QGuiApplication>

fwGuiRegisterMacro( ::fwGuiQml::dialog::SelectorDialog, ::fwGui::dialog::ISelectorDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

SelectorDialog::SelectorDialog(::fwGui::GuiBaseObject::Key key) :
    m_message(""),
    m_title("")
{
}

//------------------------------------------------------------------------------

SelectorDialog::~SelectorDialog()
{
}

//------------------------------------------------------------------------------

void SelectorDialog::setSelections(std::vector< std::string > _selections)
{
    this->m_selections = _selections;
}

//------------------------------------------------------------------------------

void SelectorDialog::setTitle(std::string _title)
{
    this->m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

std::string SelectorDialog::show()
{
    ::fwGuiQml::model::RoleListModel model;
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    m_isClicked                     = false;

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/SelectorDialog.qml");

    // load the qml ui component
    engine->getRootContext()->setContextProperty("selectorDialog", this);
    QObject* dialog = engine->createComponent(dialogPath);
    Q_EMIT titleChanged();

    model.addRole(Qt::UserRole + 1, "textOption");
    model.addRole(Qt::UserRole + 2, "check");
    for(std::string selection :  m_selections)
    {
        QHash<QByteArray, QVariant> data;
        if (!model.isEmpty())
        {
            data.insert("check", true);
        }
        else
        {
            data.insert("check", false);
        }
        data.insert("textOption", QString::fromStdString(selection));
        model.addData(QHash<QByteArray, QVariant>(data));
    }
    engine->getRootContext()->setContextProperty("selectorModel", &model);

    if(!m_message.isEmpty())
    {
        Q_EMIT messageChanged();
    }

    std::string selection = "";
    QMetaObject::invokeMethod(dialog, "open");
    while (!m_isClicked && m_visible)
    {
        qGuiApp->processEvents();
    }
    return m_selection.toStdString();
}

//------------------------------------------------------------------------------

void SelectorDialog::setMessage(const std::string& msg)
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

void SelectorDialog::resultDialog(QVariant selection)
{
    m_selection = selection.toString();
    m_isClicked = true;
}

//------------------------------------------------------------------------------

void SelectorDialog::addCustomButton(const std::string& label, std::function<void()> clickedFn)
{
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
