/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"

#include "fwData/Model.hpp"

fwDataRegisterMacro( ::fwData::Model );
namespace fwData
{
Model::Model(::fwData::Object::Key key)
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

void Model::shallowCopy(const Object::csptr &_source )
{
    Model::csptr other = Model::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_map.clear();
    m_map = other->m_map;
}

//------------------------------------------------------------------------------

void Model::deepCopy(const Object::csptr &_source )
{
    Model::csptr other = Model::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
            "Unable to copy" + (_source?_source->getClassname():std::string("<NULL>"))
            + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source );

    m_map.clear();
    for(    Model::Container::const_iterator iter = other->getCRefMap().begin();
            iter != other->getCRefMap().end();
            ++iter )
    {
        ::fwData::TriangularMesh::NewSptr newTrian;
        ::fwData::Material::NewSptr newMaterial;

        newTrian->deepCopy( iter->first );
        newMaterial->deepCopy( iter->second );
        m_map[newTrian] = newMaterial;
    }
}

} // namespace fwData

