/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/location/Folder.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>
#include <fwServices/macros.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwXML/reader/FwXMLObjectReader.hpp>

#include "ioTuto/CompositeReaderService.hpp"


REGISTER_SERVICE( ::io::IReader , ::ioTuto::CompositeReaderService , ::fwData::Composite ) ;

namespace ioTuto
{
//------------------------------------------------------------------------------
CompositeReaderService::CompositeReaderService( ) throw()
: m_fsExternalDataPath("")
{}

//------------------------------------------------------------------------------
CompositeReaderService::~CompositeReaderService() throw()
{}

//------------------------------------------------------------------------------
void CompositeReaderService::info(std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void CompositeReaderService::configuring() throw( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------
void CompositeReaderService::starting() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------
void CompositeReaderService::stopping() throw( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------
void CompositeReaderService::updating( ) throw(::fwTools::Failed)
{
    this->configureWithIHM();
    ::fwData::Composite::sptr pComposite = this->getObject< ::fwData::Composite >();
    assert( pComposite );

    ::fwXML::reader::FwXMLObjectReader myReader;

    myReader.setFile(m_fsExternalDataPath);
    if (m_fsExternalDataPath != "")
    {
        try
        {
            myReader.read();
            ::fwData::Composite::sptr newCompo;
            newCompo = ::fwData::Composite::dynamicCast( myReader.getObject() );
            pComposite->shallowCopy(newCompo);
            std::copy( newCompo->children().begin(),  newCompo->children().end(), std::back_inserter( pComposite->children() ) );

        }
        catch (const std::exception & e)
        {
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();

            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
        catch( ... )
        {
            std::stringstream ss;
            ss << "Warning during loading.";

            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Warning");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.show();
        }
    }
}

//------------------------------------------------------------------------------
void CompositeReaderService::updating( ::fwServices::ObjectMsg::csptr _msg) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void CompositeReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an xml file to read");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("xml", "*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        m_fsExternalDataPath = result->getPath();
    }
}

//------------------------------------------------------------------------------

} // namespace ioTuto
