/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// fwTools - copyright (C) 2005 IRCAD.
#ifndef COMBINATORY_HPP_
#define COMBINATORY_HPP_

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/type_traits.hpp>

namespace fwTools {

/**
 * @brief	Helper for BinaryCartesianProduct two Set
 * @struct	AppendValueFirst
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @param 	TYPE should be a simple type
 * @param 	SETOFSET should be a container of type list i.e vector< vector< singleTypes>, vector< singleTypes>, ... >.
 * If SET is empty then vector<vector<TYPE> > is created
 *
 * From a type and a set generate a new set where elements are concatenation of type and element of second set i.e
 * AppendValueFirst ( a , [ [B] , [C] ] ) --> [ [a,B], [b,C] ] and  AppendValueFirst ( a , [ ] ) --> [ [a] ]
 */
struct AppendValueFirst
{
	template<class TYPE,class SETOFSET>
	struct apply
	{

		typedef BOOST_DEDUCED_TYPENAME boost::mpl::eval_if<	boost::mpl::empty<SETOFSET>,
														boost::mpl::vector< boost::mpl::vector<TYPE> >,
														boost::mpl::transform< SETOFSET, boost::mpl::push_front< boost::mpl::_1 , TYPE > >
													>::type type;
	};
};


/**
 * @brief 	MetaFunction which create an boost::boost::mpl::vector
 * @struct	make_vector
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */
struct make_vector
{
	template<class T>
	struct apply
	{
		typedef BOOST_DEDUCED_TYPENAME boost::mpl::vector< T >:: type type;
	};
};


/**
 * @brief 	MetaFunction ( used for pseudo Curryfication ) which transform a set where new elements are
 * singleton of 1st set elements i.e { a , b , c } --> { {a}, {b}, {c} }
 * @struct	makeSetOfSingletons
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 **/
struct makeSetOfSingletons
{
	template<class Set>
	struct apply
	{
		typedef BOOST_DEDUCED_TYPENAME boost::mpl::transform<Set, make_vector >::type type;
	};
};




/**
 * @brief	Helper which compute from a set and a multi set
 * @struct	BinaryCartesianProductRecurser
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 *
 * Set1: A = { a_1, a_2, ..., a_N }  \n  MultiSet = {   { ... b_i ...} , { ... c_i ...}, ... { z_i ...}  }\n
 * the set
 * {   {a_1, ... b_i ...} , { a_1,  ... c_i ...}, ... {a_1,  ... z_i ...}  ,        {a_2, ... b_i ...} , { a_2,  ... c_i ...}, {a_2,  ... z_i ...},
 *   , .... ,  {a_N, ... b_i ...} , { a_N,  ... c_i ...}, ... {a_N,  ... z_i ...}
 */
struct BinaryCartesianProductRecurser
{
	template< class Set1, class MultiSet >
	struct apply
	{
		typedef BOOST_DEDUCED_TYPENAME boost::mpl::accumulate< Set1,
												boost::mpl::vector<>,
												boost::mpl::copy<   boost::mpl::apply2< AppendValueFirst, boost::mpl::_2, MultiSet > ,  boost::mpl::back_inserter< boost::mpl::_1>  >

