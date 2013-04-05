/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>

#include <fwAtomsBoostIO/Writer.hpp>

#include <fwAtomConversion/convert.hpp>

#include <fwData/Composite.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>

#include "ioAtom/SWriter.hpp"

namespace ioAtom
{

fwServicesRegisterMacro( ::io::IWriter , ::ioAtom::SWriter , ::fwData::Composite );

//-----------------------------------------------------------------------------

void SWriter::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_formatsMap[".json"] = ::fwAtomsBoostIO::Writer::JSON;
    m_formatsMap[".jf"] = ::fwAtomsBoostIO::Writer::JSON_Z;
    m_formatsMap[".xml"] = ::fwAtomsBoostIO::Writer::XML;
    m_formatsMap[".yaf"] = ::fwAtomsBoostIO::Writer::XML_Z;

    m_filters["XML"] = "*.xml";
    m_filters["JSON"] = "*.json";
    m_filters["Zipped JSON"] = "*.jf";
    m_filters["Zipped XML"] = "*.yaf";
}

//-----------------------------------------------------------------------------

void SWriter::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SWriter::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if(this->hasLocationDefined())
    {
        ::fwData::Composite::sptr data = this->getObject< ::fwData::Composite >();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        const ::boost::filesystem::path& file = this->getFile();
        const std::string ext = ::boost::filesystem::extension(file);

        OSLM_ASSERT("Extension is empty", !ext.empty());
        OSLM_ASSERT("Extension '" << ext << "' not found", m_formatsMap.find(ext) != m_formatsMap.end());

        ::fwAtomsBoostIO::Writer writer( ::fwAtomConversion::convert(data) );
        writer.write(file.parent_path(), m_formatsMap[ext]);

        cursor.setDefaultCursor(); 
    }
}

//-----------------------------------------------------------------------------

void SWriter::info(std::ostream& _stream)
{
    _stream << "SWriter::info";
}

//-----------------------------------------------------------------------------

::io::IOPathType SWriter::getIOPathType() const
{
    return ::io::FILE;
}

//-----------------------------------------------------------------------------

void SWriter::configureWithIHM()
{
   static ::boost::filesystem::path _sDefaultPath;

   ::fwGui::dialog::LocationDialog dialogFile;
   dialogFile.setTitle("Enter file name for medical workspace");
   dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
   dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
   dialogFile.setType(::fwGui::dialog::LocationDialog::SINGLE_FILE);

   FiltersType::const_iterator cIt = m_filters.begin();
   while(cIt != m_filters.end())
   {
       dialogFile.addFilter(cIt->first, cIt->second);
       ++cIt;
   }

   ::fwData::location::SingleFile::sptr result
       = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

   if (result)
   {
       _sDefaultPath = result->getPath();
       this->setFile( _sDefaultPath );
       dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath.parent_path()) );
   }
   else
   {
       this->clearLocations();
   }
}

//-----------------------------------------------------------------------------

} // namespace ioAtom

