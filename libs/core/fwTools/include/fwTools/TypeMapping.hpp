/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __FWTOOLS_TYPEMAPPING_HPP__
#define __FWTOOLS_TYPEMAPPING_HPP__

#include <boost/mpl/bool.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/static_assert.hpp>

#include <iterator>
#include <stdexcept>

namespace fwTools
{

// forward declaration
template< class TSEQ, class KeyTypeContainer >
struct isMappingMultiMPLHelper;

template< class T, class KeyType >
struct isMappingSingleMPLHelper;

//
/**
 * @brief   Create a type (T) binding/mapping with a KeyType ( std::string, ipop::PixelType etc...
 * @tparam TSingle_or_TSEQ a sequence or 1 element type to test
 * @tparam KeyType_or_KeyTypeContainer to keys (sequence or single one)
 * @return  true iff the value of the KeyType can deal with the specified type T
 *
 *
 * This function should be specializated to create a Mapping with a KeyType value and a type.
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
template< class TSingle_or_TSEQ, class KeyType_or_KeyTypeContainer >
bool isMapping(const KeyType_or_KeyTypeContainer& key)
{
    namespace mpl = ::boost::mpl;
    typedef BOOST_DEDUCED_TYPENAME mpl::if_<
            mpl::is_sequence< TSingle_or_TSEQ >,
            isMappingMultiMPLHelper< TSingle_or_TSEQ, KeyType_or_KeyTypeContainer >,
            isMappingSingleMPLHelper< TSingle_or_TSEQ, KeyType_or_KeyTypeContainer >
            >::type typex;
    return typex::evaluate(key);

}

/**
 * @brief   an isMapping() helper : This function is called iff TSingle_or_TSEQ is not a sequence and
 * isMapping<SingleType> is not specialized
 * This class is intended to avoid developer to forgive the specialization of isMapping<TYPE>
 * @tparam  T the type to test
 * @tparam  KeyType the type to match
 */
template< class T, class KeyType >
struct isMappingSingleMPLHelper
{

    /// this function is called iff TSingle_or_TSEQ is not a sequence and isMapping<SingleType>
    static bool evaluate(const KeyType& key)
    {
        FwCoreNotUsedMacro(key);
        BOOST_STATIC_ASSERT(sizeof(T) == 0);  // note its a compilator workaround of BOOST_STATIC_ASSERT(false);
        // ** if the compilation trap here its because you have not specialized
        // ** isMapping<MySingleType,MyCorrespondingKeyType>(keytypevalue)
        std::string msg("isMapping<TYPE>::(const KEYTYPE &key) not specializated for TYPE and/or KEYTYPE!!!");
        throw std::invalid_argument(msg);
        return false;
    }
};

/**
 * @brief   Test whatever a typelist is mapping a container of KeyType
 *
 * @return  true if same size & each element of type list mappes a single element of KeyType
 */
template< class TSEQ, class KeyTypeContainer >
bool isMappingMulti(const KeyTypeContainer& keys)
{
    return isMappingMultiMPLHelper<TSEQ, KeyTypeContainer>::evaluate(keys);
}

/**
 * @brief an helper to isMapping() using iterator
 */
template<  class KeyTypeContainer >
struct EmptyListMapping
{
    //------------------------------------------------------------------------------

    static bool evaluate(typename KeyTypeContainer::const_iterator& begin,
                         typename KeyTypeContainer::const_iterator& end)
    {
        assert( begin == end ); // assertion fails iff TypeList & KeyType container does not have the same size
        return true; // an empty typelist with an emty keyType matches
    }
};

/**
 * @brief an helper to isMapping() using iterator : this class is called when TSEQ is a sequence. it is recursivelly
 * called with head element
 * removed from TSEQ
 */
template< class TSEQ, class KeyTypeContainer >
struct
isMappingMultiMPLHelper
{
    static bool evaluate(typename KeyTypeContainer::const_iterator& begin,
                         typename KeyTypeContainer::const_iterator& end);

    //------------------------------------------------------------------------------

    static bool evaluate(const KeyTypeContainer& keys)
    {

        namespace mpl = ::boost::mpl;

        if ( keys.size() != static_cast<unsigned long>(mpl::size<TSEQ>::value) )
        {
            std::string msg("isMappingMulti TypeList & KeyType container does not have the same size !!!");
            throw std::invalid_argument(msg);
            return false;
        }

        typename KeyTypeContainer::const_iterator begin = keys.begin(); // needed to have cste ptr
        typename KeyTypeContainer::const_iterator end   = keys.end();
        return isMappingMultiMPLHelper<TSEQ, KeyTypeContainer>::evaluate( begin, end );

    }

};

//------------------------------------------------------------------------------

template< class TSEQ, class KeyTypeContainer >
bool
isMappingMultiMPLHelper<TSEQ, KeyTypeContainer>::evaluate(typename KeyTypeContainer::const_iterator& begin,
                                                          typename KeyTypeContainer::const_iterator& end)
{
    namespace mpl = ::boost::mpl;

    typedef BOOST_DEDUCED_TYPENAME mpl::front<TSEQ>::type Head;
    typedef BOOST_DEDUCED_TYPENAME mpl::pop_front<TSEQ>::type Tail;

    typedef BOOST_DEDUCED_TYPENAME mpl::if_<
            mpl::empty<Tail>,
            EmptyListMapping < KeyTypeContainer >,
            isMappingMultiMPLHelper <Tail, KeyTypeContainer >
            >::type typex;

    bool firstKeyIsOK = isMapping< Head >( *begin );     // call a isMapping with a single key

    if ( firstKeyIsOK == false )     // OPTIMISATION
    {
        return false;     // the first key doesn't match : do not try to test other
    }

    bool otherKeys = typex::evaluate( ++begin, end );
    return firstKeyIsOK && otherKeys;

}

} // namespace fwTools

#endif /*__FWTOOLS_TYPEMAPPING_HPP__*/
