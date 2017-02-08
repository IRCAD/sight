/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioMidas/ExportDialog.hpp"

#include "ioMidas/FolderDialog.hpp"

#include <fwCore/Exception.hpp>

#include <fwData/location/Folder.hpp>

#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>

#include <fwTools/System.hpp>

#include <midasIO/IResponseHandler.hpp>
#include <midasIO/ModelSeriesWriter.hpp>
#include <midasIO/RequestHandler.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QString>
#include <QTabWidget>
#include <QUrl>
#include <QVBoxLayout>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <json_spirit/json_spirit_writer_template.h>

#include <istream>

namespace ioMidas
{

// Temporary folder to save ModelSeries being exported on Midas server
static ::boost::filesystem::path TMP_PATH = ::fwTools::System::getTemporaryFolder();

//-----------------------------------------------------------------------------------------------------

void write(const ::fwMedData::ModelSeries::sptr& modelSeries, const std::string& folderName)
{
    ::boost::filesystem::path tmp_path = TMP_PATH / folderName;
    SLM_INFO("Writing ModelSeries to '" + tmp_path.string() + "'");

    ::boost::filesystem::create_directories(tmp_path);

    ::fwData::location::Folder::sptr meshPath = ::fwData::location::Folder::New();
    meshPath->setFolder(tmp_path);

    ::midasIO::ModelSeriesWriter::sptr writer = ::midasIO::ModelSeriesWriter::New();
    writer->setObject(modelSeries);
    writer->setLocation(meshPath);
    writer->write();
}

//-----------------------------------------------------------------------------------------------------

ExportDialog::ExportDialog(
    const ::midasIO::IConfiguration::sptr& config, const ::fwMedData::ModelSeries::sptr& modelSeries)
    : m_config(config), m_modelSeries(modelSeries)
{
    m_txtEmail = new QLineEdit();

    m_txtUrl = new QLineEdit();
    m_txtUrl->setText(QString::fromStdString(m_config->getUrl()));

    m_txtPassword = new QLineEdit();
    m_txtPassword->setEchoMode(QLineEdit::Password);

    m_btnConnect = new QPushButton("Connect");
    m_btnConnect->setEnabled(false);

    m_btnExport = new QPushButton("Export");
    m_btnExport->setEnabled(false);

    m_progress = new QProgressBar();
    m_progress->setVisible(false);

    m_txtAppName = new QLineEdit();
    m_txtAppName->setText(QString::fromStdString(m_config->getAppName()));

    m_txtFolder = new QLineEdit();
    m_txtFolder->setText(QString::fromStdString(m_config->getRootFolderName()));

    m_cbPermission = new QComboBox();
    m_cbPermission->addItem(QString::fromStdString("Public"));
    m_cbPermission->addItem(QString::fromStdString("Private"));
    m_cbPermission->addItem(QString::fromStdString("Parent"));

    m_cbLicense = new QComboBox();
    for(
        ::midasIO::IConfiguration::LicenseContainerType::value_type it : ::midasIO::IConfiguration::s_LICENSES)
    {
        m_cbLicense->insertItem(it.first, QString::fromStdString(it.second));
    }

    m_cbLicense->setCurrentIndex(m_config->getDefaultLicense());

    switch(m_config->getPrivacy())
    {
        case ::midasIO::IConfiguration::PUBLIC:
            m_cbPermission->setCurrentIndex(0); break;
        case ::midasIO::IConfiguration::PRIVATE:
            m_cbPermission->setCurrentIndex(1); break;
        case ::midasIO::IConfiguration::PARENT:
            m_cbPermission->setCurrentIndex(2); break;
        default: break;
    }

    QHBoxLayout* connectLayout = new QHBoxLayout();
    connectLayout->setAlignment(Qt::AlignRight);
    connectLayout->addWidget(m_btnConnect);

    QFormLayout* formLayout = new QFormLayout;
    formLayout->addRow(tr("Midas &URL:"), m_txtUrl);
    formLayout->addRow(tr("&Login:"), m_txtEmail);
    formLayout->addRow(tr("&Password:"), m_txtPassword);
    formLayout->addRow(connectLayout);

    QGroupBox* groupBox = new QGroupBox(tr("Connection"));
    groupBox->setLayout(formLayout);

    m_lstCommunityList = new QListWidget();
    m_lstCommunityList->setSelectionMode(QAbstractItemView::SingleSelection);

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(m_btnExport, Qt::AlignRight);

    QVBoxLayout* layoutCommunity = new QVBoxLayout();
    layoutCommunity->addWidget(m_lstCommunityList);
    layoutCommunity->addLayout(bottomLayout);

    QGroupBox* groupBoxList = new QGroupBox(tr("Select community"));
    groupBoxList->setLayout(layoutCommunity);

    m_txtStatus = new QLabel("");

    QHBoxLayout* statusLayout = new QHBoxLayout();
    statusLayout->setAlignment(Qt::AlignBottom);
    statusLayout->addWidget(m_txtStatus, Qt::AlignLeft);
    statusLayout->addWidget(m_progress, Qt::AlignRight);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(groupBox);
    mainLayout->addWidget(groupBoxList);
    mainLayout->addItem(statusLayout);

    QFormLayout* advancedLayout = new QFormLayout;
    advancedLayout->addRow(tr("&Application name:"), m_txtAppName);
    advancedLayout->addRow(tr("&Default folder:"), m_txtFolder);
    advancedLayout->addRow(tr("&Permission:"), m_cbPermission);
    advancedLayout->addRow(tr("&Default license:"), m_cbLicense);

    QTabWidget* tabWidget = new QTabWidget();

    QWidget* tabMain = new QWidget;
    tabMain->setLayout(mainLayout);

    QWidget* tabAdvanced = new QWidget;
    tabAdvanced->setLayout(advancedLayout);

    tabWidget->addTab(tabMain, QString::fromStdString("Main"));
    tabWidget->addTab(tabAdvanced, QString::fromStdString("Advanced"));

    QVBoxLayout* dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(tabWidget);

    this->setWindowTitle(QString::fromStdString("Export acquisition to Midas server"));
    this->setMinimumSize(500, 300);
    this->setLayout(dialogLayout);

    QObject::connect(m_btnConnect, SIGNAL(clicked()), this, SLOT(onConnectClicked()));
    QObject::connect(m_txtUrl, SIGNAL(textChanged(const QString &)), this, SLOT(onFormChanged(const QString &)));
    QObject::connect(m_txtPassword, SIGNAL(textChanged(const QString &)), this, SLOT(onFormChanged(const QString &)));
    QObject::connect(m_txtEmail, SIGNAL(textChanged(const QString &)), this, SLOT(onFormChanged(const QString &)));

    QObject::connect(m_txtAppName, SIGNAL(textChanged(const QString &)), this, SLOT(onAppNameChanged(const QString &)));
    QObject::connect(m_txtFolder, SIGNAL(textChanged(const QString &)), this, SLOT(onFolderChanged(const QString &)));
    QObject::connect(m_cbPermission, SIGNAL(currentIndexChanged(int)), this, SLOT(onPrivacyChanged(int)));
    QObject::connect(m_cbLicense, SIGNAL(currentIndexChanged(int)), this, SLOT(onLicenseChanged(int)));

    QObject::connect(m_lstCommunityList, SIGNAL(itemSelectionChanged()), this, SLOT(onCommunityChanged()));
    QObject::connect(m_btnExport, SIGNAL(clicked()), this, SLOT(onExportClicked()));
}

//-----------------------------------------------------------------------------------------------------

ExportDialog::~ExportDialog()
{
}

//-----------------------------------------------------------------------------------------------------

bool ExportDialog::isFormValid() const
{
    QUrl url(m_txtUrl->text());
    url.setScheme("http");
    const bool urlOk = !m_txtUrl->text().isEmpty() && url.isValid();

    return urlOk && !m_txtPassword->text().isEmpty() && !m_txtEmail->text().isEmpty();
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onConnectClicked()
{
    m_config->setEmail(m_txtEmail->text().toStdString());
    m_config->setPassword(m_txtPassword->text().toStdString());
    m_config->setUrl(m_txtUrl->text().toStdString());

    ::midasIO::RequestHandler reqHandler(m_config);
    const ::midasIO::IResponseHandler::sptr& respLogin = reqHandler.login();

    if(!respLogin->isSuccess())
    {
        m_txtStatus->setText(QString::fromStdString(respLogin->getErrorMsg()));

        m_communityList.clear();
        m_lstCommunityList->clear();
        m_btnExport->setEnabled(false);
    }
    else
    {
        m_txtStatus->setText("Connected");
        const ::midasIO::IResponseHandler::sptr& respList = reqHandler.getCommunityList();

        if(respList->isSuccess())
        {
            this->updateCommunityList(respList);
        }
        else
        {
            m_txtStatus->setText("Failed to retrieve community list");
            m_communityList.clear();
            m_lstCommunityList->clear();
        }
    }
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::updateCommunityList(const ::midasIO::IResponseHandler::sptr& handler)
{
    m_communityList.clear();

    ::midasIO::IResponseHandler::ArrayType data         = handler->getArrayData();
    ::midasIO::IResponseHandler::ArrayType::iterator it = data.begin();

    while(it != data.end())
    {
        ::json_spirit::mObject& obj = it->get_obj();
        SLM_ASSERT("Didn't find attribute 'name' in response", obj.find("name") != obj.end());
        SLM_ASSERT("Didn't find attribute 'folder_id' in response", obj.find("folder_id") != obj.end());
        const std::string& name = obj["name"].get_str();
        const std::string& uuid = obj["folder_id"].get_str();

        m_communityList[name] = uuid;
        ++it;
    }

    m_lstCommunityList->clear();

    for( ::midasIO::IConfiguration::CommunityListType::value_type c :  m_communityList)
    {
        m_lstCommunityList->addItem(QString::fromStdString(c.first));
    }

    m_btnExport->setEnabled(false);
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onCommunityChanged()
{
    m_btnExport->setEnabled(m_lstCommunityList->count() != 0 && m_lstCommunityList->currentRow() >= 0);
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onExportClicked()
{
    const std::string& communityName = m_lstCommunityList->currentItem()->text().toStdString();

    // Check if "Patients" folder exists in community
    ::midasIO::RequestHandler reqHandler(m_config);
    const std::string& folderId = m_communityList[communityName];
    ::midasIO::IResponseHandler::csptr resp = reqHandler.getChildren(folderId);

    if(!resp->isSuccess())
    {
        m_txtStatus->setText(QString::fromStdString(resp->getErrorMsg()));
        SLM_ERROR("Failed to get folder of id '" + folderId + "' on Midas : '" + resp->getErrorMsg() + "'");
        return;
    }

    ::midasIO::IResponseHandler::ObjectType obj = resp->getObjectData();
    SLM_ASSERT("Didn't find attribute 'folders' in response", obj.find("folders") != obj.end());

    ::json_spirit::mArray folders = resp->getObjectData()["folders"].get_array();

    bool rootFolderExists = false;

    // Search if root folder exists within retrieved folders
    for(::json_spirit::mArray::iterator it = folders.begin(); it != folders.end(); ++it)
    {
        ::json_spirit::mObject& obj = it->get_obj();
        SLM_ASSERT("Didn't find attribute 'name' in response", obj.find("name") != obj.end());
        SLM_ASSERT("Didn't find attribute 'folder_id' in response", obj.find("folder_id") != obj.end());
        const std::string& name = obj["name"].get_str();

        if(name == m_config->getRootFolderName())
        {
            m_config->setRootFolderId(obj["folder_id"].get_str());
            rootFolderExists = true;
            break;
        }
    }

    if(!rootFolderExists)
    {
        ::midasIO::IResponseHandler::csptr resp = reqHandler.createFolder(m_config->getRootFolderName(), "", folderId);

        if(resp->isSuccess())
        {
            ::midasIO::IResponseHandler::ObjectType obj = resp->getObjectData();
            SLM_ASSERT("Didn't find attribute 'name' in response", obj.find("name") != obj.end());
            SLM_ASSERT("Didn't find attribute 'folder_id' in response", obj.find("folder_id") != obj.end());
            m_config->setRootFolderId(obj["folder_id"].get_str());
        }
        else
        {
            m_txtStatus->setText(QString::fromStdString("Failed to create root folder"));
            return;
        }
    }

    //XXX Check patient
    ::fwMedData::Patient::sptr patient = m_modelSeries->getPatient();

    std::string laboID = ::boost::lexical_cast< std::string >(patient->getName());


    ::ioMidas::FolderDialog folderDialog;
    folderDialog.setFolderId(laboID);

    int ret = folderDialog.exec();
    if(ret == QDialog::Accepted)
    {
        laboID = folderDialog.getMidasFolderName();
    }
    else
    {
        return;
    }

    resp = reqHandler.createFolder(laboID, "", m_config->getRootFolderId());    // creates or updates folder

    if(!resp->isSuccess())
    {
        m_txtStatus->setText(QString::fromStdString("Failed to create folder '" + laboID + "'"));
        SLM_ERROR("Failed to create folder '" + laboID + "'");
    }
    else    // If folder creation succeeded, retrieve folder ID to upload files into folder later
    {
        ::midasIO::IResponseHandler::ObjectType obj = resp->getObjectData();
        SLM_ASSERT("Didn't find attribute 'name' in response", obj.find("name") != obj.end());
        SLM_ASSERT("Didn't find attribute 'folder_id' in response", obj.find("folder_id") != obj.end());

        this->exportAcquisition(laboID, obj["folder_id"].get_str());
    }
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::exportAcquisition(const std::string& laboID, const std::string& folderId)
{
    namespace fs = ::boost::filesystem;
    fs::path directory = TMP_PATH / laboID;

    m_progress->setRange(0, 0);
    m_progress->setVisible(true);
    this->setEnabled(false);
    m_progress->setEnabled(true);

    // Write acquisition to temporary folder
    write(m_modelSeries, laboID);

    // Compute size of acquisition
    int totalDirectorySize = 0;
    for(fs::directory_iterator it(directory); it != fs::directory_iterator(); ++it)
    {
        if(fs::extension(*it) == ".xml" || fs::extension(*it) == ".obj")
        {
            totalDirectorySize += fs::file_size(*it);
        }
    }

    m_progress->setRange(0, totalDirectorySize);
    m_txtStatus->setText(QString::fromStdString("Exporting acquisition..."));

    bool uploadSucceeded = true;
    ::midasIO::RequestHandler handler(m_config);
    std::string errorMsg = "";

    for(fs::directory_iterator it(directory); it != fs::directory_iterator(); ++it)
    {
        if(fs::extension(*it) == ".xml" || fs::extension(*it) == ".obj")
        {
            m_progress->setValue(m_progress->value() + fs::file_size(*it));

            try
            {
                ::midasIO::IResponseHandler::csptr resp = handler.uploadFile(
                    *it, folderId, m_cbLicense->currentIndex());

                if(!resp->isSuccess())
                {
                    const fs::path& path = *it;
                    errorMsg = resp->getErrorMsg();
                    SLM_ERROR("Failed to upload file '" + path.filename().string() + "': '" + errorMsg + "'");
                    m_progress->setValue(0);
                    uploadSucceeded = false;
                    break;
                }
            }
            catch(::fwCore::Exception e)
            {
                m_txtStatus->setText(QString::fromStdString(e.what()));
                this->setEnabled(true);
                m_progress->setVisible(false);
                fs::remove_all(directory);
                return;
            }
        }
    }

    m_txtStatus->setText(
        QString::fromStdString(
            uploadSucceeded
            ? "Export succeeded in folder " + laboID + "."
            : "Failed to export acquisition : '" + errorMsg + "'"));

    fs::remove_all(directory);

    m_progress->setVisible(false);
    this->setEnabled(true);
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onFormChanged(const QString& text)
{
    m_btnConnect->setEnabled(this->isFormValid());
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onAppNameChanged(const QString&)
{
    m_config->setAppName(m_txtAppName->text().toStdString());
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onFolderChanged(const QString&)
{
    m_config->setRootFolderName(m_txtFolder->text().toStdString());
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onLicenseChanged(int index)
{
    OSLM_ASSERT("License ID '" << index << "' is not valid",
                ::midasIO::IConfiguration::s_LICENSES.find(index) != ::midasIO::IConfiguration::s_LICENSES.end());
    m_config->setDefaultLicense(index);
}

//-----------------------------------------------------------------------------------------------------

void ExportDialog::onPrivacyChanged(int index)
{
    switch(index)
    {
        case 0:
            m_config->setPrivacy(::midasIO::IConfiguration::PUBLIC); break;
        case 1:
            m_config->setPrivacy(::midasIO::IConfiguration::PRIVATE); break;
        case 2:
            m_config->setPrivacy(::midasIO::IConfiguration::PARENT); break;
        default:
            break;
    }
}

} // namespace ioMidas

