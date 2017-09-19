/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRenderVTK/vtk/fwVtkWheelRepresentation.hpp>

#include <fwServices/macros.hpp>

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkQuad.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <cmath>

vtkStandardNewMacro(fwVtkWheelRepresentation);

//------------------------------------------------------------------------------

fwVtkWheelRepresentation::fwVtkWheelRepresentation() :
    WheelPoints(vtkPoints::New()),
    WheelActor(vtkActor2D::New()),
    Orientation(0.),
    nSectors(360),
    nMarkedSectors(10)
{
    this->BuildRepresentation();
}

//------------------------------------------------------------------------------

fwVtkWheelRepresentation::~fwVtkWheelRepresentation()
{
    WheelPoints->Delete();
    WheelActor->Delete();
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::BuildRepresentation()
{
    /// Default viewport size used at initialization.
    /// Updated later when the viewport is.
    const int viewportWidth  = 512;
    const int viewportHeight = 512;
    this->Center.x = static_cast<double>(viewportWidth) / 2.0;
    this->Center.y = static_cast<double>(viewportHeight) / 2.0;

    double minViewportComp = std::min(viewportWidth, viewportHeight);
    this->WheelInnerRadius = 2.0 / 3.0 * minViewportComp / 2.0;
    this->WheelOuterRadius = this->WheelInnerRadius + 0.05 * minViewportComp;
    double outerMarkedRadius = this->WheelOuterRadius + 0.033 * minViewportComp;

    const double deltaAngle = 2.0 * ::glm::pi<double>() / static_cast<double>(this->nSectors);
    double initAngle        = -deltaAngle / 2.0 + Orientation;

    // Setup points

    // Define some colors
    double color[4] = {0, 0, 0, 255.};

    // Setup the colors array
    vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
    colors->SetNumberOfComponents(4);
    colors->SetName("Colors");

    // Create a triangle
    vtkCellArray* quads = vtkCellArray::New();

    // Add the points to a vtkPoints object
    this->WheelPoints = vtkPoints::New();

    double c, s;
    c = std::cos(initAngle + 0.0);
    s = std::sin(initAngle + 0.0);
    this->WheelPoints->InsertNextPoint(this->Center.x + this->WheelInnerRadius * c,
                                       this->Center.y + this->WheelInnerRadius * s, 0.0);
    this->WheelPoints->InsertNextPoint(this->Center.x + this->WheelOuterRadius * c,
                                       this->Center.y + this->WheelOuterRadius * s, 0.0);

    for(unsigned int i = 0; i < this->nSectors; i++)
    {
        c = std::cos(initAngle + static_cast<double>(i + 1) * deltaAngle);
        s = std::sin(initAngle + static_cast<double>(i + 1) * deltaAngle);

        this->WheelPoints->InsertNextPoint(this->Center.x + this->WheelInnerRadius * c,
                                           this->Center.y + this->WheelInnerRadius * s, 0.0);

        if(i % this->nMarkedSectors == 0 || i % this->nMarkedSectors == 1)
        {
            this->WheelPoints->InsertNextPoint(this->Center.x + outerMarkedRadius * c,
                                               this->Center.y + outerMarkedRadius * s, 0.0);
        }
        else
        {
            this->WheelPoints->InsertNextPoint(this->Center.x + this->WheelOuterRadius * c,
                                               this->Center.y + this->WheelOuterRadius * s, 0.0);
        }

        vtkQuad* quad = vtkQuad::New();

        quad->GetPointIds()->SetId(0, (i * 2) + 0);
        quad->GetPointIds()->SetId(1, (i * 2) + 1);
        quad->GetPointIds()->SetId(2, (i * 2) + 3);
        quad->GetPointIds()->SetId(3, (i * 2) + 2);

        quads->InsertNextCell(quad);

        double frequency = 2 * ::glm::pi<double>() / static_cast<double>(this->nSectors);

        color[0] = std::sin(frequency * i + 0) * 127. + 128.;
        color[1] = std::sin(frequency * i + 1.0 / 3.0 * 2 * ::glm::pi<double>()) * 127. + 128.;
        color[2] = std::sin(frequency * i + 2.0 / 3.0 * 2 * ::glm::pi<double>()) * 127. + 128.;
        colors->InsertNextTuple(color);
    }

    this->CenterOuterRadius = 0.1 * this->WheelOuterRadius;
    this->CenterInnerRadius = 0.3 * this->CenterOuterRadius;

    initAngle = -deltaAngle / 2.0;
    c         = std::cos(initAngle + 0.0);
    s         = std::sin(initAngle + 0.0);

    this->WheelPoints->InsertNextPoint(this->Center.x + this->CenterInnerRadius * c,
                                       this->Center.y + this->CenterInnerRadius * s, 0.0);
    this->WheelPoints->InsertNextPoint(this->Center.x + this->CenterOuterRadius * c,
                                       this->Center.y + this->CenterOuterRadius * s, 0.0);

    for(unsigned int i = this->nSectors + 1; i < this->nSectors * 2; i++)
    {
        c = std::cos(initAngle + static_cast<double>(i + 1) * deltaAngle);
        s = std::sin(initAngle + static_cast<double>(i + 1) * deltaAngle);

        this->WheelPoints->InsertNextPoint(this->Center.x + this->CenterInnerRadius * c,
                                           this->Center.y + this->CenterInnerRadius * s, 0.0);

        this->WheelPoints->InsertNextPoint(this->Center.x + this->CenterOuterRadius * c,
                                           this->Center.y + this->CenterOuterRadius * s, 0.0);

        vtkQuad* quad = vtkQuad::New();

        quad->GetPointIds()->SetId(0, (i * 2) + 0);
        quad->GetPointIds()->SetId(1, (i * 2) + 1);
        quad->GetPointIds()->SetId(2, (i * 2) + 3);
        quad->GetPointIds()->SetId(3, (i * 2) + 2);

        quads->InsertNextCell(quad);

        double frequency = 2 * ::glm::pi<double>() / static_cast<double>(this->nSectors);
        color[0] = std::sin(frequency * i + 0) * 127. + 128.;
        color[1] = std::sin(frequency * i + 1.0 / 3.0 * 2 * ::glm::pi<double>()) * 127. + 128.;
        color[2] = std::sin(frequency * i + 2.0 / 3.0 * 2 * ::glm::pi<double>()) * 127. + 128.;
        colors->InsertNextTuple(color);
    }

    // Create a polydata object and add everything to it.
    vtkPolyData* wheelPolyData = vtkPolyData::New();
    wheelPolyData->SetPoints(this->WheelPoints);
    wheelPolyData->SetPolys(quads);
    wheelPolyData->GetCellData()->SetScalars(colors);

    // Visualize
    vtkPolyDataMapper2D* wheelMapper = vtkPolyDataMapper2D::New();
    wheelMapper->SetInputData(wheelPolyData);
    this->WheelActor->SetMapper(wheelMapper);
    this->WheelActor->PickableOn();

    const ::glm::dvec2 actorPos(this->WheelActor->GetPosition()[0], this->WheelActor->GetPosition()[1]);

    this->WidgetToCenterTranslation = this->Center - actorPos;
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::UpdateRepresentation()
{
    if ( this->GetMTime() > this->BuildTime ||
         (this->Renderer && this->Renderer->GetVTKWindow() &&
          this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime) )
    {
        const auto viewportSize  = this->GetRenderer()->GetRenderWindow()->GetSize();
        const int viewportWidth  = viewportSize[0];
        const int viewportHeight = viewportSize[1];

        // Clamp actor position to ensure that the widget center stays inside the viewport.
        ::glm::dvec2 actorPos(this->WheelActor->GetPosition()[0], this->WheelActor->GetPosition()[1]);

        actorPos.x = ::glm::clamp(actorPos.x, -this->WidgetToCenterTranslation.x,
                                  static_cast<double>(viewportWidth) - this->WidgetToCenterTranslation.x);
        actorPos.y = ::glm::clamp(actorPos.y, -this->WidgetToCenterTranslation.y,
                                  static_cast<double>(viewportHeight) - this->WidgetToCenterTranslation.y);

        this->WheelActor->SetPosition(actorPos.x, actorPos.y);

        // Update the outer wheel by modifying all its points.
        const double deltaAngle = 2.0 * ::glm::pi<double>() / static_cast<double>(this->nSectors);
        double initAngle        = -deltaAngle / 2.0 + Orientation;

        double minViewportComp = std::min(viewportWidth, viewportHeight);
        this->WheelInnerRadius = 2.0 / 3.0 * minViewportComp / 2.0;
        this->WheelOuterRadius = this->WheelInnerRadius + 0.05 * minViewportComp;
        double outerMarkedRadius = this->WheelOuterRadius + 0.033 * minViewportComp;

        double c, s;
        c = std::cos(initAngle + 0.0);
        s = std::sin(initAngle + 0.0);

        this->WheelPoints->SetPoint(0, this->Center.x + this->WheelInnerRadius * c,
                                    this->Center.y + this->WheelInnerRadius * s, 0.0);
        this->WheelPoints->SetPoint(1, this->Center.x + this->WheelOuterRadius * c,
                                    this->Center.y + this->WheelOuterRadius * s, 0.0);

        for(unsigned int i = 0; i < this->nSectors; i++)
        {
            c = std::cos(initAngle + static_cast<double>(i + 1) * deltaAngle);
            s = std::sin(initAngle + static_cast<double>(i + 1) * deltaAngle);

            const unsigned int j = i + 1;
            this->WheelPoints->SetPoint(j*2, this->Center.x + this->WheelInnerRadius * c,
                                        this->Center.y + this->WheelInnerRadius * s, 0.0);

            if(i % this->nMarkedSectors == 0 || i % this->nMarkedSectors == 1)
            {
                this->WheelPoints->SetPoint(j*2 + 1, this->Center.x + outerMarkedRadius * c,
                                            this->Center.y + outerMarkedRadius * s, 0.0);
            }
            else
            {
                this->WheelPoints->SetPoint(j*2 + 1, this->Center.x + this->WheelOuterRadius * c,
                                            this->Center.y + this->WheelOuterRadius * s, 0.0);
            }
        }

        // Update the center.
        this->CenterOuterRadius = 0.1 * this->WheelOuterRadius;
        this->CenterInnerRadius = 0.3 * this->CenterOuterRadius;

        initAngle = -deltaAngle / 2.0;
        c         = std::cos(initAngle + 0.0);
        s         = std::sin(initAngle + 0.0);

        this->WheelPoints->SetPoint((this->nSectors + 1) * 2, this->Center.x + this->CenterInnerRadius * c,
                                    this->Center.y + this->CenterInnerRadius * s, 0.0);

        this->WheelPoints->SetPoint((this->nSectors + 1) * 2 + 1, this->Center.x + this->CenterOuterRadius * c,
                                    this->Center.y + this->CenterOuterRadius * s, 0.0);

        for(unsigned int i = this->nSectors + 1; i < this->nSectors * 2; i++)
        {
            c = std::cos(initAngle + static_cast<double>(i + 1) * deltaAngle);
            s = std::sin(initAngle + static_cast<double>(i + 1) * deltaAngle);

            const unsigned int j = i + 1;

            this->WheelPoints->SetPoint(j * 2, this->Center.x + this->CenterInnerRadius * c,
                                        this->Center.y + this->CenterInnerRadius * s, 0.0);

            this->WheelPoints->SetPoint(j * 2 + 1, this->Center.x + this->CenterOuterRadius * c,
                                        this->Center.y + this->CenterOuterRadius * s, 0.0);
        }

        this->WheelPoints->Modified();
        this->BuildTime.Modified();
    }
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::GetActors2D(vtkPropCollection* pc)
{
    pc->AddItem(this->WheelActor);
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::ReleaseGraphicsResources(vtkWindow* w)
{
    this->WheelActor->ReleaseGraphicsResources(w);
    this->Superclass::ReleaseGraphicsResources(w);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::RenderOverlay(vtkViewport* v)
{
    this->UpdateRepresentation();
    if(!this->WheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->WheelActor->RenderOverlay(v);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::RenderOpaqueGeometry(vtkViewport* w)
{
    this->UpdateRepresentation();
    if ( !this->WheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->WheelActor->RenderOpaqueGeometry(w);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport* w)
{
    this->UpdateRepresentation();
    if ( !this->WheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->WheelActor->RenderTranslucentPolygonalGeometry(w);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::HasTranslucentPolygonalGeometry()
{
    this->UpdateRepresentation();
    if ( !this->WheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->WheelActor->HasTranslucentPolygonalGeometry();
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::SetOrientation(double orientation)
{
    this->Orientation = ::glm::mod(orientation, ::glm::two_pi<double>());
    this->Modified();
    this->UpdateRepresentation();
}

//------------------------------------------------------------------------------

glm::dvec2 fwVtkWheelRepresentation::GetCenterInScreenSpace() const
{
    const ::glm::dvec2 actorPos(this->WheelActor->GetPosition()[0], this->WheelActor->GetPosition()[1]);
    return actorPos + this->WidgetToCenterTranslation;
}

//------------------------------------------------------------------------------

bool fwVtkWheelRepresentation::isInCenter(int X, int Y) const
{
    const ::glm::dvec2 actorPos(this->WheelActor->GetPosition()[0], this->WheelActor->GetPosition()[1]);

    const ::glm::dvec2 delta = ::glm::dvec2(X, Y) - (actorPos + this->WidgetToCenterTranslation);

    const double squaredDistance = delta.x * delta.x + delta.y * delta.y;

    const double squaredRadius = this->CenterOuterRadius * this->CenterOuterRadius;

    return (squaredDistance < squaredRadius);
}

//------------------------------------------------------------------------------

bool fwVtkWheelRepresentation::isOnWheel(int X, int Y) const
{
    const ::glm::dvec2 actorPos(this->WheelActor->GetPosition()[0], this->WheelActor->GetPosition()[1]);

    const ::glm::dvec2 delta = ::glm::dvec2(X, Y) - (actorPos + this->WidgetToCenterTranslation);

    const double squaredDistance = delta.x * delta.x + delta.y * delta.y;

    const double squaredOuterRadius = this->WheelOuterRadius * this->WheelOuterRadius;
    const double squaredInnerRadius = this->WheelInnerRadius * this->WheelInnerRadius;

    return (squaredDistance < squaredOuterRadius) && (squaredDistance > squaredInnerRadius);
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::PrintSelf(ostream&, vtkIndent)
{
    SLM_FATAL("Method not implemented.");
}

//------------------------------------------------------------------------------
