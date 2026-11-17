/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "core/tools/type_mapping.hpp"

#include <core/macros.hpp>

#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>

#include <stdexcept>

namespace sight::core::tools
{

/**
 * @brief   Limit cases for empty typelist
 * @see core::tools::dispatcher
 */
struct end_type_list_action
{
    /// Perform nothing see Dispatcher<>::invoke()
    static void invoke()
    {
    }

    /// Throw an exception to inform end-user that key_t value have no correspondence in type list
    template<class key_type>
    static void invoke(const key_type& _key_t)
    {
        std::string msg = _key_t.name()
                          + " : key_t value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
    }

    /// Throw an exception to inform end-user that key_t value have no correspondence in type list
    template<class key_type, class parameter>
    static void invoke(const key_type& _key_t, const parameter& _param)
    {
        SIGHT_NOT_USED(_param);
        std::string msg = _key_t.name()
                          + " : key_t value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
    }

    /// Throw an exception to inform end-user that key_t value have no correspondence in type list
    template<class key_type, class ... Args>
    static void invoke(const key_type& _key_t, Args&& ... /*_args*/)
    {
        std::string msg = _key_t.name()
                          + " : key_t value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
    }

    /// Throw an exception to inform end-user that key_t value have no correspondence in type list
    template<class base_class_t, class key_type>
    static base_class_t* instantiate(const key_type& _key_t)
    {
        std::string msg = _key_t.name()
                          + " : key_t value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
        return NULL;
    }
};

/**
 * @brief   Create an automatic template instantiater example Dispatcher< TYPESEQUENCE , FUNCTOR>::invoke("int");
 *
 * Will instantiate class FUNCTOR then for a type T in TYPESEQUENCE (here int) call the corresponding operator() method
 * according to parameter of invoke static method. ie FUNCTOR().operator<int>();
 */
template<class TSEQ, class FUNCTOR>
struct dispatcher
{
    private:

        using tail = boost::mpl::pop_front<TSEQ>::type;
        using head = boost::mpl::front<TSEQ>::type;

    public:

        /**
         * @brief Instantiate and invoke all functors
         */
        static void invoke()
        {
            namespace mpl = boost::mpl;

            // create the functor then execute it
            FUNCTOR f;
            f.template operator()<head>();

            // recursively call other element in the list
            using type_x = BOOST_DEDUCED_TYPENAME mpl::if_<
                mpl::empty<tail>,
                end_type_list_action,
                dispatcher<tail, FUNCTOR>
                           >::type;
            type_x::invoke();
        }

        /**
         * @brief Invoke only the specified Type only
         */
        template<class key_type>
        static void invoke(const key_type& _key_t)
        {
            namespace mpl = boost::mpl;

            if(is_mapping<head>(_key_t))
            {
                // create the functor then excute it
                FUNCTOR f;
                f.template operator()<head>();
            }
            else
            {
                // recursively call other element in the list
                using type_x = BOOST_DEDUCED_TYPENAME mpl::if_<
                    mpl::empty<tail>,
                    end_type_list_action,
                    dispatcher<tail, FUNCTOR>
                               >::type;
                type_x::invoke(_key_t);
            }
        }

        // NOTE gcc seems unable to explicit call of static template function member :/
        // all arguments needs to be present specified template seems ignored

        /**
         * @brief Invoke only the specified Type only with a fixed parameter
         * @note That parameter is *NOT* const so functor can update value
         */
        template<class key_type, class parameter>
        static void invoke(const key_type& _key_t, parameter& _param)
        {
            namespace mpl = boost::mpl;

            if(is_mapping<head>(_key_t))
            {
                // create the functor then excute it
                FUNCTOR f;
                f.template operator()<head>(_param);
            }
            else
            {
                // recursively call other element in the list
                using type_x = BOOST_DEDUCED_TYPENAME mpl::if_<
                    mpl::empty<tail>,
                    end_type_list_action,
                    dispatcher<tail, FUNCTOR>
                               >::type;
                type_x::invoke(_key_t, _param);
            }
        }

        /**
         * @brief Invoke only the specified Type only with a variable number of parameters
         * @note The parameters are perfectly forwarded to the functor
         */
        template<class key_type, class ... Args>
        static void invoke(const key_type& _key_t, Args&& ... _args)
        {
            namespace mpl = boost::mpl;

            if(is_mapping<head>(_key_t))
            {
                // create the functor then execute it
                FUNCTOR f;
                f.template operator()<head>(std::forward<Args>(_args) ...);
            }
            else
            {
                // recursively call other element in the list
                using type_x = BOOST_DEDUCED_TYPENAME mpl::if_<
                    mpl::empty<tail>,
                    end_type_list_action,
                    dispatcher<tail, FUNCTOR>
                               >::type;
                type_x::invoke(_key_t, std::forward<Args>(_args) ...);
            }
        }
};

using integer_types = boost::mpl::vector<std::int8_t, std::uint8_t,
                                         std::int16_t, std::uint16_t,
                                         std::int32_t, std::uint32_t>::type;

using intrinsic_types = boost::mpl::push_back<boost::mpl::push_back<integer_types, float>::type, double>::type;

} //end namespace sight::core::tools

#include <core/tools/type_key_type_mapping.hpp>
