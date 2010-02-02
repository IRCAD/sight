/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Model.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Model, ::fwData::Model );
namespace fwData
{
Model::Model()
{}

//------------------------------------------------------------------------------

Model::~Model() throw()
{
	m_map.clear();
}

FWDATA_API Model::Container &Model::getRefMap()
{
	return m_map ;
}


FWDATA_API const Model::Container &Model::getCRefMap() const
{
	return m_map ;
}


} // namespace fwData

