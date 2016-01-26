/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QIntValidator>

#include <boost/lexical_cast.hpp>

#include "ioMidas/FolderDialog.hpp"


namespace ioMidas
{

FolderDialog::FolderDialog() : m_folderID(""), m_extraInformation(""), m_separator("-"), m_midasFolderName("")
{
    m_txtFolderID = new QLineEdit();

    m_txtExtraInformation = new QLineEdit();
    m_txtExtraInformation->setValidator(new QIntValidator);
    m_txtExtraInformation->setCursorPosition(0);

    m_txtMidasFolderName = new QLabel();

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignRight);

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(tr("&Folder ID:"), m_txtFolderID);
    formLayout->addRow(tr("&Extra information (optional):"), m_txtExtraInformation);
    formLayout->addRow(tr("&Midas folder name:"), m_txtMidasFolderName);

    QPointer<QDialogButtonBox> buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    this->setWindowTitle(QString::fromStdString("Midas folder name"));
    this->setLayout(mainLayout);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QObject::connect(m_txtFolderID, SIGNAL(textChanged(const QString &)),
                     this, SLOT(onFolderIDChanged(const QString &)));
    QObject::connect(m_txtExtraInformation, SIGNAL(textChanged(const QString &)),
                     this, SLOT(onExtraInformationChanged(const QString &)));

    m_folderID = m_txtFolderID->text().toStdString();

    this->setTxtMidasFolderName();
}

//-----------------------------------------------------------------------------------------------------

void FolderDialog::setFolderId(const std::string& folderId)
{
    if(folderId != "0")
    {
        m_folderID = folderId;
        m_txtFolderID->setText(QString::fromStdString(folderId));
        this->setTxtMidasFolderName();
    }
}

//-----------------------------------------------------------------------------------------------------

void FolderDialog::onFolderIDChanged(const QString& txt)
{
    m_folderID = txt.toStdString();
    this->setTxtMidasFolderName();
}

//-----------------------------------------------------------------------------------------------------

void FolderDialog::onExtraInformationChanged(const QString& txt)
{
    m_extraInformation = txt.toStdString();
    this->setTxtMidasFolderName();
}

//-----------------------------------------------------------------------------------------------------

void FolderDialog::setTxtMidasFolderName()
{
    m_midasFolderName = m_folderID;

    if(!m_extraInformation.empty())
    {
        m_midasFolderName += m_separator + m_extraInformation;
    }

    m_txtMidasFolderName->setText(QString::fromStdString(m_midasFolderName));
}

} // namespace ioMidas

