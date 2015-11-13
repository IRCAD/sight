/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIEXT_DIALOG_LOGGERDIALOG_HPP__
#define __FWGUIEXT_DIALOG_LOGGERDIALOG_HPP__

#include <fwCore/base.hpp>
#include <fwLog/Logger.hpp>

#include "fwGuiExt/config.hpp"
#include "fwGuiExt/dialog/ILoggerDialog.hpp"

namespace fwGuiExt
{
namespace dialog
{
/**
 * @class LoggerDialog
 * @brief ILoggerDialog is used to display a dialog message and a set of logs
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation is fwQt libraries
 */
class FWGUIEXT_CLASS_API LoggerDialog : public ILoggerDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LoggerDialog)(::fwGuiExt::dialog::ILoggerDialog), (()), new LoggerDialog );

    /**
     * Opens a default logger dialog with the specified title, message and logs.
     * @param[in] title Title of the dialog
     * @param[in] message Message of the dialog
     * @param[in] logger Logger of the dialog
     * @return Returns true or false whether the user has selected the OK or CANCEL button
     */
    FWGUIEXT_API static bool showLoggerDialog(const std::string& title, const std::string& message,
                                              const ::fwLog::Logger::sptr& logger);

    /// Constructor that will instantiate the concrete implementation
    FWGUIEXT_API LoggerDialog();

    /// Destructor
    FWGUIEXT_API virtual ~LoggerDialog();

    /**
     * Constructor. Create a selector with the specified title, message and logs.
     * @param[in] title Title of the dialog
     * @param[in] message Message of the dialog
     * @param[in] logger Logger of the dialog
     */
    FWGUIEXT_API LoggerDialog(const std::string& title, const std::string& message,
                              const ::fwLog::Logger::sptr& logger);

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    FWGUIEXT_API virtual void setTitle(const std::string& title);

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    FWGUIEXT_API virtual void setMessage(const std::string& message);

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    FWGUIEXT_API virtual void setLogger(const ::fwLog::Logger::sptr& logger);

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    FWGUIEXT_API virtual bool show();

protected:

    ::fwGuiExt::dialog::ILoggerDialog::sptr m_implementation;

};

} // namespace dialog
} // namespace fwGuiExt

#endif /*__FWGUIEXT_DIALOG_LOGGERDIALOG_HPP__*/


