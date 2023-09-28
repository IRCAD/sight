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

    /// Throw an exception to inform end-user that KeyType value have no correspondence in type list
    template<class key_type>
    static void invoke(const key_type& keytype)
    {
        std::string msg = keytype.name()
                          + " : KeyType value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
    }

    /// Throw an exception to inform end-user that KeyType value have no correspondence in type list
    template<class key_type, class parameter>
    static void invoke(const key_type& keytype, const parameter& param)
    {
        SIGHT_NOT_USED(param);
        std::string msg = keytype.name()
                          + " : KeyType value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
    }

    /// Throw an exception to inform end-user that KeyType value have no correspondence in type list
    template<class base_class, class key_type>
    static base_class* instantiate(const key_type& keytype)
    {
        std::string msg = keytype.name()
                          + " : KeyType value incorrect : no corresponding Type in typelist";
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

        typedef BOOST_DEDUCED_TYPENAME boost::mpl::pop_front<TSEQ>::type tail;
        typedef BOOST_DEDUCED_TYPENAME boost::mpl::front<TSEQ>::type head;

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
        static void invoke(const key_type& keytype)
        {
            namespace mpl = boost::mpl;

            if(is_mapping<head>(keytype))
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
                type_x::invoke(keytype);
            }
        }

        // NOTE gcc seems unable to explicit call of static template function member :/
        // all arguments needs to be present specified template seems ignored

        /**
         * @brief Invoke only the specified Type only with a fixed parameter
         * @note That parameter is *NOT* const so functor can update value
         */
        template<class key_type, class parameter>
        static void invoke(const key_type& keytype, parameter& param)
        {
            namespace mpl = boost::mpl;

            if(is_mapping<head>(keytype))
            {
                // create the functor then excute it
                FUNCTOR f;
#ifdef _WIN32
                f.operator()<head>(param);
#else
                f.template operator()<head>(param);
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
                type_x::invoke(keytype, param);
            }
        }
};

typedef boost::mpl::vector<
        std::int8_t,
        std::uint8_t,
        std::int16_t,
        std::uint16_t,
        std::int32_t,
        std::uint32_t
#ifndef DEBUG
        , std::int64_t,
        std::uint64_t
#endif
>::type integer_types;

#ifdef DEBUG
typedef boost::mpl::push_back<integer_types, float>::type intrinsic_types;
#else
typedef boost::mpl::push_back<boost::mpl::push_back<integer_types, float>::type, double>::type intrinsic_types;
#endif

} //end namespace sight::core::tools

#include <core/tools/type_key_type_mapping.hpp>
