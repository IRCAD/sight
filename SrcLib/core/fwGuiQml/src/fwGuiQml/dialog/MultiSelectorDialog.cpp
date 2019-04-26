/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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
    // the listmodel that will be added as a context for the repeater.
    ::fwGuiQml::model::RoleListModel model;
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    m_isClicked                     = false;

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath =
        ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/MultiSelectorDialog.qml");
    // set the root context for the model
    engine->getRootContext()->setContextProperty("multiSelectorModel", &model);
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("multiSelectorDialog", this);
    // load the qml ui component
    QObject* dialog = engine->createComponent(dialogPath, context);

    Q_EMIT titleChanged();
    // fill the repeater for each checkbox that has to be created
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
    SLM_ASSERT("The MultiSelector need at least one selection", !model.isEmpty());
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
        // retreive each check state of the selection list
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
