/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>


#include "fwGuiQt/dialog/ProgressDialog.hpp"

#include <QPaintEvent>

REGISTER_BINDING( ::fwGui::dialog::IProgressDialog, ::fwGuiQt::dialog::ProgressDialog, ::fwGui::dialog::IProgressDialog::FactoryRegistryKeyType , ::fwGui::dialog::IProgressDialog::REGISTRY_KEY );


namespace fwGuiQt
{
namespace dialog
{
//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog( const std::string title, const std::string message)
{
    QWidget *activeWindow = qApp->activeWindow();

    if(!activeWindow)
    {
        BOOST_FOREACH (QWidget *widget, QApplication::topLevelWidgets())
        {
            activeWindow = qobject_cast< QMainWindow * >(widget);
            if (activeWindow)
            {
                break;
            }
        }
    }

    m_pmainWindow = qobject_cast< QMainWindow * >(activeWindow);

    m_pcancelButton = new QPushButton("Cancel");
    QObject::connect(m_pcancelButton, SIGNAL(clicked()), this, SLOT(cancelPressed()));

    if(m_pmainWindow)
    {
        m_pprogressbar = new QProgressBar();
        m_pprogressbar->setRange(0,100);
        m_pprogressbar->setValue(0);
        m_pmainWindow->statusBar()->addPermanentWidget(m_pprogressbar,0);
        m_pmainWindow->statusBar()->addPermanentWidget(m_pcancelButton,0);
        m_pmainWindow->statusBar()->setMinimumHeight(25);
        m_pmainWindow->statusBar()->setMaximumHeight(25);
    }
    else
    {
        m_pdialog = new QProgressDialog( activeWindow, Qt::WindowStaysOnTopHint );

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
    this->setTitle("");
    this->setMessage("");

    if (m_pdialog)
    {
        m_pdialog->hide();
        m_pdialog->deleteLater();
    }
    if (m_pprogressbar)
    {
        m_pprogressbar->hide();
        m_pprogressbar->deleteLater();
    }

    if (m_pprogressbar)
    {
        m_pcancelButton->hide();
        m_pcancelButton->deleteLater();
    }


    m_pmainWindow = 0;
}

//------------------------------------------------------------------------------

void ProgressDialog::operator()(float percent,std::string msg)
{
    SLM_ASSERT("m_pdialog or m_pprogressbar not instanced", m_pprogressbar || m_pdialog);
    int value = (int)(percent*100);
    OSLM_TRACE( "ProgressDialog msg" << msg << " : " << value <<"%");
    this->setMessage(msg);

    QWidget *widget;
    if (widget = m_pprogressbar)
    {
        m_pprogressbar->setValue(value);
    }
    else if (widget = m_pdialog)
    {
        m_pdialog->setValue(value);
    }

    if (m_processUserEvents)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }
    else
    {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
}

//------------------------------------------------------------------------------


void ProgressDialog::setTitle(const std::string &title)
{
    m_title = QString::fromStdString(title);
    if (m_pprogressbar)
    {
        m_pmainWindow->statusBar()->showMessage(m_title);
    }
    if (m_pdialog)
    {
        m_pdialog->setWindowTitle(m_title);
    }
}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string &msg)
{
    QString message("");
    if (!m_title.isEmpty())
    {
        message += m_title;
        message += " - ";
    }

    message += QString::fromStdString(msg);
    if (m_pprogressbar)
    {
        m_pmainWindow->statusBar()->showMessage(message);
    }
    if (m_pdialog)
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
} // namespace fwGuiQt
