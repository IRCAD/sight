/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iterator>
#include <iostream>

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwComEd/helper/Array.hpp>

#include <fwServices/macros.hpp>

#include "Tuto15MultithreadCtrl/SShowArray.hpp"

fwServicesRegisterMacro( ::fwServices::IService, ::Tuto15MultithreadCtrl::SShowArray, ::fwData::Array );

namespace Tuto15MultithreadCtrl
{

SShowArray::SShowArray() throw()
{
}

SShowArray::~SShowArray() throw()
{
}

void SShowArray::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void SShowArray::stopping() throw( ::fwTools::Failed )
{

}

void SShowArray::updating() throw( ::fwTools::Failed )
{
    ::fwData::Array::sptr array = this->getObject< ::fwData::Array >();
    ::fwData::mt::ObjectReadLock readLock(array);
    SLM_ASSERT("No array.", array);

    ::fwComEd::helper::Array arrayHelper(array);
    unsigned int *buffer = static_cast< unsigned int* >( arrayHelper.getBuffer() );

    std::cout << "Buffer : ";
    std::ostream_iterator<unsigned int> coutIter (std::cout,", ");
    std::copy(buffer, buffer+10, coutIter );
    std::cout << std::endl;
}

void SShowArray::swapping( ) throw( ::fwTools::Failed )
{

}

void SShowArray::configuring() throw( ::fwTools::Failed )
{

}

} // namespace Tuto15MultithreadCtrl
