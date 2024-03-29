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

#include "core/tools/type_mapping.hpp"

#include <core/macros.hpp>

#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/pop_front.hpp>
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
 * Will instanciante class FUNCTOR then for a type T in TYPESEQUENCE (here int) call the corresponding operator() method
 * according to parameter of invoke static method. ie FUNCTOR().operator<int>();
 */
template<class TSEQ, class FUNCTOR>
struct dispatcher
{
    private:

        using tail = typename boost::mpl::pop_front<TSEQ>::type;
        using head = typename boost::mpl::front<TSEQ>::type;

    public:

        /**
         * @brief Instantiate and invoke all functors
         */
        static void invoke()
        {
            namespace mpl = boost::mpl;

            // create the functor then execute it
            FUNCTOR f;
#ifdef _WIN32
            f.operator()<head>();
#else
            f.template operator()<head>();
#endif

            // recursively call other element in the list
            typedef BOOST_DEDUCED_TYPENAME mpl::if_<
                    mpl::empty<tail>,
                    end_type_list_action,
                    dispatcher<tail, FUNCTOR>
            >::type type_x;
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
#ifdef _WIN32
                f.operator()<head>();
#else
                f.template operator()<head>();
#endif
            }
            else
            {
                // recursively call other element in the list
                typedef BOOST_DEDUCED_TYPENAME mpl::if_<
                        mpl::empty<tail>,
                        end_type_list_action,
                        dispatcher<tail, FUNCTOR>
                >::type type_x;
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
#ifdef _WIN32
                f.operator()<head>(_param);
#else
                f.template operator()<head>(_param);
#endif
            }
            else
            {
                // recursively call other element in the list
                typedef BOOST_DEDUCED_TYPENAME mpl::if_<
                        mpl::empty<tail>,
                        end_type_list_action,
                        dispatcher<tail, FUNCTOR>
                >::type type_x;
                type_x::invoke(_key_t, _param);
            }
        }
};

using integer_types = boost::mpl::vector<std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t,
                                         std::uint32_t>::type;

#ifdef DEBUG
using intrinsic_types = boost::mpl::push_back<integer_types, float>::type;
#else
typedef boost::mpl::push_back<boost::mpl::push_back<integer_types, float>::type, double>::type intrinsic_types;
#endif

} //end namespace sight::core::tools

#include <core/tools/type_key_type_mapping.hpp>
