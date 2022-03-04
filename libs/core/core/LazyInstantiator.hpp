/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

struct LazyInstantiatorDefaultTag {};

/**
 * @brief A base class for lazy instantiation, constructed on first demand.
 * @tparam INSTANCE_TYPE Type of object to instantiate.
 * @tparam TAG Tag used to allow several LazyInstantiator for one INSTANCE_TYPE.
 */
template<typename INSTANCE_TYPE, typename TAG = LazyInstantiatorDefaultTag>
class LazyInstantiator : boost::noncopyable
{
public:

    typedef INSTANCE_TYPE InstanceType;
    typedef TAG TagType;
    typedef std::shared_ptr<InstanceType> InstanceSptrType;

    /// Returns the singleton instance. This method is thread safe.
    static InstanceSptrType getInstance()
    {
        static boost::once_flag flag = BOOST_ONCE_INIT;
        boost::call_once(&initInstance, flag);
        return instance();
    }

protected:

    /// Initializes the singleton instance
    static void initInstance()
    {
        instance();
    }

    /// Initializes once and returns the instance (not thread-safe)
    static InstanceSptrType instance()
    {
        static InstanceSptrType s_instance;
        if(!s_instance)
        {
            s_instance = std::make_shared<InstanceType>();
        }

        return s_instance;
    }
};

} //namespace sight::core
