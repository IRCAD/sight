/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "SSurfaceSegmentationWriter.hpp"

#include <core/tools/ProgressToLogger.hpp>

#include <data/DicomSeries.hpp>
#include <data/location/Folder.hpp>
#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <service/macros.hpp>

#include <io/base/service/IWriter.hpp>
#include <io/dicom/helper/Fiducial.hpp>
#include <io/dicom/writer/Series.hpp>
#include <io/dicom/writer/SurfaceSegmentation.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

#include <string>

namespace sight::module::io::dicom
{


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
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::LocationDialog::FOLDER);

    data::location::Folder::sptr result;
    result = data::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
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
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::updating()
{
    if( this->hasLocationDefined() )
    {
        const std::filesystem::path& folder = this->getFolder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::base::dialog::MessageDialog dialog;
            dialog.setMessage("Folder '"+folder.string()+"' isn't empty, files can be overwritten."
                              "\nDo you want to continue ?");
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(ui::base::dialog::MessageDialog::QUESTION);
            dialog.addButton( sight::ui::base::dialog::MessageDialog::YES_NO );
            sight::ui::base::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == sight::ui::base::dialog::MessageDialog::NO)
            {
                m_writeFailed = true;
                return;
            }
        }
        else
        {
            m_writeFailed = true;
        }

        // Retrieve dataStruct associated with this service
        data::ModelSeries::csptr model = this->getInput< data::ModelSeries >(sight::io::base::service::s_DATA_KEY);

        if(!model->getDicomReference())
        {
            m_writeFailed = true;
            sight::ui::base::dialog::MessageDialog::show(
                "Warning", "DICOM image reference is missing, DICOM Surface Segmentation cannot be generated",
                sight::ui::base::dialog::IMessageDialog::WARNING);
            return;
        }

        /* Build up the filename */
        std::filesystem::path outputPath = this->getFolder() / "imSeg";

        /* Write the data */
        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
        saveSurfaceSegmentation( outputPath, model );
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

void SSurfaceSegmentationWriter::saveSurfaceSegmentation( const std::filesystem::path filename,
                                                          const data::ModelSeries::csptr& model)
{
    auto writer = sight::io::dicom::writer::SurfaceSegmentation::New();

    writer->setObject(model);

    data::location::SingleFile::sptr location = data::location::SingleFile::New();
    location->setPath(filename);
    writer->setLocation(location);

    try
    {
        writer->write();
    }
    catch (const std::exception& e)
    {
        m_writeFailed = true;
        std::stringstream ss;
        ss << "Warning during saving: " << e.what();
        sight::ui::base::dialog::MessageDialog::show(
            "Warning", ss.str(), sight::ui::base::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        m_writeFailed = true;
        sight::ui::base::dialog::MessageDialog::show(
            "Warning", "Warning during saving", sight::ui::base::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

sight::io::base::service::IOPathType SSurfaceSegmentationWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

} // namespace sight::module::io::dicom
