/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "ComputeMaterial.hpp"

#include "core/frame_graph.hpp"

#include <core/runtime/path.hpp>

#include <QByteArray>

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QDispatchCompute>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMemoryBarrier>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTechniqueFilter>

#include <QUrl>

namespace sight::viz::qt3d
{

//------------------------------------------------------------------------------

compute_material::compute_material(Qt3DCore::QNode* _parent) :
    QMaterial(_parent),
    m_effect(new Qt3DRender::QEffect),
    m_compute_shader(new Qt3DRender::QShaderProgram),
    m_compute_render_pass(new Qt3DRender::QRenderPass),
    m_index_buffer(new Qt3DRender::QBuffer),
    m_index_buffer_parameter(new Qt3DRender::QParameter),
    m_compute_filter_key(new Qt3DRender::QFilterKey),
    m_compute_technique(new Qt3DRender::QTechnique)
{
    //Initialize shader program
    const auto compute_shader_path = sight::core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/quadToTriangleMesh_CP.glsl"
    );
    const auto source = QUrl::fromLocalFile(QString::fromStdString(compute_shader_path.string()));
    m_compute_shader->setComputeShaderCode(Qt3DRender::QShaderProgram::loadSource(source));

    m_compute_render_pass->setShaderProgram(m_compute_shader);

    //Initialize parameters
    this->set_index_buffer(m_index_buffer);
    m_compute_render_pass->addParameter(m_index_buffer_parameter);

    //Set OpenGL version
    m_compute_technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_compute_technique->graphicsApiFilter()->setMajorVersion(4);
    m_compute_technique->graphicsApiFilter()->setMinorVersion(3);
    m_compute_technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    //Set filter Keys
    m_compute_filter_key->setName(QStringLiteral("pass"));
    m_compute_filter_key->setValue(QStringLiteral("compute"));

    //Add to technique
    m_compute_technique->addFilterKey(m_compute_filter_key);
    m_compute_technique->addRenderPass(m_compute_render_pass);

    m_effect->addTechnique(m_compute_technique);
    this->setEffect(m_effect);
}

//------------------------------------------------------------------------------

compute_material::~compute_material()
= default;

//------------------------------------------------------------------------------

void compute_material::set_index_buffer(Qt3DRender::QBuffer* _buffer)
{
    m_index_buffer = _buffer;
    m_index_buffer_parameter->setName(QStringLiteral("c_in_index"));

    //Set the buffer as parameter data
    QVariant tmp_variant;
    tmp_variant.setValue(m_index_buffer);
    m_index_buffer_parameter->setValue(tmp_variant);
}

//------------------------------------------------------------------------------

void compute_material::update_frame_graph(viz::qt3d::core::frame_graph* _frame_graph, int _number_of_cells)
{
    // Memory barrier to wait for compute shader to finish before rendering the mesh.
    auto* const mem_barrier = new Qt3DRender::QMemoryBarrier();
    mem_barrier->setWaitOperations(Qt3DRender::QMemoryBarrier::VertexAttributeArray);

    // Set the memory barrier as a child of framegraph's camera selector.
    _frame_graph->add_node(mem_barrier, _frame_graph->get_camera_selector());

    // FrameGraph node handling openGL dispatch call.
    auto* const empty_node = new Qt3DRender::QFrameGraphNode();
    {
        auto* const compute_tech_filter = new Qt3DRender::QTechniqueFilter(empty_node);
        compute_tech_filter->addMatch(m_compute_filter_key);
        {
            auto* const dispatch = new Qt3DRender::QDispatchCompute(compute_tech_filter);
            dispatch->setWorkGroupX(_number_of_cells);
        }
    }

    // Creates a new branch in the framegraph which is child of framegraph's camera selector.
    _frame_graph->add_node(
        empty_node,
        qobject_cast<Qt3DRender::QFrameGraphNode*>(_frame_graph->get_camera_selector()->parent())
    );
}

} //namespace sight::viz::qt3d
