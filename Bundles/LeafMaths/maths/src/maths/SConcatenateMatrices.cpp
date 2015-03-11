/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "maths/SConcatenateMatrices.hpp"

#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>


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

void SConcatenateMatrices::starting() throw (fwTools::Failed)
{
    m_connections = ::fwServices::helper::SigSlotConnection::New();

    for( TransformMatrix &currentMatrix : m_matrixVector)
    {
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(currentMatrix.m_uid);
        SLM_ASSERT("Object '" + currentMatrix.m_uid + "' is not found.", obj);

        currentMatrix.m_matrix = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        SLM_ASSERT("Object '" + currentMatrix.m_uid + "' is not a TransformationMatrix3D", currentMatrix.m_matrix);

        if(currentMatrix.m_connect)
        {
            m_connections->connect(currentMatrix.m_matrix, this->getSptr(), this->getObjSrvConnections());
        }
    }
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::stopping() throw (fwTools::Failed)
{
    m_connections->disconnect();
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::updating() throw (fwTools::Failed)
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

    ::fwComEd::TransformationMatrix3DMsg::sptr msg = ::fwComEd::TransformationMatrix3DMsg::New();
    msg->addEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED);
    ::fwServices::IEditionService::notify(this->getSptr(), matrix, msg);
}

// ----------------------------------------------------------------------------

void SConcatenateMatrices::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    ::fwComEd::TransformationMatrix3DMsg::csptr matrixMsg =
        ::fwComEd::TransformationMatrix3DMsg::dynamicConstCast(_msg);

    if (matrixMsg && matrixMsg->getSubject().lock() != this->getObject())
    {
        this->updating();
    }
}

// ----------------------------------------------------------------------------


}  // namespace maths
