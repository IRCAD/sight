/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <memory>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/parameter_types.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/typeof/std/utility.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/algorithm/string/join.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>



#include <boost/signals2/signal.hpp>


template <typename F> struct convert_function_types;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template <typename R, typename C, typename Args1, typename Args2, typename Args3, typename Args4, typename Args5,
          typename Args6, typename Args7, typename Args8, typename Args9 >
struct convert_function_types< R (C::*)( Args1, Args2, Args3, Args4, Args5, Args6, Args7, Args8, Args9 )  >
{
    typedef R type ( Args1, Args2, Args3, Args4, Args5, Args6, Args7, Args8, Args9 );
};
template <typename R, typename C, typename Args1, typename Args2, typename Args3, typename Args4, typename Args5,
          typename Args6, typename Args7, typename Args8 >
struct convert_function_types< R (C::*)( Args1, Args2, Args3, Args4, Args5, Args6, Args7, Args8 )  >
{
    typedef R type ( Args1, Args2, Args3, Args4, Args5, Args6, Args7, Args8 );
};
template <typename R, typename C, typename Args1, typename Args2, typename Args3, typename Args4, typename Args5,
          typename Args6, typename Args7 >
struct convert_function_types< R (C::*)( Args1, Args2, Args3, Args4, Args5, Args6, Args7 )  >
{
    typedef R type ( Args1, Args2, Args3, Args4, Args5, Args6, Args7 );
};
template <typename R, typename C, typename Args1, typename Args2, typename Args3, typename Args4, typename Args5,
          typename Args6 >
struct convert_function_types< R (C::*)( Args1, Args2, Args3, Args4, Args5, Args6 )  >
{
    typedef R type ( Args1, Args2, Args3, Args4, Args5, Args6 );
};
template <typename R, typename C, typename Args1, typename Args2, typename Args3, typename Args4, typename Args5 >
struct convert_function_types< R (C::*)( Args1, Args2, Args3, Args4, Args5 )  >
{
    typedef R type ( Args1, Args2, Args3, Args4, Args5 );
};
template <typename R, typename C, typename Args1, typename Args2, typename Args3, typename Args4 >
struct convert_function_types< R (C::*)( Args1, Args2, Args3, Args4 )  >
{
    typedef R type ( Args1, Args2, Args3, Args4 );
};
template <typename R, typename C, typename Args1, typename Args2, typename Args3 >
struct convert_function_types< R (C::*)( Args1, Args2, Args3 )  >
{
    typedef R type ( Args1, Args2, Args3 );
};
template <typename R, typename C, typename Args1, typename Args2 >
struct convert_function_types< R (C::*)( Args1, Args2 )  >
{
    typedef R type ( Args1, Args2 );
};
template <typename R, typename C, typename Args1 >
struct convert_function_types< R (C::*)( Args1 )  >
{
    typedef R type ( Args1 );
};
template <typename R, typename C>
struct convert_function_types< R (C::*)()  >
{
    typedef R type ();
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template <typename R, typename C, typename ... Args >
struct convert_function_types< R (C::*)( Args ... )  >
{
    typedef R type ( Args ... );
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES


template <typename F>
struct convert_function_types< boost::function< F > >
{
    typedef F type;
};

template <typename F>
struct convert_function_types
{
    typedef typename boost::remove_pointer<F>::type type;
};

//-----------------------------------------------------------------------------


template < typename F, typename enable = void >
struct function_types;

template < typename F>
struct function_types <F, typename boost::enable_if<typename boost::is_function< F >::type >::type >
{
    typedef  F FunctionType;
    enum { arity = boost::function_types::function_arity< FunctionType >::value };
    typedef typename boost::function_types::result_type<FunctionType>::type result_type;
    template < int ARG_NUMBER, bool check = false >
    struct arg
    {
        BOOST_STATIC_ASSERT( (!check) || ARG_NUMBER < boost::function_types::function_arity< FunctionType >::value );
        typedef typename boost::mpl::at_c<boost::function_types::parameter_types< FunctionType >,ARG_NUMBER>::type type;
    };
};



//-----------------------------------------------------------------------------


struct Slot
{

    struct bad_call { };
    struct bad_run { };

    virtual ~Slot()
    {
    }

    template< typename A1, typename A2, typename A3 > void run(A1 a1, A2 a2, A3 a3);
    template< typename A1, typename A2 > void run(A1 a1, A2 a2);
    template< typename A1 > void run(A1 a1);
    void run();

    template< typename R, typename A1, typename A2, typename A3 > R call(A1 a1, A2 a2, A3 a3);
    template< typename R, typename A1, typename A2 > R call(A1 a1, A2 a2);
    template< typename R, typename A1 > R call(A1 a1);
    template< typename R > R call();


    protected:
        template < typename F >
        std::string getTypeName()
        {
            std::string signature = std::string("type(") + typeid(F).name() + ")";
            return signature;
        }

