/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/io/igtl/config.hpp>

#include "io/igtl/detail/exception/conversion.hpp"

#include <core/tools/failed.hpp>

#include <boost/function.hpp>

#include <igtlMessageBase.h>

#include <map>

namespace sight::io::igtl::detail
{

/**
 * @brief MessageFactory contains static function to create and registrate igtl messages in the factory (s_creators).
 */
class SIGHT_IO_IGTL_CLASS_API message_factory
{
public:

    using creator_t           = boost::function< ::igtl::MessageBase::Pointer()>;
    using creator_container_t = std::map<std::string, creator_t>;

    message_factory()  = delete;
    ~message_factory() = delete;

    /**
     * @brief initFactory initialize the factory
     * @return the map of equivalence between a string device type igtl and a
     * method to create the appropiate message
     */
    static SIGHT_IO_IGTL_API creator_container_t init_factory();
    /**
     * @brief create create a message with the type specified in parameter
     * @param[in] _type
     * @return a smart pointer to a igtl message
     */
    static SIGHT_IO_IGTL_API ::igtl::MessageBase::Pointer create(std::string const& _type);

    /// Map of equivalence between a device type and igtl message creator method
    static SIGHT_IO_IGTL_API creator_container_t s_creators;

    /**
     * @struct MessageMaker
     * struct contain create function to create igtl message.
     * Hack to compile in msvc, with others compiler, this struct may be useless
     * (see assignement of  pointer function to a overload function to std::function).
     * @tparam T class derivate from ::igtl::MessageBase
     * @tparam withParam    true use the specialized structure with parameters, false take the no parameters one.
     */
    template<typename T, bool withParam>
    struct MessageMaker;

    /// Partial specialization of MessageMaker (with no parameters).
    template<typename T>
    struct MessageMaker<T, false>
    {
        /**
         * @brief create message from type specified in template
         * @return a ::igtl::MessageBase smart pointer contain the message
         */
        static ::igtl::MessageBase::Pointer create_message()
        {
            typename T::Pointer msg;

            msg = T::New();
            return ::igtl::MessageBase::Pointer(msg);
        }
    };

    /// Partial specialization of MessageMaker (with parameters).
    template<typename T>
    struct MessageMaker<T, true>
    {
        /**
         * @brief create message from type specified in template
         * @return a ::igtl::MessageBase smart pointer contain the message
         */
        static ::igtl::MessageBase::Pointer create_message(std::string const& _body_type)
        {
            typename T::Pointer msg;

            msg = T::New(_body_type);
            return ::igtl::MessageBase::Pointer(msg);
        }
    };
};

} //namespace sight::io::igtl::detail
