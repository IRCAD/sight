/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "maths/SConcatenateMatrices.hpp"

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>


fwServicesRegisterMacro(::fwServices::IController, ::maths::SConcatenateMatrices, ::fwData::TransformationMatrix3D);

namespace maths
{

// ----------------------------------------------------------------------------

SConcatenateMatrices::SConcatenateMatrices() throw ()
{
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::configuring() throw (::fwTools::Failed)
{
    if(this->isVersion2())
    {
        typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
        std::vector< ConfigurationType > inCfgs = m_configuration->find("in");
        SLM_ASSERT("Config must contain one input group named 'matrix'.", inCfgs.size() == 1);

        SLM_ASSERT("Missing 'in group=\"matrix\"'", inCfgs[0]->getAttributeValue("group") == "matrix");

        std::vector< ConfigurationType > matrixCfgs = inCfgs[0]->find("key");

        for(ConfigurationType cfg : matrixCfgs)
        {
            TransformMatrix currentMatrix;
            const std::string inverse = cfg->getAttributeValue("inverse");
            if(!inverse.empty())
            {
                currentMatrix.m_inverse = (inverse == "true");
            }
            else
            {
                currentMatrix.m_inverse = false;
            }

            currentMatrix.m_connect = false; // Unused with appXml2
            m_matrixVector.push_back(currentMatrix);
        }
    }
    else
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
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::starting() throw (fwTools::Failed)
{
    if(!this->isVersion2())
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
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::stopping() throw (fwTools::Failed)
{
    if(!this->isVersion2())
    {
        m_connections.disconnect();
    }
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::updating() throw (fwTools::Failed)
{
    if(this->isVersion2())
    {
        auto inverse      = ::fwData::TransformationMatrix3D::New();
        auto outputMatrix = this->getInOut< ::fwData::TransformationMatrix3D >("output");

        ::fwDataTools::TransformationMatrix3D::identity(outputMatrix);

        size_t index = 0;
        for( TransformMatrix currentMatrix : m_matrixVector)
        {
            auto inputMatrix = this->getInput< ::fwData::TransformationMatrix3D >("matrix", index++);
            if( currentMatrix.m_inverse )
            {
                ::fwDataTools::TransformationMatrix3D::invert(inputMatrix, inverse);
                ::fwDataTools::TransformationMatrix3D::multiply(outputMatrix, inverse, outputMatrix);
            }
            else
            {
                ::fwDataTools::TransformationMatrix3D::multiply(outputMatrix, inputMatrix, outputMatrix);
            }
        }

        auto sig = outputMatrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
    else
    {
        ::fwData::TransformationMatrix3D::sptr inverse = ::fwData::TransformationMatrix3D::New();
        ::fwData::TransformationMatrix3D::sptr matrix  = this->getObject< ::fwData::TransformationMatrix3D >();

        ::fwDataTools::TransformationMatrix3D::identity(matrix);

        for( TransformMatrix currentMatrix : m_matrixVector)
        {
            if( currentMatrix.m_inverse )
            {
                ::fwDataTools::TransformationMatrix3D::invert(currentMatrix.m_matrix, inverse);
                ::fwDataTools::TransformationMatrix3D::multiply(matrix, inverse, matrix);
            }
            else
            {
                ::fwDataTools::TransformationMatrix3D::multiply(matrix, currentMatrix.m_matrix, matrix);
            }
        }

        auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
}

// ----------------------------------------------------------------------------


}  // namespace maths
