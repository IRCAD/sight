/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwData/Object.hpp>

#include "fwXML/writer/FwXMLObjectWriter.hpp"
#include "fwXML/Serializer.hpp"
#include "fwXML/policy/NeverSplitPolicy.hpp"
#include "fwXML/policy/UniquePathPolicy.hpp"


#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwTools/ClassRegistrar.hpp>


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::fwXML::writer::FwXMLObjectWriter,  ::fwXML::writer::FwXMLObjectWriter);


namespace fwXML
{

namespace writer
{

//------------------------------------------------------------------------------

FwXMLObjectWriter::FwXMLObjectWriter(bool saveSchema)
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_saveSchema(saveSchema)
{
}

//------------------------------------------------------------------------------

FwXMLObjectWriter::~FwXMLObjectWriter()
{
}

//------------------------------------------------------------------------------

void FwXMLObjectWriter::write()
{
    assert(getObject());
    assert( ! getFile().string().empty() );

    ::fwXML::Serializer serializer;
    serializer.rootFolder() = getFile().parent_path().string();

    ::boost::shared_ptr< ::fwXML::NeverSplitPolicy > spolicy ( new ::fwXML::NeverSplitPolicy );
    serializer.setSplitPolicy( spolicy );

    ::boost::shared_ptr< ::fwXML::UniquePathPolicy > pPathPolicy ( new ::fwXML::UniquePathPolicy( getFile().leaf() ) );
    serializer.setPathPolicy( pPathPolicy );

    // forward event progress to its parents
    ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &FwXMLObjectWriter::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
    serializer.addHandler ( handler );

    //do the serialization job
    serializer.serialize( getObject(), m_saveSchema );
}

//------------------------------------------------------------------------------

std::string  FwXMLObjectWriter::extension()
{
    return ".fxz";
}
} // namespace writer

} // namespace fwXML