        std::string m_signature;

};
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename A1 = boost::mpl::void_, typename A2 = boost::mpl::void_, typename A3 = boost::mpl::void_,
          typename A4 = boost::mpl::void_, typename A5 = boost::mpl::void_, typename A6 = boost::mpl::void_,
          typename A7 = boost::mpl::void_, typename A8 = boost::mpl::void_, typename A9 = boost::mpl::void_ >
struct slot_args : Slot
{
    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 ) = 0;
};
template< typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct slot_args< A1, A2, A3, A4, A5, A6, A7, A8, boost::mpl::void_> : Slot
{
    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 ) = 0;
};
template< typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct slot_args< A1, A2, A3, A4, A5, A6, A7, boost::mpl::void_, boost::mpl::void_> : Slot
{
    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 ) = 0;
};
template< typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct slot_args< A1, A2, A3, A4, A5, A6, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_> : Slot
{
    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 ) = 0;
};
template< typename A1, typename A2, typename A3, typename A4, typename A5 >
struct slot_args< A1, A2, A3, A4, A5, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_> : Slot
{
    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ) = 0;
};
template< typename A1, typename A2, typename A3, typename A4 >
struct slot_args< A1, A2, A3, A4, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                  boost::mpl::void_> : Slot
{
    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4 ) = 0;
};
template< typename A1, typename A2, typename A3 >
struct slot_args< A1, A2, A3, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                  boost::mpl::void_, boost::mpl::void_> : Slot
{
    virtual void run( A1 a1, A2 a2, A3 a3 ) = 0;
};
template< typename A1, typename A2 >
struct slot_args< A1, A2, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                  boost::mpl::void_, boost::mpl::void_> : Slot
{
    virtual void run( A1 a1, A2 a2 ) = 0;
};
template< typename A1 >
struct slot_args< A1, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                  boost::mpl::void_, boost::mpl::void_, boost::mpl::void_> : Slot
{
    virtual void run( A1 a1 ) = 0;
};
template<>
struct slot_args< boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                  boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_> : Slot
{
    virtual void run() = 0;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename ... A>
struct slot_args : Slot
{
    virtual void run(A ... a) = 0;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename R, typename A1 = boost::mpl::void_, typename A2 = boost::mpl::void_, typename A3 = boost::mpl::void_,
          typename A4             = boost::mpl::void_, typename A5 = boost::mpl::void_, typename A6 = boost::mpl::void_,
          typename A7             = boost::mpl::void_, typename A8 = boost::mpl::void_,
          typename A9             = boost::mpl::void_  >
struct slot_result : slot_args< A1, A2, A3, A4, A5, A6, A7, A8, A9 >
{
    virtual R call( A1, A2, A3, A4, A5, A6, A7, A8, A9 ) = 0;
};
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
          typename A8 >
struct slot_result<  R, A1, A2, A3, A4, A5, A6, A7, A8, boost::mpl::void_ > : slot_args< A1, A2, A3, A4, A5, A6, A7,
                                                                                         A8 >
{
    virtual R call( A1, A2, A3, A4, A5, A6, A7, A8 ) = 0;
};
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct slot_result<  R, A1, A2, A3, A4, A5, A6, A7, boost::mpl::void_, boost::mpl::void_ > : slot_args< A1, A2, A3, A4,
                                                                                                        A5, A6, A7 >
{
    virtual R call( A1, A2, A3, A4, A5, A6, A7 ) = 0;
};
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct slot_result<  R, A1, A2, A3, A4, A5, A6, boost::mpl::void_, boost::mpl::void_,
                     boost::mpl::void_ > : slot_args< A1, A2, A3, A4, A5, A6 >
{
    virtual R call( A1, A2, A3, A4, A5, A6 ) = 0;
};
template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct slot_result<  R, A1, A2, A3, A4, A5, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                     boost::mpl::void_ > : slot_args< A1, A2, A3, A4, A5 >
{
    virtual R call( A1, A2, A3, A4, A5 ) = 0;
};
template< typename R, typename A1, typename A2, typename A3, typename A4 >
struct slot_result<  R, A1, A2, A3, A4, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                     boost::mpl::void_ > : slot_args< A1, A2, A3, A4 >
{
    virtual R call( A1, A2, A3, A4 ) = 0;
};
template< typename R, typename A1, typename A2, typename A3 >
struct slot_result<  R, A1, A2, A3, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                     boost::mpl::void_, boost::mpl::void_ > : slot_args< A1, A2, A3 >
{
    virtual R call( A1, A2, A3 ) = 0;
};
template< typename R, typename A1, typename A2 >
struct slot_result<  R, A1, A2, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                     boost::mpl::void_, boost::mpl::void_, boost::mpl::void_ > : slot_args< A1, A2 >
{
    virtual R call( A1, A2 ) = 0;
};
template< typename R, typename A1 >
struct slot_result<  R, A1, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                     boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_ > : slot_args< A1 >
{
    virtual R call( A1 ) = 0;
};
template< typename R>
struct slot_result<  R, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_,
                     boost::mpl::void_, boost::mpl::void_, boost::mpl::void_, boost::mpl::void_ > : slot_args<>
{
    virtual R call() = 0;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename R, typename ... A >
struct slot_result : slot_args<A ...>
{
    virtual R call(A ...) = 0;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES





template< typename F > struct SlotFunction;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
         typename A8, typename A9 >
struct SlotFunction< boost::function< R ( A1, A2, A3, A4, A5, A6, A7, A8, A9 ) > > : SlotFunction< R ( A1, A2, A3, A4,
                                                                                                       A5, A6, A7, A8,
                                                                                                       A9 ) >
{
    typedef R FunctorType ( A1, A2, A3, A4, A5, A6, A7, A8, A9 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
    {
        m_func( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
    }
    virtual R   call( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
    {
        return m_func( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
         typename A8 >
struct SlotFunction< boost::function< R ( A1, A2, A3, A4, A5, A6, A7, A8 ) > > : SlotFunction< R ( A1, A2, A3, A4, A5,
                                                                                                   A6, A7, A8 ) >
{
    typedef R FunctorType ( A1, A2, A3, A4, A5, A6, A7, A8 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
    {
        m_func( a1, a2, a3, a4, a5, a6, a7, a8 );
    }
    virtual R   call( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
    {
        return m_func( a1, a2, a3, a4, a5, a6, a7, a8 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct SlotFunction< boost::function< R ( A1, A2, A3, A4, A5, A6, A7 ) > > : SlotFunction< R ( A1, A2, A3, A4, A5, A6,
                                                                                               A7 ) >
{
    typedef R FunctorType ( A1, A2, A3, A4, A5, A6, A7 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
    {
        m_func( a1, a2, a3, a4, a5, a6, a7 );
    }
    virtual R   call( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
    {
        return m_func( a1, a2, a3, a4, a5, a6, a7 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct SlotFunction< boost::function< R ( A1, A2, A3, A4, A5, A6 ) > > : SlotFunction< R ( A1, A2, A3, A4, A5, A6 ) >
{
    typedef R FunctorType ( A1, A2, A3, A4, A5, A6 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
    {
        m_func( a1, a2, a3, a4, a5, a6 );
    }
    virtual R   call( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
    {
        return m_func( a1, a2, a3, a4, a5, a6 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct SlotFunction< boost::function< R ( A1, A2, A3, A4, A5 ) > > : SlotFunction< R ( A1, A2, A3, A4, A5 ) >
{
    typedef R FunctorType ( A1, A2, A3, A4, A5 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
    {
        m_func( a1, a2, a3, a4, a5 );
    }
    virtual R   call( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
    {
        return m_func( a1, a2, a3, a4, a5 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1, typename A2, typename A3, typename A4 >
struct SlotFunction< boost::function< R ( A1, A2, A3, A4 ) > > : SlotFunction< R ( A1, A2, A3, A4 ) >
{
    typedef R FunctorType ( A1, A2, A3, A4 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2, A3 a3, A4 a4 )
    {
        m_func( a1, a2, a3, a4 );
    }
    virtual R   call( A1 a1, A2 a2, A3 a3, A4 a4 )
    {
        return m_func( a1, a2, a3, a4 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1, typename A2, typename A3 >
struct SlotFunction< boost::function< R ( A1, A2, A3 ) > > : SlotFunction< R ( A1, A2, A3 ) >
{
    typedef R FunctorType ( A1, A2, A3 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2, A3 a3 )
    {
        m_func( a1, a2, a3 );
    }
    virtual R   call( A1 a1, A2 a2, A3 a3 )
    {
        return m_func( a1, a2, a3 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1, typename A2 >
struct SlotFunction< boost::function< R ( A1, A2 ) > > : SlotFunction< R ( A1, A2 ) >
{
    typedef R FunctorType ( A1, A2 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1, A2 a2 )
    {
        m_func( a1, a2 );
    }
    virtual R   call( A1 a1, A2 a2 )
    {
        return m_func( a1, a2 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R, typename A1 >
struct SlotFunction< boost::function< R ( A1 ) > > : SlotFunction< R ( A1 ) >
{
    typedef R FunctorType ( A1 );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run( A1 a1 )
    {
        m_func( a1 );
    }
    virtual R   call( A1 a1 )
    {
        return m_func( a1 );
    }
    protected:
        boost::function< FunctorType > m_func;
};
template<typename R>
struct SlotFunction< boost::function< R () > > : SlotFunction< R () >
{
    typedef R FunctorType ();

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run()
    {
        m_func();
    }
    virtual R   call()
    {
        return m_func();
    }
    protected:
        boost::function< FunctorType > m_func;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ... A >
struct SlotFunction< boost::function< R ( A ... ) > > : SlotFunction< R ( A ... ) >
{
    typedef R FunctorType ( A ... );

    template< typename FUNCTOR > SlotFunction( FUNCTOR f ) : m_func(f)
    {
    }


    virtual void run(A ... a)
    {
        m_func(a ...);
    }
    virtual R   call(A ... a)
    {
        return m_func(a ...);
    }
    protected:
        boost::function< FunctorType > m_func;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
         typename A8, typename A9 >
struct SlotFunction< R ( A1, A2, A3, A4, A5, A6, A7, A8, A9 ) > : slot_result< R, A1, A2, A3, A4, A5, A6, A7, A8, A9 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2, A3, A4, A5, A6, A7, A8, A9 ) >();
    }

};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
         typename A8 >
struct SlotFunction< R ( A1, A2, A3, A4, A5, A6, A7, A8 ) > : slot_result< R, A1, A2, A3, A4, A5, A6, A7, A8 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2, A3, A4, A5, A6, A7, A8 ) >();
    }

};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct SlotFunction< R ( A1, A2, A3, A4, A5, A6, A7 ) > : slot_result< R, A1, A2, A3, A4, A5, A6, A7 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2, A3, A4, A5, A6, A7 ) >();
    }

};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct SlotFunction< R ( A1, A2, A3, A4, A5, A6 ) > : slot_result< R, A1, A2, A3, A4, A5, A6 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2, A3, A4, A5, A6 ) >();
    }

};
template<typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct SlotFunction< R ( A1, A2, A3, A4, A5 ) > : slot_result< R, A1, A2, A3, A4, A5 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2, A3, A4, A5 ) >();
    }

};
template<typename R, typename A1, typename A2, typename A3, typename A4 >
struct SlotFunction< R ( A1, A2, A3, A4 ) > : slot_result< R, A1, A2, A3, A4 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2, A3, A4 ) >();
    }

};
template<typename R, typename A1, typename A2, typename A3 >
struct SlotFunction< R ( A1, A2, A3 ) > : slot_result< R, A1, A2, A3 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2, A3 ) >();
    }

};
template<typename R, typename A1, typename A2 >
struct SlotFunction< R ( A1, A2 ) > : slot_result< R, A1, A2 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1, A2 ) >();
    }

};
template<typename R, typename A1 >
struct SlotFunction< R ( A1 ) > : slot_result< R, A1 >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A1 ) >();
    }

};
template<typename R>
struct SlotFunction< R () > : slot_result< R>
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R () >();
    }

};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ... A >
struct SlotFunction< R ( A ... ) > : slot_result< R, A ... >
{
    SlotFunction()
    {
        // this-> needed by gcc 4.2
        this->Slot::m_signature = Slot::getTypeName< R ( A ... ) >();
    }

};
#endif  // BOOST_NO_VARIADIC_TEMPLATES








template< typename A1, typename A2, typename A3 > void Slot::run(A1 a1, A2 a2, A3 a3)
{
    typedef slot_args< A1, A2, A3 > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1,a2,a3);
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " <<
            Slot::getTypeName< void(A1, A2, A3) >()  << std::endl;
        this->run(a1,a2);
    }
}
template< typename A1, typename A2 > void Slot::run(A1 a1, A2 a2)
{
    typedef slot_args< A1, A2 > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1,a2);
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " << Slot::getTypeName< void(A1, A2) >()  << std::endl;
        this->run(a1);
    }
}
template< typename A1 > void Slot::run(A1 a1)
{
    typedef slot_args< A1 > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1);
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " << Slot::getTypeName< void(A1) >()  << std::endl;
        this->run();
    }
}
void Slot::run()
{
    typedef slot_args< > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        fun->run();
    }
    else
    {
        std::cerr << "failed to run : " << m_signature << " with " << Slot::getTypeName< void() >()  << std::endl;
        throw Slot::bad_run();
    }
}




template< typename R, typename A1, typename A2, typename A3 > R Slot::call(A1 a1, A2 a2, A3 a3)
{
    typedef SlotFunction< R (A1, A2, A3) > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1,a2,a3);
    }
    else
    {
        std::cerr << "failed to call : " << m_signature << " with " <<
            Slot::getTypeName< void(A1, A2, A3) >()  << std::endl;
        return this->call<R>(a1,a2);
    }
}
template< typename R, typename A1, typename A2 > R Slot::call(A1 a1, A2 a2)
{
    typedef SlotFunction< R(A1, A2) > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1,a2);
    }
    else
    {
        std::cerr << "failed to call : " << m_signature << " with " <<
            Slot::getTypeName< void(A1, A2) >()  << std::endl;
        return this->call<R>(a1);
    }
}
template< typename R, typename A1 > R Slot::call(A1 a1)
{
    typedef SlotFunction< R(A1) > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1);
    }
    else
    {
        std::cerr << "failed to call : " << m_signature << " with " << Slot::getTypeName< void(A1) >()  << std::endl;
        return this->call<R>();
    }
}
template< typename R > R Slot::call()
{
    typedef slot_result< R > SlotFuncType;
    SlotFuncType *fun = dynamic_cast< SlotFuncType* >(this);
    if(fun)
    {
        std::cerr << "failed to call : " << m_signature << " with " << Slot::getTypeName< void() >()  << std::endl;
        return fun->call();
    }
    throw Slot::bad_call();
}




