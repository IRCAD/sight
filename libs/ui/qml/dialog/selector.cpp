/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "ui/qml/dialog/selector.hpp"

#include "ui/qml/model/RoleListModel.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>

#include <ui/__/macros.hpp>
#include <ui/qml/QmlEngine.hpp>

#include <QGuiApplication>

SIGHT_REGISTER_GUI(sight::ui::qml::dialog::selector, sight::ui::dialog::selector_base::REGISTRY_KEY);

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

void selector::set_choices_preset(choices_preset_t _selections)
{
    this->m_choices = _selections;
}

//------------------------------------------------------------------------------

void selector::setTitle(std::string _title)
{
    this->m_title = QString::fromStdString(_title);
}

//------------------------------------------------------------------------------

void selector::set_multiple(bool _multiple)
{
    this->m_multiple = _multiple;
}

//------------------------------------------------------------------------------

selector::selections_t selector::show()
{
    // list model for the repeater
    ui::qml::model::RoleListModel model;
    // get the qml engine QmlApplicationEngine
    SPTR(ui::qml::QmlEngine) engine = ui::qml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    const auto dialogPath = core::runtime::get_library_resource_file_path("ui_qml/dialog/selector.qml");
    // set the root context for the model
    engine->getRootContext()->setContextProperty("selectorModel", &model);
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("selectorDialog", this);
    // load the qml ui component
    QObject* window = engine->createComponent(dialogPath, context);
    SIGHT_ASSERT("The Qml File selector is not found or not loaded", window);
    // keep window to destroy it

    window->setProperty("title", m_title);
    auto* dialog = window->findChild<QObject*>("dialog");
    SIGHT_ASSERT("The dialog is not found inside the window", dialog);

    // create all radiobutton
    model.addRole(Qt::UserRole + 1, "textOption");
    model.addRole(Qt::UserRole + 2, "check");
    for(const auto& selection : m_choices)
    {
        QHash<QByteArray, QVariant> data;
        data.insert("textOption", QString::fromStdString(selection.first));
        data.insert("check", selection.second);
        model.addData(QHash<QByteArray, QVariant>(data));
    }

    SIGHT_ASSERT("The selector need at least one option", !model.isEmpty());

    if(!m_message.isEmpty())
    {
        Q_EMIT messageChanged();
    }

    QEventLoop loop;
    //slot to retrieve the result and open the dialog with invoke
    connect(dialog, SIGNAL(accepted()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(rejected()), &loop, SLOT(quit()));
    connect(dialog, SIGNAL(reset()), &loop, SLOT(quit()));
    connect(window, SIGNAL(closing(QQuickCloseEvent*)), &loop, SLOT(quit()));
    QMetaObject::invokeMethod(dialog, "open");
    loop.exec();

    delete window;

    selections_t selections;
    for(const auto& selection : m_choices)
    {
        if(selection.second)
        {
            selections.push_back(selection.first);
        }
    }

    return selections;
}

//------------------------------------------------------------------------------

void selector::setMessage(const std::string& msg)
{
    m_message = QString::fromStdString(msg);
}

//------------------------------------------------------------------------------

void selector::resultDialog(QVariant selection)
{
    m_selection = selection.toString();
}

//------------------------------------------------------------------------------

void selector::addCustomButton(const std::string& /*label*/, std::function<void()> /*clickedFn*/)
{
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qml::dialog
