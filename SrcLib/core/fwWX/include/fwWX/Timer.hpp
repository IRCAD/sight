/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_TIMER_HPP_
#define FWWX_TIMER_HPP_

//#include <boost/preprocessor/repetition/enum_params.hpp>
//#include <boost/preprocessor/repetition/repeat.hpp>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <wx/timer.h>
#include <wx/event.h>

#include "fwWX/config.hpp"

namespace fwWX
{

template< typename F = ::boost::function0<void> >

/**
 * @brief	This class allows us to use a timer.
 * @class	Timer.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 * @todo Timer is not commented.
 */
struct Timer : public wxEvtHandler
{

	typedef F function;
    typedef ::boost::shared_ptr< Timer<F> > ptr;

	 Timer() throw() ;
	 ~Timer() throw() ;

	 void test() throw() ;

	 void setFunction( function _function );
     void setOneShot( bool isOneShot );
     void setRate( int rate );

	// Start clocking
	 void start() ;
	 void start(int rate) ;
	 void start(int rate, bool isOneShot) ;

	// Stop clocking
	 void stop() ;

//#define DO_NOTHING(z, N, data) void doNothing ( BOOST_PP_ENUM_PARAMS (N, void* notUsed) ) {} ;
//    BOOST_PP_REPEAT(10, DO_NOTHING, )
//#undef DO_NOTHING

private:

	/*
	 * @brief	declares the wxWidgets event table
	 */
	DECLARE_EVENT_TABLE()

    int m_rate;
    bool m_isOneShot;

	wxTimer *m_wxTimer;
    function m_function;

	void OnTimer(wxTimerEvent& event) ;

};

}

#include "fwWX/Timer.hxx"

#endif /*FWWX_TIMER_HPP_*/
