/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"

#include "fwData/Model.hpp"

REGISTER_DATA( ::fwData::Model );
namespace fwData
{
Model::Model()
{}

//------------------------------------------------------------------------------

Model::~Model() throw()
{
    m_map.clear();
}

//------------------------------------------------------------------------------

Model::Container &Model::getRefMap()
{
    return m_map ;
}

//------------------------------------------------------------------------------

const Model::Container &Model::getCRefMap() const
{
    return m_map ;
}

//------------------------------------------------------------------------------

void Model::shallowCopy( Model::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );

    this->m_map.clear();
    this->m_map = _source->m_map;
}

//------------------------------------------------------------------------------

void Model::deepCopy( Model::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->m_map.clear();
    for(    Model::Container::const_iterator iter = _source->getCRefMap().begin();
            iter != _source->getCRefMap().end();
            ++iter )
    {
        ::fwData::TriangularMesh::NewSptr newTrian;
        ::fwData::Material::NewSptr newMaterial;

        newTrian->deepCopy( iter->first );
        newMaterial->deepCopy( iter->second );
        this->m_map[newTrian] = newMaterial;
    }
}

} // namespace fwData

