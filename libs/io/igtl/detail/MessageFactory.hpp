/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/igtl/config.hpp"
#include "io/igtl/detail/exception/Conversion.hpp"

#include <core/tools/Failed.hpp>

#include <boost/function.hpp>

#include <igtlMessageBase.h>

#include <map>

namespace sight::io::igtl::detail
{

/**
 * @brief MessageFactory contains static function to create and registrate igtl messages in the factory (s_creators).
 */
class IO_IGTL_CLASS_API MessageFactory
{
public:

    typedef ::boost::function< ::igtl::MessageBase::Pointer()> CreatorType;
    typedef std::map<std::string, CreatorType> CreatorContainer;

    /**
     * @brief initFactory initialize the factory
     * @return the map of equivalence between a string device type igtl and a
     * method to create the appropiate message
     */
    static IO_IGTL_API CreatorContainer initFactory();
    /**
     * @brief create create a message with the type specified in parameter
     * @param[in] type
     * @return a smart pointer to a igtl message
     */
    static IO_IGTL_API ::igtl::MessageBase::Pointer create(std::string const& type);

    /// Map of equivalence between a device type and igtl message creator method
    static IO_IGTL_API CreatorContainer s_creators;

    /**
     * @struct MessageMaker
     * struct contain create function to create igtl message.
     * Hack to compile in msvc, with others compiler, this struct may be useless
     * (see assignement of  pointer function to a overload function to std::function).
     * @tparam T class derivate from ::igtl::MessageBase
     * @tparam Bool withParam, true use the specialized structure with parameters, false take the no parameters one.
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
        static ::igtl::MessageBase::Pointer createMessage()
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
        static ::igtl::MessageBase::Pointer createMessage(std::string const& bodyType)
        {
            typename T::Pointer msg;

            msg = T::New(bodyType);
            return ::igtl::MessageBase::Pointer(msg);
        }
    };

private:

    MessageFactory();
    ~MessageFactory();
};

} //namespace sight::io::igtl::detail
