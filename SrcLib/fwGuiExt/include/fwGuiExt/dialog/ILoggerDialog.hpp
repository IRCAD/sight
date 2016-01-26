/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIEXT_DIALOG_ILOGGERDIALOG_HPP__
#define __FWGUIEXT_DIALOG_ILOGGERDIALOG_HPP__

#include <string>
#include <vector>

#include <fwGui/GuiBaseObject.hpp>
#include <fwLog/Logger.hpp>

#include "fwGuiExt/config.hpp"

namespace fwGuiExt
{
namespace dialog
{

/**
 * @class ILoggerDialog
 * @brief ILoggerDialog is used to display a dialog message and a set of logs
 */
class FWGUIEXT_CLASS_API ILoggerDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ILoggerDialog)(::fwGui::GuiBaseObject) );

    typedef std::string FactoryRegistryKeyType;

    /// This *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    FWGUIEXT_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor
     */
    FWGUIEXT_API ILoggerDialog();

    /// Destructor
    FWGUIEXT_API virtual ~ILoggerDialog();

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    FWGUIEXT_API virtual void setTitle(const std::string& title) = 0;

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    FWGUIEXT_API virtual void setMessage(const std::string& message) = 0;

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    FWGUIEXT_API virtual void setLogger(const ::fwLog::Logger::sptr& logger) = 0;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    FWGUIEXT_API virtual bool show() = 0;
};

} //namespace dialog
} //namespace fwGuiExt

#endif /*__FWGUIEXT_DIALOG_ILOGGERDIALOG_HPP__*/
