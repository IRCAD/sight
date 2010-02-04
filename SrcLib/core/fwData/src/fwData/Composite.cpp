/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>


#include "fwData/Composite.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Composite,  ::fwData::Composite);

namespace fwData
{


Composite::Composite()
{
	SLM_TRACE_FUNC();
	// TODO Auto-generated constructor stub
}


Composite::~Composite()
{
	SLM_TRACE_FUNC();
	// TODO Auto-generated destructor stub
}


Composite::Container &Composite::getRefMap()
{
	return m_map;
}


Composite::Container const &Composite::getRefMap() const
{
	return m_map;
}

//------------------------------------------------------------------------------

Composite & Composite::operator=( const Composite & _composite )
{
   //Copy encoding
    m_map = _composite.m_map;

    return *this;
}

}
