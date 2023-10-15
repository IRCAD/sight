/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "point_list_registration.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/composite.hpp>
#include <data/matrix4.hpp>
#include <data/mesh.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

namespace sight::module::filter::point
{

const core::com::slots::key_t point_list_registration::CHANGE_MODE = "changeMode";
static const core::com::signals::key_t ERROR_COMPUTED_SIG          = "errorComputed";

point_list_registration::point_list_registration()
{
    new_signal<ErrorComputedSignalType>(ERROR_COMPUTED_SIG);
    new_slot(CHANGE_MODE, &point_list_registration::changeMode, this);
}

// ----------------------------------------------------------------------------

point_list_registration::~point_list_registration()
= default;

// ----------------------------------------------------------------------------

void point_list_registration::configuring()
{
    const auto configTree = this->get_config();
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

void point_list_registration::starting()
{
}

// ----------------------------------------------------------------------------

void point_list_registration::stopping()
{
}

//-----------------------------------------------------------------------------

void point_list_registration::computeRegistration(core::hires_clock::type /*timestamp*/)
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
        if(!firstPoint->getLabel().empty() && !firstPointReg->getLabel().empty())
        {
            // ... Then match them according to that label.
            for(const data::point::sptr& pointRef : referencePL->getPoints())
            {
                const std::string labelRef = pointRef->getLabel();

                for(const data::point::sptr& pointReg : registeredPL->getPoints())
                {
                    const std::string labelReg = pointRef->getLabel();

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
                (*matrix)(l, c) = m->GetElement(l, c);
            }
        }

        //compute RMSE
        double errorValue = 0.;

        for(vtkIdType i = 0 ; i < sourcePts->GetNumberOfPoints() ; ++i)
        {
            std::array<double, 3> p1 {};
            sourcePts->GetPoint(i, p1.data());
            std::array<double, 3> p2 {};
            targetPts->GetPoint(i, p2.data());

            // to have homogeneous coordinates (x,y,z,w)
            std::array p2H {1., 1., 1., 1.};
            std::copy(std::begin(p2), std::end(p2), std::begin(p2H));

            //p' = M*p
            std::array<double, 4> newP {};
            m->MultiplyPoint(p2H.data(), newP.data());

            errorValue += std::sqrt(
                ((p1[0] - newP[0]) * (p1[0] - newP[0]))
                + ((p1[1] - newP[1]) * (p1[1] - newP[1]))
                + ((p1[2] - newP[2]) * (p1[2] - newP[2]))
            );
        }

        errorValue /= static_cast<double>(sourcePts->GetNumberOfPoints());

        this->signal<ErrorComputedSignalType>(ERROR_COMPUTED_SIG)->async_emit(errorValue);

        // Notify Matrix modified
        auto sig = matrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit();
        }
    }
    else
    {
        if(registeredPL->getPoints().size() < 3)
        {
            sight::ui::dialog::message::show(
                "Error",
                "You must enter 3 or more points for the registration to work.",
                sight::ui::dialog::message::WARNING
            );
        }
        else
        {
            std::string msg = "The pointlists doesn't have the same number of points : ";
            msg += std::to_string(registeredPL->getPoints().size()) + " != " + std::to_string(
                referencePL->getPoints().size()
            );
            sight::ui::dialog::message::show(
                "Error",
                msg,
                sight::ui::dialog::message::WARNING
            );
        }
    }
}

// ----------------------------------------------------------------------------

void point_list_registration::updating()
{
    const core::hires_clock::type timestamp = core::hires_clock::get_time_in_milli_sec();
    this->computeRegistration(timestamp);
}

//----------------------------------------------------------------------------

void point_list_registration::changeMode(std::string _value)
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
