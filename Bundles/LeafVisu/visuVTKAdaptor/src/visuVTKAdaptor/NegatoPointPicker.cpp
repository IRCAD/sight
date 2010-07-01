/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */


#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <fwTools/helpers.hpp>

#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwData/ValuedPointList.hpp>
#include <fwData/TransfertFunction.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwRuntime/ConfigurationElementContainer.hpp>

#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkCommand.h>

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwRenderVTK/vtk/fwVtkCellPicker.hpp>
#include <fwRenderVTK/vtk/Helpers.hpp>

#include "visuVTKAdaptor/NegatoPointPicker.hpp"
#include "visuVTKAdaptor/ImageSlice.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::NegatoPointPicker, ::visuVTKAdaptor::NegatoOneSlice ) ;


#define START_MULTIPICKING_EVENT 		vtkCommand::LeftButtonPressEvent
#define STOP_MULTIPICKING_EVENT  		vtkCommand::LeftButtonReleaseEvent
#define MULTIPICKING_ACQUISITION_EVENT  vtkCommand::MouseMoveEvent
#define SINGLEPICKING_EVENT				vtkCommand::LeftButtonPressEvent

namespace visuVTKAdaptor
{

class NegatoPointPickerCallback : public vtkCommand
{
public:


    static NegatoPointPickerCallback *New()
    { return new NegatoPointPickerCallback(); }

    NegatoPointPickerCallback() : m_acquisitionState(NOTACQUIRE_POINTS)
    {
        this->PassiveObserverOff();
    }

