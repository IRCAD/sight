/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWPYTHON_BINDINGS_STLCONTAINERS_HPP_
#define _FWPYTHON_BINDINGS_STLCONTAINERS_HPP_

namespace fwPython {
namespace bindings {

void export_STLContainers();

/// an helper to convert std container to python list
template< typename STDCONTAINER>
::boost::python::list make_pylist( STDCONTAINER &cont);

} //namespace bindings
} //namespace fwPython

#include "fwPython/bindings/STLContainers.hxx"

#endif // _FWPYTHON_BINDINGS_STLCONTAINERS_HPP_
