/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
static const core::com::signals::key_t ERROR_COMPUTED_SIG          = "error_computed";

point_list_registration::point_list_registration()
{
    new_signal<error_computed_t>(ERROR_COMPUTED_SIG);
    new_slot(CHANGE_MODE, &point_list_registration::change_mode, this);
}

// ----------------------------------------------------------------------------

point_list_registration::~point_list_registration()
= default;

// ----------------------------------------------------------------------------

void point_list_registration::configuring()
{
    const auto config_tree = this->get_config();
    const auto config      = config_tree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        const std::string mode = config->get<std::string>("mode", "rigid");

        if(mode == "rigid")
        {
            m_registration_mode = rigid;
        }
        else if(mode == "similarity")
        {
            m_registration_mode = similarity;
        }
        else if(mode == "affine")
        {
            m_registration_mode = affine;
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
        m_registration_mode = rigid;
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

void point_list_registration::compute_registration(core::clock::type /*timestamp*/)
{
    const auto registered_pl = m_registered_pl.lock();
    SIGHT_ASSERT("No 'registeredPL' found", registered_pl);
    const auto reference_pl = m_reference_pl.lock();
    SIGHT_ASSERT("No 'referencePL' found", reference_pl);

    if(registered_pl->get_points().size() >= 3
       && registered_pl->get_points().size() == reference_pl->get_points().size())
    {
        vtkSmartPointer<vtkLandmarkTransform> landmark_transform = vtkSmartPointer<vtkLandmarkTransform>::New();

        vtkSmartPointer<vtkPoints> source_pts = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> target_pts = vtkSmartPointer<vtkPoints>::New();

        const auto& first_point     = reference_pl->get_points()[0];
        const auto& first_point_reg = registered_pl->get_points()[0];

        // If the points have labels ...
        if(!first_point->get_label().empty() && !first_point_reg->get_label().empty())
        {
            // ... Then match them according to that label.
            for(const data::point::sptr& point_ref : reference_pl->get_points())
            {
                const std::string label_ref = point_ref->get_label();

                for(const data::point::sptr& point_reg : registered_pl->get_points())
                {
                    const std::string label_reg = point_ref->get_label();

                    if(label_ref == label_reg)
                    {
                        auto coord = point_ref->get_coord();
                        source_pts->InsertNextPoint(coord[0], coord[1], coord[2]);

                        coord = point_reg->get_coord();
                        target_pts->InsertNextPoint(coord[0], coord[1], coord[2]);
                    }
                }
            }
        }
        else
        {
            // ... Else match them according to their order.
            for(const auto& ref_point : reference_pl->get_points())
            {
                const auto& coords = ref_point->get_coord();
                source_pts->InsertNextPoint(coords[0], coords[1], coords[2]);
            }

            for(const auto& reg_point : registered_pl->get_points())
            {
                const auto& coords = reg_point->get_coord();
                target_pts->InsertNextPoint(coords[0], coords[1], coords[2]);
            }
        }

        landmark_transform->SetSourceLandmarks(source_pts);
        landmark_transform->SetTargetLandmarks(target_pts);

        if(m_registration_mode == affine)
        {
            landmark_transform->SetModeToAffine();
        }
        else if(m_registration_mode == similarity)
        {
            landmark_transform->SetModeToSimilarity();
        }
        else
        {
            landmark_transform->SetModeToRigidBody();
        }

        landmark_transform->Update();

        // Get the resulting transformation matrix (this matrix takes the source points to the target points)
        vtkSmartPointer<vtkMatrix4x4> m = landmark_transform->GetMatrix();
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
        double error_value = 0.;

        for(vtkIdType i = 0 ; i < source_pts->GetNumberOfPoints() ; ++i)
        {
            std::array<double, 3> p1 {};
            source_pts->GetPoint(i, p1.data());
            std::array<double, 3> p2 {};
            target_pts->GetPoint(i, p2.data());

            // to have homogeneous coordinates (x,y,z,w)
            std::array p2_h {1., 1., 1., 1.};
            std::copy(std::begin(p2), std::end(p2), std::begin(p2_h));

            //p' = M*p
            std::array<double, 4> new_p {};
            m->MultiplyPoint(p2_h.data(), new_p.data());

            error_value += std::sqrt(
                ((p1[0] - new_p[0]) * (p1[0] - new_p[0]))
                + ((p1[1] - new_p[1]) * (p1[1] - new_p[1]))
                + ((p1[2] - new_p[2]) * (p1[2] - new_p[2]))
            );
        }

        error_value /= static_cast<double>(source_pts->GetNumberOfPoints());

        this->signal<error_computed_t>(ERROR_COMPUTED_SIG)->async_emit(error_value);

        // Notify Matrix modified
        auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit();
        }
    }
    else
    {
        if(registered_pl->get_points().size() < 3)
        {
            sight::ui::dialog::message::show(
                "Error",
                "You must enter 3 or more points for the registration to work.",
                sight::ui::dialog::message::warning
            );
        }
        else
        {
            std::string msg = "The pointlists doesn't have the same number of points : ";
            msg += std::to_string(registered_pl->get_points().size()) + " != " + std::to_string(
                reference_pl->get_points().size()
            );
            sight::ui::dialog::message::show(
                "Error",
                msg,
                sight::ui::dialog::message::warning
            );
        }
    }
}

// ----------------------------------------------------------------------------

void point_list_registration::updating()
{
    const core::clock::type timestamp = core::clock::get_time_in_milli_sec();
    this->compute_registration(timestamp);
}

//----------------------------------------------------------------------------

void point_list_registration::change_mode(std::string _value)
{
    if(_value == "RIGID")
    {
        m_registration_mode = rigid;
    }
    else if(_value == "SIMILARITY")
    {
        m_registration_mode = similarity;
    }
    else if(_value == "AFFINE")
    {
        m_registration_mode = affine;
    }
    else
    {
        SIGHT_ERROR("key " + _value + " is not handled.");
    }
}

//----------------------------------------------------------------------------

} // namespace sight::module::filter::point
