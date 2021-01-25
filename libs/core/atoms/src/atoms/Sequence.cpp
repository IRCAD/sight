/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "atoms/Sequence.hpp"

#include "atoms/registry/macros.hpp"

#include <algorithm>
#include <functional>

fwAtomsRegisterMacro( sight::atoms::Sequence );

namespace sight::atoms
{

//------------------------------------------------------------------------------

Base::sptr Sequence::clone() const
{
    Sequence::sptr cloneSeq = Sequence::New();
    cloneSeq->m_value.resize(m_value.size());
    std::transform(m_value.begin(), m_value.end(), cloneSeq->m_value.begin(),
                   std::bind(&atoms::Base::clone, std::placeholders::_1));
    return cloneSeq;
}

}
