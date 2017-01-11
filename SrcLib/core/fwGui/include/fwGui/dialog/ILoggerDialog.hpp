/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_DIALOG_ILOGGERDIALOG_HPP__
#define __FWGUI_DIALOG_ILOGGERDIALOG_HPP__

#include <string>
#include <vector>

#include <fwGui/GuiBaseObject.hpp>
#include <fwLog/Logger.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{
namespace dialog
{

/**
 * @brief ILoggerDialog is used to display a dialog message and a set of logs
 */
class FWGUI_CLASS_API ILoggerDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ILoggerDialog)(::fwGui::GuiBaseObject) );

    typedef std::string FactoryRegistryKeyType;

    /// This *unique* key should  be used *for all* factory for specific Selector(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Constructor
     */
    FWGUI_API ILoggerDialog();

    /// Destructor
    FWGUI_API virtual ~ILoggerDialog();

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    FWGUI_API virtual void setTitle(const std::string& title) = 0;

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    FWGUI_API virtual void setMessage(const std::string& message) = 0;

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    FWGUI_API virtual void setLogger(const ::fwLog::Logger::sptr& logger) = 0;

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    FWGUI_API virtual bool show() = 0;
};

} //namespace dialog
} //namespace fwGui

#endif /*__FWGUI_DIALOG_ILOGGERDIALOG_HPP__*/