    ~NegatoPointPickerCallback()
    {

    }
    bool getWorldPickedPoint(double worlPickedPoint[3])
    {
    	double pickPoint[3];
        double pickedPoint[3];
        bool	isPicked = false;
    	m_adaptor.lock()->getInteractor()->GetEventPosition(m_x, m_y);
    	pickPoint[0] = m_x;
    	pickPoint[1] = m_y;
    	pickPoint[2] = 0;

    	if ( m_picker->Pick( pickPoint , m_adaptor.lock()->getRenderer() ) )
    	{
    		//Retrieve actor associated to the negato to check that m_pickedProp is this one
    		::fwRenderVTK::IVtkAdaptorService::sptr adaptor = m_negato.lock()->getImageSliceAdaptor();
    		::visuVTKAdaptor::ImageSlice::sptr imageSliceAdaptor = ::visuVTKAdaptor::ImageSlice::dynamicCast( adaptor );
    		vtkImageActor * actor = imageSliceAdaptor->getImageActor();

    		m_picker->GetPickPosition(pickedPoint);
    		m_pickedProp = ::fwRenderVTK::vtk::getNearestPickedProp(m_picker, m_adaptor.lock()->getRenderer());
    		if( m_pickedProp == (vtkProp *) actor )
    		{
    			OSLM_TRACE("PickedProp = " << m_pickedProp);
    			OSLM_TRACE("PickedPoint = " << pickedPoint[0] << " - " << pickedPoint[1] << " - " << pickedPoint[2] << " - ");
    			m_localPicker = fwVtkCellPicker::New();
    			m_localPicker->InitializePickList();
    			m_localPicker->PickFromListOn();
    			m_localPicker->AddPickList(m_pickedProp);

    			if ( m_localPicker->Pick( pickPoint , m_adaptor.lock()->getRenderer() ) )
    			{
    				m_localPicker->GetPickPosition(worlPickedPoint);
    				OSLM_TRACE("worlPickedPoint = " << worlPickedPoint[0] << " - " << worlPickedPoint[1] << " - " << worlPickedPoint[2] << " - ");
    				isPicked = true;
    			}
    		}
    	}
    	return isPicked;
    }
    void ExecuteText(vtkObject *caller, unsigned long eventId, void *)
    {
    	assert( m_adaptor.lock()->isTextShown() );
        if( eventId == MULTIPICKING_ACQUISITION_EVENT )
        {
        	double worlPickedPoint[3];
        	if( this->getWorldPickedPoint(worlPickedPoint) )
        	{
        		m_adaptor.lock()->updateText(worlPickedPoint);
        	}
        	else
        	{
        		m_adaptor.lock()->clearText();
        	}
        }
    }
    void ExecuteSinglePoint(vtkObject *caller, unsigned long eventId, void *)
    {
    	assert(m_adaptor.lock()->getPickingMode() == NegatoPointPicker::SINGLEPOINT);
        if( eventId == SINGLEPICKING_EVENT )
        {
        	m_adaptor.lock()->emptyPickedPoints();
        	double worlPickedPoint[3];
        	if( this->getWorldPickedPoint(worlPickedPoint) )
        	{
        		SetAbortFlag(1);
        		m_adaptor.lock()->addPickedPoint(worlPickedPoint);
        		m_adaptor.lock()->notifyPicking();
        	}
        }
    }
    void ExecuteMultiPoint(vtkObject *caller, unsigned long eventId, void *)
    {
    	assert(m_adaptor.lock()->getPickingMode() == NegatoPointPicker::MULTIPOINT);
    	//Start acquisition
    	if( eventId == START_MULTIPICKING_EVENT && m_acquisitionState == NOTACQUIRE_POINTS )
    	{
    		m_acquisitionState = ACQUIRE_POINTS ;
    		m_adaptor.lock()->emptyPickedPoints();
    		if( !m_adaptor.lock()->isTextShown() ) //because textActicated involves reaction on MULTIPICKING_ACQUISITION_EVENT
    		{
    			m_adaptor.lock()->getInteractor()->AddObserver(MULTIPICKING_ACQUISITION_EVENT  , this, 1.);
    		}
    		//Ne pas faire si plusieurs interactors cohabitent sur le type d'événement, car cela a pour effet de ne traiter ceux qui suivent
    		//A faire par exemple pour le cas vtkCommand::LeftButtonReleaseEvent soit traité: pourquoi ????
    		SetAbortFlag(1);
    	}
    	//Stop acquisition
    	else if( eventId == STOP_MULTIPICKING_EVENT && m_acquisitionState == ACQUIRE_POINTS )
    	{
    		m_acquisitionState = NOTACQUIRE_POINTS ;
    		if( !m_adaptor.lock()->isTextShown() )//because textActicated involves reaction on MULTIPICKING_ACQUISITION_EVENT
    		{
    			m_adaptor.lock()->getInteractor()->RemoveObservers(MULTIPICKING_ACQUISITION_EVENT  , this );
    		}
    		m_adaptor.lock()->notifyPicking();
    	}
    	//Acquire
    	else if( eventId == MULTIPICKING_ACQUISITION_EVENT && m_acquisitionState == ACQUIRE_POINTS )
    	{
        	double worlPickedPoint[3];
        	if( this->getWorldPickedPoint(worlPickedPoint) )
        	{
        		m_adaptor.lock()->addPickedPoint(worlPickedPoint);
        	}
    	}
    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *_data)
    {
        if( m_adaptor.lock()->isTextShown() )
        {
        	this->ExecuteText(caller, eventId, _data);
        }
    	if( m_adaptor.lock()->getPickingMode() == NegatoPointPicker::SINGLEPOINT )
    	{
    		this->ExecuteSinglePoint(caller, eventId, _data);
    	}
    	else if( m_adaptor.lock()->getPickingMode() == NegatoPointPicker::MULTIPOINT )
    	{
    		this->ExecuteMultiPoint(caller, eventId, _data);
    	}
    }

    void setAdaptor( NegatoPointPicker::wptr adaptor)
    {
        m_adaptor = adaptor;
        m_negato  = m_adaptor.lock()->getObject< ::visuVTKAdaptor::NegatoOneSlice >();
    }

    void setPicker( vtkAbstractPropPicker *picker)
    {
        m_picker = picker;
    }

protected :
    NegatoPointPicker::wptr 				m_adaptor;
    vtkAbstractPropPicker *					m_picker;
    vtkAbstractPropPicker *					m_localPicker;
    vtkProp *								m_pickedProp;
    ::visuVTKAdaptor::NegatoOneSlice::wptr 	m_negato ;
    int 									m_x;
    int 									m_y;

    /**
	 * @brief State for managing the acquisition of multiple points
	 * when moving the mouse with left mouse button pressed, all points are acquired (state ACQUIRE_POINTS)
	 * acquisition stops when releasing the left mouse button (state becomes NOTACQUIRE_POINTS)
	 */
	typedef enum
	{
		ACQUIRE_POINTS,
		NOTACQUIRE_POINTS,
	}	ACQUISITIONSTATE ;

