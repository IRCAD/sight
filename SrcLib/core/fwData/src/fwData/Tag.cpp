/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/Tag.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Tag, ::fwData::Tag );
namespace fwData
{
//------------------------------------------------------------------------------

Tag::Tag () :
m_size(0.5)
{
	SLM_TRACE("::fwData::Tag()");
}

//------------------------------------------------------------------------------

Tag::~Tag ()
{
	m_pointList->getRefPoints().clear();
	SLM_TRACE("::fwData::~Tag()");
}

//------------------------------------------------------------------------------

} // namespace fwData


