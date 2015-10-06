/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "syncTimeline/SMatrixTLSynchronizer.hpp"

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/fwID.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <extData/MatrixTL.hpp>
#include <extData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwComEd/helper/Array.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <functional>

fwServicesRegisterMacro(::arServices::ISynchronizer, ::syncTimeline::SMatrixTLSynchronizer, ::extData::MatrixTL);

namespace syncTimeline
{

// ----------------------------------------------------------------------------

SMatrixTLSynchronizer::SMatrixTLSynchronizer() throw ()
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::configuring() throw (::fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    ConfigurationType matricesConfig = m_configuration->findConfigurationElement("matrices");
    SLM_WARN_IF("Missing \"matrices\" tag.", !matricesConfig);

    if (matricesConfig)
    {
        std::vector< ConfigurationType > matrixCfgs = matricesConfig->find("matrix");

        SLM_ASSERT("Missing 'matrix' tag", !matrixCfgs.empty() );

        for(ConfigurationType cfg : matrixCfgs)
        {
            const std::string index      = cfg->getAttributeValue("index");
            const std::uint8_t indexInTL = ::boost::lexical_cast< std::uint8_t >(index);

            const std::string matrixUID = cfg->getAttributeValue("to");

            SLM_ASSERT("Missing attribute 'index' and/or 'to'", !index.empty() && !matrixUID.empty() );
            m_matrixIndexName[indexInTL] = matrixUID;
        }
    }
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::starting() throw (::fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::stopping() throw (::fwTools::Failed)
{
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::updating() throw (::fwTools::Failed)
{
    this->synchronize();
}

// ----------------------------------------------------------------------------

void SMatrixTLSynchronizer::synchronize()
{

    ::fwCore::HiResClock::HiResClockType currentTimestamp =
        std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();

    ::extData::MatrixTL::sptr matrixTL            = this->getObject< ::extData::MatrixTL >();
    CSPTR(::extData::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(currentTimestamp);

    if(buffer)
    {
        for(const MatrixIndexNameType::value_type& el : m_matrixIndexName)
        {
            if(buffer->isPresent(el.first))
            {
                const float* values = buffer->getElement(el.first);

                ::fwTools::Object::sptr obj                   = ::fwTools::fwID::getObject(el.second);
                ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::dynamicCast(obj);

                SLM_ASSERT("Matrix '" + el.second + "' not found.", matrix);

                for(unsigned int i = 0; i < 4; ++i)
                {
                    for(unsigned int j = 0; j < 4; ++j)
                    {
                        matrix->setCoefficient(i,j,values[i*4+j]);
                    }
                }

                auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(
                    ::fwData::Object::s_MODIFIED_SIG);
                sig->asyncEmit();
            }
        }
    }
}

// ----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SMatrixTLSynchronizer::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::extData::MatrixTL::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

// ----------------------------------------------------------------------------
}  // namespace syncTimeline
