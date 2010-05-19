/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <vtkPoints.h>
#include <vtkProp3DCollection.h>
#include <vtkPicker.h>
#include <vtkCellPicker.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkIO/vtk.hpp>

#include "visuVTKAdaptor/NegatoPickingCallBack.hpp"

namespace visuVTKAdaptor
{

NegatoOneSlicePickingCallback * NegatoOneSlicePickingCallback::New()
{
    return new NegatoOneSlicePickingCallback();
}

NegatoOneSlicePickingCallback::NegatoOneSlicePickingCallback()
{
    m_picker    = vtkAbstractPropPicker::SafeDownCast(vtkCellPicker::New());
    m_vtkImage    = vtkImageData::New();
    this->PassiveObserverOff();
}

NegatoOneSlicePickingCallback::~NegatoOneSlicePickingCallback()
{
    m_picker->Delete();
    m_picker = NULL;
    m_vtkImage->Delete();
    m_vtkImage = NULL;
}

void NegatoOneSlicePickingCallback::showInformations()
{
    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl ;
    std::cout << "GENERAL INFORMATIONS: " << std::endl ;
    ///////////////////////////
    //Pickable elements
    ///////////////////////////
    vtkPropCollection* pickableCollection = m_picker->GetPickList();
    vtkProp *pickageElement;
    pickableCollection->InitTraversal();
    std::cout << "Pickable collection : " ;
    while ( (pickageElement = pickableCollection->GetNextProp()) )
    {
        std::cout << "Element " << pickageElement << " - " ;
    }
    std::cout << std::endl;
    ///////////////////////////
    //Negatoscope Elements
    ///////////////////////////
    vtkPropCollection *adaptorCollection = vtkPropCollection::New();
    m_adaptor.lock()->getAllSubProps(adaptorCollection,-1);
    adaptorCollection->InitTraversal();
    vtkProp *adaptorElement;
    std::cout << "Negatoscope (associated adaptor) elements : " ;
    while ( (adaptorElement = adaptorCollection->GetNextProp()) )
    {
        std::cout << "Element " << adaptorElement << " - " ;
    }
    std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl ;
}

void NegatoOneSlicePickingCallback::Execute( vtkObject *caller, unsigned long eventId, void *)
{
    unsigned long     vtkEventId = eventId ;
    double             intensity = 0 ;
    int                index[3];
    double             world[3];
    bool             isPickingValid = false;

//    std::cout << (vtkEventId == vtkCommand::LeftButtonPressEvent ? "vtkCommand::LeftButtonPressEvent" : "Not LeftButtonPressEvent" ) << std::endl;
//    std::cout << (vtkEventId == vtkCommand::LeftButtonReleaseEvent ? "vtkCommand::LeftButtonReleaseEvent" : "Not LeftButtonReleaseEvent" ) << std::endl;
//    std::cout << (vtkEventId == vtkCommand::MouseMoveEvent ? "vtkCommand::MouseMoveEvent" : "Not MouseMoveEvent" ) << std::endl;
//
//    if( vtkEventId == vtkCommand::LeftButtonPressEvent )
//    {
//        SetAbortFlag(1);
//    }
    // For monitoring
//    this->showInformations();

    //Current active coordinates on (2D) screen
    int x,y;
    double display[3];
    m_adaptor.lock()->getInteractor()->GetEventPosition(x,y);
    display[0] = x; display[1] = y; display[2] = 0;

//    std::cout << "Display picked point : x = " << display[0] << " - y = " << display[1] << " - z = " << display[2] << std::endl;
    //Search picked elements
    vtkPicker *picker = vtkPicker::SafeDownCast(m_picker);
    assert( picker );
    if( picker->Pick( display , m_adaptor.lock()->getRenderer() ) )
    {
        vtkProp3DCollection *pickedCollection = picker->GetProp3Ds();
        vtkProp *pickedElement;
        ///Return a list of the points the the actors returned by GetProp3Ds were intersected at. The order of this list will match the order of GetProp3Ds.
        vtkPoints * points = picker->GetPickedPositions();
        vtkIdType id=0;
        pickedCollection->InitTraversal();
        while ( (pickedElement = pickedCollection->GetNextProp()) )
        {
            if( pickedElement == (vtkProp *)(m_adaptor.lock()->getImageActor()) )
            {
                isPickingValid = true;
                points->GetPoint (id, world ); ++id;
                m_adaptor.lock()->worldToImageSliceIndex(world,index);


                //Does not work: invalid vtkImage ????
                //intensity = m_adaptor.lock()->getImageActor()->GetInput()->GetScalarComponentAsDouble( index[0],index[1],index[2],0);
                //Should probably be done less frequently!!!
                ::vtkIO::toVTKImage(m_adaptor.lock()->getObject< ::fwData::Image >() ,m_vtkImage);
                intensity = m_vtkImage->GetScalarComponentAsDouble( index[0],index[1],index[2],0);

//                    std::cout     << "Picked element (" << pickedElement << ") : ["
//                            << world[0] << "," << world[1] << "," << world[2] << "]"
//                            << " <=> [" << index[0] << "," << index[1] << "," << index[2] << "] value = " << intensity;

            }
        }
    }
//        else
//        {
//            std::cout << "Nothing picked" << std::endl;
//        }
    //Now, we process picking
    m_pickingProcessor.lock()->updatePickedIndex(index,intensity);
    m_pickingProcessor.lock()->updateWorldPickedPoint(world);
    m_pickingProcessor.lock()->updatePickingEventId(vtkEventId);
    m_pickingProcessor.lock()->updatePickingValidity(isPickingValid);
    m_pickingProcessor.lock()->processPicking();
}

void NegatoOneSlicePickingCallback::setContext( ::visuVTKAdaptor::NegatoOneSlice::wptr adaptor,::visuVTKAdaptor::INegatoInteractor::wptr processor)
{
    assert( m_picker );

    m_adaptor = adaptor;
    m_pickingProcessor = processor;

    m_picker->InitializePickList();
    m_picker->PickFromListOn();
    vtkPicker::SafeDownCast(m_picker)->SetTolerance(0);
    this->PassiveObserverOff();
    m_picker->AddPickList((vtkProp *)(m_adaptor.lock()->getImageActor()));

    assert( !m_adaptor.expired() );
    assert( !m_pickingProcessor.expired() );
    assert( m_picker );
}

}
