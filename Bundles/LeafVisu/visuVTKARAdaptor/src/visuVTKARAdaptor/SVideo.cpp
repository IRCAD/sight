/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKARAdaptor/SVideo.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/TransferFunction.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkCamera.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapper3D.h>
#include <vtkImageMapToColors.h>
#include <vtkMatrix4x4.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKARAdaptor::SVideo, ::fwData::Image );

namespace visuVTKARAdaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_IN           = "frame";
static const ::fwServices::IService::KeyType s_CAMERA_IN          = "camera";
static const ::fwServices::IService::KeyType s_TF_SELECTION_INOUT = "tfSelection";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT         = "updateImage";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_IMAGE_OPACITY_SLOT = "updateImageOpacity";
static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT                 = "show";
static const  ::fwCom::Slots::SlotKeyType s_CALIBRATE_SLOT           = "calibrate";

//------------------------------------------------------------------------------

SVideo::SVideo() noexcept :
    m_imageData(vtkImageData::New()),
    m_actor(vtkImageActor::New()),
    m_isTextureInit(false),
    m_reverse(true),
    m_lookupTable(vtkSmartPointer<vtkLookupTable>::New()),
    m_hasTF(false)
{
    newSlot(s_UPDATE_IMAGE_SLOT, &SVideo::updateImage, this);
    newSlot(s_UPDATE_IMAGE_OPACITY_SLOT, &SVideo::updateImageOpacity, this);
    newSlot(s_SHOW_SLOT, &SVideo::show, this);
    newSlot(s_CALIBRATE_SLOT, &SVideo::offsetOpticalCenter, this);
    this->installTFSlots(this);
}

//------------------------------------------------------------------------------

SVideo::~SVideo() noexcept
{
    m_actor->Delete();
    m_actor = nullptr;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVideo::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_IMAGE_SLOT);
    connections.push( s_IMAGE_IN, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT);
    connections.push( s_IMAGE_IN, ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG, s_UPDATE_IMAGE_OPACITY_SLOT);
    connections.push( s_IMAGE_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    connections.push( s_CAMERA_IN, ::arData::Camera::s_MODIFIED_SIG, s_CALIBRATE_SLOT);
    connections.push( s_CAMERA_IN, ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_CALIBRATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SVideo::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    m_reverse = config.get<bool>("reverse", m_reverse);

    this->setSelectedTFKey(config.get<std::string>("selectedTFKey", ""));

    m_hasTF = !this->getSelectedTFKey().empty();
}

//------------------------------------------------------------------------------

void SVideo::starting()
{
    this->initialize();

    if (m_reverse)
    {
        m_actor->RotateZ(180);
        m_actor->RotateY(180);
    }

    if(m_hasTF)
    {
        ::fwData::Composite::sptr tfSelection = this->getInOut< ::fwData::Composite>(s_TF_SELECTION_INOUT);
        this->setTransferFunctionSelection(tfSelection);
        this->installTFConnections();
        this->updatingTFPoints();
    }

    this->updating();

    if (this->getPicker())
    {
        this->addToPicker(m_actor);
    }
}

//------------------------------------------------------------------------------

void SVideo::updating()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_IN);
    const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (!imageIsValid)
    {
        return;
    }

    ::fwVtkIO::toVTKImage(image, m_imageData);

    if(!m_isTextureInit)
    {
        this->removeAllPropFromRenderer();

        // Reset the transform matrix first
        m_actor->PokeMatrix(vtkMatrix4x4::New());
        if (m_reverse)
        {
            m_actor->RotateZ(180);
            m_actor->RotateY(180);
        }

        const ::fwData::Image::SizeType size = image->getSize();

        if(m_hasTF)
        {
            auto scalarValuesToColors = vtkSmartPointer<vtkImageMapToColors>::New();
            scalarValuesToColors->SetLookupTable(m_lookupTable);
            scalarValuesToColors->PassAlphaToOutputOn();
            scalarValuesToColors->SetInputData(m_imageData);
            m_actor->GetMapper()->SetInputConnection(scalarValuesToColors->GetOutputPort());
        }
        else
        {
            m_actor->SetInputData(m_imageData);
        }
        this->addToRenderer(m_actor);

        m_isTextureInit = true;

        this->getRenderer()->InteractiveOff();
        this->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
        this->getRenderer()->ResetCamera();
        this->getRenderer()->GetActiveCamera()->SetParallelScale(size[1] / 2.0);
        this->offsetOpticalCenter();
    }

    m_imageData->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SVideo::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SVideo::stopping()
{
    this->unregisterServices();
    this->removeAllPropFromRenderer();
    if (this->getPicker())
    {
        this->removeFromPicker(m_actor);
    }
}

//------------------------------------------------------------------------------

void SVideo::updateImageOpacity()
{
    ::fwData::Image::csptr img = this->getInput< ::fwData::Image >(s_IMAGE_IN);
    if(img->getField( "TRANSPARENCY" ) )
    {
        ::fwData::Integer::sptr transparency = img->getField< ::fwData::Integer >( "TRANSPARENCY" );
        const double imageOpacity = (100 - (*transparency) ) / 100.0;
        m_actor->SetOpacity(imageOpacity);
    }
    if(img->getField( "VISIBILITY" ) )
    {
        ::fwData::Boolean::sptr visible = img->getField< ::fwData::Boolean >( "VISIBILITY" );
        m_actor->SetVisibility(*visible);
    }

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::updateImage()
{
    m_isTextureInit = false;
    this->updating();
}

//------------------------------------------------------------------------------

void SVideo::show(bool visible)
{
    m_actor->SetVisibility(visible);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::offsetOpticalCenter()
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_IN);
    if (camera)
    {
        ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

        const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
        if (!imageIsValid)
        {
            return;
        }

        const ::fwData::Image::SizeType size = image->getSize();

        const double shiftX = size[0] / 2. - camera->getCx();
        const double shiftY = size[1] / 2. - camera->getCy();

        if (m_reverse)
        {
            m_actor->SetPosition(shiftX, -shiftY, 0);
        }
        else
        {
            m_actor->SetPosition(-shiftX, shiftY, 0);
        }
    }
}

//------------------------------------------------------------------------------

void SVideo::updatingTFPoints()
{
    ::fwVtkIO::helper::TransferFunction::toVtkLookupTable(this->getTransferFunction(), m_lookupTable);
}

//------------------------------------------------------------------------------

void SVideo::updatingTFWindowing(double window, double level)
{
    ::fwVtkIO::helper::TransferFunction::toVtkLookupTable(this->getTransferFunction(), m_lookupTable);
}

//------------------------------------------------------------------------------

} //namespace visuVTKARAdaptor
