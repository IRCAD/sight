/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "module/filter/mesh/vtk_mesher.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/profiling.hpp>

#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <io/vtk/helper/mesh.hpp>
#include <io/vtk/vtk.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include <vtkCommand.h>
#include <vtkConnectivityFilter.h>
#include <vtkDecimatePro.h>
#include <vtkDiscreteFlyingEdges3D.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkExecutionTimer.h>
#include <vtkGeometryFilter.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkQuadricDecimation.h>
#include <vtkSmartPointer.h>
#include <vtkThreshold.h>
#include <vtkWindowedSincPolyDataFilter.h>

namespace sight::module::filter::mesh
{

//-----------------------------------------------------------------------------

class error_observer final : public vtkCommand
{
public:

    //------------------------------------------------------------------------------

    [[nodiscard]] static error_observer* New()
    {
        return new error_observer;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::optional<std::string> get_error() const
    {
        return m_error_message;
    }

    //------------------------------------------------------------------------------

    void clear()
    {
        m_error_message = std::nullopt;
    }

    //------------------------------------------------------------------------------

    // NOLINTNEXTLINE(google-runtime-int)
    void Execute(vtkObject* vtkNotUsed(caller) /*caller*/, unsigned long _event, void* _calldata) final
    {
        if(_event == vtkCommand::ErrorEvent)
        {
            m_error_message = static_cast<char*>(_calldata);
        }
    }

private:

    std::optional<std::string> m_error_message;
};

//-----------------------------------------------------------------------------

vtk_mesher::vtk_mesher() noexcept :
    filter(m_signals),
    notifier(m_signals)
{
    new_signal<signals::empty_t>(signals::COMPLETED);
    new_signal<signals::empty_t>(signals::FAILED);
}

//-----------------------------------------------------------------------------

void vtk_mesher::configuring(const config_t& _config)
{
    auto mode = _config.get<std::string>("config.<xmlattr>.mode", "add");
    if(mode == "add")
    {
        m_mode = mode_t::ADD;
    }
    else if(mode == "replace")
    {
        m_mode = mode_t::REPLACE;
    }
    else
    {
        SIGHT_ERROR("Unsupported value for mode config : " << std::quoted(mode));
    }
}

//-----------------------------------------------------------------------------

void vtk_mesher::starting()
{
}

//-----------------------------------------------------------------------------

void vtk_mesher::stopping()
{
}

//-----------------------------------------------------------------------------

void vtk_mesher::updating()
{
    FW_PROFILE("mesh");

    auto image_series = m_image.lock();
    auto model_series = m_model.lock();

    if(!image_series || !model_series)
    {
        std::string msg = "Invalid input/output data for model series reconstruction.";
        SIGHT_ERROR(msg);
        this->notifier::failure(msg);

        this->async_emit(signals::FAILED);

        return;
    }

    model_series->series::deep_copy(image_series.get_shared());
    model_series->set_dicom_reference(image_series->get_dicom_reference());

    // vtk img
    auto vtk_image = vtkSmartPointer<vtkImageData>::New();
    sight::io::vtk::to_vtk_image(image_series.get_shared(), vtk_image);

    post_reconstruction_jobs(vtk_image, model_series.get_shared());
    {
        this->async_emit(signals::COMPLETED);
    }

    {
        model_series->async_emit(sight::data::object::MODIFIED_SIG);
    }
}

//-----------------------------------------------------------------------------

vtkSmartPointer<vtkPolyData> vtk_mesher::reconstruct(vtkSmartPointer<vtkImageData> _image, int _value)
{
    // Initialize filters to be used
    vtkNew<error_observer> error_obs;

    vtkSmartPointer<vtkPolyDataAlgorithm> contour_filter;

    // Contour filter
    if(*m_use_flying_edges)
    {
        vtkSmartPointer<vtkDiscreteFlyingEdges3D> flying_edges = vtkSmartPointer<vtkDiscreteFlyingEdges3D>::New();
        flying_edges->ComputeScalarsOn();
        flying_edges->ComputeNormalsOn();

        // Initialize the contour filter
        flying_edges->SetValue(0, _value);

        contour_filter = flying_edges;
    }
    else
    {
        vtkSmartPointer<vtkDiscreteMarchingCubes> marching_cubes = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();
        marching_cubes->ComputeScalarsOn();
        marching_cubes->ComputeNormalsOn();

        // Initialize the contour filter
        marching_cubes->SetValue(0, _value);

        contour_filter = marching_cubes;
    }

    contour_filter->SetInputData(_image);
    contour_filter->AddObserver(vtkCommand::ErrorEvent, error_obs);

    auto contour_timer = vtkSmartPointer<vtkExecutionTimer>::New();
    contour_timer->SetFilter(contour_filter);

    // Smooth filter
    auto smooth_filter = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    smooth_filter->AddObserver(vtkCommand::ErrorEvent, error_obs);
    smooth_filter->SetInputConnection(contour_filter->GetOutputPort());
    smooth_filter->SetNumberOfIterations(static_cast<int>(*m_num_iterations));
    smooth_filter->SetBoundarySmoothing(static_cast<vtkTypeBool>(*m_boundary_smoothing));
    smooth_filter->SetPassBand(*m_pass_band);
    smooth_filter->SetEdgeAngle(90);
    smooth_filter->SetFeatureEdgeSmoothing(static_cast<vtkTypeBool>(*m_feature_smoothing));
    smooth_filter->SetFeatureAngle(*m_feature_angle);
    smooth_filter->SetNonManifoldSmoothing(static_cast<vtkTypeBool>(*m_non_manifold_smoothing));
    // This improves stability, always on
    smooth_filter->NormalizeCoordinatesOn();

    auto smooth_timer = vtkSmartPointer<vtkExecutionTimer>::New();
    smooth_timer->SetFilter(smooth_filter);

    auto decimate_timer                               = vtkSmartPointer<vtkExecutionTimer>::New();
    vtkSmartPointer<vtkPolyDataAlgorithm> last_filter = smooth_filter;

    if(*m_reduction > 0)
    {
        // Decimator (if needed)
        if(*m_quadric_reduction)
        {
            auto decimate_filter = vtkSmartPointer<vtkQuadricDecimation>::New();
            decimate_filter->AddObserver(vtkCommand::ErrorEvent, error_obs);
            decimate_filter->SetInputConnection(smooth_filter->GetOutputPort());
            decimate_filter->SetTargetReduction(std::clamp(static_cast<double>(*m_reduction), 0.0, 100.0) / 100.0);
            decimate_filter->VolumePreservationOn();
            decimate_timer->SetFilter(decimate_filter);
            last_filter = decimate_filter;
        }
        else
        {
            auto decimate_filter = vtkSmartPointer<vtkDecimatePro>::New();
            decimate_filter->AddObserver(vtkCommand::ErrorEvent, error_obs);
            decimate_filter->SetInputConnection(smooth_filter->GetOutputPort());
            decimate_filter->SetTargetReduction(std::clamp(static_cast<double>(*m_reduction), 0.0, 100.0) / 100.0);
            decimate_filter->SetPreserveTopology(static_cast<vtkTypeBool>(*m_preserve_topology));
            decimate_filter->SplittingOn();
            decimate_filter->BoundaryVertexDeletionOn();

            decimate_timer->SetFilter(decimate_filter);
            last_filter = decimate_filter;
        }
    }

    last_filter->Update();

    vtkSmartPointer<vtkPolyData> poly_data = last_filter->GetOutput();

    SIGHT_INFO(this->get_id() << ": Value: " << _value << "Flying edges: " << std::to_string(*m_use_flying_edges));
    SIGHT_INFO(this->get_id() << ": Contour timer: " << contour_timer->GetElapsedWallClockTime() << "s");
    SIGHT_INFO(this->get_id() << ": Smooth timer: " << smooth_timer->GetElapsedWallClockTime() << "s");
    SIGHT_INFO(this->get_id() << ": Decimate timer: " << decimate_timer->GetElapsedWallClockTime() << "s");
    SIGHT_INFO(this->get_id() << ": Number of points: " << poly_data->GetNumberOfPoints());
    SIGHT_INFO(this->get_id() << ": Number of cells: " << poly_data->GetNumberOfCells());

    // Check for errors
    if(auto error = error_obs->get_error(); error.has_value())
    {
        SIGHT_ERROR(*error);
        this->notifier::failure(*error);
        this->async_emit(signals::FAILED);

        return nullptr;
    }

    error_obs->clear();

    return poly_data;
}

//-----------------------------------------------------------------------------

void vtk_mesher::post_reconstruction_jobs(
    vtkSmartPointer<vtkImageData> _image,
    sight::data::model_series::sptr _model_series
)
{
    sight::data::model_series::reconstruction_vector_t recs;

    auto config = this->get_config();

    if(not config.get_child_optional("config.organ").has_value())
    {
        sight::service::config_t organ_cfg;
        organ_cfg.put("organ.<xmlattr>.value", *m_value);
        config.add_child("config", organ_cfg);
    }

    const auto srv_config = config.get_child("config");

    const auto num_threads = std::min((unsigned int) (srv_config.count("organ")), std::thread::hardware_concurrency());
    auto thread_pool       = std::make_shared<boost::asio::thread_pool>(num_threads);

    for(const auto& elt : boost::make_iterator_range(srv_config.equal_range("organ")))
    {
        auto reconstruct_lambda =
            [this, _image, elt, &recs]
            {
                const auto value          = elt.second.get<int>("<xmlattr>.value");
                const auto name           = elt.second.get<std::string>("<xmlattr>.name", "organ");
                const auto type           = elt.second.get<std::string>("<xmlattr>.type", "");
                const auto color_cfg      = elt.second.get<std::string>("<xmlattr>.color", "#ffffffff");
                const auto representation = elt.second.get<std::string>("<xmlattr>.representation", "SURFACE");
                const auto split          = elt.second.get<bool>("<xmlattr>.split", false);
                const auto selected       = elt.second.get<bool>("<xmlattr>.selected", false);

                // Initialize the contour filter
                vtkSmartPointer<vtkPolyData> poly_data = this->reconstruct(_image, value);

                auto create_mesh = [&](vtkSmartPointer<vtkPolyData> _poly_data)
                                   {
                                       auto mesh = std::make_shared<sight::data::mesh>();
                                       sight::io::vtk::helper::mesh::from_vtk_mesh(_poly_data, mesh);

                                       auto reconstruction = std::make_shared<sight::data::reconstruction>();

                                       reconstruction->set_organ_name(name);
                                       reconstruction->set_structure_type(type);
                                       reconstruction->set_is_visible(true);
                                       reconstruction->set_mesh(mesh);

                                       auto material = std::make_shared<sight::data::material>();
                                       auto color    = std::make_shared<sight::data::color>(color_cfg);
                                       material->set_diffuse(color);
                                       material->set_representation_mode(
                                           sight::data::material::string_to_representation_mode(
                                               representation
                                           )
                                       );
                                       if(selected)
                                       {
                                           material->set_options_mode(sight::data::material::options_t::selected);
                                       }

                                       reconstruction->set_material(material);

                                       recs.push_back(reconstruction);
                                   };
                if(poly_data != nullptr)
                {
                    if(split)
                    {
                        auto connectivity_filter = vtkSmartPointer<vtkConnectivityFilter>::New();
                        connectivity_filter->SetInputData(poly_data);
                        connectivity_filter->SetExtractionModeToAllRegions();
                        connectivity_filter->ColorRegionsOn();
                        connectivity_filter->Update();
                        for(int i = 0 ; i < connectivity_filter->GetNumberOfExtractedRegions() ; i++)
                        {
                            auto threshold_filter = vtkSmartPointer<vtkThreshold>::New();
                            threshold_filter->SetInputData(connectivity_filter->GetOutput());
                            threshold_filter->SetLowerThreshold(i);
                            threshold_filter->SetUpperThreshold(i);
                            threshold_filter->SetInputArrayToProcess(
                                0,
                                0,
                                0,
                                vtkDataObject::FIELD_ASSOCIATION_CELLS,
                                "RegionId"
                            );
                            threshold_filter->Update();

                            auto geometry_filter = vtkSmartPointer<vtkGeometryFilter>::New();
                            geometry_filter->SetInputData(threshold_filter->GetOutput());
                            geometry_filter->Update();

                            vtkSmartPointer<vtkPolyData> region_poly_data = geometry_filter->GetOutput();
                            create_mesh(region_poly_data);
                        }
                    }
                    else
                    {
                        create_mesh(poly_data);
                    }
                }
            };

        boost::asio::post(
            *thread_pool,
            reconstruct_lambda
        );
    }

    thread_pool->join();

    sight::data::model_series::reconstruction_vector_t out_recs = _model_series->get_reconstruction_db();
    if(m_mode == mode_t::REPLACE)
    {
        out_recs.clear();
    }

    for(auto& rec : recs)
    {
        if(rec)
        {
            out_recs.push_back(rec);
        }
    }

    _model_series->set_reconstruction_db(out_recs);
}

//-------------------------------------------------------------------------

vtk_mesher::connections_t vtk_mesher::auto_connections() const
{
    return {
        {m_boundary_smoothing, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_non_manifold_smoothing, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_feature_smoothing, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_preserve_topology, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_use_flying_edges, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_value, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_pass_band, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_num_iterations, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_feature_angle, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_reduction, data::object::MODIFIED_SIG, service::slots::UPDATE},
        {m_quadric_reduction, data::object::MODIFIED_SIG, service::slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::module::filter::mesh
