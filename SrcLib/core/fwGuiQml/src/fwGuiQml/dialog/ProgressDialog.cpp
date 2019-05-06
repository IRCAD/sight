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
#include <QQmlProperty>
#include <QQuickWindow>

fwGuiRegisterMacro( ::fwGuiQml::dialog::ProgressDialog, ::fwGui::dialog::IProgressDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog( ::fwGui::GuiBaseObject::Key key, const std::string& title, const std::string& message)
{
    m_visible = false;
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-" FWGUIQML_VER "/dialog/ProgressDialog.qml");
    // set the context for the new component
    QSharedPointer<QQmlContext> context = QSharedPointer<QQmlContext>(new QQmlContext(engine->getRootContext()));
    context->setContextProperty("progressDialog", this);
    // load the qml ui component
    m_dialog = engine->createComponent(dialogPath, context);

    //TODO FIXME: change this progress dialog to only have QProperty and no findChild and find a way to update the
    // progress bar without the undetermine to true
    m_messageObject = m_dialog->findChild<QObject*>("message");
    m_valueObject   = m_dialog->findChild<QObject*>("progressBar");

    this->setTitle(title);
    this->setMessage(message);
    this->m_value = 0;
    Q_EMIT valueChanged();
    m_visible = true;
    QMetaObject::invokeMethod(m_dialog, "open");
}

//------------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    delete m_dialog;
}

//------------------------------------------------------------------------------

void ProgressDialog::operator()(float percent, std::string msg)
{
    SLM_ASSERT("m_dialog not instanced", m_dialog);
    // check if the dialog box has been closed by the user and cancel the progress
    if (!m_visible)
    {
        this->cancelPressed();
        return;
    }
    int value = int(percent*100);
    if(value != this->m_value)
    {
        OSLM_TRACE( "ProgressDialog msg" << msg << " : " << value <<"%");
        this->setMessage(msg);

        this->m_value = value;
        Q_EMIT valueChanged();

        m_valueObject->setProperty("text", QString::number(m_value));

        if ( m_processUserEvents )
        {
            qGuiApp->processEvents();
        }
        else
        {
            qGuiApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        }
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string& title)
{
    SLM_ASSERT("The progress dialog is not initialized or has been closed", m_dialog);
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    m_title                         = QString::fromStdString(title);
    Q_EMIT titleChanged();
    if (m_visible && !title.empty())
    {
        QMetaObject::invokeMethod(m_dialog, "close");
        qGuiApp->processEvents();
        m_dialog->setProperty("title", m_title);
        QMetaObject::invokeMethod(m_dialog, "open");
        qGuiApp->processEvents();
    }

}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string& msg)
{
    SLM_ASSERT("The progress dialog is not initialized or has been closed", m_dialog);
    m_message = "";
    if (!m_title.isEmpty())
    {
        m_message += m_title;
        m_message += " - ";
    }
    m_message = m_message + QString::fromStdString(msg);
    Q_EMIT messageChanged();
    emitMessageChanged(m_message);
    if (m_visible && !msg.empty())
    {
        m_messageObject->setProperty("text", m_message);
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::emitMessageChanged(QString message)
{
    m_message = message;
    Q_EMIT messageChanged();
}

//------------------------------------------------------------------------------

void ProgressDialog::cancelPressed()
{
    IProgressDialog::cancelPressed();
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
