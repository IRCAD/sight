/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_ACTION_SQUIT_HPP__
#define __GUI_ACTION_SQUIT_HPP__


#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

namespace gui
{
namespace action
{

/**
 * @brief   This action tries to close the window and reset root object.
 */
class GUI_CLASS_API SQuit : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SQuit)(::fwGui::IActionSrv) );

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SQuit() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SQuit() noexcept;

protected:

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or stopped
     */
    GUI_API void configuring();

    GUI_API virtual void starting();

    GUI_API virtual void stopping();

    /**
     * @brief This method tries to close the window and reset root object.
     */
    GUI_API virtual void updating();

    /*
     * @brief This method gives information about the class.
     */
    GUI_API virtual void info(std::ostream &_sstream );

};


} // namespace action
} // namespace gui


#endif /*__GUI_ACTION_SQUIT_HPP__*/
