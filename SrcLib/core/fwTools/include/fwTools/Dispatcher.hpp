/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef DISPATCHER_HPP_
#define DISPATCHER_HPP_

#include <stdexcept>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/identity.hpp>

#include "fwTools/TypeMapping.hpp"


namespace fwTools {


/**
 * @brief       Limit cases for empty typelist
 * @class       EnTypeListAction
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 * @see ::fwTools::Dispatcher
 */
 struct EnTypeListAction
 {

        /// Perform nothing see Dispatcher<...>::invoke
        static void invoke() {};

        /// Throw an exception to inform end-user that KeyType value have no correspondance in type list
        template< class KeyType>
        static void  invoke(const KeyType &keytype)
        {
                throw std::invalid_argument("KeyType value incorrect : no corresponding Type in typelist");
        }

        /// Throw an exception to inform end-user that KeyType value have no correspondance in type list
        template< class KeyType,class Parameter>
        static void  invoke( const KeyType &keytype,const Parameter &param )
        {
                throw std::invalid_argument("KeyType value incorrect : no corresponding Type in typelist");
        }

        /// Throw an exception to inform end-user that KeyType value have no correspondance in type list
        template< class BaseClass, class KeyType>
        static BaseClass  *instanciate(const KeyType &keytype)
        {
                throw std::invalid_argument("KeyType value incorrect : no corresponding Type in typelist");
                return NULL;
        }

 };




/**
 * @brief       Create an automatic template instancier exple Dispatcher< TYPESEQUENCE , FUNCTOR>::invoke("int");
 * @class       Dispatcher
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 *
 * Will instanciante class FUNCTOR then for a type T in TYPESEQUENCE (here int) call the corresponding operator() method
 * according to parameter of invoke static method. ie FUNCTOR().operator<int>();
 */
template< class TSEQ, class FUNCTOR >
struct Dispatcher
{

         private:
        typedef BOOST_DEDUCED_TYPENAME boost::mpl::pop_front<TSEQ>::type Tail;
        typedef BOOST_DEDUCED_TYPENAME boost::mpl::front<TSEQ>::type Head;

     public :



     /**
      * @brief Instanciate and invoke all functors
      */
     static void invoke()
     {
        namespace mpl = boost::mpl;

        // create the functor then excute it
         FUNCTOR f;
#ifdef _WIN32
         f.operator()<Head>();
#else
         f.template operator()<Head>();
#endif


        // recursively call other element in the list
        typedef BOOST_DEDUCED_TYPENAME mpl::eval_if<
                mpl::empty<Tail>,
                mpl::identity< EnTypeListAction  >,
                mpl::identity< Dispatcher<Tail,FUNCTOR > >
            >::type typex;
            typex::invoke();
     }



     /**
      * @brief Invoke only the specified Type only
      */
     template< class KeyType >
     static void invoke( const KeyType &keytype )
     {
        namespace mpl = boost::mpl;

        if   ( isMapping< Head>(keytype) )
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
                        typedef BOOST_DEDUCED_TYPENAME mpl::eval_if<
                mpl::empty<Tail>,
                mpl::identity< EnTypeListAction  >,
                mpl::identity< Dispatcher<Tail,FUNCTOR > >
            >::type typex;
            typex::invoke(keytype);

        }
     }
     // NOTE gcc seems unable to explicit call of static template fonction member :/
     // all arguments needs to be present specicied template seems ignored


     /**
      * @brief Invoke only the specified Type only with a fixed parameter
      * @note That parameter is *NOT* const so functor can update value
      */
     template< class KeyType,class Parameter >
     static void invoke( const KeyType &keytype,  Parameter &param )
     {
        namespace mpl = boost::mpl;

        if   ( isMapping< Head>(keytype) )
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
                        typedef BOOST_DEDUCED_TYPENAME mpl::eval_if<
                mpl::empty<Tail>,
                mpl::identity< EnTypeListAction  >,
                mpl::identity< Dispatcher<Tail,FUNCTOR > >
            >::type typex;
            typex::invoke(keytype,param);

        }
     }


};


} //end namespace fwTools {

#endif /*DISPATCHER_HPP_*/
