/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_DIALOG_LOGGERDIALOG_HPP__
#define __FWGUI_DIALOG_LOGGERDIALOG_HPP__

#include <fwCore/base.hpp>
#include <fwLog/Logger.hpp>

#include "fwGui/config.hpp"
#include "fwGui/dialog/ILoggerDialog.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief ILoggerDialog is used to display a dialog message and a set of logs
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation is fwQt libraries
 */
class FWGUI_CLASS_API LoggerDialog : public ILoggerDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LoggerDialog)(::fwGui::dialog::ILoggerDialog), (()), new LoggerDialog );

    /**
     * Opens a default logger dialog with the specified title, message and logs.
     * @param[in] title Title of the dialog
     * @param[in] message Message of the dialog
     * @param[in] logger Logger of the dialog
     * @return Returns true or false whether the user has selected the OK or CANCEL button
     */
    FWGUI_API static bool showLoggerDialog(const std::string& title, const std::string& message,
                                           const ::fwLog::Logger::sptr& logger);

    /// Constructor that will instantiate the concrete implementation
    FWGUI_API LoggerDialog();

    /// Destructor
    FWGUI_API virtual ~LoggerDialog();

    /**
     * Constructor. Create a selector with the specified title, message and logs.
     * @param[in] title Title of the dialog
     * @param[in] message Message of the dialog
     * @param[in] logger Logger of the dialog
     */
    FWGUI_API LoggerDialog(const std::string& title, const std::string& message,
                           const ::fwLog::Logger::sptr& logger);

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    FWGUI_API virtual void setTitle(const std::string& title);

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    FWGUI_API virtual void setMessage(const std::string& message);

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    FWGUI_API virtual void setLogger(const ::fwLog::Logger::sptr& logger);

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    FWGUI_API virtual bool show();

protected:

    ::fwGui::dialog::ILoggerDialog::sptr m_implementation;

};

} // namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_LOGGERDIALOG_HPP__*/


