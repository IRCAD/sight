/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioGdcm/SSurfaceSegmentationWriter.hpp"

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/Vector.hpp>

#include <fwGdcmIO/helper/Fiducial.hpp>
#include <fwGdcmIO/writer/Series.hpp>
#include <fwGdcmIO/writer/SurfaceSegmentation.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/DicomSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <io/IWriter.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro( ::io::IWriter, ::ioGdcm::SSurfaceSegmentationWriter, ::fwData::Vector );

//------------------------------------------------------------------------------

SSurfaceSegmentationWriter::SSurfaceSegmentationWriter() noexcept
{
}

//------------------------------------------------------------------------------

SSurfaceSegmentationWriter::~SSurfaceSegmentationWriter() noexcept
{
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::starting()
{
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::configuring()
{
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::updating()
{
    if( this->hasLocationDefined() )
    {
        const ::boost::filesystem::path& folder = this->getFolder();
        if(!::boost::filesystem::is_empty(folder))
        {
            ::fwGui::dialog::MessageDialog dialog;
            dialog.setMessage("Folder '"+folder.string()+"' isn't empty, files can be overwritten."
                              "\nDo you want to continue ?");
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(::fwGui::dialog::MessageDialog::QUESTION);
            dialog.addButton( ::fwGui::dialog::MessageDialog::YES_NO );
            ::fwGui::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == ::fwGui::dialog::MessageDialog::NO)
            {
                return;
            }
        }

        // Retrieve dataStruct associated with this service
        ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

        if(vector->getContainer().size() > 2)
        {
            SLM_FATAL("Writing with more than one DICOM Series is not supported as of now.");
        }

        ::fwMedData::DicomSeries::sptr dicom = nullptr;
        ::fwMedData::ModelSeries::sptr model = nullptr;

        /* Look for a DicomSeries and a ModelSeries in the input vector */
        for(size_t i = 0; i < vector->getContainer().size(); i++)
        {
            if(!dicom && vector->getContainer().at(i)->getClassname() == "::fwMedData::DicomSeries")
            {
                dicom = ::fwMedData::DicomSeries::dynamicCast(vector->getContainer().at(i));
            }

            if(!model && vector->getContainer().at(i)->getClassname() == "::fwMedData::ModelSeries")
            {
                model = ::fwMedData::ModelSeries::dynamicCast(vector->getContainer().at(i));
            }
        }

        if(!model || !dicom)
        {
            SLM_ERROR("Input data must be DicomSeries and ModelSeries");
            return;
        }

        /* Build up the filename */
        std::ostringstream oss;
        oss << this->getFolder().string() << "/imSeg";

        /* Write the data */
        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        saveSurfaceSegmentation( oss.str(), dicom, model );
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::saveSurfaceSegmentation( const ::boost::filesystem::path filename,
                                                          ::fwMedData::DicomSeries::sptr dicom,
                                                          ::fwMedData::ModelSeries::sptr model)
{
    ::fwGdcmIO::writer::SurfaceSegmentation::sptr writer = ::fwGdcmIO::writer::SurfaceSegmentation::New();
    writer->setObject(model);
    writer->setAssociatedDicomSeries(dicom);

    ::fwData::location::SingleFile::sptr location = ::fwData::location::SingleFile::New();
    location->setPath(filename);
    writer->setLocation(location);

    try
    {
        writer->write();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                 "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                 "Warning", "Warning during saving", ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

::io::IOPathType SSurfaceSegmentationWriter::getIOPathType() const
{
    return ::io::FOLDER;
}

} // namespace ioGdcm
