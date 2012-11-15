/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_WEAKCALL_HPP__
#define __FWCOM_UTIL_WEAKCALL_HPP__

namespace fwCom
{

namespace util
{

template< typename T, typename R >
struct WeakCall
{
    WeakCall( const ::boost::shared_ptr< T const > &ptr, ::boost::function< R() > f)
        : m_weakPtr(ptr), m_func(f)
    { }

    R operator ()() const
    {
        boost::shared_ptr< T const > ptr(this->m_weakPtr);

        return this->m_func();
    }

protected:
    ::boost::weak_ptr< T const > m_weakPtr;
    ::boost::function< R() > m_func;
};



template <typename T, typename R >
WeakCall<T, R> weakcall( const ::boost::shared_ptr< T const > &ptr, ::boost::function< R() > f)
{
    return WeakCall<T, R>(ptr, f);
}




} // namespace util



} // namespace fwCom

#endif /* __FWCOM_UTIL_WEAKCALL_HPP__ */




