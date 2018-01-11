/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioITK/SJpgImageSeriesWriter.hpp"

#include "ioITK/JpgImageWriterService.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwServices/macros.hpp>

#include <io/IWriter.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::io::IWriter, ::ioITK::SJpgImageSeriesWriter, ::fwMedData::ImageSeries );

//------------------------------------------------------------------------------

SJpgImageSeriesWriter::SJpgImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

SJpgImageSeriesWriter::~SJpgImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

::io::IOPathType SJpgImageSeriesWriter::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::configuring()
{
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save image" : m_windowTitle);
    dialog.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialog.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialog.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;

    while (result = ::fwData::location::Folder::dynamicCast( dialog.show() ))
    {
        if( ::boost::filesystem::is_empty(result->getFolder()) )
        {
            break;
        }
        // message box
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
        if( messageBox.show() == ::fwGui::dialog::IMessageDialog::YES)
        {
            break;
        }
    }

    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialog.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFolder(result->getFolder());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::info(std::ostream& _sstream )
{
    _sstream << "SJpgImageSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SJpgImageSeriesWriter::updating()
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::ImageSeries::sptr imageSeries = this->getObject< ::fwMedData::ImageSeries >();
        SLM_ASSERT("Image series is not instanced", imageSeries);
        SLM_ASSERT("Image from image series is not instanced", imageSeries->getImage());

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        JpgImageWriterService::saveImage(this->getFolder(), imageSeries->getImage());
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK

