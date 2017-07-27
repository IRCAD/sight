/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/ImagesProbeCursor.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

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

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::ImagesProbeCursor, ::fwData::Composite );

#define START_PROBE_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_PROBE_EVENT  vtkCommand::LeftButtonReleaseEvent

namespace visuVTKAdaptor
{

class ImagesProbingCallback : public vtkCommand
{
public:
    //------------------------------------------------------------------------------

    static ImagesProbingCallback* New()
    {
        return new ImagesProbingCallback();
    }

    ImagesProbingCallback() :
        m_priority(-1),
        m_mouseMoveObserved(false)
    {
        m_picker = NULL;
        this->PassiveObserverOff();
    }

    virtual ~ImagesProbingCallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
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
                    m_adaptor->StartImagesProbeCursor();
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

    void setAdaptor( ImagesProbeCursor::sptr adaptor)
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
    ImagesProbeCursor::sptr m_adaptor;
    vtkAbstractPropPicker* m_picker;
    float m_priority;

    bool m_mouseMoveObserved;

};

//------------------------------------------------------------------------------

ImagesProbeCursor::ImagesProbeCursor() noexcept :
    m_priority(.6),
    m_vtkObserver(nullptr),
    m_textActor(vtkActor2D::New()),
    m_textMapper(vtkTextMapper::New()),
    m_cursorPolyData( vtkPolyData::New() ),
    m_cursorMapper( vtkPolyDataMapper::New() ),
    m_cursorActor(    vtkActor::New() )
{
}

//------------------------------------------------------------------------------

ImagesProbeCursor::~ImagesProbeCursor() noexcept
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

void ImagesProbeCursor::setVisibility( bool visibility )
{
    m_textActor->SetVisibility(visibility);
    m_cursorActor->SetVisibility(visibility);
    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::doConfigure()
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");

    std::vector< ::fwRuntime::ConfigurationElement::sptr > configs = m_configuration->find("image");
    SLM_ASSERT("Missing tag 'image' ", !configs.empty());
    for(::fwRuntime::ConfigurationElement::sptr element :  configs)
    {
        SLM_ASSERT("Missing attribute 'objectId'", element->hasAttribute("objectId"));
        std::string objectId = element->getAttributeValue("objectId");
        std::string name     = objectId;
        if (element->hasAttribute("name"))
        {
            name = element->getAttributeValue("name");
        }

        m_imagesId.push_back(std::make_pair(objectId, name));
    }
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::buildTextActor()
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

void ImagesProbeCursor::doStart()
{
    buildTextActor();
    this->addToRenderer(m_textActor );

    buildPolyData();
    m_cursorMapper->SetInputData( m_cursorPolyData );
    m_cursorActor->SetMapper(m_cursorMapper);
    m_cursorActor->GetProperty()->SetColor(1, 0, 0);
    if(!this->getTransformId().empty())
    {
        m_cursorActor->SetUserTransform(this->getTransform());
    }
    this->addToRenderer(m_cursorActor);

    ImagesProbingCallback* observer = ImagesProbingCallback::New();
    observer->setAdaptor( ImagesProbeCursor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());
    observer->setPriority(  m_priority );

    m_vtkObserver = observer;

    this->getInteractor()->AddObserver(START_PROBE_EVENT, m_vtkObserver, m_priority);
    this->getInteractor()->AddObserver(STOP_PROBE_EVENT, m_vtkObserver, m_priority);
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::doUpdate()
{
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::doSwap()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::doStop()
{
    this->getInteractor()->RemoveObservers(START_PROBE_EVENT, m_vtkObserver);
    this->getInteractor()->RemoveObservers(STOP_PROBE_EVENT, m_vtkObserver);
    m_vtkObserver->Delete();
    m_vtkObserver = NULL;
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::StartImagesProbeCursor( )
{
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::updateView( double world[3] )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    std::stringstream txt;

    if (composite->find(m_imagesId.begin()->first) != composite->end())
    {
        ::fwData::Image::sptr image = ::fwData::Image::dynamicCast((*composite)[m_imagesId.begin()->first]);
        OSLM_ASSERT("Object '" << m_imagesId.begin()->first << "' must be an image", image);

        if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
        {
            this->updateImageInfos(image);

            int index[3];
            this->worldToImageSliceIndex( world, index );
            OSLM_TRACE("index=" << index[0] << "," << index[1] << "," << index[2] );

            if (    world[0] < image->getOrigin()[0] ||
                    world[1] < image->getOrigin()[1] ||
                    world[2] < image->getOrigin()[2] ||
                    index[0] < 0 || index[1] < 0 || index[2] < 0 ||
                    index[0] >= image->getSize()[0] ||
                    index[1] >= image->getSize()[1] ||
                    index[2] >= image->getSize()[2]
                    )
            {
                txt << "(---,---,---)" << std::endl;
            }
            else
            {
                ::fwDataTools::helper::Image imageHelper(image);
                txt << (::boost::format("(% 4li,% 4li,% 4li)") % index[0] % index[1] % index[2] ).str() << std::endl;

                // update polyData
                double worldCross[4][3];
                this->computeCrossExtremity( index, worldCross);

                vtkPoints* points = m_cursorPolyData->GetPoints();
                for ( int i = 0; i < 4; ++i)
                {
                    OSLM_TRACE("p=" << worldCross[i][0] << "," << worldCross[i][2] << "," << worldCross[i][2] << "," );
                    points->SetPoint(i, worldCross[i]);
                }
                m_cursorPolyData->Modified();
            }
        }
    }

    for(ImagesIdPair element :  m_imagesId)
    {
        if (composite->find(element.first) != composite->end())
        {
            ::fwData::Image::sptr image = ::fwData::Image::dynamicCast((*composite)[element.first]);
            OSLM_ASSERT("Object '" << element.first << "' must be an image", image);

            if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
            {
                ::fwDataTools::helper::Image imageHelper(image);
                this->updateImageInfos(image);

                int index[3];
                this->worldToImageSliceIndex( world, index );
                OSLM_TRACE("index=" << index[0] << "," << index[1] << "," << index[2] << "," );

                if ( !( world[0] < image->getOrigin()[0] ||
                        world[1] < image->getOrigin()[1] ||
                        world[2] < image->getOrigin()[2]  ||
                        index[0] < 0 || index[1] < 0 || index[2] < 0 ||
                        index[0] >= image->getSize()[0] ||
                        index[1] >= image->getSize()[1] ||
                        index[2] >= image->getSize()[2])
                     )
                {
                    std::string greyLevel = imageHelper.getPixelAsString(index[0], index[1], index[2] );
                    txt << element.second << " : " << greyLevel << std::endl;
                }
            }
        }
    }
    m_textMapper->SetInput(txt.str().c_str());

    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::computeCrossExtremity( const int probeSlice[3], double worldCross[4][3] )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    if (composite->find(m_imagesId.begin()->first) != composite->end())
    {
        ::fwData::Image::sptr image = ::fwData::Image::dynamicCast((*composite)[m_imagesId.begin()->first]);
        OSLM_ASSERT("Object '" << m_imagesId.begin()->first << "' must be an image", image);

        unsigned int sliceIndex[3]; // the current sliceIndex

        sliceIndex[2] = m_axialIndex->value();
        sliceIndex[1] = m_frontalIndex->value();
        sliceIndex[0] = m_sagittalIndex->value();

        double probeWorld[3]; // probe index in world positioning system
        for (int dim = 0; dim < 3; ++dim )
        {
            if ( probeSlice[dim] == sliceIndex[dim] ) // FIXME if (sliceIndex==probeWorld)
            {
                //setOrientation( (dim==2?2:(dim+1)%2) ); // KEEP Z but swap X,Y
                this->setOrientation(dim);
            }
            probeWorld[dim] = probeSlice[dim]*image->getSpacing()[dim] + image->getOrigin().at(dim);
        }

        for ( int p = 0; p < 2; ++p )
        {
            for (int dim = 0; dim < 3; ++dim )
            {
                worldCross[p][dim]   = probeWorld[dim];
                worldCross[p+2][dim] = probeWorld[dim];
                if ( (dim + p + 1)%3 == m_orientation )
                {
                    worldCross[p][dim]              = image->getOrigin().at(dim);
                    ::fwData::Image::IndexType size = image->getSize().at(dim)-1;
                    double spacing = image->getSpacing().at(dim);
                    worldCross[p+2][dim] = size * spacing + image->getOrigin().at(dim);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void ImagesProbeCursor::buildPolyData()
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

} //namespace visuVTKAdaptor
