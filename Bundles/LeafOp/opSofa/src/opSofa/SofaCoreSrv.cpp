/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QMessageBox>

#include <fwMedData/ModelSeries.hpp>

#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataIO/reader/MeshReader.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/UUID.hpp>

#include "opSofa/SofaCoreSrv.hpp"

namespace opSofa
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::opSofa::SofaCoreSrv, ::fwMedData::ModelSeries );

SofaCoreSrv::SofaCoreSrv() throw()
{
}

SofaCoreSrv::~SofaCoreSrv() throw()
{
}

void SofaCoreSrv::configuring() throw ( ::fwTools::Failed )
{
    if(m_configuration->findConfigurationElement("addTools"))
    {
        if (m_configuration->findConfigurationElement("addTools")->getValue() == "yes")
        {
            this->addMesh(std::string(BUNDLE_PREFIX) + "/opSofa_0-1/mors2.trian", "mors2");
            this->addMesh(std::string(BUNDLE_PREFIX) + "/opSofa_0-1/cam.trian", "cam");
        }
    }
}

void SofaCoreSrv::starting() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void SofaCoreSrv::stopping() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

void SofaCoreSrv::receiving( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
    if (msg->hasEvent("NEW_SOFA_SCENE"))
    {
        if (m_sofa && m_sofa->isAnimate())
        {
            // Stop animation
            m_sofa->stopThread();
        }

        // Get Path Scn
        ::fwData::String::csptr pathScn = ::fwData::String::dynamicConstCast(msg->getDataInfo("NEW_SOFA_SCENE"));


        ::fwMedData::ModelSeries::sptr ms = this->getObject< ::fwMedData::ModelSeries >();
        SLM_ASSERT("Invalid object", ms);

        // Create object sofa
        m_sofa.reset();
        m_sofa = ::boost::make_shared< SofaBusiness >();
        m_sofa->loadScn(pathScn->value(), ms, this->getSptr());

        // Apply at m_sofa the number of image by second
        m_sofa->setTimeStepAnimation(1000/50);
        m_sofa->startThread();
    }
    else if (msg->hasEvent("START_STOP_SOFA"))
    {
        if (m_sofa)
        {
            // if animation is running
            if (m_sofa->isAnimate())
            {
                // Stop animation
                m_sofa->stopThread();
            }
            else
            {
                // Start animation
                m_sofa->startThread();
            }
        }
        else
        {
            QMessageBox::warning(0, "Warning", "To launch animation you must first load scene file !");
        }
    }
    else if (msg->hasEvent("EDITOR_MESH_SOFA"))
    {
        if (m_sofa)
        {
            ::fwData::Vector::csptr data =
                ::fwData::Vector::dynamicConstCast(msg->getDataInfo("EDITOR_MESH_SOFA"));
            ::fwData::String::csptr idMesh    = ::fwData::String::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr strength = ::fwData::Integer::dynamicConstCast(data->at(1));
            m_sofa->shakeMesh(idMesh->value(), strength->value());
        }
    }
    else if (msg->hasEvent("MOVE_MESH_SOFA"))
    {
        if (m_sofa)
        {
            ::fwData::Vector::csptr data   = ::fwData::Vector::dynamicConstCast(msg->getDataInfo("MOVE_MESH_SOFA"));
            ::fwData::String::csptr idMesh = ::fwData::String::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr x     = ::fwData::Integer::dynamicConstCast(data->at(1));
            ::fwData::Integer::csptr y     = ::fwData::Integer::dynamicConstCast(data->at(2));
            ::fwData::Integer::csptr z     = ::fwData::Integer::dynamicConstCast(data->at(3));
            ::fwData::Float::csptr rx      = ::fwData::Float::dynamicConstCast(data->at(4));
            ::fwData::Float::csptr ry      = ::fwData::Float::dynamicConstCast(data->at(5));
            ::fwData::Float::csptr rz      = ::fwData::Float::dynamicConstCast(data->at(6));
            m_sofa->moveMesh(idMesh->value(), x->value(), y->value(), z->value(), rx->value(), ry->value(),
                             rz->value());
        }
    }
    else if (msg->hasEvent("KINECT_NEW_POSITION_HAND"))
    {
        if (m_sofa)
        {
            // Get the position x y z of the main and his id
            ::fwData::Vector::csptr data =
                ::fwData::Vector::dynamicConstCast(msg->getDataInfo("KINECT_NEW_POSITION_HAND"));
            ::fwData::Integer::csptr x  = ::fwData::Integer::dynamicConstCast(data->at(0));
            ::fwData::Integer::csptr y  = ::fwData::Integer::dynamicConstCast(data->at(1));
            ::fwData::Integer::csptr z  = ::fwData::Integer::dynamicConstCast(data->at(2));
            ::fwData::Integer::csptr id = ::fwData::Integer::dynamicConstCast(data->at(3));

            static int idTool1 = 0;
            static int idTool2 = 0;

            // Allow to switch tool
            if (id->value() == idTool1)
            {
                // move tool 1
                m_sofa->moveMesh("souris_mors2", x->value()/2 + 190, y->value()/2 + 152, z->value()/2 - 200, 0, 0, 0);
            }
            else if (id->value() == idTool2)
            {
                // move tool 2
                m_sofa->moveMesh("souris_cam", x->value()/2 + 190, y->value()/2 + 152, z->value()/2 - 200, 0, 0, 0);
            }
            else
            {
                static int stage = 0;
                if (stage%2)
                {
                    idTool1 = id->value();
                }
                else
                {
                    idTool2 = id->value();
                }
                stage++;
            }
        }
    }
}

void SofaCoreSrv::updating() throw ( ::fwTools::Failed )
{
}

void SofaCoreSrv::info ( std::ostream &_sstream )
{
}

void SofaCoreSrv::addMesh(std::string meshPath, std::string meshName)
{
    // Create mesh
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->setName(meshName);
    ::fwDataIO::reader::MeshReader::sptr reader1 = ::fwDataIO::reader::MeshReader::New();
    reader1->setObject(mesh);
    reader1->setFile(meshPath);
    reader1->read();

    // Create reconstruction
    ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();
    reconstruction->setStructureType("OrganType");
    reconstruction->setIsVisible(true);
    reconstruction->setMesh(mesh);
    reconstruction->setOrganName(meshName);

    ::fwMedData::ModelSeries::sptr ms = this->getObject< ::fwMedData::ModelSeries >();
    SLM_ASSERT("Invalid object", ms);
    ::fwMedData::ModelSeries::ReconstructionVectorType recs = ms->getReconstructionDB();
    recs.push_back(reconstruction);
    ms->setReconstructionDB(recs);
}

}

