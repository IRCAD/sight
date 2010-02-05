/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

#include <boost/shared_ptr.hpp>

namespace fwTools {
/**
 * @brief       Stereotype class to define a singleton
 * @class       Singleton
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 *
 * @li To make <tt>MyClass</tt> as a singleton simply inherits it from <tt>Singleton<Myclass></tt>
 * @li <b>iff</b> MyClass default constructor is protected make Singleton<Myclass> a friend class of MyClass
 *
 * example to make MyClass become a singleton :
 * @code#include <fwTools/Singleton.hpp>
 *
 * class MyClass : singleton<MyClass>
 * {
 *              friend class singleton<MyClass>;
 *
 *              protected :
 *                      MyClass();
 * };
 * @endcode
 *
 **/
template<class T>
class Singleton
{
public:

        /// retrieve the default instance of the Singleton
        static  boost::shared_ptr< T > getDefault()
        {
                if (m_ClassInstance == 0)
                {
                        m_ClassInstance = boost::shared_ptr< T >( new T );
                }
                return m_ClassInstance;
        }

        /// Default Destructor. Do nothing.
        virtual ~Singleton() {};

protected :
        /// Default constructor. Do nothing.
        Singleton() {};



        /// the instance
        static boost::shared_ptr< T > m_ClassInstance;
};


// define static
template <typename T>
boost::shared_ptr< T > Singleton<T>::m_ClassInstance = boost::shared_ptr< T >();

} //end namespace fwTools {

#endif // SINGLETON_HPP_