													  >::type type;
	};
};



/**
 * @brief 	Compute Cartesian Product of two set (type list) to generate all possible combinaison.
 * @struct	BinaryCartesianProduct
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 *
 * From two type list generate a new type list where all elemenent a combinaison of each set. For example :
 * @code
 * 	using namespace boost::mpl;
 *
 * typedef vector< char, short, long > Set1;
 * typedef vector< double, float > Set2;
 *
 *	typedef vector< vector< char,float>, vector<char, double>, vector< short,float>, vector<short, double>, vector< long,float>, vector<long, double> > Wanted;
 *
 *	typedef apply< BinaryCartesianProduct, Set1, Set2 >::type  Result;
 *
 *	BOOST_MPL_ASSERT_RELATION( size<Wanted>::value , == , size<Result>::value  );
 *	BOOST_MPL_ASSERT(( equal< front<Wanted> , front<Result> > ));
 *	BOOST_MPL_ASSERT(( equal< at_c<Wanted,0> , at_c<Result,0> > ));
 *	BOOST_MPL_ASSERT(( equal< at_c<Wanted,1> , at_c<Result,1> > ));
 *	BOOST_MPL_ASSERT(( equal< at_c<Wanted,2> , at_c<Result,2> > ));
 * @endcode
 * This operator can deal with empty set :
 * @code
 * 	using namespace boost::mpl;
	typedef vector<> emptySet;
	typedef vector< char, short>  Set1;

	typedef vector< vector<char>, vector<short> >::type Wanted;

	typedef apply< BinaryCartesianProduct, Set1, emptySet >::type  Result;
	BOOST_MPL_ASSERT_RELATION( size<Wanted>::value , == , size<Result>::value  );
	BOOST_MPL_ASSERT(( equal< front<Wanted> , front<Result> > ));
	BOOST_MPL_ASSERT(( equal< at_c<Wanted,0> , at_c<Result,0> > ));
	BOOST_MPL_ASSERT(( equal< at_c<Wanted,1> , at_c<Result,1> > ));
 * @endcode
 */
struct BinaryCartesianProduct
{
	template< class Set1, class Set2 >
	struct apply
	{
		typedef BOOST_DEDUCED_TYPENAME  boost::mpl::apply1< makeSetOfSingletons,  Set2>::type  Set2WithSingletons;

		typedef BOOST_DEDUCED_TYPENAME  boost::mpl::apply2<BinaryCartesianProductRecurser, Set1, Set2WithSingletons >::type type;
	};
};




/**
 * @brief compute the cartesian product of many set
 * @struct	CartesianProduct
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @param 	MultiSet must be of the following form vector<  vector< ElementaryType1, ElementaryType2,... > , vector< ...ElementaryTypes...> ... > where Elementary
 *       	types are not boost::mpl::Container : i.e int, classes, std::vector<int> etc...
 *
 * Example
 * @code
 * 	using namespace boost::mpl;

	typedef vector< vector<signed char, signed short, signed int>,  vector< float, double >  , vector< std::string, unsigned char >   > ::type MultiSet;

	typedef apply< CartesianProduct, MultiSet>::type Result;

	BOOST_MPL_ASSERT_RELATION( size< Result >::value , == , 12 );

	BOOST_MPL_ASSERT(( equal<  at_c<Result,0>::type, vector<  signed char,  float  , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,1>::type, vector<  signed char,  float  , unsigned char >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,2>::type, vector<  signed char,  double , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,3>::type, vector<  signed char,  double , unsigned char >    > ));

	BOOST_MPL_ASSERT(( equal<  at_c<Result,4>::type, vector< signed short,  float  , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,5>::type, vector< signed short,  float  , unsigned char >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,6>::type, vector< signed short,  double , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,7>::type, vector< signed short,  double , unsigned char >    > ));

	BOOST_MPL_ASSERT(( equal<  at_c<Result,8>::type, vector<  signed int,  float  , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,9>::type, vector<  signed int,  float  , unsigned char >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,10>::type, vector< signed int,  double , std::string >    > ));
	BOOST_MPL_ASSERT(( equal<  at_c<Result,11>::type, vector< signed int,  double , unsigned char >    > ));
 * @endcode
 **/
struct CartesianProduct
{
	template< class MultiSet >
	struct apply
	{
		typedef BOOST_DEDUCED_TYPENAME  boost::mpl::reverse_fold< 	MultiSet,
															boost::mpl::vector<>,
															boost::mpl::apply2< BinaryCartesianProductRecurser, boost::mpl::_2, boost::mpl::_1 >
							>::type type;
	};
};



} //end namespace fwTools {


#endif /*COMBINATORY_HPP_*/
