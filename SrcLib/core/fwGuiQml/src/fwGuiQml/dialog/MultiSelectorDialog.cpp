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

#include "fwGuiQml/dialog/MultiSelectorDialog.hpp"

#include "fwGuiQml/model/RoleListModel.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <QGuiApplication>

fwGuiRegisterMacro( ::fwGuiQml::dialog::MultiSelectorDialog, ::fwGui::dialog::IMultiSelectorDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

MultiSelectorDialog::MultiSelectorDialog(::fwGui::GuiBaseObject::Key key) :
    m_message(""),
    m_title("")
{
}

//------------------------------------------------------------------------------

MultiSelectorDialog::~MultiSelectorDialog()
{
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setSelections(Selections _selections)
{
    this->m_selections = _selections;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setTitle(std::string _title)
{
    this->m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

::fwGui::dialog::IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    ::fwGuiQml::model::RoleListModel model;
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    m_isClicked                     = false;

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/MultiSelectorDialog.qml");

    // load the qml ui component
    engine->getRootContext()->setContextProperty("multiSelectorModel", &model);
    engine->getRootContext()->setContextProperty("multiSelectorDialog", this);
    QObject* dialog = engine->createComponent(dialogPath);
    Q_EMIT titleChanged();
    model.addRole(Qt::UserRole + 1, "textOption");
    model.addRole(Qt::UserRole + 2, "check");
    for( Selections::value_type selection :  m_selections)
    {
        QHash<QByteArray, QVariant> data;
        data.insert("textOption", QString::fromStdString(selection.first));
        data.insert("check", selection.second);
        model.addData(QHash<QByteArray, QVariant>(data));
    }
    if(!m_message.isNull() && !m_message.isEmpty())
    {
        Q_EMIT messageChanged();
    }
    QMetaObject::invokeMethod(dialog, "open");
    while (!m_isClicked && m_visible)
    {
        qGuiApp->processEvents();
    }
    delete dialog;
    return m_selections;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::resultDialog(QVariant checkList, bool state)
{
    if (state == true)
    {
        QList<QVariant> checkListState = checkList.toList();
        int index                      = 0;
        for( Selections::value_type selection :  m_selections)
        {
            m_selections[selection.first] = checkListState[index].toBool();
            index++;
        }
    }
    m_isClicked = true;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string& msg)
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
