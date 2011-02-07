/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QtCore>
#include <QString>
#include <QFutureWatcher>
#include <QApplication>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>


#include "fwGuiQt/dialog/PulseProgressDialog.hpp"


REGISTER_BINDING( ::fwGui::dialog::IPulseProgressDialog, ::fwGuiQt::dialog::PulseProgressDialog, ::fwGui::dialog::IPulseProgressDialog::FactoryRegistryKeyType , ::fwGui::dialog::IPulseProgressDialog::REGISTRY_KEY );


namespace fwGuiQt
{
namespace dialog
{
//------------------------------------------------------------------------------

PulseProgressDialog::PulseProgressDialog()
{
    m_pdialog = new QProgressDialog(qApp->activeWindow());
}

//------------------------------------------------------------------------------

PulseProgressDialog::~PulseProgressDialog()
{
    if (m_pdialog)
    {
        m_pdialog->setParent(0);
        m_pdialog->deleteLater();
    }
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setTitle(const std::string &title)
{
    m_pdialog->setWindowTitle(QString::fromStdString(title));
}

//------------------------------------------------------------------------------

void PulseProgressDialog::setMessage(const std::string &msg)
{
    m_pdialog->setLabelText(QString::fromStdString(msg));
}

//------------------------------------------------------------------------------

void PulseProgressDialog::show()
{
    // Create a QFutureWatcher and connect signals and slots.
    QFutureWatcher<void> futureWatcher;
    QObject::connect(&futureWatcher, SIGNAL(finished()), m_pdialog, SLOT(reset()));
    QObject::connect(m_pdialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), m_pdialog, SLOT(setRange(int,int)));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), m_pdialog, SLOT(setValue(int)));

    // Start the computation.
    futureWatcher.setFuture(QtConcurrent::run(m_stuff));

    m_pdialog->exec();
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQt
