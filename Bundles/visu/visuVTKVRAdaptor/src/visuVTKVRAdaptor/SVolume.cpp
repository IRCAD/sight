/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKVRAdaptor/SVolume.hpp"

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkBoundingBox.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkColorTransferFunction.h>
#include <vtkCommand.h>
#include <vtkImageImport.h>
#include <vtkImageResample.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPlaneCollection.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkTransform.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKVRAdaptor::SVolume);

//-----------------------------------------------------------------------------

namespace visuVTKVRAdaptor
{

class TransformCallback : public ::vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static TransformCallback* New(SVolume* adaptor)
    {
        TransformCallback* cb = new TransformCallback();
        cb->m_adaptor = adaptor;
        return cb;
    }

    //------------------------------------------------------------------------------

    virtual void Execute( ::vtkObject* caller, unsigned long, void* )
    {
        m_adaptor->updateCropBoxTransform();
        m_adaptor->crop();
    }

private:
    SVolume* m_adaptor;
};

//------------------------------------------------------------------------------

class AbortCallback : public vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static AbortCallback* New()
    {
        return new AbortCallback();
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        vtkRenderWindow* win = vtkRenderWindow::SafeDownCast(caller);
        if ( win )
        {
            if( win->GetEventPending() )
            {
                win->SetAbortRender(1);
            }
        }
    }
};

//------------------------------------------------------------------------------

class CroppingCallback : public vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static CroppingCallback* New(SVolume* adaptor)
    {
        CroppingCallback* callback = new CroppingCallback();
        callback->m_adaptor = adaptor;
        return callback;
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        m_adaptor->crop();
        m_adaptor->updateTransform();
    }

private:
    SVolume* m_adaptor;
};

static const ::fwCom::Slots::SlotKeyType s_RESET_BOX_WIDGET_SLOT      = "resetBoxWidget";
static const ::fwCom::Slots::SlotKeyType s_ACTIVATE_BOX_CLIPPING_SLOT = "activateBoxClipping";
static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT                  = "show";

const ::fwServices::IService::KeyType SVolume::s_IMAGE_INOUT = "image";
const ::fwServices::IService::KeyType SVolume::s_TF_INOUT    = "tf";

//------------------------------------------------------------------------------

SVolume::SVolume() noexcept :
    m_helperTF(std::bind(&SVolume::updateTF, this)),
    ::fwRenderVTK::IAdaptor(),
    m_clippingPlanes(nullptr),
    m_volumeMapper( vtkSmartVolumeMapper::New()),
    m_volumeProperty(vtkVolumeProperty::New()),
    m_volume(vtkVolume::New()),
    m_opacityTransferFunction(vtkPiecewiseFunction::New()),
    m_colorTransferFunction(vtkColorTransferFunction::New()),
    m_abortCommand(AbortCallback::New()),
    m_boxWidget(vtkBoxWidget2::New()),
    m_croppingCommand(nullptr),
    m_transformCommand(nullptr),
    m_croppingBoxDefaultState(true),
    m_cropBoxTransform(nullptr),
    m_autoResetCamera(true),
    m_reductionFactor(1.0),
    m_blendMode("composite")
{
    m_boxWidget->KeyPressActivationOff();
    m_boxWidget->SetRotationEnabled(0);
    vtkBoxRepresentation* repr = vtkBoxRepresentation::New();
    m_boxWidget->SetRepresentation(repr);
    repr->Delete();

    newSlot(s_RESET_BOX_WIDGET_SLOT, &SVolume::resetBoxWidget, this);
    newSlot(s_ACTIVATE_BOX_CLIPPING_SLOT, &SVolume::activateBoxClipping, this);
    newSlot(s_SHOW_SLOT, &SVolume::show, this);
}

//------------------------------------------------------------------------------

SVolume::~SVolume() noexcept
{
    m_volumeMapper->Delete();
    m_volumeMapper = nullptr;

    m_volume->Delete();
    m_volume = nullptr;

    m_abortCommand->Delete();
    m_abortCommand = nullptr;

    if (m_clippingPlanes)
    {
        m_clippingPlanes->Delete();
        m_clippingPlanes = nullptr;
    }

    m_boxWidget->Delete();
    m_boxWidget = nullptr;

    m_opacityTransferFunction->Delete();
    m_opacityTransferFunction = nullptr;

    m_colorTransferFunction->Delete();
    m_colorTransferFunction = nullptr;
}

