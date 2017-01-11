/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOMIDAS_EXPORTDIALOG_HPP__
#define __IOMIDAS_EXPORTDIALOG_HPP__

#include <QDialog>
#include <QPointer>

#include <fwCore/macros.hpp>

#include <midasIO/IConfiguration.hpp>

#include "ioMidas/config.hpp"

class QLineEdit;
class QPushButton;
class QLabel;
class QListWidget;
class QComboBox;
class QProgressBar;

namespace fwMedData
{
class ModelSeries;
}

namespace midasIO
{
class IResponseHandler;
}

namespace ioMidas
{

/**
 * @class ExportDialog
 * @brief Dialog to export a patient to Midas server.
 * It provides a login method to the server (login, password and server URL required).
 * Once the user is logged in, a list of communities visible to user is shown.
 * Once a community is selected, the user can export the related patient to the server.
 */
class IOMIDAS_CLASS_API ExportDialog : public QDialog
{
Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param config Midas configuration
     * @param modelSeries acquisition to export
     */
    IOMIDAS_API ExportDialog(
        const SPTR(::midasIO::IConfiguration)& config,
        const SPTR(::fwMedData::ModelSeries)& modelSeries);

    /// Destructor
    IOMIDAS_API ~ExportDialog();

    /// Returns true if the form is valid (URL is valid, login and password are set)
    IOMIDAS_API bool isFormValid() const;

private Q_SLOTS:
    /**
     * @name Slots
     * @{ */
    void onConnectClicked();
    void onFormChanged(const QString&);

    void onAppNameChanged(const QString&);
    void onFolderChanged(const QString&);
    void onPrivacyChanged(int);
    void onLicenseChanged(int);

    void onCommunityChanged();
    void onExportClicked();
    /**  @} */

private:

    /**
     * @brief Fill the list widget of communities with the data from the given response.
     * @param handler a response object containing JSON data
     */
    void updateCommunityList(const SPTR(::midasIO::IResponseHandler)& handler);

    /**
     * @brief Midas configuration object
     * @see ::midasIO::IConfiguration
     */
    SPTR(::midasIO::IConfiguration) m_config;

    /**
     * @brief Export acquisition to Midas.
     * @param laboId acquistion laboID attribute
     * @param folderId Midas folder ID to export acquisition
     */
    void exportAcquisition(const std::string& laboId, const std::string& folderId);

    /// Acquisition to export to Midas server
    SPTR(::fwMedData::ModelSeries) m_modelSeries;

    /// List of Midas communities visible to the current user
    ::midasIO::IConfiguration::CommunityListType m_communityList;

    /**
     * @name Main tab widgets
     * @{ */
    QPointer< QLineEdit > m_txtUrl;
    QPointer< QLineEdit > m_txtEmail;
    QPointer< QLineEdit > m_txtPassword;
    QPointer< QLabel > m_txtStatus;
    QPointer< QPushButton > m_btnConnect;
    QPointer< QListWidget > m_lstCommunityList;
    QPointer< QPushButton > m_btnExport;
    QPointer< QProgressBar > m_progress;
    /**  @} */

    /**
     * @name Advanced setting tab widgets
     * @{ */
    QPointer< QLineEdit > m_txtAppName;
    QPointer< QLineEdit > m_txtFolder;
    QPointer< QComboBox > m_cbPermission;
    QPointer< QComboBox > m_cbLicense;
    /**  @} */

};

} // namespace ioMidas

#endif // __IOMIDAS_EXPORTDIALOG_HPP__

