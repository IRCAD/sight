/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwServices/macros.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>
#include <fwData/Color.hpp>
#include <fwData/String.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkCommand.h>
#include <vtkPlaneCollection.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageImport.h>
#include <vtkVolumeProperty.h>
#include <vtkVolume.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkVolumeTextureMapper2D.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastIsosurfaceFunction.h>
#include <vtkVolumeRayCastMIPFunction.h>
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkObjectFactory.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>


#include "visuVTKVRAdaptor/Volume.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKVRAdaptor::Volume, ::fwData::Image ) ;

namespace visuVTKVRAdaptor
{

class vtkBoxRepresentationHack: public vtkBoxRepresentation
{
    // Hack to disable box rotation, since SetRotationEnable is ineffective...
public:

    static vtkBoxRepresentationHack *New();

    vtkTypeRevisionMacro(vtkBoxRepresentationHack,vtkWidgetRepresentation);

    int ComputeInteractionState(int X, int Y, int modify)
    {
        return vtkBoxRepresentation::ComputeInteractionState(X,Y,1);
    }

};

vtkCxxRevisionMacro(vtkBoxRepresentationHack, "$Revision: 1.8 $");
vtkStandardNewMacro(vtkBoxRepresentationHack);



class AbortCallback : public vtkCommand
{

public :
    //--------------------------------------------------------------------------
    static AbortCallback *New()
    { return new AbortCallback(); }

    //--------------------------------------------------------------------------
    AbortCallback( )
    { }

    //--------------------------------------------------------------------------
    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {

        vtkRenderWindow *win = vtkRenderWindow::SafeDownCast(caller);
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

public :
    //--------------------------------------------------------------------------
    static CroppingCallback *New(vtkAbstractMapper *mapper)
    {

        CroppingCallback *callback = new CroppingCallback();
        callback->mapper = vtkVolumeMapper::SafeDownCast(mapper);
        return callback;
    }

    //--------------------------------------------------------------------------
    CroppingCallback( )
    { }

    //--------------------------------------------------------------------------
    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {

        vtkBoxWidget2 *widget = vtkBoxWidget2::SafeDownCast( caller );
        mapper->SetCroppingRegionPlanes( widget->GetRepresentation()->GetBounds() );
    }

private:
    vtkVolumeMapper *mapper;
};


//------------------------------------------------------------------------------

Volume::Volume() throw() :
    ::fwRenderVTK::IVtkAdaptorService(),
    ::fwComEd::helper::MedicalImageAdaptor(),
     m_bClippingBoxIsActivate(false),
     m_autoResetCamera(true)
{
    m_clippingPlanesId = "";
    m_clippingPlanes   = 0;

    m_volume = vtkVolume::New();
    m_volumeProperty = vtkVolumeProperty::New();
    m_volumeMapper = vtkVolumeTextureMapper3D::New();

    vtkVolumeTextureMapper3D::SafeDownCast(m_volumeMapper)->SetPreferredMethodToFragmentProgram();
    vtkVolumeTextureMapper3D::SafeDownCast(m_volumeMapper)->SetSampleDistance (.2);

    m_abortCommand    = AbortCallback::New();

    m_opacityTransferFunction = vtkPiecewiseFunction::New();
    m_colorTransferFunction = vtkColorTransferFunction::New();

    m_boxWidget = vtkBoxWidget2::New();
    m_boxWidget->KeyPressActivationOff();
    m_boxWidget->SetRotationEnabled(0);
    vtkBoxRepresentationHack *repr = vtkBoxRepresentationHack::New();
    m_boxWidget->SetRepresentation(repr);
    repr->Delete();

    m_croppingCommand = CroppingCallback::New(m_volumeMapper);
    m_boxWidget->AddObserver(vtkCommand::InteractionEvent, m_croppingCommand);

    // Manage events
    this->installTFSelectionEventHandler(this);
    this->addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
    this->addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE );
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::MODIFIED_POINTS );
    this->addNewHandledEvent( ::fwComEd::TransferFunctionMsg::WINDOWING );
    this->addNewHandledEvent( "SHOWHIDE_BOX_WIDGET" );
    this->addNewHandledEvent( "RESET_BOX_WIDGET" );
}

