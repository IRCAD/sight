/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef TYPEMAPPING_HPP_
#define TYPEMAPPING_HPP_

#include <iterator>
#include <boost/static_assert.hpp>
#include <assert.h>
#include <stdexcept>

#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/eval_if.hpp>

#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>

namespace fwTools {

// forward declaration
template< class TSEQ, class KeyTypeContainer >
struct isMappingMultiMPLHelper;

template< class T, class KeyType >
struct isMappingSingleMPLHelper;


//
/**
 * @brief       Create a type (T) binding/mapping with a KeyType ( std::string, ipop::PixelType etc...
 * @tparam TSingle_or_TSEQ a sequence or 1 element type to test
 * @tparam KeyType_or_KeyTypeContainer to keys (sequence or single one)
 * @return      true iff the value of the KeyType can deal with the specified type T
 * @author      IRCAD (Research and Development Team).
 *
 * This function should be specializated to create a Mapping with a KeyType value and a type.
 * This function is used by Dispatcher:<..>::invoke(key) to know what instance to execute.
 * If isMapping function is missing for a given type then a compilation error
 * "invalid application of 'sizeof' to incomplete type 'boost::STATIC_ASSERTION_FAILURE<false>" is raised
 * to inform develloper.
 *
 * *Example* : if keytype type is a std::string and we need to have a binding within unsigned char
 * @code
 * template<>
 * bool isMapping<unsigned char>(const std::string &key)
 * {
 *      return key=="unsigned char";
 * }
 * @endcode
 */
template< class TSingle_or_TSEQ, class KeyType_or_KeyTypeContainer >
bool isMapping(const KeyType_or_KeyTypeContainer &key)
{
        namespace mpl = boost::mpl;
        typedef BOOST_DEDUCED_TYPENAME mpl::eval_if<
                                                                                mpl::is_sequence< TSingle_or_TSEQ >,
                                                                                mpl::identity< isMappingMultiMPLHelper<TSingle_or_TSEQ,KeyType_or_KeyTypeContainer> >,
                                                                                mpl::identity< isMappingSingleMPLHelper<TSingle_or_TSEQ,KeyType_or_KeyTypeContainer> >
                                                                        >::type typex;
        return typex::evaluate(key);

}





/**
 * @brief       an isMapping() helper : This function is called iff TSingle_or_TSEQ is not a sequence and isMapping<SingleType> is not specialized
 * This class is intended to avoid developer to forgive the specialization of isMapping<TYPE>
 * @class       isMappingSingleMPLHelper.
 * @tparam  T the type to test
 * @tparam  KeyType the type to match
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
template< class T, class KeyType >
struct isMappingSingleMPLHelper
{
        /**
         * @brief
         */
        //this function is called iff TSingle_or_TSEQ is not a sequence and isMapping<SingleType>
        static bool evaluate(const KeyType &key)
        {
                BOOST_STATIC_ASSERT(sizeof(T) == 0);  // note its a compilator workaround of BOOST_STATIC_ASSERT(false);
                // ** if the compilation trap here its because you have not specialized
                // ** isMapping<MySingleType,MyCorrespondingKeyType>(keytypevalue)
                std::string msg("isMapping<TYPE>::(const KEYTYPE &key) not specializated for TYPE and/or KEYTYPE!!!");
                throw std::invalid_argument(msg);
                return false;
        }
};




// // forward declaration
//template< class TSEQ, class KeyTypeContainer >
//struct isMappingMultiMPLHelper;



/**
 * @brief       Test whatever a typelist is mapping a container of KeyType
 * @author      IRCAD (Research and Development Team).
 * @return      true if same size & each element of type list mappes a single element of KeyType
 */
template< class TSEQ, class KeyTypeContainer >
bool isMappingMulti(const KeyTypeContainer& keys)
{
        return  isMappingMultiMPLHelper<TSEQ,KeyTypeContainer>::evaluate(keys);
}




/**
 * @class       EmptyListMapping.
 * @brief an helper to isMapping() using iterator
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
template<  class KeyTypeContainer >
struct EmptyListMapping
{
        static bool evaluate(typename KeyTypeContainer::const_iterator & begin, typename KeyTypeContainer::const_iterator & end)
        {
                assert( begin == end ); // assertion fails iff TypeList & KeyType container does not have the same size
                return true; // an empty typelist with an emty keyType matches
        }
};




/**
 * @class       isMappingMultiMPLHelper.
 * @brief an helper to isMapping() using iterator : this class is called when TSEQ is a sequence. it is recursivelly called with head element
 * removed from TSEQ
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
template< class TSEQ, class KeyTypeContainer >
struct
isMappingMultiMPLHelper
{
        static bool evaluate(typename KeyTypeContainer::const_iterator & begin, typename KeyTypeContainer::const_iterator & end);

        static bool evaluate(const KeyTypeContainer& keys)
        {

                namespace mpl = boost::mpl;

                if ( keys.size() !=  static_cast<unsigned long>(mpl::size<TSEQ>::value) )
                {
                        std::string msg("isMappingMulti TypeList & KeyType container does not have the same size !!!");
                        throw std::invalid_argument(msg);
                        return false;
                }

                typename KeyTypeContainer::const_iterator begin = keys.begin(); // needed to have cste ptr
                typename KeyTypeContainer::const_iterator end = keys.end();
        return isMappingMultiMPLHelper<TSEQ,KeyTypeContainer>::evaluate( begin, end );

        }

};


template< class TSEQ, class KeyTypeContainer >
bool
isMappingMultiMPLHelper<TSEQ,KeyTypeContainer>::evaluate(typename KeyTypeContainer::const_iterator & begin, typename KeyTypeContainer::const_iterator & end)
{
                namespace mpl = boost::mpl;

                typedef BOOST_DEDUCED_TYPENAME mpl::front<TSEQ>::type Head;
                typedef BOOST_DEDUCED_TYPENAME mpl::pop_front<TSEQ>::type Tail;

                typedef BOOST_DEDUCED_TYPENAME mpl::eval_if<
                mpl::empty<Tail>,
                mpl::identity< EmptyListMapping < KeyTypeContainer > >,
                mpl::identity< isMappingMultiMPLHelper <Tail,KeyTypeContainer > >
                 >::type typex;

                bool firstKeyIsOK = isMapping< Head >( *begin ); // call a isMapping with a single key

                if ( firstKeyIsOK == false ) // OPTIMISATION
                {
                        return false; // the first key doesn't match : do not try to test other
                }

                bool otherKeys = typex::evaluate( ++begin , end );
        return firstKeyIsOK && otherKeys;

}



} // namespace fwTools {


#endif /*TYPEMAPPING_HPP_*/
