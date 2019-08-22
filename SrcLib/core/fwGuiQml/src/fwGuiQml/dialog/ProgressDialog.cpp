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

#include "fwGuiQml/dialog/ProgressDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>
#include <fwGui/IFrameSrv.hpp>
#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <QGuiApplication>
#include <QObject>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickWindow>

fwGuiRegisterMacro( ::fwGuiQml::dialog::ProgressDialog, ::fwGui::dialog::IProgressDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog( ::fwGui::GuiBaseObject::Key key, const std::string& title, const std::string& message)
{
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    // find if toolBar exist on the ApplicationWindow
    const auto& rootObjects = engine->getRootObjects();
    QObject* toolBar        = nullptr;
    for (const auto& root: rootObjects)
    {
        toolBar = root->findChild<QObject*>("fwGuiQml_ProgressBar");
        if (toolBar)
        {
            break;
        }
    }
    // TODO: find a way to remove the context from rootContext but instead only on the object
    engine->getRootContext()->setContextProperty("progressDialog", this);
    if (toolBar)
    {
        // get the path of the qml ui file in the 'rc' directory
        const auto& dialogPath =
            ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/Progress.qml");
        // load the qml ui component
        m_dialog = engine->createComponent(dialogPath);
        SLM_ASSERT("The Qml File ProgressDialog is not found or not loaded", m_dialog);
        QQuickItem* item = qobject_cast<QQuickItem*>(m_dialog);
        // get ownership to not get Progress qml destroyed
        QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        // set visual parent of progress qml for render
        item->setParent(toolBar);
        // set the progress qml inside the root
        item->setParentItem(qobject_cast<QQuickItem*>(toolBar));
    }
    else
    {
        // get the path of the qml ui file in the 'rc' directory
        const auto& dialogPath =
            ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/ProgressDialog.qml");
        // load the qml ui component
        m_window = engine->createComponent(dialogPath);
        SLM_ASSERT("The Qml File ProgressDialog is not found or not loaded", m_window);
        m_window->setProperty("title", QString::fromStdString(title));
        m_dialog = m_window->findChild<QObject*>("dialog");
        SLM_ASSERT("The dialog is not found inside the window", m_dialog);
        QMetaObject::invokeMethod(m_dialog, "open");

    }
    m_visible = true;
    this->setTitle(title);
    this->setMessage(message);
}

//------------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    if (m_window)
    {
        m_window->deleteLater();
    }
    else
    {
        m_dialog->deleteLater();
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::operator()(float percent, std::string msg)
{
    SLM_ASSERT("m_dialog not instanced", m_dialog);
    // check if the dialog box has been closed by the user and cancel the progress
    if (!m_visible)
    {
        if (m_cancelCallback)
        {
            this->cancelPressed();
        }
        return;
    }
    const int& value = static_cast<int>(percent*100);
    if(value != this->m_value)
    {
        OSLM_TRACE( "ProgressDialog msg" << msg << " : " << value <<"%");

        this->m_value = value;
        this->setMessage(msg);
        this->setTitle(m_title.toStdString());
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string& title)
{
    SLM_ASSERT("The progress dialog is not initialized or has been closed", m_dialog);

    m_title = QString::fromStdString(title);
    if (m_window)
    {
        Q_EMIT titleChanged();
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string& msg)
{
    SLM_ASSERT("The progress dialog is not initialized or has been closed", m_dialog);
    QString message = "";
    QString title   = m_title;
    if (!title.isEmpty() && !this->m_window)
    {
        message += title;
        message += " - ";
    }
    message = message + QString::fromStdString(msg);
    if (m_visible)
    {
        QMetaObject::invokeMethod(m_dialog, "changeValue", Q_ARG(QVariant, message), Q_ARG(QVariant, qreal(m_value)));
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::hideCancelButton()
{
    m_hasCallback = false;
    Q_EMIT hasCallbackChanged();
}

//------------------------------------------------------------------------------

void ProgressDialog::cancelPressed()
{
    IProgressDialog::cancelPressed();
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
