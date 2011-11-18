/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Array.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwTools/Failed.hpp>

#include <fwServices/macros.hpp>

#include <fwDataIO/writer/GzArrayWriter.hpp>
#include <fwDataIO/reader/GzArrayReader.hpp>

#include "fwXML/ArrayFileFormatService.hpp"

REGISTER_SERVICE( ::fwXML::IFileFormatService , ::fwXML::ArrayFileFormatService , ::fwData::Array);

namespace fwXML
{


//------------------------------------------------------------------------------

ArrayFileFormatService::ArrayFileFormatService()
{
    m_reader = ::fwDataIO::reader::GzArrayReader::New();
    m_writer = ::fwDataIO::writer::GzArrayWriter::New();
}

//------------------------------------------------------------------------------

ArrayFileFormatService::~ArrayFileFormatService()
{}

//------------------------------------------------------------------------------

void ArrayFileFormatService::load()
{
    SLM_ASSERT("Filename is empty", !m_filename.empty() );
    SLM_ASSERT("Reader not instanced", m_reader);

    ::fwData::Array::sptr array = this->getObject< ::fwData::Array >() ;
    SLM_ASSERT("array not instanced", array);

    m_reader->setObject(array);
    ::fwData::location::SingleFile::NewSptr path;
    path->setPath( this->getFullPath() );
    m_reader->setLocation( path );
    m_reader->read();
}

//------------------------------------------------------------------------------

void ArrayFileFormatService::save()
{
    SLM_ASSERT("Filename is empty", !m_filename.empty() );
    SLM_ASSERT("Writer not instanced", m_writer);

    ::fwData::Array::sptr array = this->getObject< ::fwData::Array >() ;
    SLM_ASSERT("array not instanced", array);

    if (array->getBuffer() )
    {
        m_writer->setObject(array);
        this->extension() = m_writer->extension();
        ::fwData::location::SingleFile::NewSptr path;
        path->setPath( this->getFullPath() );
        m_writer->setLocation( path );
        m_writer->write();
    }
}

//------------------------------------------------------------------------------

}
