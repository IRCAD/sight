/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO15MULTITHREADCTRL_SSHOWARRAY_HPP__
#define __TUTO15MULTITHREADCTRL_SSHOWARRAY_HPP__

#include <fwServices/IService.hpp>

#include "Tuto15MultithreadCtrl/config.hpp"

namespace Tuto15MultithreadCtrl
{

class TUTO15MULTITHREADCTRL_CLASS_API SShowArray : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SShowArray)(::fwServices::IService) );

    TUTO15MULTITHREADCTRL_API SShowArray() throw();
    TUTO15MULTITHREADCTRL_API virtual ~SShowArray() throw();

protected:

    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    virtual void swapping() throw ( ::fwTools::Failed );

    virtual void updating() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    //virtual void info( std::ostream &_sstream ) ;


};

}  // namespace Tuto15MultithShowCtrl

#endif  // __TUTO15MULTITHREADCTRL_SSHOWARRAY_HPP__

