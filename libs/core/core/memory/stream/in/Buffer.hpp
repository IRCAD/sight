/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#pragma once

#include "core/config.hpp"
#include "core/memory/stream/in/IFactory.hpp"

#include <core/macros.hpp>

#include <functional>

namespace sight::core::memory
{
namespace stream
{
namespace in
{

class CORE_CLASS_API Buffer : public IFactory
{
public:
    typedef SPTR (void) LockType;
    typedef std::function< LockType() > CounterFactoryType;

    Buffer(void* buf, size_t size);
    Buffer(void* buf, size_t size, CounterFactoryType counterFactory);

protected:

    CORE_API SPTR(std::istream) get();

    void* m_buf;
    size_t m_size;
    CounterFactoryType m_counterFactory;
};

} // namespace in
} // namespace stream
} // namespace sight::core::memory
