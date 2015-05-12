/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO15MULTITHREADCTRL_SREADARRAY_HPP__
#define __TUTO15MULTITHREADCTRL_SREADARRAY_HPP__

#include <fwServices/IService.hpp>

#include "Tuto15MultithreadCtrl/config.hpp"

namespace Tuto15MultithreadCtrl
{

class TUTO15MULTITHREADCTRL_CLASS_API SReadArray : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SReadArray)(::fwServices::IService) );

    TUTO15MULTITHREADCTRL_API SReadArray() throw();
    TUTO15MULTITHREADCTRL_API virtual ~SReadArray() throw();

protected:

    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    virtual void swapping() throw ( ::fwTools::Failed );

    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    virtual void updating() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    //virtual void info( std::ostream &_sstream ) ;


};

}  // namespace Tuto15MultithreadCtrl

#endif  // __TUTO15MULTITHREADCTRL_SREADARRAY_HPP__

