/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "visuVTKAdaptor/SProbeCursor.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <boost/format.hpp>

#include <vtkAbstractPropPicker.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleImage.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SProbeCursor);

#define START_PROBE_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_PROBE_EVENT  vtkCommand::LeftButtonReleaseEvent

namespace visuVTKAdaptor
{

class ProbingCallback : public vtkCommand
{
public:
    //------------------------------------------------------------------------------

    static ProbingCallback* New()
    {
        return new ProbingCallback();
    }

    ProbingCallback() :
        m_picker(nullptr),
        m_priority(-1),
        m_mouseMoveObserved(false)
    {
        this->PassiveObserverOff();
    }

    ~ProbingCallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* /*caller*/, unsigned long eventId, void*)
    {
        assert(m_priority >= 0);
        SLM_ASSERT("m_adaptor not instanced", m_adaptor);
        SLM_ASSERT("m_picker not instanced", m_picker);
        if ( m_mouseMoveObserved || !m_adaptor->getInteractor()->GetShiftKey() )
        {
            if ( eventId == vtkCommand::MouseMoveEvent )
            {
                SLM_ASSERT("m_mouseMoveObserved not instanced", m_mouseMoveObserved);
                process();
            }
            else if ( eventId == START_PROBE_EVENT)
            {
                assert(m_mouseMoveObserved == false );
                if ( pickSomething() )
                {
                    m_mouseMoveObserved = true;
                    SetAbortFlag(1);
                    m_adaptor->setVisibility(true);
                    m_adaptor->startSProbeCursor();
                    process();
                    m_adaptor->getInteractor()->AddObserver(vtkCommand::MouseMoveEvent, this, m_priority);
                }
            }
            else if ( eventId == STOP_PROBE_EVENT && m_mouseMoveObserved)
            {
                SLM_ASSERT("m_mouseMoveObserved not instanced", m_mouseMoveObserved);
                m_adaptor->setVisibility(false);
                m_adaptor->getInteractor()->RemoveObservers(vtkCommand::MouseMoveEvent, this);
                m_mouseMoveObserved = false;
            }
        }
    }

    //------------------------------------------------------------------------------

    bool pickSomething()
    {
        int x, y;
        double display[3];

        m_adaptor->getInteractor()->GetEventPosition(x, y);
        display[0] = x;
        display[1] = y;
        display[2] = 0;

        return m_picker->Pick( display, m_adaptor->getRenderer() );
    }

    //------------------------------------------------------------------------------

    void process() // from
    {
        double world[3] = {-1, 0, 0};
        if ( pickSomething() )
        {
            ::fwRenderVTK::vtk::getNearestPickedPosition(m_picker, m_adaptor->getRenderer(), world);
            OSLM_TRACE("PICK" << world[0] << " ," << world[1] << " ," << world[2] );
            m_adaptor->updateView( world);
        }
        m_adaptor->updateView(world);
    }

    //------------------------------------------------------------------------------

    void setAdaptor( SProbeCursor::sptr adaptor)
    {
        m_adaptor = adaptor;
    }

    //------------------------------------------------------------------------------

    void setPicker( vtkAbstractPropPicker* adaptor)
    {
        m_picker = adaptor;
    }

    //------------------------------------------------------------------------------

    void setPriority( float priority )
    {
        m_priority = priority;
    }

protected:
    SProbeCursor::sptr m_adaptor;
    vtkAbstractPropPicker* m_picker;
    float m_priority;

    bool m_mouseMoveObserved;

};

const ::fwCom::Slots::SlotKeyType SProbeCursor::s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SProbeCursor::SProbeCursor() noexcept :
    m_priority(.6f),
    m_vtkObserver(nullptr),
    m_textActor(vtkActor2D::New()),
    m_textMapper(vtkTextMapper::New()),
    m_cursorPolyData( vtkPolyData::New() ),
    m_cursorMapper(vtkPolyDataMapper::New() ),
    m_cursorActor(vtkActor::New() )
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SProbeCursor::updateSliceIndex, this);
}

//------------------------------------------------------------------------------

SProbeCursor::~SProbeCursor() noexcept
{
    m_textMapper->Delete();
    m_textActor->Delete();

    m_cursorActor->Delete();
    m_cursorActor = nullptr;
    m_cursorMapper->Delete();
    m_cursorMapper = nullptr;
    m_cursorPolyData->Delete();
}

