/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPYTHON_BINDINGS_STLCONTAINERS_HXX__
#define __FWPYTHON_BINDINGS_STLCONTAINERS_HXX__

#include <boost/python.hpp>
#include <boost/python/tuple.hpp>

#undef tolower //defined by python, conflicting with std::lower

namespace fwPython
{
namespace bindings
{

template< typename STDCONTAINER>
::boost::python::list make_pylist( STDCONTAINER& cont)
{
    ::boost::python::list list;
    typename STDCONTAINER::const_iterator iter;
    for ( iter = cont.begin(); iter != cont.end(); ++iter )
    {
        list.append(  ::boost::python::object(*iter) );
    }
    return list;
}

} //namespace bindings
} //namespace fwPython

#endif // __FWPYTHON_BINDINGS_STLCONTAINERS_HXX__

