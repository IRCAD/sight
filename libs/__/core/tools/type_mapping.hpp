/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <boost/mpl/bool.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector.hpp>

#include <iterator>
#include <stdexcept>

namespace sight::core::tools
{

// forward declaration
template<class TSEQ, class key_type_container>
struct is_mapping_multi_mplhelper;

template<class T, class key_type>
struct is_mapping_single_mplhelper;

//
/**
 * @brief   Create a type (T) binding/mapping with a KeyType ( std::string, PixelType etc...
 * @tparam TSingle_or_TSEQ a sequence or 1 element type to test
 * @tparam KeyType_or_KeyTypeContainer to keys (sequence or single one)
 * @return  true iff the value of the KeyType can deal with the specified type T
 *
 *
 * This function should be specialized to create a Mapping with a KeyType value and a type.
 * This function is used by Dispatcher<>::invoke(key) to know what instance to execute.
 * If isMapping function is missing for a given type then a compilation error
 * "invalid application of 'sizeof' to incomplete type 'boost::STATIC_ASSERTION_FAILURE<false>" is raised
 * to inform developer.
 *
 * *Example* : if keytype type is a std::string and we need to have a binding within unsigned char
 * @code
 * template<>
 * bool isMapping<unsigned char>(const std::string &key)
 * {
 *  return key=="unsigned char";
 * }
 * @endcode
 */
template<class single_or_seq, class key_type_or_key_type_container>
bool is_mapping(const key_type_or_key_type_container& type)
{
    namespace mpl = boost::mpl;
    typedef BOOST_DEDUCED_TYPENAME mpl::if_<
            mpl::is_sequence<single_or_seq>,
            is_mapping_multi_mplhelper<single_or_seq, key_type_or_key_type_container>,
            is_mapping_single_mplhelper<single_or_seq, key_type_or_key_type_container>
    >::type type_x;
    return type_x::evaluate(type);
}

/**
 * @brief   an isMapping() helper : This function is called iff TSingle_or_TSEQ is not a sequence and
 * isMapping<SingleType> is not specialized
 * This class is intended to avoid developer to forgive the specialization of isMapping<TYPE>
 * @tparam  T the type to test
 * @tparam  KeyType the type to match
 */
template<class T, class key_type>
struct is_mapping_single_mplhelper
{
    /// this function is called iff TSingle_or_TSEQ is not a sequence and isMapping<SingleType>
    static bool evaluate(const key_type& key)
    {
        SIGHT_NOT_USED(key);
        static_assert(sizeof(T) == 0); // note its a compilator workaround of BOOST_STATIC_ASSERT(false);
        // ** if the compilation trap here its because you have not specialized
        // ** isMapping<MySingleType,MyCorrespondingKeyType>(keytypevalue)
        std::string msg("isMapping<type>(const KEYTYPE &key) not specialized for TYPE and/or KEYTYPE!!!");
        throw std::invalid_argument(msg);
        return false;
    }
};

/**
 * @brief   Test whatever a typelist is mapping a container of KeyType
 *
 * @return  true if same size & each element of type list mappes a single element of KeyType
 */
template<class TSEQ, class key_type_container>
bool is_mapping_multi(const key_type_container& keys)
{
    return is_mapping_multi_mplhelper<TSEQ, key_type_container>::evaluate(keys);
}

/**
 * @brief an helper to isMapping() using iterator
 */
template<class key_type_container>
struct empty_list_mapping
{
    //------------------------------------------------------------------------------

    static bool evaluate(
        [[maybe_unused]] typename key_type_container::const_iterator& begin,
        [[maybe_unused]] typename key_type_container::const_iterator& end
)
    {
        assert(begin == end); // assertion fails iff TypeList & KeyType container does not have the same size
        return true;          // an empty typelist with an empty keyType matches
    }
};

/**
 * @brief an helper to isMapping() using iterator : this class is called when TSEQ is a sequence. it is recursively
 * called with head element
 * removed from TSEQ
 */
template<class TSEQ, class key_type_container>
struct
is_mapping_multi_mplhelper
{
    static bool evaluate(
        typename key_type_container::const_iterator& begin,
        typename key_type_container::const_iterator& end
    );

    //------------------------------------------------------------------------------

    static bool evaluate(const key_type_container& keys)
    {
        namespace mpl = boost::mpl;

        if(keys.size() != static_cast<std::uint64_t>(mpl::size<TSEQ>::value))
        {
            std::string msg("isMappingMulti TypeList & KeyType container does not have the same size !!!");
            throw std::invalid_argument(msg);
            return false;
        }

        typename key_type_container::const_iterator begin = keys.begin(); // needed to have const ptr
        typename key_type_container::const_iterator end   = keys.end();
        return is_mapping_multi_mplhelper<TSEQ, key_type_container>::evaluate(begin, end);
    }
};

//------------------------------------------------------------------------------

template<class TSEQ, class key_type_container>
bool is_mapping_multi_mplhelper<TSEQ, key_type_container>::evaluate(
    typename key_type_container::const_iterator& begin,
    typename key_type_container::const_iterator& end
)
{
    namespace mpl = boost::mpl;

    typedef BOOST_DEDUCED_TYPENAME mpl::front<TSEQ>::type head;
    typedef BOOST_DEDUCED_TYPENAME mpl::pop_front<TSEQ>::type tail;

    typedef BOOST_DEDUCED_TYPENAME mpl::if_<
            mpl::empty<tail>,
            empty_list_mapping<key_type_container>,
            is_mapping_multi_mplhelper<tail, key_type_container>
    >::type type_x;

    bool first_key_is_ok = is_mapping<head>(*begin); // call a isMapping with a single key

    if(!first_key_is_ok) // OPTIMISATION
    {
        return false; // the first key doesn't match : do not try to test other
    }

    bool other_keys = type_x::evaluate(++begin, end);
    return first_key_is_ok && other_keys;
}

} // namespace sight::core::tools