//------------------------------------------------------------------------------

Volume::~Volume() throw()
{
    m_volumeMapper->Delete();
    m_volumeMapper = NULL;

    m_volume->Delete();
    m_volume = NULL;

    m_abortCommand->Delete();
    m_abortCommand = NULL;

    if (m_clippingPlanes)
    {
        m_clippingPlanes->Delete();
        m_clippingPlanes = NULL;
    }

    m_boxWidget->RemoveObserver(m_croppingCommand);
    m_boxWidget->Delete();
    m_boxWidget = NULL;
    m_croppingCommand->Delete();
    m_croppingCommand = NULL;
}

//------------------------------------------------------------------------------

void Volume::setClippingPlanesId(::fwRenderVTK::VtkRenderService::VtkObjectIdType id)
{
    m_clippingPlanesId = id;
}

//------------------------------------------------------------------------------

void Volume::setVtkClippingPlanes(vtkPlaneCollection *planes)
{
    m_clippingPlanes = planes;
}

//------------------------------------------------------------------------------

void Volume::doStart() throw(fwTools::Failed)
{
    this->addToRenderer(m_volume);

    this->getInteractor()->GetRenderWindow()->AddObserver("AbortCheckEvent", m_abortCommand);
    this->doUpdate(); //TODO: remove me ?
    this->installTFObserver( this->getSptr() );
}

//------------------------------------------------------------------------------

void Volume::doStop() throw(fwTools::Failed)
{
    this->removeTFObserver();
    this->removeAllPropFromRenderer();
    this->getInteractor()->GetRenderWindow()->RemoveObserver(m_abortCommand);
}

//------------------------------------------------------------------------------

void Volume::doSwap() throw(fwTools::Failed)
{
    this->removeTFObserver();
    this->doUpdate();
    this->installTFObserver( this->getSptr() );
}

//------------------------------------------------------------------------------

void Volume::doUpdate() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        this->buildPipeline();
        this->updateImage(image);
        this->updateTransferFunction(image);
        this->updateWindowing(image);
    }
}

//------------------------------------------------------------------------------

void Volume::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            this->doUpdate();
        }

        if (this->upadteTFObserver(msg) || msg->hasEvent( ::fwComEd::TransferFunctionMsg::MODIFIED_POINTS ) )
        {
            this->updateTransferFunction(image);
        }

        if ( msg->hasEvent( ::fwComEd::TransferFunctionMsg::WINDOWING ) )
        {
            this->updateTransferFunction(image);
            this->updateWindowing(image);
        }

        if ( msg->hasEvent( "SHOWHIDE_BOX_WIDGET" ) )
        {
            m_bClippingBoxIsActivate = ! m_bClippingBoxIsActivate;
            this->activateBoxClipping( m_bClippingBoxIsActivate );
        }

        if ( msg->hasEvent( "RESET_BOX_WIDGET" ) )
        {
            this->resetBoxWidget();
        }
    }
}

//------------------------------------------------------------------------------

void Volume::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setClippingPlanesId( m_configuration->getAttributeValue("clippingplanes") );

    if (m_configuration->hasAttribute("autoresetcamera") )
    {
        std::string autoresetcamera = m_configuration->getAttributeValue("autoresetcamera");
        m_autoResetCamera = (autoresetcamera == "yes");
    }
    this->parseTFConfig( m_configuration );
}

//------------------------------------------------------------------------------

