/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto15MultithreadCtrl/SShowArray.hpp"

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwServices/macros.hpp>

#include <iterator>
#include <sstream>

fwServicesRegisterMacro( ::fwServices::IController, ::Tuto15MultithreadCtrl::SShowArray, ::fwData::Array );

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
}

void SShowArray::stopping() throw( ::fwTools::Failed )
{
}

void SShowArray::updating() throw( ::fwTools::Failed )
{
    ::fwData::Array::sptr array = this->getObject< ::fwData::Array >();
    ::fwData::mt::ObjectReadLock readLock(array);
    SLM_ASSERT("No array.", array);

    ::fwDataTools::helper::Array arrayHelper(array);
    unsigned int* buffer = static_cast< unsigned int* >( arrayHelper.getBuffer() );

    std::stringstream str;
    std::ostream_iterator<unsigned int> coutIter (str,", ");
    std::copy(buffer, buffer+10, coutIter );
    SLM_INFO("Buffer : " + str.str());
}

void SShowArray::configuring() throw( ::fwTools::Failed )
{
}

} // namespace Tuto15MultithreadCtrl
