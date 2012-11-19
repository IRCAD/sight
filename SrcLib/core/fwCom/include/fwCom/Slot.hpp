/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOT_HPP__
#define __FWCOM_SLOT_HPP__

#include <boost/function.hpp>

#include "fwCom/SlotCall.hpp"
#include "fwCom/util/log.hpp"

namespace fwCom
{


template< typename F >
struct Slot;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
struct Slot< ::boost::function< R ( A1, A2, A3 ) > > : Slot< R ( A1, A2, A3 ) >
{
    typedef R SignatureType( A1, A2, A3 );
    typedef ::boost::function< SignatureType > FunctionType;


    template< typename FUNCTOR >
        Slot( FUNCTOR f ) : Slot< R ( A1, A2, A3 ) >(),  m_func(f)
    { }

    virtual void run( A1 a1, A2 a2, A3 a3 ) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (run)");
        m_func( a1, a2, a3 );
    };

    virtual R   call( A1 a1, A2 a2, A3 a3 ) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (call)");
        return m_func( a1, a2, a3 );
    };


protected:
    FunctionType m_func;
};
template<typename R, typename A1, typename A2 >
struct Slot< ::boost::function< R ( A1, A2 ) > > : Slot< R ( A1, A2 ) >
{
    typedef R SignatureType( A1, A2 );
    typedef ::boost::function< SignatureType > FunctionType;


    template< typename FUNCTOR >
        Slot( FUNCTOR f ) : Slot< R ( A1, A2 ) >(),  m_func(f)
    { }

    virtual void run( A1 a1, A2 a2 ) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (run)");
        m_func( a1, a2 );
    };

    virtual R   call( A1 a1, A2 a2 ) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (call)");
        return m_func( a1, a2 );
    };


protected:
    FunctionType m_func;
};
template<typename R, typename A1 >
struct Slot< ::boost::function< R ( A1 ) > > : Slot< R ( A1 ) >
{
    typedef R SignatureType( A1 );
    typedef ::boost::function< SignatureType > FunctionType;


    template< typename FUNCTOR >
        Slot( FUNCTOR f ) : Slot< R ( A1 ) >(),  m_func(f)
    { }

    virtual void run( A1 a1 ) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (run)");
        m_func( a1 );
    };

    virtual R   call( A1 a1 ) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (call)");
        return m_func( a1 );
    };


protected:
    FunctionType m_func;
};
template<typename R>
struct Slot< ::boost::function< R () > > : Slot< R () >
{
    typedef R SignatureType();
    typedef ::boost::function< SignatureType > FunctionType;


    template< typename FUNCTOR >
        Slot( FUNCTOR f ) : Slot< R () >(),  m_func(f)
    { }

    virtual void run() const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (run)");
        m_func();
    };

    virtual R   call() const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (call)");
        return m_func();
    };


protected:
    FunctionType m_func;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
struct Slot< ::boost::function< R ( A... ) > > : Slot< R ( A... ) >
{
    typedef R SignatureType(A...);
    typedef ::boost::function< SignatureType > FunctionType;


    template< typename FUNCTOR >
        Slot( FUNCTOR f ) : Slot< R ( A... ) >(),  m_func(f)
    { }

    virtual void run(A...a) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (run)");
        m_func(a...);
    };

    virtual R   call(A...a) const
    {
        OSLM_COM("Executes slot '"<< this->getID() <<"' (call)");
        return m_func(a...);
    };


protected:
    FunctionType m_func;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
struct Slot< Slot< R ( A1, A2, A3 ) > > : Slot< boost::function < R ( A1, A2, A3 ) > >
{


    typedef R SignatureType ( A1, A2, A3 ) ;
    typedef ::boost::function< SignatureType > FunctionType;

    template< typename F >
    Slot( SPTR( SlotRun< F > ) slot );
    template< typename F >
    Slot( SPTR( Slot< F > ) slot );

    template< typename F_IN >
    static SPTR( Slot< R( A1, A2, A3 ) > ) New( SPTR( SlotRun< F_IN > ) slot ); //{}

};
template<typename R, typename A1, typename A2 >
struct Slot< Slot< R ( A1, A2 ) > > : Slot< boost::function < R ( A1, A2 ) > >
{


    typedef R SignatureType ( A1, A2 ) ;
    typedef ::boost::function< SignatureType > FunctionType;

    template< typename F >
    Slot( SPTR( SlotRun< F > ) slot );
    template< typename F >
    Slot( SPTR( Slot< F > ) slot );

