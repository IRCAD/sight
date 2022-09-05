/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SShowArray.hpp"

#include <sstream>

namespace Tuto05MultithreadConsoleCpp
{

static const std::string s_ARRAY_INPUT = "array";

//------------------------------------------------------------------------------

SShowArray::SShowArray() noexcept =
    default;

//------------------------------------------------------------------------------

SShowArray::~SShowArray() noexcept =
    default;

//------------------------------------------------------------------------------

void SShowArray::configuring()
{
}

//------------------------------------------------------------------------------

void SShowArray::starting()
{
}

//------------------------------------------------------------------------------

void SShowArray::updating()
{
    const auto array = m_array.lock();

    const auto dumpLock = array->dump_lock();

    auto itr       = array->begin<unsigned int>();
    const auto end = array->end<unsigned int>();

    std::stringstream str;
    std::ostream_iterator<unsigned int> coutIter(str, ", ");
    std::copy(itr, end, coutIter);
    SIGHT_INFO("Buffer : " + str.str());
}

//------------------------------------------------------------------------------

void SShowArray::stopping()
{
}

//------------------------------------------------------------------------------

} // namespace Tuto05MultithreadConsoleCpp