//------------------------------------------------------------------------------
void SProbeCursor::setVisibility( bool visibility )
{
    m_textActor->SetVisibility(visibility);
    m_cursorActor->SetVisibility(visibility);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SProbeCursor::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SProbeCursor::buildTextActor()
{
    vtkTextProperty* textprop = m_textMapper->GetTextProperty();
    textprop->SetColor(1, 1, 1);
    textprop->SetFontFamilyToArial();
    textprop->SetFontSize(20);
    textprop->BoldOn();
    textprop->ItalicOff();
    textprop->ShadowOn();
    textprop->SetJustificationToLeft();
    textprop->SetVerticalJustificationToTop();

    m_textActor->SetMapper( m_textMapper );

    vtkCoordinate* coord = m_textActor->GetPositionCoordinate();
    coord->SetCoordinateSystemToNormalizedViewport();
    coord->SetValue(.01, .98);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SProbeCursor::starting()
{
    this->initialize();

    this->buildTextActor();
    this->addToRenderer(m_textActor );

    this->buildPolyData();
    m_cursorMapper->SetInputData( m_cursorPolyData );
    m_cursorActor->SetMapper(m_cursorMapper);
    m_cursorActor->GetProperty()->SetColor(1, 0, 0);
    m_cursorActor->GetProperty()->SetOpacity(0.9);
    if(!this->getTransformId().empty())
    {
        m_cursorActor->SetUserTransform(this->getTransform());
    }
    this->addToRenderer(m_cursorActor);

    ProbingCallback* observer = ProbingCallback::New();
    observer->setAdaptor( SProbeCursor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());
    observer->setPriority(  m_priority );

    m_vtkObserver = observer;

    this->getInteractor()->AddObserver(START_PROBE_EVENT, m_vtkObserver, m_priority);
    this->getInteractor()->AddObserver(STOP_PROBE_EVENT, m_vtkObserver, m_priority);

    this->updating();
}

//------------------------------------------------------------------------------

void SProbeCursor::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    m_helper.updateImageInfos(image);
    this->setVisibility(false);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SProbeCursor::stopping()
{
    this->getInteractor()->RemoveObservers(START_PROBE_EVENT, m_vtkObserver);
    this->getInteractor()->RemoveObservers(STOP_PROBE_EVENT, m_vtkObserver);
    m_vtkObserver->Delete();
    m_vtkObserver = nullptr;
    this->removeAllPropFromRenderer();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SProbeCursor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    int pos[] {sagittal, frontal, axial};
    m_helper.setSliceIndex(pos);
}

//------------------------------------------------------------------------------

void SProbeCursor::startSProbeCursor( )
{
}

//------------------------------------------------------------------------------

void SProbeCursor::updateView( double world[3] )
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    int index[3];
    m_helper.worldToImageSliceIndex( world, index );
    OSLM_TRACE("index=" << index[0] << "," << index[1] << "," << index[2] << "," );

    std::string txt;

    if ( world[0] < image->getOrigin()[0] || world[1] < image->getOrigin()[1] || world[2] < image->getOrigin()[2]  ||
         index[0] < 0 || index[1] < 0 || index[2] < 0 ||
         index[0] >= static_cast<int>(image->getSize()[0]) ||
         index[1] >= static_cast<int>(image->getSize()[1]) ||
         index[2] >= static_cast<int>(image->getSize()[2])
         )
    {
        txt = "(---,---,---)";
        m_textMapper->SetInput(txt.c_str());
    }
    else
    {
        ::fwDataTools::helper::Image imageHelper(image);
        const size_t x              = static_cast<size_t>(index[0]);
        const size_t y              = static_cast<size_t>(index[1]);
        const size_t z              = static_cast<size_t>(index[2]);
        const std::string greyLevel = imageHelper.getPixelAsString(x, y, z);
        txt = (::boost::format("(% 4li,% 4li, % 4li) : %s ") % index[0] % index[1] % index[2] % greyLevel ).str();

        m_textMapper->SetInput(txt.c_str());

        // update polyData
        double worldCross[4][3];
        this->computeCrossExtremity(image, index, worldCross);

        vtkPoints* points = m_cursorPolyData->GetPoints();
        for ( int i = 0; i < 4; ++i)
        {
            OSLM_TRACE("p=" << worldCross[i][0] << "," << worldCross[i][2] << "," << worldCross[i][2] << "," );
            points->SetPoint(i, worldCross[i]);
        }
        m_cursorPolyData->Modified();
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SProbeCursor::computeCrossExtremity(::fwData::Image::csptr image, const int probeSlice[3],
                                         double worldCross[4][3] )
{
    int sliceIndex[3]; // the current sliceIndex

    ::fwData::Integer::sptr arr[3];
    m_helper.getSliceIndex(arr);

    sliceIndex[2] = arr[2]->value();
    sliceIndex[1] = arr[1]->value();
    sliceIndex[0] = arr[0]->value();

    double probeWorld[3]; // probe index in world positioning system
    for (unsigned int dim = 0; dim < 3; ++dim )
    {
        if ( probeSlice[dim] == sliceIndex[dim] ) // FIXME if (sliceIndex==probeWorld)
        {
            m_helper.setOrientation(static_cast<int>(dim));
        }
        probeWorld[dim] = probeSlice[dim]*image->getSpacing()[dim] + image->getOrigin().at(dim);
    }

    for (unsigned int p = 0; p < 2; ++p )
    {
        for (unsigned int dim = 0; dim < 3; ++dim )
        {
            worldCross[p][dim]   = probeWorld[dim];
            worldCross[p+2][dim] = probeWorld[dim];
            if ( (dim + p + 1)%3 == m_helper.getOrientation() )
            {
                worldCross[p][dim] = image->getOrigin().at(dim);
                const ::fwData::Image::SizeType::value_type size       = image->getSize().at(dim)-1;
                const ::fwData::Image::SpacingType::value_type spacing = image->getSpacing().at(dim);
                worldCross[p+2][dim] = size * spacing + image->getOrigin().at(dim);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SProbeCursor::buildPolyData()
{
    // point are stored Left,right,up,down
    int nbPoints      = 4;
    vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
    points->SetNumberOfPoints(nbPoints);
    int i;
    for (i = 0; i < nbPoints; i++)
    {
        points->SetPoint(i, 0.0, 0.0, 0.0);
    }

    vtkCellArray* cells = vtkCellArray::New();
    cells->Allocate(cells->EstimateSize(nbPoints, 2));

    vtkIdType pts[2];
    pts[0] = 0; pts[1] = 2;
    cells->InsertNextCell(2, pts);
    pts[0] = 1; pts[1] = 3;
    cells->InsertNextCell(2, pts);

    m_cursorPolyData->SetPoints(points);
    points->Delete();
    m_cursorPolyData->SetLines(cells);
    cells->Delete();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SProbeCursor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
