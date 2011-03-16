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
#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include "ioTuto/CompositeWriterService.hpp"

namespace ioTuto
{

REGISTER_SERVICE( ::io::IWriter , ioTuto::CompositeWriterService , ::fwData::Composite ) ;

//------------------------------------------------------------------------------

CompositeWriterService::CompositeWriterService( )throw()
        : m_fsExternalDataPath("")
{}

//------------------------------------------------------------------------------

CompositeWriterService::~CompositeWriterService() throw()
{}

//------------------------------------------------------------------------------

void CompositeWriterService::info(std::ostream &_sstream )
{}

//------------------------------------------------------------------------------

void CompositeWriterService::configuring() throw( ::fwTools::Failed )
{}

//------------------------------------------------------------------------------

void CompositeWriterService::starting() throw( ::fwTools::Failed )
{}

//------------------------------------------------------------------------------

void CompositeWriterService::stopping() throw( ::fwTools::Failed )
{}

//------------------------------------------------------------------------------

void CompositeWriterService::updating( ) throw(::fwTools::Failed)
{
    this->configureWithIHM();
    ::fwData::Composite::sptr pComposite = this->getObject< ::fwData::Composite >();
    SLM_ASSERT("pComposite not instanced", pComposite);

    ::fwXML::writer::FwXMLObjectWriter myWriter;

    if (m_fsExternalDataPath != "")
    {
        myWriter.setObject(pComposite);
        myWriter.setFile(m_fsExternalDataPath);

        try
        {
            myWriter.write();
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

void CompositeWriterService::updating(::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg) throw(::fwTools::Failed)
{}

//-----------------------------------------------------------------------------

void CompositeWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an xml file to read");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("xml", "*.xml");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

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