//-----------------------------------------------------------------------------


class Slots
{
public:
    typedef std::string SlotKeyType;
    typedef boost::shared_ptr< Slot > SlotPtr;
    typedef std::map< SlotKeyType, SlotPtr > SlotMap;

    Slots()
    {
    }
    Slots( const SlotKeyType &key, SlotPtr slot )
    {
        (*this)(key, slot);
    }


    Slots& operator()( const SlotKeyType &key, SlotPtr slot )
    {
        m_slots.insert( SlotMap::value_type(key, slot) );
        return *this;
    }

    SlotPtr operator()( const SlotKeyType &key )
    {
        SlotMap::iterator it = m_slots.find(key);

        if(it != m_slots.end())
        {
            return it->second;
        }

        return SlotPtr();
    }

protected:
    SlotMap m_slots;
};




//-----------------------------------------------------------------------------



template<typename T>
struct is_shared_ptr
{
    enum { value = false };
};

template<typename T>
struct is_shared_ptr< boost::shared_ptr< T > >
{
    enum { value = true };
};


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// thread management


void WorkerThread( boost::asio::io_service & io_service )
{
    std::cout << "Thread " << boost::this_thread::get_id() <<" Start\n";
    io_service.run();
    std::cout << "Thread " << boost::this_thread::get_id() <<" Finish\n";
}


class Worker
{
public:
    typedef ::boost::asio::io_service IOServiceType;
    typedef ::boost::asio::io_service::work WorkType;
    typedef ::boost::shared_ptr< WorkType > WorkPtrType;

    Worker();
    ~Worker();

    template< typename Handler>
    void post(Handler handler)
    {
        m_ioService.post(handler);
    }

protected:
    Worker( const Worker& );
    Worker& operator=( const Worker& );


    IOServiceType m_ioService;
    WorkPtrType m_work;
    boost::thread m_thread;
};

Worker::Worker() :
    m_ioService(),
    m_work( ::boost::make_shared< WorkType >(boost::ref(m_ioService)) ),
    m_thread( ::boost::bind(&WorkerThread, boost::ref(m_ioService)) )
{
}

Worker::~Worker()
{
    m_work.reset();
    m_ioService.stop();
    m_thread.join();
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


template <typename R>
struct TaskHandler
{
    TaskHandler(boost::packaged_task<R>& task) : m_Task(boost::move(task))
    {
    }
    TaskHandler(const TaskHandler& that) : m_Task(boost::move(that.m_Task))
    {
    }

    void operator ()()
    {
        this->m_Task();
    }

    private:
        mutable boost::packaged_task<R> m_Task;
};


template <typename R>
inline boost::function< void() > moveTaskIntoFunction(boost::packaged_task<R>& task)
{
    return TaskHandler<R>(task);
}





class Method
{
public:


    Method(Slots::SlotPtr slot) : m_slot(slot)
    {
    }

    Slots::SlotPtr operator=(Slots::SlotPtr slot)
    {
        m_slot = slot; return slot;
    }
    Method& operator=(const Method &other)
    {
        m_slot = other.m_slot; return *this;
    }
    // template < typename SLOTPTR >
    // Method( const SLOTPTR &slot, typename boost::enable_if_c< is_shared_ptr< SLOTPTR >::value >::type*x=0 )  : m_slot(slot) {}

    template<typename F>
    Method( F f,
            typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity ==
                                         0 >::type*x = 0  )
    {
        setSlot(f);
    }
    template<typename F>
    Method( F f,
            typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity ==
                                         1 >::type*x = 0  )
    {
        setSlot(f,_1);
    }
    template<typename F>
    Method( F f,
            typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity ==
                                         2 >::type*x = 0  )
    {
        setSlot(f,_1,_2);
    }
    template<typename F>
    Method( F f,
            typename boost::enable_if_c< function_types< typename boost::remove_pointer<F>::type >::arity ==
                                         3 >::type*x = 0  )
    {
        setSlot(f,_1,_2,_3);
    }

    template<typename F, typename O >
    Method( F f, O *o,
            typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity ==
                                         0 >::type*x = 0 )
    {
        setSlot(f,o);
    }
    template<typename F, typename O >
    Method( F f, O *o,
            typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity ==
                                         1 >::type*x = 0 )
    {
        setSlot(f,o,_1);
    }
    template<typename F, typename O >
    Method( F f, O *o,
            typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity ==
                                         2 >::type*x = 0 )
    {
        setSlot(f,o,_1,_2);
    }
    template<typename F, typename O >
    Method( F f, O *o,
            typename boost::enable_if_c< function_types< typename convert_function_types< F >::type >::arity ==
                                         3 >::type*x = 0 )
    {
        setSlot(f,o,_1,_2,_3);
    }
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8,
               typename A9 >
    void operator()( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8, A9 args9 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6, args7, args8, args9 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
    void operator()( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6, args7, args8 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
    void operator()( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6, args7 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
    void operator()( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5 >
    void operator()( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 )
    {
        m_slot->run( args1, args2, args3, args4, args5 );
    }

    template < typename A1, typename A2, typename A3, typename A4 >
    void operator()( A1 args1, A2 args2, A3 args3, A4 args4 )
    {
        m_slot->run( args1, args2, args3, args4 );
    }

    template < typename A1, typename A2, typename A3 >
    void operator()( A1 args1, A2 args2, A3 args3 )
    {
        m_slot->run( args1, args2, args3 );
    }

    template < typename A1, typename A2 >
    void operator()( A1 args1, A2 args2 )
    {
        m_slot->run( args1, args2 );
    }

    template < typename A1 >
    void operator()( A1 args1 )
    {
        m_slot->run( args1 );
    }


    void operator()()
    {
        m_slot->run();
    }

//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template <typename ... A>
    void operator()( A ... args  )
    {
        m_slot->run( args ... );
    }

#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8,
               typename A9 >
    void run( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8, A9 args9 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6, args7, args8, args9 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
    void run( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6, args7, args8 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
    void run( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6, args7 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
    void run( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6 )
    {
        m_slot->run( args1, args2, args3, args4, args5, args6 );
    }

    template < typename A1, typename A2, typename A3, typename A4, typename A5 >
    void run( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 )
    {
        m_slot->run( args1, args2, args3, args4, args5 );
    }

    template < typename A1, typename A2, typename A3, typename A4 >
    void run( A1 args1, A2 args2, A3 args3, A4 args4 )
    {
        m_slot->run( args1, args2, args3, args4 );
    }

    template < typename A1, typename A2, typename A3 >
    void run( A1 args1, A2 args2, A3 args3 )
    {
        m_slot->run( args1, args2, args3 );
    }

    template < typename A1, typename A2 >
    void run( A1 args1, A2 args2 )
    {
        m_slot->run( args1, args2 );
    }

    template < typename A1 >
    void run( A1 args1 )
    {
        m_slot->run( args1 );
    }


    void run()
    {
        m_slot->run();
    }

//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template <typename ... A>
    void run( A ... args  )
    {
        m_slot->run( args ... );
    }

#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
              typename A8, typename A9 >
    R call( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8, A9 args9 )
    {
        return m_slot->call<R>( args1, args2, args3, args4, args5, args6, args7, args8, args9 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
              typename A8 >
    R call( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8 )
    {
        return m_slot->call<R>( args1, args2, args3, args4, args5, args6, args7, args8 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
    R call( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7 )
    {
        return m_slot->call<R>( args1, args2, args3, args4, args5, args6, args7 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
    R call( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6 )
    {
        return m_slot->call<R>( args1, args2, args3, args4, args5, args6 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
    R call( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 )
    {
        return m_slot->call<R>( args1, args2, args3, args4, args5 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4 >
    R call( A1 args1, A2 args2, A3 args3, A4 args4 )
    {
        return m_slot->call<R>( args1, args2, args3, args4 );
    }



    template <typename R, typename A1, typename A2, typename A3 >
    R call( A1 args1, A2 args2, A3 args3 )
    {
        return m_slot->call<R>( args1, args2, args3 );
    }



    template <typename R, typename A1, typename A2 >
    R call( A1 args1, A2 args2 )
    {
        return m_slot->call<R>( args1, args2 );
    }



    template <typename R, typename A1 >
    R call( A1 args1 )
    {
        return m_slot->call<R>( args1 );
    }



    template <typename R>
    R call()
    {
        return m_slot->call<R>();
    }



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template <typename R, typename ... A>
    R call( A ... args  )
    {
        return m_slot->call<R>( args ... );
    }



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template< typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8,
              typename A9 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6,
                                             A7 args7, A8 args8, A9 args9 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2, A3, A4, A5, A6, A7, A8, A9 >( args1, args2, args3,
                                                                                                 args4, args5, args6,
                                                                                                 args7, args8,
                                                                                                 args9 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6,
                                             A7 args7, A8 args8 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2, A3, A4, A5, A6, A7, A8 >( args1, args2, args3, args4,
                                                                                             args5, args6, args7,
                                                                                             args8 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6,
                                             A7 args7 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2, A3, A4, A5, A6, A7 >( args1, args2, args3, args4,
                                                                                         args5, args6, args7 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5,
                                             A6 args6 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2, A3, A4, A5, A6 >( args1, args2, args3, args4, args5,
                                                                                     args6 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1, typename A2, typename A3, typename A4, typename A5 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2, A3, A4, A5 >( args1, args2, args3, args4, args5 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1, typename A2, typename A3, typename A4 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2, A3, A4 >( args1, args2, args3, args4 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1, typename A2, typename A3 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2, A3 args3 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2, A3 >( args1, args2, args3 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1, typename A2 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1, A2 args2 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1, A2 >( args1, args2 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



    template< typename A1 >
    ::boost::shared_future< void > asyncRun( Worker &worker, A1 args1 )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A1 >( args1 ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    ::boost::shared_future< void > asyncRun( Worker &worker)
    {
        ::boost::packaged_task<void> task(  this->bindRun() );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template< typename ... A >
    ::boost::shared_future< void > asyncRun( Worker &worker, A ... args )
    {
        ::boost::packaged_task<void> task(  this->bindRun< A ... >( args ... ) );
        ::boost::future< void > ufuture = task.get_future();

        boost::function< void () > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
              typename A8, typename A9 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6,
                                           A7 args7, A8 args8, A9 args9 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2, A3, A4, A5, A6, A7, A8, A9 >( args1, args2, args3,
                                                                                                 args4, args5, args6,
                                                                                                 args7, args8,
                                                                                                 args9 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
              typename A8 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6,
                                           A7 args7, A8 args8 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2, A3, A4, A5, A6, A7, A8 >( args1, args2, args3, args4,
                                                                                             args5, args6, args7,
                                                                                             args8 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6,
                                           A7 args7 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2, A3, A4, A5, A6, A7 >( args1, args2, args3, args4,
                                                                                         args5, args6, args7 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2, A3, A4, A5, A6 >( args1, args2, args3, args4, args5,
                                                                                     args6 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2, A3, A4, A5 >( args1, args2, args3, args4, args5 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1, typename A2, typename A3, typename A4 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2, A3 args3, A4 args4 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2, A3, A4 >( args1, args2, args3, args4 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1, typename A2, typename A3 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2, A3 args3 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2, A3 >( args1, args2, args3 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1, typename A2 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1, A2 args2 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1, A2 >( args1, args2 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R, typename A1 >
    ::boost::shared_future< R > asyncCall( Worker &worker, A1 args1 )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A1 >( args1 ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




    template< typename R>
    ::boost::shared_future< R > asyncCall( Worker &worker)
    {
        ::boost::packaged_task<R> task( this->bindCall< R>() );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template< typename R, typename ... A >
    ::boost::shared_future< R > asyncCall( Worker &worker, A ... args )
    {
        ::boost::packaged_task<R> task( this->bindCall< R, A ... >( args ... ) );
        ::boost::future< R > ufuture = task.get_future();

        boost::function< void() > f = moveTaskIntoFunction(task);

        worker.post(f);

        return boost::move(ufuture);
    }




#endif  // BOOST_NO_VARIADIC_TEMPLATES
    Slots::SlotPtr slot()
    {
        return m_slot;
    }
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8,
               typename A9 >
    boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8,
                                       A9 args9 )
    {
        return boost::bind( ( void (Method::*)( A1, A2, A3, A4, A5, A6, A7, A8,
                                                A9 ) ) &Method::run< A1, A2, A3, A4, A5, A6, A7, A8, A9 >, this, args1, args2, args3, args4, args5, args6, args7, args8,
                            args9 );
    }


    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
    boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8 )
    {
        return boost::bind( ( void (Method::*)( A1, A2, A3, A4, A5, A6, A7,
                                                A8 ) ) &Method::run< A1, A2, A3, A4, A5, A6, A7, A8 >, this, args1, args2, args3, args4, args5, args6, args7,
                            args8 );
    }


    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
    boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7 )
    {
        return boost::bind( ( void (Method::*)( A1, A2, A3, A4, A5, A6,
                                                A7 ) ) &Method::run< A1, A2, A3, A4, A5, A6, A7 >, this, args1, args2, args3, args4, args5, args6,
                            args7 );
    }


    template < typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
    boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6 )
    {
        return boost::bind( ( void (Method::*)( A1, A2, A3, A4, A5,
                                                A6 ) ) &Method::run< A1, A2, A3, A4, A5, A6 >, this, args1, args2, args3, args4, args5,
                            args6 );
    }


    template < typename A1, typename A2, typename A3, typename A4, typename A5 >
    boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 )
    {
        return boost::bind( ( void (Method::*)( A1, A2, A3, A4,
                                                A5 ) ) &Method::run< A1, A2, A3, A4, A5 >, this, args1, args2, args3, args4,
                            args5 );
    }


    template < typename A1, typename A2, typename A3, typename A4 >
    boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3, A4 args4 )
    {
        return boost::bind( ( void (Method::*)( A1, A2, A3,
                                                A4 ) ) &Method::run< A1, A2, A3, A4 >, this, args1, args2, args3,
                            args4 );
    }


    template < typename A1, typename A2, typename A3 >
    boost::function< void() > bindRun( A1 args1, A2 args2, A3 args3 )
    {
        return boost::bind( ( void (Method::*)( A1, A2, A3 ) ) &Method::run< A1, A2, A3 >, this, args1, args2, args3 );
    }


    template < typename A1, typename A2 >
    boost::function< void() > bindRun( A1 args1, A2 args2 )
    {
        return boost::bind( ( void (Method::*)( A1, A2 ) ) &Method::run< A1, A2 >, this, args1, args2 );
    }


    template < typename A1 >
    boost::function< void() > bindRun( A1 args1 )
    {
        return boost::bind( ( void (Method::*)( A1 ) ) &Method::run< A1 >, this, args1 );
    }



    boost::function< void() > bindRun()
    {
        return boost::bind( ( void (Method::*)() ) &Method::run, this);
    }


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template <typename ... A>
    boost::function< void() > bindRun( A ... args  )
    {
        return boost::bind( ( void (Method::*)(A ...) ) &Method::run< A ... >, this, args ... );
    }


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
              typename A8, typename A9 >
    boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8,
                                     A9 args9 )
    {
        return boost::bind( ( R (Method::*)( A1, A2, A3, A4, A5, A6, A7, A8,
                                             A9 ) ) &Method::call<R, A1, A2, A3, A4, A5, A6, A7, A8, A9 >, this, args1, args2, args3, args4, args5, args6, args7, args8,
                            args9 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
              typename A8 >
    boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7, A8 args8 )
    {
        return boost::bind( ( R (Method::*)( A1, A2, A3, A4, A5, A6, A7,
                                             A8 ) ) &Method::call<R, A1, A2, A3, A4, A5, A6, A7, A8 >, this, args1, args2, args3, args4, args5, args6, args7,
                            args8 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
    boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6, A7 args7 )
    {
        return boost::bind( ( R (Method::*)( A1, A2, A3, A4, A5, A6,
                                             A7 ) ) &Method::call<R, A1, A2, A3, A4, A5, A6, A7 >, this, args1, args2, args3, args4, args5, args6,
                            args7 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
    boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5, A6 args6 )
    {
        return boost::bind( ( R (Method::*)( A1, A2, A3, A4, A5,
                                             A6 ) ) &Method::call<R, A1, A2, A3, A4, A5, A6 >, this, args1, args2, args3, args4, args5,
                            args6 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
    boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4, A5 args5 )
    {
        return boost::bind( ( R (Method::*)( A1, A2, A3, A4,
                                             A5 ) ) &Method::call<R, A1, A2, A3, A4, A5 >, this, args1, args2, args3, args4,
                            args5 );
    }



    template <typename R, typename A1, typename A2, typename A3, typename A4 >
    boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3, A4 args4 )
    {
        return boost::bind( ( R (Method::*)( A1, A2, A3,
                                             A4 ) ) &Method::call<R, A1, A2, A3, A4 >, this, args1, args2, args3,
                            args4 );
    }



    template <typename R, typename A1, typename A2, typename A3 >
    boost::function< R() > bindCall( A1 args1, A2 args2, A3 args3 )
    {
        return boost::bind( ( R (Method::*)( A1, A2, A3 ) ) &Method::call<R, A1, A2, A3 >, this, args1, args2, args3 );
    }



    template <typename R, typename A1, typename A2 >
    boost::function< R() > bindCall( A1 args1, A2 args2 )
    {
        return boost::bind( ( R (Method::*)( A1, A2 ) ) &Method::call<R, A1, A2 >, this, args1, args2 );
    }



    template <typename R, typename A1 >
    boost::function< R() > bindCall( A1 args1 )
    {
        return boost::bind( ( R (Method::*)( A1 ) ) &Method::call<R, A1 >, this, args1 );
    }



    template <typename R>
    boost::function< R() > bindCall()
    {
        return boost::bind( ( R (Method::*)() ) &Method::call<R>, this);
    }



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template <typename R, typename ... A>
    boost::function< R() > bindCall( A ... args  )
    {
        return boost::bind( ( R (Method::*)(A ...) ) &Method::call<R, A ...>, this, args ... );
    }



#endif  // BOOST_NO_VARIADIC_TEMPLATES
protected:
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
    template<typename F, typename Bindings1, typename Bindings2, typename Bindings3, typename Bindings4,
             typename Bindings5, typename Bindings6, typename Bindings7, typename Bindings8,
             typename Bindings9 > void setSlot(
        F f, Bindings1 bindings1, Bindings2 bindings2, Bindings3 bindings3, Bindings4 bindings4, Bindings5 bindings5,
        Bindings6 bindings6, Bindings7 bindings7, Bindings8 bindings8, Bindings9 bindings9 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func = boost::bind(f, bindings1, bindings2, bindings3, bindings4, bindings5, bindings6, bindings7, bindings8,
                           bindings9 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1, typename Bindings2, typename Bindings3, typename Bindings4,
             typename Bindings5, typename Bindings6, typename Bindings7, typename Bindings8 > void setSlot(F f,
                                                                                                           Bindings1 bindings1,
                                                                                                           Bindings2 bindings2,
                                                                                                           Bindings3 bindings3,
                                                                                                           Bindings4 bindings4,
                                                                                                           Bindings5 bindings5,
                                                                                                           Bindings6 bindings6,
                                                                                                           Bindings7 bindings7,
                                                                                                           Bindings8 bindings8 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func =
            boost::bind(f, bindings1, bindings2, bindings3, bindings4, bindings5, bindings6, bindings7, bindings8 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1, typename Bindings2, typename Bindings3, typename Bindings4,
             typename Bindings5, typename Bindings6, typename Bindings7 > void setSlot(F f, Bindings1 bindings1,
                                                                                       Bindings2 bindings2,
                                                                                       Bindings3 bindings3,
                                                                                       Bindings4 bindings4,
                                                                                       Bindings5 bindings5,
                                                                                       Bindings6 bindings6,
                                                                                       Bindings7 bindings7 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings1, bindings2, bindings3, bindings4, bindings5, bindings6, bindings7 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1, typename Bindings2, typename Bindings3, typename Bindings4,
             typename Bindings5, typename Bindings6 > void setSlot(F f, Bindings1 bindings1, Bindings2 bindings2,
                                                                   Bindings3 bindings3, Bindings4 bindings4,
                                                                   Bindings5 bindings5,
                                                                   Bindings6 bindings6 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings1, bindings2, bindings3, bindings4, bindings5, bindings6 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1, typename Bindings2, typename Bindings3, typename Bindings4,
             typename Bindings5 > void setSlot(F f, Bindings1 bindings1, Bindings2 bindings2, Bindings3 bindings3,
                                               Bindings4 bindings4, Bindings5 bindings5 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings1, bindings2, bindings3, bindings4, bindings5 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1, typename Bindings2, typename Bindings3, typename Bindings4 > void setSlot(
        F f, Bindings1 bindings1, Bindings2 bindings2, Bindings3 bindings3, Bindings4 bindings4 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings1, bindings2, bindings3, bindings4 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1, typename Bindings2, typename Bindings3 > void setSlot(F f,
                                                                                                   Bindings1 bindings1,
                                                                                                   Bindings2 bindings2,
                                                                                                   Bindings3 bindings3 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings1, bindings2, bindings3 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1, typename Bindings2 > void setSlot(F f, Bindings1 bindings1,
                                                                               Bindings2 bindings2 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings1, bindings2 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F, typename Bindings1 > void setSlot(F f, Bindings1 bindings1 )
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings1 );
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


    template<typename F> void setSlot(F f)
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f);
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
    template<typename F, typename ... Bindings> void setSlot(F f, Bindings ... bindings)
    {
        typedef ::boost::function< typename convert_function_types< F >::type > FunctionType;
        FunctionType func;
        func   = boost::bind(f, bindings ...);
        m_slot = boost::make_shared< SlotFunction< FunctionType > > ( func );
    }


#endif  // BOOST_NO_VARIADIC_TEMPLATES
    Slots::SlotPtr m_slot;
};



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


struct Event {};
struct KeyEvent : Event {};

struct has_slots
{
    Slots::SlotPtr slot( Slots::SlotKeyType key )
    {
        return m_slots(key);
    }
    protected:
        Slots m_slots;
};

//-----------------------------------------------------------------------------

class A : public has_slots
{

public:
    A()
    {
        has_slots::m_slots
            ( "start", Method(&A::start, this).slot() )
            ( "update", Method(&A::update, this).slot() )
            ( "onEvent", Method(&A::onEvent, this).slot() )
            ( "onKeyPress", Method(&A::onKeyPress, this).slot() )
        ;

    }
    void start()
    {
        std::cout<<"A::start"<<std::endl;
    }
    void update()
    {
        std::cout<<"A::update"<<std::endl;
    }
    void onEvent( Event *e )
    {
        std::cout<<"A::onEvent "<< e <<std::endl;
    }
    void onKeyPress( KeyEvent *e )
    {
        std::cout<<"A::onKeyPress " << e <<std::endl;
    }
    void print( std::string msg )
    {
        std::cout << "A::print " <<msg<<std::endl;
    }

};

//-----------------------------------------------------------------------------

class B : public has_slots
{
public:
    B()
    {
        has_slots::m_slots
            ( "invoke", Method(&B::invoke, this).slot() )
        ;

    }
    void invoke()
    {
        std::cout<<"B::invoke"<<std::endl;
    }

    void print( std::string str )
    {
        std::cout<< str <<std::endl;
    }
};

//-----------------------------------------------------------------------------


int sum(int a, int b)
{
    return a + b;
}

void fe(Event*)
{
    std::cout << "fe " <<std::endl;
}

void fke(KeyEvent*)
{
    std::cout << "fke " <<std::endl;
}


int main(int argc, char* argv[])
{

    using namespace std;

    A * a = new A();
    B b;

    Event e;
    KeyEvent ke;

    cout << "============ v2 ============" << endl;
    // v2
    {
        Method call ( &A::start, a );
        call();
        call.call<void>();
    }

    {
        Method call ( &B::invoke, &b );
        call();
        call.call<void>();
        call.call<void>("toto", 3);
        call.call<void>(true, Event());
    }


    {
        Method call ( &A::onEvent, a );
        call(&e);
        call.run<Event*>(&ke);
    }

    {
        Method call ( &sum );
        call(5,7);
        call(5,7,8);
        call.run(5,7,8);
        cout << "sum: " << call.call<int>(5,7) << endl;
    }

    {
        Method call ( &A::onKeyPress, a );
        call.run(&ke);
    }

    cout << "============ v3 ============" << endl;

    // v3
    {
        Method call ( a->slot("start") );
        call.run();
        call.call<void>();

        call = b.slot("invoke");

        Method call2 = call;
        call2.run();

        call2 = call;
        call2.run();

        call = a->slot("update");
        Method call3(call);
        call3.run();
    }

    {
        Method call ( b.slot("invoke") );
        call();
        call.call<void>();
    }

    {
        Method call ( a->slot("onEvent") );
        call.run(&e);
        call.run<Event*>(&ke);
    }

    {
        Method call ( a->slot("onKeyPress") );
        call.run(&ke);

        a->slot("onKeyPress")->run(&ke);
    }


    {

        Slots slots;
        slots
            ( "sum_slot", Method( &sum ).slot() )
        ;

        Method call ( slots("sum_slot") );

        cout << "sum_slot: " << call.call<int>(44,66) << endl;
    }


    cout << "============ slot ============" << endl;

    // direct slot
    {
        a->slot("start")->run(&ke);
        a->slot("onEvent")->run(&e);
        a->slot("onKeyPress")->run(&ke);
        b.slot("invoke")->run();
    }


    cout << "============ signals ============" << endl;
    // signal
    {
        boost::signals2::signal<void (Event*)> sig;

        Method call1 ( &A::start, a );
        Method call2 ( &B::invoke, &b );
        Method call3 ( &A::onEvent, a );

        sig.connect(boost::ref(call1));
        sig.connect(boost::ref(call2));
        sig.connect(boost::ref(call3));

        sig( &e );
    }


    cout << "============ worker ============" << endl;
    cout << "============ worker & async ============" << endl;

    {
        Method invoke ( &B::invoke, &b );
        Method print ( &B::print, &b );
        // call.call<void>();
        // call.call<void>("toto", 3);
        // call.call<void>(true, Event());

        Worker w;

        w.post(invoke);
        w.post( boost::bind( &B::invoke, &b ) );
        w.post( boost::bind( &B::print, &b, "iosrv") );

        // print.bindRun(std::string("bind test"))();

        // w.post(print.bindRun(std::string("bind test")));

        w.post( boost::bind( &B::print, &b, "print : 1") );
        w.post( boost::bind( &B::print, &b, "print : 2") );
        w.post( boost::bind( &B::print, &b, "print : 3") );


        Method sumFunc ( &sum );

        Worker w1;
        Worker w2;

        boost::shared_future< int > future = sumFunc.asyncCall<int>(w1, 2,8);
        cout << "sum.is_ready(): " << future.is_ready() << endl;

        invoke.asyncRun(w1);
        print.asyncRun(w1, std::string("async run w1 0"));
        print.asyncRun(w1, std::string("async run w1 1"));
        print.asyncRun(w1, std::string("async run w1 2"));
        print.asyncRun(w1, std::string("async run w1 3"));
        print.asyncRun(w1, std::string("async run w1 4"));
        print.asyncRun(w1, std::string("async run w1 5"));


        print.asyncRun(w2, std::string("async run w2 0"));
        print.asyncRun(w2, std::string("async run w2 1"));
        print.asyncRun(w2, std::string("async run w2 2"));
        print.asyncRun(w2, std::string("async run w2 3"));
        print.asyncRun(w2, std::string("async run w2 4"));
        print.asyncRun(w2, std::string("async run w2 5"));



        future.wait();
        cout << "sum.is_ready(): " << future.is_ready() << endl;
        cout << "sum.has_value(): " << future.has_value() << endl;
        cout << "sum.has_exception(): " << future.has_exception() << endl;
        cout << "sum: " << future.get() << endl;



        print.asyncRun(w1, std::string("async run w1 with wait")).wait();
        print.asyncRun(w2, std::string("async run w2 with wait")).wait();

    }



    delete a;
    return 0;
}

/* Output :
   ============ v2 ============
   A::start
   A::start
   B::invoke
   B::invoke
   B::invoke
   B::invoke
   A::onEvent 0x7fff5fbff500
   A::onEvent 0x7fff5fbff4f8
   sum: 12
   A::onKeyPress 0x7fff5fbff4f8
   ============ v3 ============
   A::start
   A::start
   B::invoke
   B::invoke
   A::onEvent 0x7fff5fbff500
   A::onEvent 0x7fff5fbff4f8
   A::onKeyPress 0x7fff5fbff4f8
   A::onKeyPress 0x7fff5fbff4f8
   sum_slot: 110
   ============ slot ============
   A::start
   A::onEvent 0x7fff5fbff500
   A::onKeyPress 0x7fff5fbff4f8
   B::invoke
   ============ signals ============
   A::start
   B::invoke
   A::onEvent 0x7fff5fbff500

 */


