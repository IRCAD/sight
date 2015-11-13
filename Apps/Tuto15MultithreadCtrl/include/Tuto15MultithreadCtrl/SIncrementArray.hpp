/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO15MULTITHREADCTRL_SINCREMENTARRAY_HPP__
#define __TUTO15MULTITHREADCTRL_SINCREMENTARRAY_HPP__

#include <fwServices/IService.hpp>


#include "Tuto15MultithreadCtrl/config.hpp"

fwCorePredeclare((fwThread)(Timer));

namespace Tuto15MultithreadCtrl
{

class TUTO15MULTITHREADCTRL_CLASS_API SIncrementArray : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SIncrementArray)(::fwServices::IService) );

    TUTO15MULTITHREADCTRL_API SIncrementArray() throw();
    TUTO15MULTITHREADCTRL_API virtual ~SIncrementArray() throw();

    void setPeriod( unsigned int period )
    {
        m_periodInMillisec = period;
    }

protected:

    virtual void starting() throw(::fwTools::Failed);

    virtual void stopping() throw(::fwTools::Failed);

    virtual void swapping() throw ( ::fwTools::Failed );

    virtual void updating() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    /// Slot: starts the timer
    void startTimer();

    //virtual void info( std::ostream &_sstream ) ;

    SPTR( fwThread::Timer ) m_timer;

    unsigned int m_periodInMillisec;

};

}  // namespace Tuto15MultithreadCtrl

#endif  // __TUTO15MULTITHREADCTRL_SINCREMENTARRAY_HPP__

