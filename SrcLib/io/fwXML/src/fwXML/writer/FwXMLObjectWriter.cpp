/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwData/Object.hpp>

#include "fwXML/writer/FwXMLObjectWriter.hpp"
#include "fwXML/writer/fwxmlextension.hpp"
#include "fwXML/Serializer.hpp"
#include "fwXML/policy/NeverSplitPolicy.hpp"
#include "fwXML/policy/UniquePathPolicy.hpp"


#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

fwDataIOWriterRegisterMacro( ::fwXML::writer::FwXMLObjectWriter);

namespace fwXML
{

namespace writer
{

//------------------------------------------------------------------------------

FwXMLObjectWriter::FwXMLObjectWriter(::fwDataIO::writer::IObjectWriter::Key key)
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
  m_saveSchema(true)
{
}

//------------------------------------------------------------------------------

FwXMLObjectWriter::~FwXMLObjectWriter()
{
}

//------------------------------------------------------------------------------

FwXMLObjectWriter::sptr FwXMLObjectWriter::ObjectWriterFactory(bool saveSchema)
{
    FwXMLObjectWriter::sptr writer = ::fwXML::writer::FwXMLObjectWriter::New();
    writer->m_saveSchema = saveSchema;
    return writer;
}

//------------------------------------------------------------------------------

void FwXMLObjectWriter::write()
{
    assert(getObject());
    assert( ! getFile().string().empty() );

    ::fwXML::Serializer serializer;
    serializer.rootFolder() = getFile().parent_path().string();

    ::fwXML::NeverSplitPolicy::NewSptr spolicy;
    serializer.setSplitPolicy( spolicy );
#if BOOST_FILESYSTEM_VERSION > 2
    ::fwXML::UniquePathPolicy::sptr pPathPolicy( new ::fwXML::UniquePathPolicy( getFile().filename().string() ) );
#else
    ::fwXML::UniquePathPolicy::sptr pPathPolicy( new ::fwXML::UniquePathPolicy( getFile().leaf() ) );
#endif
    serializer.setPathPolicy( pPathPolicy );

    // forward event progress to its parents
    ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &FwXMLObjectWriter::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
    serializer.addHandler ( handler );

    //do the serialization job
    serializer.serialize( ::fwData::Object::dynamicCast( this->getObject() ), m_saveSchema );
}

//------------------------------------------------------------------------------

std::string  FwXMLObjectWriter::extension()
{
    return "." FWXML_ARCHIVE_EXTENSION;
}
} // namespace writer

} // namespace fwXML
