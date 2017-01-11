/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "maths/SAddMatrices.hpp"

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>


fwServicesRegisterMacro(::fwServices::IController, ::maths::SAddMatrices, ::fwData::TransformationMatrix3D);

namespace maths
{

// ----------------------------------------------------------------------------

SAddMatrices::SAddMatrices() throw ()
{
}

// ----------------------------------------------------------------------------

void SAddMatrices::configuring() throw (::fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector< ConfigurationType > matrixCfgs = m_configuration->find("matrix");

    for(ConfigurationType cfg : matrixCfgs)
    {
        TransformMatrix currentMatrix;
        currentMatrix.m_uid = cfg->getValue();

        const std::string inverse = cfg->getAttributeValue("inverse");
        if(!inverse.empty())
        {
            currentMatrix.m_inverse = (inverse == "true");

        }
        else
        {
            currentMatrix.m_inverse = false;
        }

        const std::string connect = cfg->getAttributeValue("connect");
        if(!connect.empty())
        {
            currentMatrix.m_connect = (connect == "true");
        }
        else
        {
            currentMatrix.m_connect = true;
        }

        m_matrixVector.push_back(currentMatrix);
    }
}

// ----------------------------------------------------------------------------

void SAddMatrices::starting() throw (fwTools::Failed)
{
    for( TransformMatrix &currentMatrix : m_matrixVector)
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(currentMatrix.m_uid);
        SLM_ASSERT("Object '" + currentMatrix.m_uid + "' is not found.", obj);

        currentMatrix.m_matrix = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        SLM_ASSERT("Object '" + currentMatrix.m_uid + "' is not a TransformationMatrix3D", currentMatrix.m_matrix);

        if(currentMatrix.m_connect)
        {
            m_connections.connect(currentMatrix.m_matrix, this->getSptr(), this->getObjSrvConnections());
        }
    }
}

// ----------------------------------------------------------------------------

void SAddMatrices::stopping() throw (fwTools::Failed)
{
    m_connections.disconnect();
}

// ----------------------------------------------------------------------------

void SAddMatrices::updating() throw (fwTools::Failed)
{
    ::fwData::TransformationMatrix3D::sptr inverse = ::fwData::TransformationMatrix3D::New();
    ::fwData::TransformationMatrix3D::sptr matrix  = this->getObject< ::fwData::TransformationMatrix3D >();

    ::fwDataTools::TransformationMatrix3D::identity(matrix);

    for( TransformMatrix currentMatrix : m_matrixVector)
    {
        if( currentMatrix.m_inverse )
        {
            ::fwDataTools::TransformationMatrix3D::invert(currentMatrix.m_matrix, inverse);
            addMatrices(matrix, inverse, matrix);
        }
        else
        {
            addMatrices(matrix, currentMatrix.m_matrix, matrix);
        }
    }

    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

void SAddMatrices::addMatrices(::fwData::TransformationMatrix3D::sptr trfA,
                               ::fwData::TransformationMatrix3D::sptr trfB,
                               ::fwData::TransformationMatrix3D::sptr _output)
{
    const ::glm::dmat4x4 matA = ::glm::make_mat4<double>(trfA->getCoefficients().data());
    const ::glm::dmat4x4 matB = ::glm::make_mat4<double>(trfB->getCoefficients().data());

    const ::glm::dmat4x4 matC = matB + matA;

    auto& coefs = _output->getRefCoefficients();
    for (size_t i = 0; i < 4; ++i)
    {
        const size_t rowDst          = i * 4;
        const ::glm::length_t rowSrc = static_cast< ::glm::length_t>(i);
        for (size_t j = 0; j < 4; ++j)
        {
            const ::glm::length_t colSrc = static_cast< ::glm::length_t>(j);
            coefs[rowDst + j] = matC[rowSrc][colSrc];
        }
    }
}

// ----------------------------------------------------------------------------
}  // namespace maths