	ACQUISITIONSTATE					m_acquisitionState;

};

//------------------------------------------------------------------------------

NegatoPointPicker::NegatoPointPicker() throw()
{
	m_pickingMode 	= SINGLEPOINT;
	m_showText 		= false;
    m_priority 		= .6;
}

//------------------------------------------------------------------------------

NegatoPointPicker::~NegatoPointPicker() throw()
{
}

//------------------------------------------------------------------------------

void NegatoPointPicker::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    //Force configuration of base object
    m_negato = this->getObject< ::visuVTKAdaptor::NegatoOneSlice >();
    m_negato.lock()->configure();
    SLM_ASSERT("NegatoOneSlice must be configured", m_negato.lock()->getConfigurationStatus() == CONFIGURED ) ;

    //Service configuration from base object (being a service) configuration
    this->setRenderId( m_negato.lock()->getRenderId() );
    this->setPickerId( m_negato.lock()->getPickerId() );

    //Manage specific configuration
    SLM_ASSERT("Attribute mode (single/multi) and text (on/off) required", m_configuration->hasAttribute("mode") && m_configuration->hasAttribute("text") );
    SLM_ASSERT("Attribute mode value is single or multi", m_configuration->getAttributeValue("mode") == "single" || m_configuration->getAttributeValue("mode") == "multi" );
    SLM_ASSERT("Attribute text value is on or off", m_configuration->getAttributeValue("text") == "on" || m_configuration->getAttributeValue("text") == "off" );
    if( m_configuration->getAttributeValue("mode") == "single" )
    {
    	m_pickingMode = SINGLEPOINT;
    }
    else if( m_configuration->getAttributeValue("mode") == "multi" )
    {
        m_pickingMode = MULTIPOINT;
    }
    m_configuration->getAttributeValue("text") == "on" ? m_showText = true : m_showText = false;
}

//------------------------------------------------------------------------------

void NegatoPointPicker::doStart() throw(fwTools::Failed)
{
	//Get base object of base object
	m_image = m_negato.lock()->getObject< ::fwData::Image >();
	//Force start of base object
	m_negato.lock()->start();
	this->setRenderService(m_negato.lock()->getRenderService());

	//Initialize callback (vtkCommand)
    SLM_ASSERT("NegatoOneSlice must be started", this->getObject< ::visuVTKAdaptor::NegatoOneSlice >()->getStatus() == STARTED ) ;
    NegatoPointPickerCallback *observer= NegatoPointPickerCallback::New();
    observer->setAdaptor( NegatoPointPicker::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());

    //Affect callback's related events
    if( m_pickingMode == SINGLEPOINT )
    {
    	m_negato.lock()->getInteractor()->AddObserver(SINGLEPICKING_EVENT, observer, m_priority);
    }
    else if( m_pickingMode == MULTIPOINT )
    {
    	m_negato.lock()->getInteractor()->AddObserver(START_MULTIPICKING_EVENT, observer, m_priority);
    	m_negato.lock()->getInteractor()->AddObserver(STOP_MULTIPICKING_EVENT, observer, m_priority);
    }

    if( m_showText )
    {
    	m_negato.lock()->getInteractor()->AddObserver(MULTIPICKING_ACQUISITION_EVENT, observer, 1.0);
    	//Note: If attachement to m_negato: no pending refresh on m_textAdaptor.lock()->setText() (render does not observe m_negato but image)
    	m_textAdaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService , ::visuVTKAdaptor::Text >(m_image.lock());
    	//Manual configuration
    	m_textAdaptor.lock()->setRenderService(	m_negato.lock()->getRenderService());
    	m_textAdaptor.lock()->setRenderId( 		m_negato.lock()->getRenderId() );
    	//Start text service
    	m_textAdaptor.lock()->start();
    }
    m_vtkObserver = observer;

    //Initialize appropriate image fields (base object of base object)
    m_image.lock()->setFieldSingleElement(::fwComEd::ImageMsg::PICKEDPOINTS, ::fwData::ValuedPointList::New());
    m_pickedPoints = m_image.lock()->getFieldSingleElement< ::fwData::ValuedPointList >(::fwComEd::ImageMsg::PICKEDPOINTS);
}

//------------------------------------------------------------------------------

