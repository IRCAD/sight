/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwCom/exception/BadRun.hpp"

#include "fwCom/SlotBase.hpp"
#include "fwCom/SlotBase.hxx"

namespace fwCom
{

#ifdef COM_LOG
size_t SlotBase::s_idCount = 0;
::fwCore::mt::Mutex SlotBase::s_mutexCounter;
#endif

void SlotBase::run() const
{
    typedef SlotRun< void () > SlotFuncType;
    const SlotFuncType *fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        fun->run();
    }
    else
    {
        OSLM_ERROR( "failed to run : " + m_signature + " with " + SlotBase::getTypeName< void() >() );
        FW_RAISE_EXCEPTION( fwCom::exception::BadRun( "Failed to find right signature for run"  ) );
    }
}

//-----------------------------------------------------------------------------

SlotBase::VoidSharedFutureType SlotBase::asyncRun() const
{
    typedef SlotRun< void () > SlotFuncType;
    const SlotFuncType *fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncRun();
    }
    else
    {
        OSLM_ERROR( "failed to asyncRun : " + m_signature + " with " + SlotBase::getTypeName< void() >() );
        FW_RAISE_EXCEPTION( fwCom::exception::BadRun( "Failed to find right signature for asyncRun"  ) );
    }
}

} // namespace fwCom