void Volume::updateImage( ::fwData::Image::sptr image  )
{
    this->updateImageInfos(image);

    vtkImageImport *imageImport = vtkImageImport::New();
    ::vtkIO::configureVTKImageImport( imageImport, image );

    m_volumeMapper->RemoveAllClippingPlanes();
    if (m_clippingPlanes)
    {
        vtkPlaneCollection *newClippingPlanes = vtkPlaneCollection::New();
        m_volumeMapper->SetClippingPlanes(m_clippingPlanes);
    }

    //m_volumeMapper->SetInputConnection(shiftScale->GetOutputPort());
    m_volumeMapper->SetInputConnection(imageImport->GetOutputPort());

    m_boxWidget->GetRepresentation()->SetPlaceFactor(1.0);
    m_boxWidget->GetRepresentation()->PlaceWidget(m_volumeMapper->GetBounds());
    m_boxWidget->SetInteractor(this->getInteractor());
    m_boxWidget->On();
    vtkVolumeMapper::SafeDownCast(m_volumeMapper)->CroppingOn();
    vtkVolumeMapper::SafeDownCast(m_volumeMapper)->SetCroppingRegionPlanes( m_volumeMapper->GetBounds() );
    m_bClippingBoxIsActivate = true;

    //shiftScale->Delete();
    imageImport->Delete();

    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Volume::updateWindowing( ::fwData::Image::sptr image )
{
}

//------------------------------------------------------------------------------

void Volume::updateTransferFunction( ::fwData::Image::sptr image )
{
    ::fwData::TransferFunction::sptr pTF = this->getTransferFunction();
    SLM_ASSERT("TransferFunction null pointer", pTF);

    m_colorTransferFunction->RemoveAllPoints();
    m_opacityTransferFunction->RemoveAllPoints();

    const ::fwData::TransferFunction::TFValueVectorType values = pTF->getScaledValues();
    ::fwData::TransferFunction::TFValueVectorType::const_iterator valueIter = values.begin();
    BOOST_FOREACH(const ::fwData::TransferFunction::TFDataType::value_type &tfPoint, pTF->getTFData())
    {
        const ::fwData::TransferFunction::TFValueType &value = *(valueIter++);
        const ::fwData::TransferFunction::TFColor &color = tfPoint.second;

        m_colorTransferFunction->AddRGBPoint( value , color.r, color.g, color.b );
        m_opacityTransferFunction->AddPoint(  value , color.a );
    }
    ::fwData::TransferFunction::TFValuePairType minMax = pTF->getMinMaxTFValues();

    m_colorTransferFunction->SetClamping(!pTF->getIsClamped());
    m_opacityTransferFunction->SetClamping(!pTF->getIsClamped());

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Volume::buildPipeline( )
{
    if (!m_clippingPlanesId.empty())
    {
        vtkObject          *o      = this->getVtkObject(m_clippingPlanesId);
        vtkPlaneCollection *planes = vtkPlaneCollection::SafeDownCast(o);
        this->setVtkClippingPlanes( planes );
    }

    m_volumeProperty->SetScalarOpacity(m_opacityTransferFunction);
    m_volumeProperty->SetColor(m_colorTransferFunction);

    m_volumeProperty->ShadeOn();
    m_volumeProperty->SetInterpolationTypeToLinear();

    m_volumeProperty->SetAmbient( 0.1 );
    m_volumeProperty->SetDiffuse( 0.7 );
    m_volumeProperty->SetSpecular( 0.2 );
    m_volumeProperty->SetSpecularPower( 10.0 );

    m_volume->SetMapper(m_volumeMapper);
    m_volume->SetProperty(m_volumeProperty);

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Volume::resetBoxWidget()
{
    m_boxWidget->GetRepresentation()->SetPlaceFactor(1.0);
    m_boxWidget->GetRepresentation()->PlaceWidget( m_volumeMapper->GetBounds() );
    vtkVolumeMapper::SafeDownCast( m_volumeMapper )->SetCroppingRegionPlanes( m_volumeMapper->GetBounds() );
    if (m_autoResetCamera)
    {
        this->getRenderer()->ResetCamera();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Volume::activateBoxClipping( bool activate )
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
}

//------------------------------------------------------------------------------

} //namespace visuVTKVRAdaptor
