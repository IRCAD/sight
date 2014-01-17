/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#ifndef __IOMIDAS_FOLDERDIALOG_HPP__
#define __IOMIDAS_FOLDERDIALOG_HPP__

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include <QPointer>

#include "ioMidas/config.hpp"

namespace ioMidas
{
/**
 * @class FolderDialog
 *
 * @brief Dialog to specifiy the name of the folder where the patient will be save on Midas.
 * It provides two fields:
 * - the first one is MANDATORY and contains the FolderId.
 * - the second one is optional and contains extra information.
 * The midasFolder name will be the concatenation of the two fields separate by a dash by default.
 */
class IOMIDAS_CLASS_API FolderDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @name Constructor/Destructor.
     * @{ */
    IOMIDAS_API FolderDialog();
    ~FolderDialog() {}
    /**  @} */

    /// Returns folder name built with this dialog.
    std::string getMidasFolderName() const { return m_midasFolderName; }

    /// Sets separator shown between folder ID and extra information field.
    void setSeparator(const std::string& sep) { m_separator = sep; }

    /// Sets default folder ID.
    IOMIDAS_API void setFolderId(const std::string& folderId);

private :
    /**
     * @brief Sets label value showing final folder name according to m_txtFolderID and m_txtExtraInformation.
     */
    void setTxtMidasFolderName();

private Q_SLOTS:

    /**
     * @name Slots triggered when Midas folder name changes.
     * @{ */
    void onFolderIDChanged(const QString &txt);
    void onExtraInformationChanged(const QString &txt);
    /**  @} */

private :

    /**
     * @name Widgets to edit and show Midas folder name.
     * @{ */
    QPointer< QLineEdit > m_txtFolderID;
    QPointer< QLineEdit > m_txtExtraInformation;
    QPointer< QLabel > m_txtMidasFolderName;
    /**  @} */

    /// Current value for folder ID.
    std::string m_folderID;

    /// Value of extra information.
    std::string m_extraInformation;

    /// Separator set between folder ID and extra information field.
    std::string m_separator;

    /// The folder name on Midas formated like this : <m_folderID><m_separator><m_extraInformation>
    std::string m_midasFolderName;

};

} // namespace ioMidas

#endif // __IOMIDAS_FOLDERDIALOG_HPP__

