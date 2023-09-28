/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#pragma once

#include "core/macros.hpp"

#include <boost/noncopyable.hpp>
#include <boost/thread/once.hpp>

namespace sight::core
{

struct lazy_instantiator_default_tag {};

/**
 * @brief A base class for lazy instantiation, constructed on first demand.
 * @tparam INSTANCE_TYPE Type of object to instantiate.
 * @tparam TAG Tag used to allow several lazy_instantiator for one INSTANCE_TYPE.
 */
template<typename INSTANCE_TYPE, typename TAG = lazy_instantiator_default_tag>
class lazy_instantiator : boost::noncopyable
{
public:

    typedef INSTANCE_TYPE instance_type;
    typedef TAG tag_type;
    typedef std::shared_ptr<instance_type> instance_sptr_type;

    /// Returns the singleton instance. This method is thread safe.
    static instance_sptr_type get_instance()
    {
        static boost::once_flag flag = BOOST_ONCE_INIT;
        boost::call_once(&init_instance, flag);
        return instance();
    }

protected:

    /// Initializes the singleton instance
    static void init_instance()
    {
        instance();
    }

    /// Initializes once and returns the instance (not thread-safe)
    static instance_sptr_type instance()
    {
        static instance_sptr_type s_instance;
        if(!s_instance)
        {
            s_instance = std::make_shared<instance_type>();
        }

        return s_instance;
    }
};

} //namespace sight::core
