/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opPOCMesher/CGoGNMesher.hpp"

#include <fwTools/fwID.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwIGG/Mesher.hpp>
#include <fwIGG/Mesher.hxx>

#include <fwGuiQt/container/QtContainer.hpp>

#include <boost/lexical_cast.hpp>

namespace opPOCMesher
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::opVTKMesh::IMesher, ::opPOCMesher::CGoGNMesher, ::fwData::Composite);

static const ::fwCom::Slots::SlotKeyType SET_INTEGER_SLOT = "setInteger";
static const ::fwCom::Slots::SlotKeyType SET_BOOLEAN_SLOT = "setBoolean";

//-----------------------------------------------------------------------------

CGoGNMesher::CGoGNMesher() throw () :
    m_valueMin(127),
    m_valueMax(127),
    m_adapt(50),
    m_radius(5),
    m_faces(50),
    m_percentage(true),
    m_closing(false)
{
    newSlot(SET_INTEGER_SLOT, &CGoGNMesher::setInteger, this);
    newSlot(SET_BOOLEAN_SLOT, &CGoGNMesher::setBoolean, this);
}

//-----------------------------------------------------------------------------

CGoGNMesher::~CGoGNMesher() throw ()
{
}

//-----------------------------------------------------------------------------

void CGoGNMesher::starting() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void CGoGNMesher::stopping() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void CGoGNMesher::configuring() throw (::fwTools::Failed)
{
    const ::fwServices::IService::ConfigType& srvConfig = this->getConfigTree();

    SLM_ASSERT("You must have one <config/> element.", srvConfig.get_child("service").count("config") == 1 );

    const ::fwServices::IService::ConfigType& config = srvConfig.get_child("service.config");

    SLM_ASSERT("You must have one <image/> element.", config.count("image") == 1);
    SLM_ASSERT("You must have one <modelSeries/> element.", config.count("modelSeries") == 1);

    const ::fwServices::IService::ConfigType& imageCfg       = config.get_child("image");
    const ::fwServices::IService::ConfigType& modelSeriesCfg = config.get_child("modelSeries");

    m_imageKey       = imageCfg.get_value<std::string>();
    m_modelSeriesKey = modelSeriesCfg.get_value<std::string>();

    if(config.count("valueMin") == 1)
    {
        m_valueMin = config.get_child("valueMin").get_value<int>();
    }

    if(config.count("valueMax") == 1)
    {
        m_valueMax = config.get_child("valueMax").get_value<int>();
    }

    if(config.count("radius") == 1)
    {
        m_radius = config.get_child("radius").get_value<int>();
    }

    if(config.count("adapt") == 1)
    {
        m_adapt = config.get_child("adapt").get_value<int>();
    }

    if(config.count("faces") == 1)
    {
        m_faces = config.get_child("faces").get_value<int>();
    }

    if(config.count("percentage") == 1)
    {
        m_percentage = config.get_child("percentage").get_value<bool>();
    }

    if(config.count("closing") == 1)
    {
        m_closing = config.get_child("closing").get_value<bool>();
    }
}

//-----------------------------------------------------------------------------

void CGoGNMesher::updating() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite           = this->getObject< ::fwData::Composite >();
    ::fwData::Composite::iterator iterImg         = composite->find(m_imageKey);
    ::fwData::Composite::iterator iterModelSeries = composite->find(m_modelSeriesKey);

    SLM_ASSERT("Key '"+m_imageKey+"' not found in composite.", iterImg != composite->end());
    SLM_ASSERT("Key '"+m_modelSeriesKey+"' not found in composite.", iterModelSeries != composite->end());

    ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(iterImg->second);
    SLM_ASSERT("Image '"+m_imageKey+"' is not valid.", image);

    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(iterModelSeries->second);
    SLM_ASSERT("ModelSeries '"+m_modelSeriesKey+"' is not valid.", modelSeries);

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    /// IGG Mesher
    if (image->getType() == ::fwTools::Type::s_INT8)
    {
        ::fwIGG::Mesher::computeMeshMCS<signed char>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                                     m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_INT16)
    {
        ::fwIGG::Mesher::computeMeshMCS<signed short>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                                      m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_INT32)
    {
        ::fwIGG::Mesher::computeMeshMCS<signed int>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                                    m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_INT64)
    {
        ::fwIGG::Mesher::computeMeshMCS<signed long long>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt
                                                          , m_radius, m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_UINT8)
    {
        ::fwIGG::Mesher::computeMeshMCS<unsigned char>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                                       m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_UINT16)
    {
        ::fwIGG::Mesher::computeMeshMCS<unsigned short>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                                        m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_UINT32)
    {
        ::fwIGG::Mesher::computeMeshMCS<unsigned int>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                                      m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_UINT64)
    {
        ::fwIGG::Mesher::computeMeshMCS<unsigned long long>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt,
                                                            m_radius, m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_FLOAT)
    {
        ::fwIGG::Mesher::computeMeshMCS<float>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                               m_percentage, m_closing);
    }
    else if (image->getType() == ::fwTools::Type::s_DOUBLE)
    {
        ::fwIGG::Mesher::computeMeshMCS<double>(image, mesh, m_valueMin, m_valueMax, m_faces, m_adapt, m_radius,
                                                m_percentage, m_closing);
    }

    // Create a reconstruction to put the mesh
    ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();

    std::stringstream sstream;
    sstream << "CGoGNMesher-"<< reconstruction->getID().substr(25) << "_" << m_valueMin << "_" << m_valueMax << "_";
    sstream << m_radius << "_" << m_adapt << "_" << m_faces << "_" << m_percentage << "_" << m_closing;
    reconstruction->setOrganName(sstream.str());
    reconstruction->setStructureType("OrganType");
    reconstruction->setIsVisible(true);

    reconstruction->setMesh(mesh);


    ::fwMedData::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();
    recs.push_back(reconstruction);
    modelSeries->setReconstructionDB(recs);

    /// Notification
    auto sig = modelSeries->signal< ::fwMedData::ModelSeries::ReconstructionsAddedSignalType >
                   (::fwMedData::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG);
    sig->asyncEmit(recs);
}

//-----------------------------------------------------------------------------

void CGoGNMesher::setInteger(int val, std::string key)
{
    if(key == "valueMin")
    {
        m_valueMin = val;
    }
    else if(key == "valueMax")
    {
        m_valueMax = val;
    }
    else if(key == "adapt")
    {
        m_adapt = val;
    }
    else if(key == "radius")
    {
        m_radius = val;
    }
    else if(key == "faces")
    {
        m_faces = val;
    }

}

//-----------------------------------------------------------------------------

void CGoGNMesher::setBoolean(bool val, std::string key)
{

    if(key == "percentage")
    {
        m_percentage = val;
    }
    else if(key == "closing")
    {
        m_closing = val;
    }
}

//-----------------------------------------------------------------------------

}
