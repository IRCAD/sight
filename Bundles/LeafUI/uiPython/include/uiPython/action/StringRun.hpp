/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIPYTHON_ACTION_STRINGRUN_HPP__
#define __UIPYTHON_ACTION_STRINGRUN_HPP__

#include "uiPython/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

namespace uiPython
{
namespace action
{

/**
 * @brief   This action tries to close the window and reset root object.
 */
class UIPYTHON_CLASS_API StringRun : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (StringRun)(::fwGui::IActionSrv) );

    /**
     * @brief Constructor. Do nothing.
     */
    UIPYTHON_API StringRun() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    UIPYTHON_API virtual ~StringRun() noexcept;

protected:

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or
     * stopped
     */
    UIPYTHON_API void configuring() override;

    UIPYTHON_API virtual void starting() override;

    UIPYTHON_API virtual void stopping() override;

    /**
     * @brief This method tries to close the window and reset root object.
     */
    UIPYTHON_API virtual void updating() override;

    /*
     * @brief This method gives information about the class.
     */
    UIPYTHON_API virtual void info(std::ostream& _sstream ) override;

};

} // namespace action
} // namespace uiPython

#endif /*__UIPYTHON_ACTION_STRINGRUN_HPP__*/
