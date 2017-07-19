/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DEVFORUM_TUTO04_IMAGEREADERACTIONSERVICE_HPP__
#define __DEVFORUM_TUTO04_IMAGEREADERACTIONSERVICE_HPP__

// Include abstract class
#include <fwGui/IActionSrv.hpp>

// Include export definitions
#include "devForum/config.hpp"

namespace devForum
{

namespace tuto04
{

/**
 * @brief   This service creates an action to read  a vtk image.
 * @class   ImageReaderActionService
 */
class DEVFORUM_CLASS_API ImageReaderActionService : public ::fwGui::IActionSrv
{

public:

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageReaderActionService)(::fwGui::IActionSrv) );

    /// Constructor, just called parent constructor to initialize standart IAction parameters.
    DEVFORUM_API ImageReaderActionService() noexcept;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageReaderActionService() noexcept;

protected:

    /// Called parent method to retrieve action parameters defined in the service configuration
    DEVFORUM_API virtual void configuring();

    /// Called parent method to create item menu and append it in the application menu
    DEVFORUM_API virtual void starting();

    // Uses parent method to remove item menu of the application
    DEVFORUM_API virtual void stopping();

    /// This method does nothing here.
    DEVFORUM_API virtual void updating();

};

} // end namespace tuto04

} // end namespace devForum


#endif // __DEVFORUM_TUTO04_IMAGEREADERACTIONSERVICE_HPP__
