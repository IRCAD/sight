/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/SRasterImageReader.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Object.hpp>
#include <fwData/ObjectLock.hpp>

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmartPointer.h>

fwServicesRegisterMacro( ::io::IReader, ::ioVTK::SRasterImageReader );

namespace ioVTK
{
// -----------------------------------------------------------------------------

SRasterImageReader::SRasterImageReader() noexcept
{
}

// -----------------------------------------------------------------------------

SRasterImageReader::~SRasterImageReader() noexcept
{
}

// -----------------------------------------------------------------------------

::io::IOPathType SRasterImageReader::getIOPathType() const
{
    return ::io::FILE;
}

// -----------------------------------------------------------------------------

void SRasterImageReader::configuring()
{
    ::io::IReader::configuring();
}

// -----------------------------------------------------------------------------

void SRasterImageReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an image" : m_windowTitle);

    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath)  );
    dialogFile.addFilter("Image file", "*.jpg *.jpeg *.bmp *.png *.tiff");

    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show()  );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath)  );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

// -----------------------------------------------------------------------------

void SRasterImageReader::starting()
{
}

// -----------------------------------------------------------------------------

void SRasterImageReader::stopping()
{
}

// -----------------------------------------------------------------------------

void SRasterImageReader::updating()
{
    ::fwData::Image::sptr pImage = this->getInOut< ::fwData::Image >("frame");
    SLM_ASSERT("pImage not instanced", pImage);

    if( this->hasLocationDefined() )
    {
        // Read new image path and update image. If the reading process is a success, we notify all listeners that image
        // has been modified.
        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        try
        {
            this->loadImage(this->getFile(), pImage);
        }
        catch(::fwTools::Failed& e)
        {
            OSLM_TRACE("Error : " << e.what());
            FW_RAISE_EXCEPTION(e);
        }

        cursor.setDefaultCursor();
    }
    /* When no file name has been specified, we default to a tranparent 1x1 image */
    else
    {
        this->generateDummyImage(pImage);
    }
}

// -----------------------------------------------------------------------------

void SRasterImageReader::generateDummyImage(::fwData::Image::sptr _pImg)
{
    ::fwData::Image::SizeType size(3);
    size[0] = 1;
    size[1] = 1;
    size[2] = 1;

    /* Initialize image parameters */
    const ::fwData::Image::SpacingType::value_type voxelSize = 1;
    _pImg->allocate(size, ::fwTools::Type("uint8"), 4);
    ::fwData::Image::OriginType origin(3, 0);

    _pImg->setOrigin(origin);
    ::fwData::Image::SpacingType spacing(3, 1);
    _pImg->setSpacing(spacing);
    _pImg->setWindowWidth(1);
    _pImg->setWindowCenter(0);

    /* Set the image to black transparent */
    ::fwDataTools::helper::Image imageHelper(_pImg);
    void* destBuffer = imageHelper.getBuffer();
    std::memset(destBuffer, 0, size[0] * size[1] * size[2] * _pImg->getNumberOfComponents());

    // Notify (needed for instance to update the texture in ::visuVTKARAdaptor::SVideoAdapter)
    auto sig =
        _pImg->signal< ::fwData::Object::ModifiedSignalType >( ::fwData::Object::s_MODIFIED_SIG );

    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// -----------------------------------------------------------------------------

void SRasterImageReader::loadImage(const ::boost::filesystem::path imgFile, ::fwData::Image::sptr _pImg)
{
    ::fwData::mt::ObjectWriteLock destLock(_pImg);
    ::fwData::Array::sptr array = _pImg->getDataArray();

    ::fwDataTools::helper::Array arrayHelper(array);

    // Use a vtkImageReader2Factory to automatically detect the type of the input file
    // And select the right reader for the file
    vtkSmartPointer<vtkImageReader2Factory> imageFactory = vtkSmartPointer< vtkImageReader2Factory >::New();
    vtkSmartPointer<vtkImageReader2> imageReader         = imageFactory->CreateImageReader2( imgFile.string().c_str() );

    // If we manage to instance a vtkImageReader2 class (the image file is correct)
    if(imageReader)
    {
        imageReader->SetFileName( imgFile.string().c_str() );
        imageReader->Update();

        vtkImageData* imageData = imageReader->GetOutput();

        /* we ensure that we have a 2D image */
        if(imageData->GetDataDimension() == 2)
        {
            ::fwVtkIO::fromVTKImage( imageData, _pImg );

            // Notify
            auto sig =
                _pImg->signal< ::fwData::Image::BufferModifiedSignalType >( ::fwData::Image::s_BUFFER_MODIFIED_SIG );
            sig->asyncEmit();

            return;
        }
    }

    /* If we reach this point, it means that either we were not able to read the image or the image is not in the right
       format */
    /* If the read image is not valid, we ensure that we have an empty image available */
    /* e.g. the file is not 2D or the filename specified in the xml is not valid */
    OSLM_DEBUG( "Could not load input image: " << imgFile.string().c_str());

    this->generateDummyImage(_pImg);
}

// -----------------------------------------------------------------------------

} // namespace ioVTK
