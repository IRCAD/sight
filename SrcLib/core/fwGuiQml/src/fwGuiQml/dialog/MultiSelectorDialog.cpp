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
    this->m_title = _title;
}

//------------------------------------------------------------------------------

::fwGui::dialog::IMultiSelectorDialog::Selections MultiSelectorDialog::show()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    QString title = QString::fromStdString(m_title);
    m_isClicked = false;

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/MultiSelectorDialog.qml");

    // load the qml ui component
    m_dialog = engine->createComponent(dialogPath);
    m_dialog->setProperty("title", title);
    QObject::connect(m_dialog, SIGNAL(resultDialog(QVariant)),
                     this, SLOT(resultDialog(QVariant)));
    QObject* options = m_dialog->findChild<QObject*>("options");
    for( Selections::value_type selection :  m_selections)
    {
        QMetaObject::invokeMethod(options, "addMessage",
                                  Q_ARG(QVariant, QString::fromStdString(selection.first)),
                                  Q_ARG(QVariant, selection.second));
    }

    if(!m_message.empty())
    {
        QObject* groupBox = m_dialog->findChild<QObject*>("groupBox");
        groupBox->setProperty("title", QString::fromStdString(m_message));
    }
    QMetaObject::invokeMethod(m_dialog, "open");
    while (!m_isClicked && m_dialog->property("visible").toBool())
    {
        qGuiApp->processEvents();
    }
    return m_selections;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::resultDialog(QVariant checkList)
{
    QList<QVariant> checkListState = checkList.toList();
    int index                      = 0;
    for( Selections::value_type selection :  m_selections)
    {
        m_selections[selection.first] = checkListState[index].toBool();
        index++;
    }
    m_isClicked = true;
}

//------------------------------------------------------------------------------

void MultiSelectorDialog::setMessage(const std::string& msg)
{
    m_message = msg;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
