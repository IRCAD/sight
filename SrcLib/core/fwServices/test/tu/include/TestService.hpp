/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_TESTSERVICE_HPP__
#define __FWSERVICES_UT_TESTSERVICE_HPP__

#include <fwServices/IService.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwServices
{
namespace ut
{

/**
 * @brief Service interface for test
 */
class TestService : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (TestService)(::fwServices::IService) );
    TestService() throw()
        :   m_isUpdated(false),
          m_isUpdatedMessage(false)
    {
    }

    virtual ~TestService() throw()
    {
    }

    virtual void configuring() throw( ::fwTools::Failed )
    {
    }
    virtual void starting() throw(::fwTools::Failed)
    {
    }
    virtual void stopping() throw(::fwTools::Failed)
    {
    }
    virtual void updating() throw(::fwTools::Failed)
    {
    }
    virtual void info(std::ostream &_sstream )
    {
        _sstream << "TestService";
    }

    /// return true if the service is updated with update() method
    bool getIsUpdated()
    {
        return m_isUpdated;
    }

    /// return true if the service is updated with update(msg) method
    bool getIsUpdatedMessage()
    {
        return m_isUpdatedMessage;
    }

protected:
    bool m_isUpdated;
    bool m_isUpdatedMessage;
};


/**
 * @brief Service implementation for test
 */
class TestServiceImplementation : public TestService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementation)(::fwServices::ut::TestService) );
    TestServiceImplementation() throw()
    {
    }
    virtual ~TestServiceImplementation() throw()
    {
    }

    virtual void configuring() throw( ::fwTools::Failed )
    {
    }
    virtual void starting() throw(::fwTools::Failed)
    {
    }
    virtual void stopping() throw(::fwTools::Failed)
    {
    }
    virtual void updating() throw(::fwTools::Failed)
    {
        m_isUpdated = true;
    }

    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
    {
        m_isUpdatedMessage = true;
    }

    virtual void info(std::ostream &_sstream )
    {
        _sstream << "TestServiceImplementation";
    }
};

} //namespace ut
} //namespace fwServices

#endif //__FWSERVICES_UT_TESTSERVICE_HPP__

