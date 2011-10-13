/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUM_TUTO4_IMAGEREADERACTIONSERVICE_HPP_
#define _DEVFORUM_TUTO4_IMAGEREADERACTIONSERVICE_HPP_

// Include abstract class
#include <fwGui/IActionSrv.hpp>

// Include export definitions
#include "devForum/config.hpp"

namespace devForum
{

namespace tuto04
{

/**
 * @brief   Service create an action to read  a vtk image
 * @class   ImageReaderActionService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class DEVFORUM_CLASS_API ImageReaderActionService : public ::fwGui::IActionSrv
{

public :

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageReaderActionService)(::fwGui::IActionSrv) );

    /// Constructor, just called parent constructor to initialize standart IAction parameters.
    DEVFORUM_API ImageReaderActionService() throw() ;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageReaderActionService() throw() ;

protected :

    /// Called parent method to retrieve action parameters defined in the service configuration
    DEVFORUM_API virtual void configuring() throw(::fwTools::Failed);

    /// Called parent method to create item menu and append it in the application menu
    DEVFORUM_API virtual void starting() throw(fwTools::Failed);

    // Uses parent method to remove item menu of the application
    DEVFORUM_API virtual void stopping() throw(fwTools::Failed);

    /// This method does nothing here.
    DEVFORUM_API virtual void updating() throw(fwTools::Failed);

    /// This method is called on msg notification, here does nothing
    DEVFORUM_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

};

} // end namespace tuto04

} // end namespace devForum


#endif // _DEVFORUM_TUTO4_IMAGEREADERACTIONSERVICE_HPP_