//------------------------------------------------------------------------------

void SVolume::setClippingPlanesId(::fwRenderVTK::SRender::VtkObjectIdType id)
{
    m_clippingPlanesId = id;
}

//------------------------------------------------------------------------------

void SVolume::setVtkClippingPlanes(vtkPlaneCollection* planes)
{
    m_clippingPlanes = planes;
}

//------------------------------------------------------------------------------

void SVolume::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    this->setClippingPlanesId( config.get<std::string>("clippingplanes", "") );

    m_autoResetCamera = config.get<std::string>("autoresetcamera", "yes") == "yes";

    // Show croppingBox
    m_croppingBoxDefaultState = config.get<std::string>("croppingBox", "yes") == "yes";

    // Get the boundingBox transformation matrix
    m_cropBoxTransformID = config.get<std::string>("cropBoxTransform", "");

    m_reductionFactor = config.get<double>("reductionFactor", 1.);

    // Blend Mode
    m_blendMode = config.get<std::string>("blend", "composite");

}

//------------------------------------------------------------------------------

void SVolume::starting()
{
    this->initialize();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    const ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    if(tf != nullptr)
    {
        ::fwData::mt::ObjectReadLock tfLock(tf);
        m_helperTF.setOrCreateTF(tf, image);
    }
    else
    {
        m_helperTF.setOrCreateTF(tf, image);
    }

    this->addToRenderer(m_volume);

    this->getInteractor()->GetRenderWindow()->AddObserver("AbortCheckEvent", m_abortCommand);
    this->updating(); //TODO: remove me ?

    this->activateBoxClipping( m_croppingBoxDefaultState );

    if(!m_cropBoxTransformID.empty())
    {
        m_cropBoxTransform = vtkTransform::SafeDownCast( m_renderService.lock()->getVtkObject(m_cropBoxTransformID));
    }

    if(m_cropBoxTransform)
    {
        m_transformCommand = TransformCallback::New(this);
        m_cropBoxTransform->AddObserver( ::vtkCommand::ModifiedEvent, m_transformCommand );

        vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_boxWidget->GetRepresentation() );
        repr->SetTransform(m_cropBoxTransform);

        this->crop();
    }

    m_croppingCommand = CroppingCallback::New(this);
    m_boxWidget->AddObserver(vtkCommand::InteractionEvent, m_croppingCommand);

    if (!this->getTransformId().empty())
    {
        m_volume->SetUserTransform(this->getTransform());
    }
}

//------------------------------------------------------------------------------

void SVolume::stopping()
{
    m_helperTF.removeTFConnections();
    this->removeAllPropFromRenderer();
    this->getInteractor()->GetRenderWindow()->RemoveObserver(m_abortCommand);
    m_boxWidget->RemoveObserver(m_croppingCommand);

    m_croppingCommand->Delete();
    m_croppingCommand = nullptr;

    if(m_cropBoxTransform)
    {
        m_cropBoxTransform->RemoveObserver( m_transformCommand );
        m_transformCommand->Delete();
        m_transformCommand = nullptr;
    }
}

//------------------------------------------------------------------------------

void SVolume::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        this->buildPipeline();
        this->updateImage(image);
        this->updateVolumeTransferFunction(image);
        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SVolume::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        const ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        if(tf != nullptr)
        {
            ::fwData::mt::ObjectReadLock tfLock(tf);
            m_helperTF.setOrCreateTF(tf, image);
        }
        else
        {
            m_helperTF.setOrCreateTF(tf, image);
        }

        this->updating();
    }
}

//------------------------------------------------------------------------------

void SVolume::updateTF()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    this->updateVolumeTransferFunction(image);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVolume::updateImage( ::fwData::Image::sptr image  )
{
    vtkImageImport* imageImport = vtkImageImport::New();
    ::fwVtkIO::configureVTKImageImport( imageImport, image );

    m_volumeMapper->RemoveAllClippingPlanes();
    if (m_clippingPlanes)
    {
        m_volumeMapper->SetClippingPlanes(m_clippingPlanes);
    }

    if ( m_reductionFactor < 1.0 )
    {
        vtkImageResample* resample = vtkImageResample::New();
        resample->SetInputConnection( imageImport->GetOutputPort() );
        resample->SetAxisMagnificationFactor(0, m_reductionFactor);
        resample->SetAxisMagnificationFactor(1, m_reductionFactor);
        resample->SetAxisMagnificationFactor(2, m_reductionFactor);
        m_volumeMapper->SetInputConnection(resample->GetOutputPort());
        resample->Delete();
    }
    else
    {
        m_volumeMapper->SetInputConnection(imageImport->GetOutputPort());
    }

    m_boxWidget->GetRepresentation()->SetPlaceFactor(1.0);
    m_boxWidget->GetRepresentation()->PlaceWidget(m_volumeMapper->GetBounds());
    m_boxWidget->SetInteractor(this->getInteractor());

    vtkVolumeMapper::SafeDownCast(m_volumeMapper)->CroppingOn();
    vtkVolumeMapper::SafeDownCast(m_volumeMapper)->SetCroppingRegionPlanes( m_volumeMapper->GetBounds() );

    imageImport->Delete();

    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SVolume::updateVolumeTransferFunction( ::fwData::Image::sptr image )
{
    const ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    SLM_ASSERT("TransferFunction null pointer", tf);

    ::fwData::mt::ObjectReadLock tfLock(tf);

    m_colorTransferFunction->RemoveAllPoints();
    m_opacityTransferFunction->RemoveAllPoints();

    ::fwData::TransferFunction::TFValueVectorType values              = tf->getScaledValues();
    ::fwData::TransferFunction::TFValueVectorType::iterator valueIter = values.begin();
    if(tf->getInterpolationMode() == ::fwData::TransferFunction::NEAREST)
    {
        m_colorTransferFunction->AllowDuplicateScalarsOn();
        m_opacityTransferFunction->AllowDuplicateScalarsOn();

        for(const ::fwData::TransferFunction::TFDataType::value_type& tfPoint :  tf->getTFData())
        {
            const ::fwData::TransferFunction::TFValueType& value = *valueIter;
            ::fwData::TransferFunction::TFValueType valuePrevious = *valueIter;
            ::fwData::TransferFunction::TFValueType valueNext     = *valueIter;
            if(valueIter != values.begin())
            {
                valuePrevious = *(valueIter - 1);
            }
            if(valueIter != (values.end()-1))
            {
                valueNext = *(valueIter + 1);
            }

            const ::fwData::TransferFunction::TFColor& color = tfPoint.second;

            m_colorTransferFunction->AddRGBPoint(valuePrevious + (value - valuePrevious) / 2., color.r, color.g,
                                                 color.b );
            m_colorTransferFunction->AddRGBPoint(value + (valueNext - value) / 2., color.r, color.g, color.b );

            m_opacityTransferFunction->AddPoint(valuePrevious + (value -valuePrevious) / 2., color.a );
            m_opacityTransferFunction->AddPoint(value + (valueNext - value) / 2., color.a );

            ++valueIter;
        }
    }
    else
    {
        for(const ::fwData::TransferFunction::TFDataType::value_type& tfPoint :  tf->getTFData())
        {
            const ::fwData::TransferFunction::TFValueType& value = *(valueIter++);
            const ::fwData::TransferFunction::TFColor& color     = tfPoint.second;

            m_colorTransferFunction->AddRGBPoint( value, color.r, color.g, color.b );
            m_opacityTransferFunction->AddPoint(  value, color.a );
        }
    }

    m_colorTransferFunction->SetClamping(!tf->getIsClamped());
    m_opacityTransferFunction->SetClamping(!tf->getIsClamped());

    if(m_blendMode == "average")
    {
        //use the TF windowing min and max values to set up the average blend range
        std::pair< double, double > averageRange = tf->getWLMinMax();
        m_volumeMapper->SetAverageIPScalarRange(averageRange.first, averageRange.second);
    }

    this->setVtkPipelineModified();

}

//------------------------------------------------------------------------------

void SVolume::buildPipeline( )
{
    if (!m_clippingPlanesId.empty())
    {
        vtkObject* o               = this->getVtkObject(m_clippingPlanesId);
        vtkPlaneCollection* planes = vtkPlaneCollection::SafeDownCast(o);
        this->setVtkClippingPlanes( planes );
    }

    m_volumeProperty->SetScalarOpacity(m_opacityTransferFunction);
    m_volumeProperty->SetColor(m_colorTransferFunction);

    m_volumeProperty->ShadeOn();
    m_volumeProperty->SetInterpolationTypeToLinear();

    m_volumeProperty->SetAmbient( 0.2 );
    m_volumeProperty->SetDiffuse( 1.0 );
    m_volumeProperty->SetSpecular( 1.0 );
    m_volumeProperty->SetSpecularPower( 10.0 );

    // set the mapper according to the blendMode
    if(m_blendMode == "min")
    {
        m_volumeMapper->SetBlendModeToMinimumIntensity();
    }
    else if(m_blendMode == "max")
    {
        m_volumeMapper->SetBlendModeToMaximumIntensity();
    }
    else if(m_blendMode == "average")
    {
        m_volumeMapper->SetRequestedRenderMode(vtkSmartVolumeMapper::GPURenderMode);
        m_volumeMapper->SetBlendModeToAverageIntensity();
    }
    else if(m_blendMode == "additive")
    {
        m_volumeMapper->SetBlendModeToAdditive();

    }
    else if(m_blendMode == "composite")
    {
        m_volumeMapper->SetBlendModeToComposite();
    }
    else
    {
        OSLM_WARN("blend mode '"<< m_blendMode <<"' is unknown. Should be min, max, average, composite or additive.");
    }

    m_volume->SetMapper(m_volumeMapper);
    m_volume->SetProperty(m_volumeProperty);

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SVolume::resetBoxWidget()
{
    m_boxWidget->GetRepresentation()->SetPlaceFactor(1.0);
    m_boxWidget->GetRepresentation()->PlaceWidget( m_volumeMapper->GetBounds() );
    vtkVolumeMapper::SafeDownCast( m_volumeMapper )->SetCroppingRegionPlanes( m_volumeMapper->GetBounds() );
    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVolume::activateBoxClipping( bool activate )
{
    if ( activate )
    {
        m_boxWidget->On();
    }
    else
    {
        m_boxWidget->Off();
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVolume::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SVolume::crop()
{
    vtkVolumeMapper* mapper      = vtkVolumeMapper::SafeDownCast(m_volumeMapper);
    double* croppingRegionPlanes = m_boxWidget->GetRepresentation()->GetBounds();

    vtkBoundingBox boundingBoxCrop(croppingRegionPlanes);
    vtkBoundingBox boundingBoxVolume(mapper->GetBounds());

    if(boundingBoxCrop.Intersects(boundingBoxVolume))
    {
        mapper->SetCroppingRegionPlanes( croppingRegionPlanes );
    }
    else
    {
        mapper->SetCroppingRegionPlanes(0., 0., 0., 0., 0., 0.);
    }
}

//------------------------------------------------------------------------------

void SVolume::updateTransform()
{
    if(m_cropBoxTransform)
    {
        vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_boxWidget->GetRepresentation() );
        if( repr )
        {
            m_cropBoxTransform->RemoveObserver(m_transformCommand);
            repr->GetTransform(m_cropBoxTransform);
            m_cropBoxTransform->Modified();
            m_cropBoxTransform->AddObserver(vtkCommand::ModifiedEvent, m_transformCommand);
        }
    }
}

//------------------------------------------------------------------------------

void SVolume::updateCropBoxTransform()
{
    if(m_cropBoxTransform)
    {
        vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_boxWidget->GetRepresentation() );
        if( repr )
        {
            m_boxWidget->RemoveObserver(m_croppingCommand);
            repr->SetTransform(m_cropBoxTransform);
            m_boxWidget->AddObserver(vtkCommand::InteractionEvent, m_croppingCommand);
        }
    }
}

//------------------------------------------------------------------------------

void SVolume::show(bool isVisible)
{
    m_volume->SetVisibility(isVisible);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

} //namespace visuVTKVRAdaptor
