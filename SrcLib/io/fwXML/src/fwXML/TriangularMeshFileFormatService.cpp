/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/TriangularMeshFileFormatService.hpp"

#include <fwData/TriangularMesh.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwCore/base.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/macros.hpp>

#include <fwDataIO/reader/TriangularMeshReader.hpp>
#include <fwDataIO/writer/TriangularMeshWriter.hpp>


REGISTER_SERVICE( ::fwXML::IFileFormatService , ::fwXML::TriangularMeshFileFormatService , ::fwData::TriangularMesh);

namespace fwXML
{

//------------------------------------------------------------------------------

TriangularMeshFileFormatService::TriangularMeshFileFormatService()
{
    RWPoliciesInstall();
}

//------------------------------------------------------------------------------

TriangularMeshFileFormatService::~TriangularMeshFileFormatService()
{}

//------------------------------------------------------------------------------

void TriangularMeshFileFormatService::RWPoliciesInstall()
{
    assert(  m_reader.get() == 0 );
    assert(  m_writer.get() == 0 );

    m_reader = ::boost::shared_ptr< ::fwDataIO::reader::TriangularMeshReader >( new ::fwDataIO::reader::TriangularMeshReader() );
    m_writer = ::boost::shared_ptr< ::fwDataIO::writer::TriangularMeshWriter >( new ::fwDataIO::writer::TriangularMeshWriter() );
}

//------------------------------------------------------------------------------

void TriangularMeshFileFormatService::load()
{
    SLM_TRACE("TriangularMeshFileFormatService::load");
    // precondition

    assert( !m_filename.empty() );
    assert( !m_rootFolder.empty() );

    SLM_ASSERT("m_reader not instanced", m_reader);

    ::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh >() ;
    SLM_ASSERT("mesh not instanced", mesh);

    m_reader->setObject(mesh);
    ::fwData::location::SingleFile::NewSptr path;
    path->setPath( this->getFullPath() );
    m_reader->setLocation( path );
    m_reader->read();
}

//------------------------------------------------------------------------------

void TriangularMeshFileFormatService::save()
{
    SLM_TRACE_FUNC();

    assert( !m_filename.empty() );
    assert( !m_rootFolder.empty() );

    SLM_ASSERT("m_writer not instanced", m_writer);

    ::fwData::TriangularMesh::sptr mesh = this->getObject< ::fwData::TriangularMesh >() ;
    SLM_ASSERT("mesh not instanced", mesh);

//    assert(mesh->points().size()); mesh points can be empty

    m_writer->setObject(mesh);
    this->extension() = m_writer->extension();
    ::fwData::location::SingleFile::NewSptr path;
    path->setPath( this->getFullPath() );
    m_writer->setLocation( path );
    m_writer->write();
}

//------------------------------------------------------------------------------

}
