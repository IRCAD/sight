/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImagesProbeCursor.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>
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

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImagesProbeCursor);

#define START_PROBE_EVENT vtkCommand::LeftButtonPressEvent
#define STOP_PROBE_EVENT  vtkCommand::LeftButtonReleaseEvent

namespace visuVTKAdaptor
{

static const ::fwServices::IService::KeyType s_IMAGE_GROUP = "image";

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
                    m_adaptor->StartSImagesProbeCursor();
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

    void setAdaptor( SImagesProbeCursor::sptr adaptor)
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
    SImagesProbeCursor::sptr m_adaptor;
    vtkAbstractPropPicker* m_picker;
    float m_priority;

    bool m_mouseMoveObserved;

};

//------------------------------------------------------------------------------

SImagesProbeCursor::SImagesProbeCursor() noexcept :
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

SImagesProbeCursor::~SImagesProbeCursor() noexcept
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

void SImagesProbeCursor::setVisibility( bool visibility )
{
    m_textActor->SetVisibility(visibility);
    m_cursorActor->SetVisibility(visibility);
    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.inout");

    SLM_ASSERT("configured group must be '" + s_IMAGE_GROUP + "'",
               config.get<std::string>("<xmlattr>.group", "") == s_IMAGE_GROUP);

    BOOST_FOREACH(const ::fwServices::IService::ConfigType::value_type &v, config.equal_range("key"))
    {
        const ::fwServices::IService::ConfigType& specAssoc = v.second;
        const ::fwServices::IService::ConfigType& attr      = specAssoc.get_child("<xmlattr>");
        const std::string name                              = attr.get("name", "");
        m_imagesNames.push_back(name);
    }
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::buildTextActor()
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

void SImagesProbeCursor::starting()
{
    this->initialize();

    this->buildTextActor();
    this->addToRenderer(m_textActor );

    this->buildPolyData();
    m_cursorMapper->SetInputData( m_cursorPolyData );
    m_cursorActor->SetMapper(m_cursorMapper);
    m_cursorActor->GetProperty()->SetColor(1, 0, 0);

    if(!this->getTransformId().empty())
    {
        m_cursorActor->SetUserTransform(this->getTransform());
    }
    this->addToRenderer(m_cursorActor);

    ImagesProbingCallback* observer = ImagesProbingCallback::New();
    observer->setAdaptor( SImagesProbeCursor::dynamicCast(this->getSptr()) );
    observer->setPicker(this->getPicker());
    observer->setPriority(  m_priority );

    m_vtkObserver = observer;

    this->getInteractor()->AddObserver(START_PROBE_EVENT, m_vtkObserver, m_priority);
    this->getInteractor()->AddObserver(STOP_PROBE_EVENT, m_vtkObserver, m_priority);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::updating()
{
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::stopping()
{
    this->getInteractor()->RemoveObservers(START_PROBE_EVENT, m_vtkObserver);
    this->getInteractor()->RemoveObservers(STOP_PROBE_EVENT, m_vtkObserver);
    m_vtkObserver->Delete();
    m_vtkObserver = NULL;
    this->removeAllPropFromRenderer();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::StartSImagesProbeCursor( )
{
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::updateView( double world[3] )
{
    const size_t nbImages = this->getKeyGroupSize(s_IMAGE_GROUP);
    SLM_ASSERT("There must be as much images as names", nbImages == m_imagesNames.size());

    std::stringstream txt;

    ::fwData::Image::sptr firstImage = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, 0);
    if (firstImage)
    {
        if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(firstImage))
        {
            this->updateImageInfos(firstImage);

            int index[3];
            this->worldToImageSliceIndex( world, index );
            OSLM_TRACE("index=" << index[0] << "," << index[1] << "," << index[2] );

            if (    world[0] < firstImage->getOrigin()[0] ||
                    world[1] < firstImage->getOrigin()[1] ||
                    world[2] < firstImage->getOrigin()[2] ||
                    index[0] < 0 || index[1] < 0 || index[2] < 0 ||
                    index[0] >= firstImage->getSize()[0] ||
                    index[1] >= firstImage->getSize()[1] ||
                    index[2] >= firstImage->getSize()[2]
                    )
            {
                txt << "(---,---,---)" << std::endl;
            }
            else
            {
                ::fwDataTools::helper::Image imageHelper(firstImage);
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

    for(size_t i = 0; i < nbImages; ++i)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, i);
        if (image)
        {
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
                    txt << m_imagesNames[i] << " : " << greyLevel << std::endl;
                }
            }
        }
    }
    m_textMapper->SetInput(txt.str().c_str());

    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::computeCrossExtremity( const int probeSlice[3], double worldCross[4][3] )
{
    ::fwData::Image::sptr firstImage = this->getInOut< ::fwData::Image >(s_IMAGE_GROUP, 0);
    if (firstImage)
    {
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
            probeWorld[dim] = probeSlice[dim]*firstImage->getSpacing()[dim] + firstImage->getOrigin().at(dim);
        }

        for ( int p = 0; p < 2; ++p )
        {
            for (int dim = 0; dim < 3; ++dim )
            {
                worldCross[p][dim]   = probeWorld[dim];
                worldCross[p+2][dim] = probeWorld[dim];
                if ( (dim + p + 1)%3 == m_orientation )
                {
                    worldCross[p][dim]              = firstImage->getOrigin().at(dim);
                    ::fwData::Image::IndexType size = firstImage->getSize().at(dim)-1;
                    double spacing = firstImage->getSpacing().at(dim);
                    worldCross[p+2][dim] = size * spacing + firstImage->getOrigin().at(dim);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SImagesProbeCursor::buildPolyData()
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
