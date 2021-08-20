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

#pragma once

#include "core/tools/Stringizer.hpp"
#include "core/tools/TypeMapping.hpp"
#include <core/macros.hpp>

#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/pop_front.hpp>

#include <stdexcept>

namespace sight::core::tools
{

/**
 * @brief   Limit cases for empty typelist
 * @see core::tools::Dispatcher
 */
struct EndTypeListAction
{
    /// Perform nothing see Dispatcher<>::invoke()
    static void invoke()
    {
    }

    /// Throw an exception to inform end-user that KeyType value have no correspondence in type list
    template<class KeyType>
    static void invoke(const KeyType& keytype)
    {
        std::string msg = core::tools::getString(keytype)
                          + " : KeyType value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
    }

    /// Throw an exception to inform end-user that KeyType value have no correspondence in type list
    template<class KeyType, class Parameter>
    static void invoke(const KeyType& keytype, const Parameter& param)
    {
        SIGHT_NOT_USED(param);
        std::string msg = core::tools::getString(keytype)
                          + " : KeyType value incorrect : no corresponding Type in typelist";
        throw std::invalid_argument(msg);
    }

    /// Throw an exception to inform end-user that KeyType value have no correspondence in type list
    template<class BaseClass, class KeyType>
    static BaseClass* instanciate(const KeyType& keytype)
    {
        std::string msg = core::tools::getString(keytype)
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
struct Dispatcher
{
    private:

        typedef BOOST_DEDUCED_TYPENAME boost::mpl::pop_front<TSEQ>::type Tail;
        typedef BOOST_DEDUCED_TYPENAME boost::mpl::front<TSEQ>::type Head;

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
            f.operator()<Head>();
#else
            f.template operator()<Head>();
#endif

            // recursively call other element in the list
            typedef BOOST_DEDUCED_TYPENAME mpl::if_<
                    mpl::empty<Tail>,
                    EndTypeListAction,
                    Dispatcher<Tail, FUNCTOR>
            >::type typex;
            typex::invoke();
        }

        /**
         * @brief Invoke only the specified Type only
         */
        template<class KeyType>
        static void invoke(const KeyType& keytype)
        {
            namespace mpl = boost::mpl;

            if(isMapping<Head>(keytype))
            {
                // create the functor then excute it
                FUNCTOR f;
#ifdef _WIN32
                f.operator()<Head>();
#else
                f.template operator()<Head>();
#endif
            }
            else
            {
                // recursively call other element in the list
                typedef BOOST_DEDUCED_TYPENAME mpl::if_<
                        mpl::empty<Tail>,
                        EndTypeListAction,
                        Dispatcher<Tail, FUNCTOR>
                >::type typex;
                typex::invoke(keytype);
            }
        }

        // NOTE gcc seems unable to explicit call of static template function member :/
        // all arguments needs to be present specified template seems ignored

        /**
         * @brief Invoke only the specified Type only with a fixed parameter
         * @note That parameter is *NOT* const so functor can update value
         */
        template<class KeyType, class Parameter>
        static void invoke(const KeyType& keytype, Parameter& param)
        {
            namespace mpl = boost::mpl;

            if(isMapping<Head>(keytype))
            {
                // create the functor then excute it
                FUNCTOR f;
#ifdef _WIN32
                f.operator()<Head>(param);
#else
                f.template operator()<Head>(param);
#endif
            }
            else
            {
                // recursively call other element in the list
                typedef BOOST_DEDUCED_TYPENAME mpl::if_<
                        mpl::empty<Tail>,
                        EndTypeListAction,
                        Dispatcher<Tail, FUNCTOR>
                >::type typex;
                typex::invoke(keytype, param);
            }
        }
};

} //end namespace sight::core::tools
