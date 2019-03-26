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

#include "fwGuiQml/dialog/ProgressDialog.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>
#include <fwGui/IFrameSrv.hpp>
#include <fwGui/registry/macros.hpp>

#include <QPaintEvent>

fwGuiRegisterMacro( ::fwGuiQml::dialog::ProgressDialog, ::fwGui::dialog::IProgressDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog( ::fwGui::GuiBaseObject::Key key, const std::string& title, const std::string& message) :
    m_title(""),
    m_pdialog( NULL ),
    m_pprogressbar( NULL ),
    m_pcancelButton( NULL ),
    m_pmainWindow( NULL )
{

    // Use progress widget defined by IFrameSrv
    ::fwGui::container::fwContainer::sptr progressWidget = ::fwGui::IFrameSrv::getProgressWidget();
    if (progressWidget != nullptr)
    {
        QWidget* activeWindow = ::fwGuiQml::container::QmlContainer::dynamicCast( progressWidget )->getQmlContainer();
        m_pmainWindow = qobject_cast< QMainWindow* >( activeWindow );
    }

//    QWidget *activeWindow = NULL;
//
//    BOOST_FOREACH (QWidget *widget, QApplication::topLevelWidgets())
//    {
//        activeWindow = qobject_cast< QMainWindow * >(widget);
//        // activeWindow must also have a layout to use statusBar()
//        if ( activeWindow && activeWindow->layout())
//        {
//            m_pmainWindow = qobject_cast< QMainWindow * >(activeWindow);
//            break;
//        }
//    }

    m_pcancelButton = new QPushButton("Cancel");
    QObject::connect( m_pcancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()) );

    if ( m_pmainWindow )
    {
        m_pprogressbar = new QProgressBar();
        m_pprogressbar->setRange(0, 100);
        m_pprogressbar->setValue(0);
        m_pmainWindow->statusBar()->addPermanentWidget(m_pprogressbar, 0);
        m_pmainWindow->statusBar()->addPermanentWidget(m_pcancelButton, 0);
        m_pmainWindow->statusBar()->setMinimumHeight(25);
        m_pmainWindow->statusBar()->setMaximumHeight(25);
    }
    else
    {
        m_pdialog = new QProgressDialog( 0, Qt::WindowStaysOnTopHint );
        //m_pdialog = new QProgressDialog( activeWindow, Qt::WindowStaysOnTopHint );

        // FIXME modal dialog has conflict with MessageHandler
        //m_pdialog->setWindowModality(Qt::WindowModal);
        m_pdialog->setWindowModality(Qt::NonModal);
        m_pdialog->setMinimum(0);
        m_pdialog->setMaximum(100);
        m_pdialog->setValue(0);
        m_pdialog->setCancelButton(m_pcancelButton);

        this->setTitle(title);
        this->setMessage(message);

        m_pdialog->show();
    }
}

//------------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    QObject::disconnect( m_pcancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()) );

    this->setTitle("");
    this->setMessage("");

    if (m_pdialog)
    {
        m_pdialog->hide();
        delete m_pdialog;
    }
    else if ( m_pprogressbar )
    {
        m_pmainWindow->statusBar()->removeWidget( m_pprogressbar );
        m_pmainWindow->statusBar()->removeWidget( m_pcancelButton );
        m_pcancelButton->hide();
        delete m_pcancelButton;
        m_pprogressbar->hide();
        delete m_pprogressbar;

    }

    m_pmainWindow = 0;
}

//------------------------------------------------------------------------------

void ProgressDialog::operator()(float percent, std::string msg)
{
    SLM_ASSERT("m_pdialog or m_pprogressbar not instanced", m_pprogressbar || m_pdialog);
    int value = (int)(percent*100);
    if(value != this->m_value)
    {
        OSLM_TRACE( "ProgressDialog msg" << msg << " : " << value <<"%");
        this->setMessage(msg);

        if ( m_pprogressbar )
        {
            m_pprogressbar->setValue(value);
        }
        else if ( m_pdialog )
        {
            m_pdialog->setValue(value);
        }

        if ( m_processUserEvents )
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
        else
        {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        this->m_value = value;
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string& title)
{
    m_title = QString::fromStdString(title);
    if ( m_pprogressbar )
    {
        m_pmainWindow->statusBar()->showMessage(m_title);
    }
    else if ( m_pdialog )
    {
        m_pdialog->setWindowTitle(m_title);
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string& msg)
{
    QString message("");
    if (!m_title.isEmpty())
    {
        message += m_title;
        message += " - ";
    }

    message += QString::fromStdString(msg);
    if ( m_pprogressbar )
    {
        m_pmainWindow->statusBar()->showMessage(message);
    }
    else if ( m_pdialog )
    {
        m_pdialog->setLabelText(message);
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::cancelPressed()
{
    IProgressDialog::cancelPressed();
}

//------------------------------------------------------------------------------
void ProgressDialog::hideCancelButton()
{
    m_pcancelButton->hide();
}

} // namespace dialog
} // namespace fwGuiQml
