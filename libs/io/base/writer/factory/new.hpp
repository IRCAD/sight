/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/base/config.hpp"
#include "io/base/writer/registry/detail.hpp"

#include <string>

namespace sight::io::base::writer
{

class IObjectWriter;

namespace factory
{

template<class CLASSNAME>
SPTR(CLASSNAME)  New();

/**
 * @brief Key class used to restrict access to Object construction.
 * See http://www.drdobbs.com/184402053
 */
class Key
{
template<typename CLASSNAME>
friend SPTR(CLASSNAME) io::base::writer::factory::New();

Key()
= default;
};

IO_BASE_API SPTR(io::base::writer::IObjectWriter) New(const io::base::writer::registry::KeyType& classname);

template<class CLASSNAME>
SPTR(CLASSNAME)  New()
{
    SPTR(CLASSNAME) obj = std::make_shared<CLASSNAME>(Key());

    return obj;
}

} // namespace factory

} // namespace sight::io::base::writer