void NegatoPointPicker::doUpdate() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void NegatoPointPicker::doStop() throw(fwTools::Failed)
{
	//Remove observers
    if( m_pickingMode == SINGLEPOINT )
    {
    	m_negato.lock()->getInteractor()->RemoveObservers(SINGLEPICKING_EVENT  , m_vtkObserver);
    }
    else if( m_pickingMode == MULTIPOINT )
    {
    	m_negato.lock()->getInteractor()->RemoveObservers(START_MULTIPICKING_EVENT, m_vtkObserver);
    	m_negato.lock()->getInteractor()->RemoveObservers(STOP_MULTIPICKING_EVENT, m_vtkObserver);
    }
    if( m_showText )
    {
    	m_negato.lock()->getInteractor()->RemoveObservers(MULTIPICKING_ACQUISITION_EVENT  , m_vtkObserver);
    	m_textAdaptor.lock()->stop();
    }

    //Remove text service if present
    if( !m_textAdaptor.expired() )
    {
    	::fwServices::erase(m_textAdaptor.lock());
    }
    //Reset callback
    m_vtkObserver = NULL;
}
NegatoPointPicker::PickingMode NegatoPointPicker::getPickingMode()
{
	return m_pickingMode ;
}
bool NegatoPointPicker::isTextShown()
{
	return m_showText;
}
void NegatoPointPicker::emptyPickedPoints()
{
	m_pickedPoints.lock()->getRefPoints().clear();
}

void NegatoPointPicker::addPickedPoint(double pickedPoint[3]) throw(fwTools::Failed)
{
    int index[3];
    m_negato.lock()->updateImageInfos(m_image.lock());
    m_negato.lock()->worldToImageSliceIndex( pickedPoint, index );
    OSLM_TRACE("index=" << index[0] << "," << index[1] << "," << index[2] );
    std::cout << "index=" << index[0] << "," << index[1] << "," << index[2] << std::endl;

    //Check that index does not correspond to the last acquired point
    unsigned int size = m_pickedPoints.lock()->getRefPoints().size();
    bool isValidPoint = true;
    if( size )
    {
    	::fwData::ValuedPoint::sptr lastPt = m_pickedPoints.lock()->getRefPoints()[size-1];
    	bool similarPoints = (lastPt->getRefCoord()[0] == index[0] && lastPt->getRefCoord()[1] == index[1] && lastPt->getRefCoord()[2] == index[2] );
    	if( similarPoints )
    	{
    		isValidPoint = false;
    	}
    }

    if( isValidPoint )
    {
    	::fwData::ValuedPoint::sptr point = ::fwData::ValuedPoint::New();
    	point->getRefCoord()[0] = index[0];
    	point->getRefCoord()[1] = index[1];
    	point->getRefCoord()[2] = index[2];
    	std::string  value = ::fwData::getPixelAsString(m_image.lock(),index[0],index[1],index[2]);

    	point->getRefValue() 	= ::boost::lexical_cast< double , std::string >(value);
    	m_pickedPoints.lock()->getRefPoints().push_back(point);
    }
}

void NegatoPointPicker::updateText(double pickedPoint[3]) throw(fwTools::Failed)
{
	if( !m_textAdaptor.expired() )
	{
		std::string text;
		int index[3];

		m_negato.lock()->updateImageInfos(m_image.lock());
		m_negato.lock()->worldToImageSliceIndex( pickedPoint, index );
		std::string  value = ::fwData::getPixelAsString(m_image.lock(),index[0],index[1],index[2]);

		std::stringstream ss;
        ss << "[" << index[0] << "," << index[1] << "," << index[2] << "] = " << value;
        text = ss.str();

        m_textAdaptor.lock()->setText(text);
        m_textAdaptor.lock()->update();
	}
}

void NegatoPointPicker::clearText() throw(fwTools::Failed)
{
	if( !m_textAdaptor.expired() )
	{
		std::string text;
		m_textAdaptor.lock()->setText(text);
		m_textAdaptor.lock()->update();
	}
}

//------------------------------------------------------------------------------
void NegatoPointPicker::notifyPicking()
{
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::PICKEDPOINTS );
    ::fwServices::IEditionService::notify( this->getSptr(), m_image.lock(), msg);
}
void NegatoPointPicker::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
}

void NegatoPointPicker::doSwap() throw(fwTools::Failed)
{
}


} //namespace visuVTKAdaptor
