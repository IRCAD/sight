/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#include "core/memory/stream/in/factory.hpp"

#include <core/macros.hpp>

#include <functional>

namespace sight::core::memory::stream::in
{

class CORE_CLASS_API buffer : public factory
{
public:

    typedef SPTR(void) lock_type;
    typedef std::function<lock_type()> counter_factory_type;

    buffer(void* buf, std::size_t size);
    buffer(void* buf, std::size_t size, counter_factory_type counter_factory);

protected:

    CORE_API SPTR(std::istream) get() override;

    void* m_buf;
    std::size_t m_size;
    counter_factory_type m_counter_factory;
};

} // namespace sight::core::memory::stream::in
