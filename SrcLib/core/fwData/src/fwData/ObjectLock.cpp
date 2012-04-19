/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/BufferObject.hpp>

#include <fwData/Image.hpp>
#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>

#include "fwData/ObjectLock.hpp"

namespace fwData
{

ObjectLock::ObjectLock( ::fwData::Object::sptr obj )
{
    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast( obj );
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast( obj );
    ::fwData::Array::sptr array = ::fwData::Array::dynamicCast( obj );
    if( image )
    {
        m_locks.push_back( image->getDataArray()->getBufferObject()->lock() ) ;
    }
    else if ( mesh )
    {
        m_locks.push_back( mesh->getPointsArray()->getBufferObject()->lock() ) ;
        m_locks.push_back( mesh->getCellDataArray()->getBufferObject()->lock() ) ;
        m_locks.push_back( mesh->getCellDataOffsetsArray()->getBufferObject()->lock() ) ;
        m_locks.push_back( mesh->getCellTypesArray()->getBufferObject()->lock() ) ;

        if(mesh->getCellColorsArray())
        {
            m_locks.push_back( mesh->getCellColorsArray()->getBufferObject()->lock() ) ;
        }
        if(mesh->getPointColorsArray())
        {
            m_locks.push_back( mesh->getPointColorsArray()->getBufferObject()->lock() ) ;
        }
        if(mesh->getCellNormalsArray())
        {
            m_locks.push_back( mesh->getCellNormalsArray()->getBufferObject()->lock() ) ;
        }
        if(mesh->getPointNormalsArray())
        {
            m_locks.push_back( mesh->getPointNormalsArray()->getBufferObject()->lock() ) ;
        }
    }
    else if ( array )
    {
        m_locks.push_back( array->getBufferObject()->lock() ) ;
    }
}

} // fwData
