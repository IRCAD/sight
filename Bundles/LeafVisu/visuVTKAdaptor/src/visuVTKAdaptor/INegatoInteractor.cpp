/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <vtkRenderWindowInteractor.h>

#include "visuVTKAdaptor/NegatoPickingCallBack.hpp"
#include "visuVTKAdaptor/INegatoInteractor.hpp"

namespace visuVTKAdaptor
{

INegatoInteractor::INegatoInteractor() throw()
{
    m_isPickingInside     = false;
    m_isCtrlPressed     = false ;
    m_isShiftPressed     = false ;
    m_keyCode             = '\0' ;
    m_pickingCallBack     = NegatoOneSlicePickingCallback::New();
    m_priority            = 0.6;
    m_valuedIndex                = ::fwData::ValuedPoint::New();
    m_worldPoint        = ::fwData::Point::New();
}

INegatoInteractor::~INegatoInteractor() throw()
{
    m_pickingCallBack->Delete();
    m_pickingCallBack = NULL;
}
//-----------------------------------------------------------------------------

void INegatoInteractor::starting() throw ( ::fwTools::Failed )
{
    ///Retrieval of the negatoscope on which interaction is based
    m_adaptor = this->getObject< ::visuVTKAdaptor::NegatoOneSlice >() ;
    assert( !m_adaptor.expired() );
    m_pickingCallBack->setContext(m_adaptor,::visuVTKAdaptor::INegatoInteractor::dynamicCast(this->getSptr()));
    this->doStart();
}

//-----------------------------------------------------------------------------

void INegatoInteractor::stopping() throw ( ::fwTools::Failed )
{
    this->doStop();
}

//-----------------------------------------------------------------------------

void INegatoInteractor::configuring() throw ( ::fwTools::Failed )
{
    this->doConfigure();
}

//-----------------------------------------------------------------------------

void INegatoInteractor::updating() throw ( ::fwTools::Failed )
{
    this->doUpdate();
}

//-----------------------------------------------------------------------------

void INegatoInteractor::updating( fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    this->doUpdate(_msg);
}

//-----------------------------------------------------------------------------

void INegatoInteractor::processPicking() throw(fwTools::Failed)
{
    m_isCtrlPressed = m_adaptor.lock()->getInteractor()->GetControlKey() ;
    m_isShiftPressed = m_adaptor.lock()->getInteractor()->GetShiftKey() ;
    m_keyCode = m_adaptor.lock()->getInteractor()->GetKeyCode() ;

    this->doProcessPicking();
}


void INegatoInteractor::updatePickedIndex(int _index[3],double _value)
{
    m_valuedIndex->getRefCoord()[0]     = _index[0];
    m_valuedIndex->getRefCoord()[1]     = _index[1];
    m_valuedIndex->getRefCoord()[2]     = _index[2];
    m_valuedIndex->getRefValue()         = _value ;
}

void INegatoInteractor::updatePickingEventId(unsigned long eventId)
{
    m_vtkEventId = eventId;
}

void INegatoInteractor::updatePickingValidity(bool isOnNegato)
{
    m_isPickingInside = isOnNegato;
}

void INegatoInteractor::updateWorldPickedPoint(double _point[3])
{
    m_worldPoint->getRefCoord()[0] = _point[0];
    m_worldPoint->getRefCoord()[1] = _point[1];
    m_worldPoint->getRefCoord()[2] = _point[2];
}

::fwComEd::helper::MedicalImageAdaptor::Orientation INegatoInteractor::getOrientation()
{
	::fwComEd::helper::MedicalImageAdaptor::Orientation orientation = this->m_adaptor.lock()->getOrientation();
    return orientation;
}

::visuVTKAdaptor::INegatoInteractor::TAxis    INegatoInteractor::getPlaneAxis()
{
    ::visuVTKAdaptor::INegatoInteractor::TAxis indices;
    ::fwComEd::helper::MedicalImageAdaptor::Orientation    orientation = this->getOrientation();
    switch( orientation )
    {
    case ::fwComEd::helper::MedicalImageAdaptor::X_AXIS :
                                                        indices = ::visuVTKAdaptor::INegatoInteractor::TAxis(1,2);
                                                        break;
    case ::fwComEd::helper::MedicalImageAdaptor::Y_AXIS :
                                                        indices = ::visuVTKAdaptor::INegatoInteractor::TAxis(0,2);
                                                        break;
    case ::fwComEd::helper::MedicalImageAdaptor::Z_AXIS :
                                                        indices = ::visuVTKAdaptor::INegatoInteractor::TAxis(0,1);
                                                        break;
    default : assert(false);
    }
    assert( 0 <= indices.first && indices.first <=2 );
    assert( 0 <= indices.second && indices.second <=2 );

    return indices;
}

::fwData::Image::wptr INegatoInteractor::getImage()
{
    return this->m_adaptor.lock()->getObject< ::fwData::Image >();
}

}
