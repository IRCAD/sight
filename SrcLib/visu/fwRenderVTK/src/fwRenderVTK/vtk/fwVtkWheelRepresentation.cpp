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
    m_wheelPoints(vtkPoints::New()),
    m_wheelActor(vtkActor2D::New()),
    m_colors(vtkUnsignedCharArray::New()),
    m_orientation(0.),
    m_nSectors(280),
    m_nMarkedSectors(8),
    m_rainbowMode(false),
    m_hover(false)
{
    this->m_center.x = -std::numeric_limits<double>::infinity();
    this->m_center.y = -std::numeric_limits<double>::infinity();

    this->BuildRepresentation();
}

//------------------------------------------------------------------------------

fwVtkWheelRepresentation::~fwVtkWheelRepresentation()
{
    m_wheelPoints->Delete();
    m_wheelActor->Delete();
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::BuildRepresentation()
{
    // Setup points

    // Define some colors
    double color[4] = {0., 0., 0., 0.};

    // Setup the colors array
    this->m_colors->SetNumberOfComponents(4);
    this->m_colors->SetName("Colors");

    // Create a triangle
    vtkCellArray* quads = vtkCellArray::New();

    // Add the points to a vtkPoints object
    this->m_wheelPoints = vtkPoints::New();

    // Add quads and corresponding colors
    for(unsigned int i = 0; i < this->m_nSectors * 2; i++)
    {
        vtkQuad* quad = vtkQuad::New();

        quad->GetPointIds()->SetId(0, (i * 2) + 0);
        quad->GetPointIds()->SetId(1, (i * 2) + 1);
        quad->GetPointIds()->SetId(2, (i * 2) + 3);
        quad->GetPointIds()->SetId(3, (i * 2) + 2);

        quads->InsertNextCell(quad);

        this->m_colors->InsertNextTuple(color);
    }

    // Add points corresponding to the previous quads (4 points per quad)
    for(unsigned int i = 0; i < (this->m_nSectors + 1) * 4; i++)
    {
        this->m_wheelPoints->InsertNextPoint(0, 0, 0.0);
    }

    // Create a polydata object and add everything to it.
    vtkPolyData* wheelPolyData = vtkPolyData::New();
    wheelPolyData->SetPoints(this->m_wheelPoints);
    wheelPolyData->SetPolys(quads);
    wheelPolyData->GetCellData()->SetScalars(this->m_colors);

    // Visualize
    vtkPolyDataMapper2D* wheelMapper = vtkPolyDataMapper2D::New();
    wheelMapper->SetInputData(wheelPolyData);
    this->m_wheelActor->SetMapper(wheelMapper);
    this->m_wheelActor->PickableOn();

    const ::glm::dvec2 actorPos(this->m_wheelActor->GetPosition()[0], this->m_wheelActor->GetPosition()[1]);
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
        ::glm::dvec2 actorPos(this->m_wheelActor->GetPosition()[0], this->m_wheelActor->GetPosition()[1]);

        if(this->m_center.x == -std::numeric_limits<double>::infinity() &&
           this->m_center.y == -std::numeric_limits<double>::infinity() )
        {
            this->m_center.x                  = static_cast<double>(viewportWidth) / 2.0;
            this->m_center.y                  = static_cast<double>(viewportHeight) / 2.0;
            this->m_widgetToCenterTranslation = this->m_center - actorPos;
        }

        actorPos.x = ::glm::clamp(actorPos.x, -this->m_widgetToCenterTranslation.x,
                                  static_cast<double>(viewportWidth) - this->m_widgetToCenterTranslation.x);
        actorPos.y = ::glm::clamp(actorPos.y, -this->m_widgetToCenterTranslation.y,
                                  static_cast<double>(viewportHeight) - this->m_widgetToCenterTranslation.y);

        this->m_wheelActor->SetPosition(actorPos.x, actorPos.y);

        // Update the outer wheel by modifying all its points.
        const double deltaAngle = 2.0 * ::glm::pi<double>() / static_cast<double>(this->m_nSectors);
        double initAngle        = -deltaAngle / 2.0 + m_orientation;

        double minViewportComp = std::min(viewportWidth, viewportHeight);
        this->m_wheelInnerRadius = 2.0 / 3.0 * minViewportComp / 2.0;
        this->m_wheelOuterRadius = this->m_wheelInnerRadius + 0.02 * minViewportComp;
        double outerMarkedRadius = this->m_wheelOuterRadius + 0.01 * minViewportComp;

        double c, s;
        c = std::cos(initAngle + 0.0);
        s = std::sin(initAngle + 0.0);

        // Set position to the two first points ...
        this->m_wheelPoints->SetPoint(0,  this->m_center.x + this->m_wheelInnerRadius * c,
                                      this->m_center.y + this->m_wheelInnerRadius * s, 0.0);
        this->m_wheelPoints->SetPoint(1,  this->m_center.x + this->m_wheelOuterRadius * c,
                                      this->m_center.y + this->m_wheelOuterRadius * s, 0.0);

        // ... then, define two second points and corresponding colors of the previously created quad
        for(unsigned int i = 0; i < this->m_nSectors; i++)
        {
            c = std::cos(initAngle + static_cast<double>(i + 1) * deltaAngle);
            s = std::sin(initAngle + static_cast<double>(i + 1) * deltaAngle);

            const unsigned int j = i + 1;
            this->m_wheelPoints->SetPoint(j*2, this->m_center.x + this->m_wheelInnerRadius * c,
                                          this->m_center.y + this->m_wheelInnerRadius * s, 0.0);

            // Difference between small and big tick of the wheel
            if(i % this->m_nMarkedSectors == 5 || i % this->m_nMarkedSectors == 6)
            {
                this->m_wheelPoints->SetPoint(j*2 + 1, this->m_center.x + outerMarkedRadius * c,
                                              this->m_center.y + outerMarkedRadius * s, 0.0);
            }
            else
            {
                this->m_wheelPoints->SetPoint(j*2 + 1, this->m_center.x + this->m_wheelOuterRadius * c,
                                              this->m_center.y + this->m_wheelOuterRadius * s, 0.0);
            }

            // Display one out of two quads
            if(i % 2 == 0)
            {
                double color[4] = {70., 100., 140., 192.};
                if(m_hover)
                {
                    color[0] = 255.;
                    color[1] = 255.;
                    color[2] = 255.;
                    color[3] = 255.;
                }
                this->m_colors->SetTuple(i, color);
            }
        }

        // Update the center.
        this->m_centerOuterRadius = 0.05 * this->m_wheelOuterRadius;
        this->m_centerInnerRadius = 0.2 * this->m_centerOuterRadius;

        initAngle = -deltaAngle / 2.0;
        c         = std::cos(initAngle + 0.0);
        s         = std::sin(initAngle + 0.0);

        // Same definition for the small inside wheel
        this->m_wheelPoints->SetPoint((this->m_nSectors + 1) * 2, this->m_center.x + this->m_centerInnerRadius * c,
                                      this->m_center.y + this->m_centerInnerRadius * s, 0.0);

        this->m_wheelPoints->SetPoint((this->m_nSectors + 1) * 2 + 1, this->m_center.x + this->m_centerOuterRadius * c,
                                      this->m_center.y + this->m_centerOuterRadius * s, 0.0);

        double holeThickness = 0.15;

        for(unsigned int i = this->m_nSectors + 1; i < this->m_nSectors * 2; i++)
        {
            c = std::cos(initAngle + static_cast<double>(i + 1) * deltaAngle);
            s = std::sin(initAngle + static_cast<double>(i + 1) * deltaAngle);

            const unsigned int j = i + 1;

            this->m_wheelPoints->SetPoint(j * 2, this->m_center.x + this->m_centerInnerRadius * c,
                                          this->m_center.y + this->m_centerInnerRadius * s, 0.0);

            this->m_wheelPoints->SetPoint(j * 2 + 1, this->m_center.x + this->m_centerOuterRadius * c,
                                          this->m_center.y + this->m_centerOuterRadius * s, 0.0);

            if( !(c < holeThickness && c > -holeThickness) &&
                !(s < holeThickness && s > -holeThickness) )
            {
                double color[4] = {70., 100., 140., 192.};
                if(m_hover)
                {
                    color[0] = 255.;
                    color[1] = 255.;
                    color[2] = 255.;
                    color[3] = 255.;
                }
                this->m_colors->SetTuple(i, color);
            }
        }

        this->m_wheelPoints->Modified();
        this->BuildTime.Modified();
    }
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::GetActors2D(vtkPropCollection* pc)
{
    pc->AddItem(this->m_wheelActor);
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::ReleaseGraphicsResources(vtkWindow* w)
{
    this->m_wheelActor->ReleaseGraphicsResources(w);
    this->Superclass::ReleaseGraphicsResources(w);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::RenderOverlay(vtkViewport* v)
{
    this->UpdateRepresentation();
    if(!this->m_wheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->m_wheelActor->RenderOverlay(v);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::RenderOpaqueGeometry(vtkViewport* w)
{
    this->UpdateRepresentation();
    if ( !this->m_wheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->m_wheelActor->RenderOpaqueGeometry(w);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport* w)
{
    this->UpdateRepresentation();
    if ( !this->m_wheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->m_wheelActor->RenderTranslucentPolygonalGeometry(w);
}

//------------------------------------------------------------------------------

int fwVtkWheelRepresentation::HasTranslucentPolygonalGeometry()
{
    this->UpdateRepresentation();
    if ( !this->m_wheelActor->GetVisibility() )
    {
        return 0;
    }
    return this->m_wheelActor->HasTranslucentPolygonalGeometry();
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::SetOrientation(double orientation)
{
    this->m_orientation = ::glm::mod(orientation, ::glm::two_pi<double>());
    this->Modified();
    this->UpdateRepresentation();
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::SetHovering(bool hover)
{
    this->m_hover = hover;
    this->Modified();
    this->UpdateRepresentation();
}

//------------------------------------------------------------------------------

glm::dvec2 fwVtkWheelRepresentation::GetCenterInScreenSpace() const
{
    const ::glm::dvec2 actorPos(this->m_wheelActor->GetPosition()[0], this->m_wheelActor->GetPosition()[1]);
    return actorPos + this->m_widgetToCenterTranslation;
}

//------------------------------------------------------------------------------

bool fwVtkWheelRepresentation::isInCenter(int X, int Y) const
{
    bool inCenter = false;

    if(this->Visibility != 0)
    {
        const ::glm::dvec2 actorPos(this->m_wheelActor->GetPosition()[0], this->m_wheelActor->GetPosition()[1]);

        const ::glm::dvec2 delta = ::glm::dvec2(X, Y) - (actorPos + this->m_widgetToCenterTranslation);

        const double squaredDistance = delta.x * delta.x + delta.y * delta.y;

        const double squaredRadius = this->m_centerOuterRadius * this->m_centerOuterRadius;

        inCenter = (squaredDistance < squaredRadius);
    }

    return inCenter;
}

//------------------------------------------------------------------------------

bool fwVtkWheelRepresentation::isOnWheel(int X, int Y) const
{
    bool pointOnWheel = false;

    if(this->Visibility != 0)
    {
        const ::glm::dvec2 actorPos(this->m_wheelActor->GetPosition()[0], this->m_wheelActor->GetPosition()[1]);

        const ::glm::dvec2 delta = ::glm::dvec2(X, Y) - (actorPos + this->m_widgetToCenterTranslation);

        const double squaredDistance = delta.x * delta.x + delta.y * delta.y;

        const double squaredOuterRadius = this->m_wheelOuterRadius * this->m_wheelOuterRadius;
        const double squaredInnerRadius = this->m_wheelInnerRadius * this->m_wheelInnerRadius;

        pointOnWheel = (squaredDistance < squaredOuterRadius) && (squaredDistance > squaredInnerRadius);
    }

    return pointOnWheel;
}

//------------------------------------------------------------------------------

void fwVtkWheelRepresentation::PrintSelf(ostream&, vtkIndent)
{
    SLM_FATAL("Method not implemented.");
}

//------------------------------------------------------------------------------
