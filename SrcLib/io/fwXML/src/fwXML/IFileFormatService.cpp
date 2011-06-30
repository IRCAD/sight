/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/IFileFormatService.hpp"
#include "fwDataIO/reader/IObjectReader.hpp"
#include "fwDataIO/writer/IObjectWriter.hpp"
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>


namespace fwXML
{

IFileFormatService::IFileFormatService()
{}

//------------------------------------------------------------------------------

IFileFormatService::~IFileFormatService()
{}

//------------------------------------------------------------------------------

std::string IFileFormatService::getPersistanceId()
{
    return std::string("ioIFileFormatService") ;
}

//------------------------------------------------------------------------------

void IFileFormatService::info(std::ostream &_sstream )
{
    _sstream <<  "FileFormat IO Service : " << (  rootFolder()/localFolder()/filename() ).string();
}

//------------------------------------------------------------------------------

void IFileFormatService::setReader( ::fwDataIO::reader::IObjectReader::sptr _reader)
{
    m_reader = _reader;
    ::fwTools::ProgressAdviser * progadviser;

    progadviser = dynamic_cast< ::fwTools::ProgressAdviser *>( m_reader.get() );

    using namespace ::boost::lambda;

    if ( progadviser  )
    {
        // forward event progress to its parents
        ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &::fwTools::ProgressAdviser::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
        progadviser->addHandler( handler  );
    }
}

//------------------------------------------------------------------------------

void IFileFormatService::setWriter( ::fwDataIO::writer::IObjectWriter::sptr _writer)
{
    m_writer = _writer;
    ::fwTools::ProgressAdviser *progadviser;

    progadviser = dynamic_cast< ::fwTools::ProgressAdviser *>( m_writer.get() );

    using namespace ::boost::lambda;

    if ( progadviser  )
    {
        // forward event progress to its parents
        ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &::fwTools::ProgressAdviser::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
        progadviser->addHandler( handler  );
    }
}

//------------------------------------------------------------------------------

::fwDataIO::reader::IObjectReader::sptr IFileFormatService::getReader()
{
    return m_reader;
}

//------------------------------------------------------------------------------

::fwDataIO::writer::IObjectWriter::sptr IFileFormatService::getWriter()
{
    return m_writer;
}

//------------------------------------------------------------------------------

}
