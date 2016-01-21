/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO15MULTITHREADCTRL_SSHOWARRAY_HPP__
#define __TUTO15MULTITHREADCTRL_SSHOWARRAY_HPP__

#include "Tuto15MultithreadCtrl/config.hpp"

#include <fwServices/IController.hpp>


namespace Tuto15MultithreadCtrl
{

/**
 * @brief This service displays the array values.
 *
 * @note To see the information, you need to activate log level "info".
 */
class TUTO15MULTITHREADCTRL_CLASS_API SShowArray : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SShowArray)(::fwServices::IController) );

    TUTO15MULTITHREADCTRL_API SShowArray() throw();
    TUTO15MULTITHREADCTRL_API virtual ~SShowArray() throw();

protected:

    /// Do nothing
    virtual void starting() throw(::fwTools::Failed);

    /// Do nothing
    virtual void stopping() throw(::fwTools::Failed);

    /// Display the array values
    virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    virtual void configuring() throw(fwTools::Failed);
};

}  // namespace Tuto15MultithShowCtrl

#endif  // __TUTO15MULTITHREADCTRL_SSHOWARRAY_HPP__

