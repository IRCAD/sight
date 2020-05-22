/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWMEMORY_STREAM_IN_IFACTORY_HPP__
#define __FWMEMORY_STREAM_IN_IFACTORY_HPP__

#include "fwMemory/config.hpp"

#include <fwCore/macros.hpp>

#include <istream>

namespace fwMemory
{
namespace stream
{
namespace in
{

class FWMEMORY_CLASS_API IFactory
{
public:
    virtual ~IFactory()
    {
    }

    SPTR(std::istream) operator()()
    {
        return this->get();
    }

protected:

    virtual SPTR(std::istream) get() = 0;

};

} // namespace in
} // namespace stream
} // namespace fwMemory

#endif // __FWMEMORY_STREAM_IN_IFACTORY_HPP__


