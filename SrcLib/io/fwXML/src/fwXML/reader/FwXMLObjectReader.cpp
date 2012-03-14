/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Failed.hpp>

#include <fwData/Object.hpp>
#include <fwData/Image.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include "fwXML/reader/FwXMLObjectReader.hpp"
#include "fwXML/writer/fwxmlextension.hpp"
#include "fwXML/Serializer.hpp"

#include "fwXML/IFileFormatService.hpp"

namespace fwXML
{

namespace reader
{

//------------------------------------------------------------------------------

FwXMLObjectReader::FwXMLObjectReader()  : ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

FwXMLObjectReader::~FwXMLObjectReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLObjectReader::read()
{
    ::boost::filesystem::path file = this->getFile();
    if(!::boost::filesystem::exists( file ) )
    {
        throw ::fwTools::Failed(file.string() + " doesn't exist.");
    }
    if( !::boost::filesystem::is_regular_file( file ))
    {
        throw ::fwTools::Failed(file.string() + " is not regular a regular file.");
    }
    if( ::boost::filesystem::file_size( file ) <= 0)
    {
        throw ::fwTools::Failed(file.string() + " is empty.");
    }

    ::fwXML::Serializer serializer;
    // forward event progress to its parents
    ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &FwXMLObjectReader::notifyProgress,this, ::boost::lambda::_1, ::boost::lambda::_2);
    serializer.addHandler ( handler );

    ::fwData::Object::sptr object = serializer.deSerialize( file, true, true );
    OSLM_ASSERT("DeSerialization failed for file "<<file.string(), object);
    m_object = object;
    m_pObject = object; //FIXME hack to be FIXED in #739
}

//------------------------------------------------------------------------------

std::string  FwXMLObjectReader::extension()
{
    return "." FWXML_ARCHIVE_EXTENSION;
}

} // namespace reader

} // namespace fwXML