    template< typename F_IN >
    static SPTR( Slot< R( A1, A2 ) > ) New( SPTR( SlotRun< F_IN > ) slot ); //{}

};
template<typename R, typename A1 >
struct Slot< Slot< R ( A1 ) > > : Slot< boost::function < R ( A1 ) > >
{


    typedef R SignatureType ( A1 ) ;
    typedef ::boost::function< SignatureType > FunctionType;

    template< typename F >
    Slot( SPTR( SlotRun< F > ) slot );
    template< typename F >
    Slot( SPTR( Slot< F > ) slot );

    template< typename F_IN >
    static SPTR( Slot< R( A1 ) > ) New( SPTR( SlotRun< F_IN > ) slot ); //{}

};
template<typename R>
struct Slot< Slot< R () > > : Slot< boost::function < R () > >
{


    typedef R SignatureType () ;
    typedef ::boost::function< SignatureType > FunctionType;

    template< typename F >
    Slot( SPTR( SlotRun< F > ) slot );
    template< typename F >
    Slot( SPTR( Slot< F > ) slot );

    template< typename F_IN >
    static SPTR( Slot< R() > ) New( SPTR( SlotRun< F_IN > ) slot ); //{}

};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
struct Slot< Slot< R ( A... ) > > : Slot< boost::function < R ( A... ) > >
{


    typedef R SignatureType ( A... ) ;
    typedef ::boost::function< SignatureType > FunctionType;

    template< typename F >
    Slot( SPTR( SlotRun< F > ) slot );
    template< typename F >
    Slot( SPTR( Slot< F > ) slot );

    template< typename F_IN >
    static SPTR( Slot< R(A...) > ) New( SPTR( SlotRun< F_IN > ) slot ); //{}

};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
struct Slot< R ( A1, A2, A3 ) > : SlotCall< R ( A1, A2, A3 ) >
{
    typedef R SignatureType( A1, A2, A3 );
    typedef Slot< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;


    Slot() : SlotCall< R ( A1, A2, A3 ) >()
    {
        // 'this->' is needed by gcc 4.2
        this->SlotBase::m_signature = SlotBase::getTypeName< R ( A1, A2, A3 ) >();
    }
};
template<typename R, typename A1, typename A2 >
struct Slot< R ( A1, A2 ) > : SlotCall< R ( A1, A2 ) >
{
    typedef R SignatureType( A1, A2 );
    typedef Slot< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;


    Slot() : SlotCall< R ( A1, A2 ) >()
    {
        // 'this->' is needed by gcc 4.2
        this->SlotBase::m_signature = SlotBase::getTypeName< R ( A1, A2 ) >();
    }
};
template<typename R, typename A1 >
struct Slot< R ( A1 ) > : SlotCall< R ( A1 ) >
{
    typedef R SignatureType( A1 );
    typedef Slot< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;


    Slot() : SlotCall< R ( A1 ) >()
    {
        // 'this->' is needed by gcc 4.2
        this->SlotBase::m_signature = SlotBase::getTypeName< R ( A1 ) >();
    }
};
template<typename R>
struct Slot< R () > : SlotCall< R () >
{
    typedef R SignatureType();
    typedef Slot< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;


    Slot() : SlotCall< R () >()
    {
        // 'this->' is needed by gcc 4.2
        this->SlotBase::m_signature = SlotBase::getTypeName< R () >();
    }
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
struct Slot< R ( A... ) > : SlotCall< R (A...) >
{
    typedef R SignatureType(A...);
    typedef Slot< SignatureType > SelfType;
    typedef SPTR( SelfType ) sptr;
    typedef WPTR( SelfType ) wptr;


    Slot() : SlotCall< R (A...) >()
    {
        // 'this->' is needed by gcc 4.2
        this->SlotBase::m_signature = SlotBase::getTypeName< R ( A... ) >();
    }
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename F, typename Bindings1, typename Bindings2, typename Bindings3 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings1  bindings1, Bindings2  bindings2, Bindings3  bindings3 ); //{}




template<typename F, typename Bindings1, typename Bindings2 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings1  bindings1, Bindings2  bindings2 ); //{}




template<typename F, typename Bindings1 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings1  bindings1 ); //{}




template<typename F>
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f); //{}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename F, typename ...Bindings>
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings ...bindings); //{}




#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SLOT_HPP__ */

