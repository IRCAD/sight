/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SPointListRegistration.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/fieldHelper/Image.hpp>
#include <data/Matrix4.hpp>
#include <data/Mesh.hpp>
#include <data/PointList.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace sight::module::filter::point
{

const core::com::Slots::SlotKeyType SPointListRegistration::s_CHANGE_MODE = "changeMode";
static const core::com::Signals::SignalKeyType s_ERROR_COMPUTED_SIG       = "errorComputed";

SPointListRegistration::SPointListRegistration() :
    m_registrationMode(RIGID)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_CHANGE_MODE, &SPointListRegistration::changeMode, this);
}

// ----------------------------------------------------------------------------

SPointListRegistration::~SPointListRegistration()
{
}

// ----------------------------------------------------------------------------

void SPointListRegistration::configuring()
{
    const auto configTree = this->getConfigTree();
    const auto config     = configTree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        const std::string mode = config->get<std::string>("mode", "rigid");

        if(mode == "rigid")
        {
            m_registrationMode = RIGID;
        }
        else if(mode == "similarity")
        {
            m_registrationMode = SIMILARITY;
        }
        else if(mode == "affine")
        {
            m_registrationMode = AFFINE;
        }
        else
        {
            SIGHT_ERROR(
                "Unknown registration mode: '" + mode + "', it must be 'rigid', 'similarity' or 'affine'."
                                                        " Defaulting to 'rigid'."
            );
        }
    }
    else
    {
        m_registrationMode = RIGID;
    }
}

// ----------------------------------------------------------------------------

void SPointListRegistration::starting()
{
}

// ----------------------------------------------------------------------------

void SPointListRegistration::stopping()
{
}

//-----------------------------------------------------------------------------

void SPointListRegistration::computeRegistration(core::HiResClock::HiResClockType)
{
    const auto registeredPL = m_registeredPL.lock();
    SIGHT_ASSERT("No 'registeredPL' found", registeredPL);
    const auto referencePL = m_referencePL.lock();
    SIGHT_ASSERT("No 'referencePL' found", referencePL);

    if(registeredPL->getPoints().size() >= 3
       && registeredPL->getPoints().size() == referencePL->getPoints().size())
    {
        vtkSmartPointer<vtkLandmarkTransform> landmarkTransform = vtkSmartPointer<vtkLandmarkTransform>::New();

        vtkSmartPointer<vtkPoints> sourcePts = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> targetPts = vtkSmartPointer<vtkPoints>::New();

        const auto& firstPoint    = referencePL->getPoints()[0];
        const auto& firstPointReg = registeredPL->getPoints()[0];

        // If the points have labels ...
        if(firstPoint->getField<data::String>(data::fieldHelper::Image::m_labelId) != nullptr
           && firstPointReg->getField<data::String>(data::fieldHelper::Image::m_labelId) != nullptr)
        {
            // ... Then match them according to that label.
            for(data::Point::sptr pointRef : referencePL->getPoints())
            {
                const std::string& labelRef =
                    pointRef->getField<data::String>(data::fieldHelper::Image::m_labelId)->value();

                for(data::Point::sptr pointReg : registeredPL->getPoints())
                {
                    const std::string& labelReg =
                        pointReg->getField<data::String>(data::fieldHelper::Image::m_labelId)->value();

                    if(labelRef == labelReg)
                    {
                        auto coord = pointRef->getCoord();
                        sourcePts->InsertNextPoint(coord[0], coord[1], coord[2]);

                        coord = pointReg->getCoord();
                        targetPts->InsertNextPoint(coord[0], coord[1], coord[2]);
                    }
                }
            }
        }
        else
        {
            // ... Else match them according to their order.
            for(const auto& refPoint : referencePL->getPoints())
            {
                const auto& coords = refPoint->getCoord();
                sourcePts->InsertNextPoint(coords[0], coords[1], coords[2]);
            }

            for(const auto& regPoint : registeredPL->getPoints())
            {
                const auto& coords = regPoint->getCoord();
                targetPts->InsertNextPoint(coords[0], coords[1], coords[2]);
            }
        }

        landmarkTransform->SetSourceLandmarks(sourcePts);
        landmarkTransform->SetTargetLandmarks(targetPts);

        if(m_registrationMode == AFFINE)
        {
            landmarkTransform->SetModeToAffine();
        }
        else if(m_registrationMode == SIMILARITY)
        {
            landmarkTransform->SetModeToSimilarity();
        }
        else
        {
            landmarkTransform->SetModeToRigidBody();
        }

        landmarkTransform->Update();

        // Get the resulting transformation matrix (this matrix takes the source points to the target points)
        vtkSmartPointer<vtkMatrix4x4> m = landmarkTransform->GetMatrix();
        m->Invert();

        auto matrix = m_output.lock();
        SIGHT_ASSERT("No 'output' found", matrix);

        for(std::uint8_t l = 0 ; l < 4 ; ++l)
        {
            for(std::uint8_t c = 0 ; c < 4 ; ++c)
            {
                matrix->setCoefficient(l, c, m->GetElement(l, c));
            }
        }

        //compute RMSE
        double errorValue = 0.;

        for(vtkIdType i = 0 ; i < sourcePts->GetNumberOfPoints() ; ++i)
        {
            double p1[3];
            sourcePts->GetPoint(i, p1);
            double p2[3];
            targetPts->GetPoint(i, p2);

            // to have homogeneous coordinates (x,y,z,w)
            double p2H[4] = {1., 1., 1., 1.};
            std::copy(std::begin(p2), std::end(p2), std::begin(p2H));

            //p' = M*p
            double newP[4];
            m->MultiplyPoint(p2H, newP);

            errorValue += std::sqrt(
                ((p1[0] - newP[0]) * (p1[0] - newP[0]))
                + ((p1[1] - newP[1]) * (p1[1] - newP[1]))
                + ((p1[2] - newP[2]) * (p1[2] - newP[2]))
            );
        }

        errorValue /= sourcePts->GetNumberOfPoints();

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(errorValue);

        // Notify Matrix modified
        auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        {
            core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
    else
    {
        if(registeredPL->getPoints().size() < 3)
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Error",
                "You must enter 3 or more points for the registration to work.",
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }
        else
        {
            std::string msg = "The pointlists doesn't have the same number of points : ";
            msg += std::to_string(registeredPL->getPoints().size()) + " != " + std::to_string(
                referencePL->getPoints().size()
            );
            sight::ui::base::dialog::MessageDialog::show(
                "Error",
                msg,
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }
    }
}

// ----------------------------------------------------------------------------

void SPointListRegistration::updating()
{
    const core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
    this->computeRegistration(timestamp);
}

//----------------------------------------------------------------------------

void SPointListRegistration::changeMode(std::string _value)
{
    if(_value == "RIGID")
    {
        m_registrationMode = RIGID;
    }
    else if(_value == "SIMILARITY")
    {
        m_registrationMode = SIMILARITY;
    }
    else if(_value == "AFFINE")
    {
        m_registrationMode = AFFINE;
    }
    else
    {
        SIGHT_ERROR("key " + _value + " is not handled.");
    }
}

//----------------------------------------------------------------------------

} // namespace sight::module::filter::point
