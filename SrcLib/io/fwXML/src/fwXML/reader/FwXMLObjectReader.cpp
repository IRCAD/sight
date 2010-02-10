/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwCore/base.hpp>
#include <fwData/Object.hpp>
#include <fwData/Image.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "fwXML/reader/FwXMLObjectReader.hpp"
#include "fwXML/Serializer.hpp"

#include "fwXML/IFileFormatService.hpp"

namespace fwXML
{

namespace reader
{

//------------------------------------------------------------------------------

FwXMLObjectReader::FwXMLObjectReader()  : ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE("FwXMLObjectReader::FwXMLObjectReader");
}

//------------------------------------------------------------------------------

FwXMLObjectReader::~FwXMLObjectReader()
{
    SLM_TRACE("FwXMLObjectReader::~FwXMLObjectReader");
}

//------------------------------------------------------------------------------

void FwXMLObjectReader::read()
{
    ::boost::filesystem::path file = this->getFile();
    assert( ::boost::filesystem::exists( file ) );
    ::fwXML::Serializer serializer;

    // forward event progress to its parents
    ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &FwXMLObjectReader::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
    serializer.addHandler ( handler );

    ::boost::shared_ptr< ::fwTools::Object > object = serializer.deSerialize( file, true, true );
    assert(object);
    m_object = object;
    m_pObject = object; //FIXME hackk to be FIXED in #739
}




//------------------------------------------------------------------------------

std::string  FwXMLObjectReader::extension()
{
    return ".fxz";
}
} // namespace reader

} // namespace fwXML
