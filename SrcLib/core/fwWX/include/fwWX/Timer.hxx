/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWWX_TIMER_HXX_
#define FWWX_TIMER_HXX_


namespace fwWX
{

template< typename F >
Timer<F>::Timer() throw()
{
    m_isOneShot = false;
    m_rate = 1000/50.;
//    not compatible with templates
//    m_function = ::boost::bind(&Timer<F>::doNothing, this);
    m_wxTimer = new wxTimer( this );
}

template< typename F >
Timer<F>::~Timer() throw()
{
	if( m_wxTimer )
	{
	    delete m_wxTimer ;
	    m_wxTimer = 0 ;
	}
}

template< typename F >
void Timer<F>::setFunction( function _function )
{
    m_function = _function;
}

template< typename F >
void Timer<F>::setOneShot( bool isOneShot ){
    m_isOneShot = isOneShot;
}

template< typename F >
void Timer<F>::setRate( int rate ){
    m_rate = rate;
}

template< typename F >
void Timer<F>::start(int rate)
{
    this->setRate(rate);
	this->start();
}

template< typename F >
void Timer<F>::start(int rate, bool isOneShot)
{
    this->setRate(rate);
    this->setOneShot(isOneShot);
	this->start();
}

template< typename F >
void Timer<F>::start()
{
    assert(m_function);
	m_wxTimer->Start( m_rate , m_isOneShot) ;
}

template< typename F >
void Timer<F>::stop()
{
	m_wxTimer->Stop() ;
}

template< typename F >
void Timer<F>::OnTimer(wxTimerEvent& event)
{
	m_function() ;
}

BEGIN_EVENT_TABLE_TEMPLATE1(Timer, wxEvtHandler, F)
	EVT_TIMER(-1, Timer::OnTimer)
END_EVENT_TABLE()

}


#endif // FWWX_TIMER_HXX_
