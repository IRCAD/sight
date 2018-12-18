/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
