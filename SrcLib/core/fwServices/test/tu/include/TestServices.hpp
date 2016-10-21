/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSERVICES_UT_TESTSERVICES_HPP__
#define __FWSERVICES_UT_TESTSERVICES_HPP__

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>


namespace fwServices
{
namespace ut
{

/**
 * @brief   Service type for test.
 */
class TestService : public ::fwServices::IService
{
public:
    fwCoreServiceClassDefinitionsMacro ( (TestService)(::fwServices::IService) );
    TestService() throw()
        :   m_isUpdated(false)
    {
    }

    virtual ~TestService() throw()
    {
    }

    /// return true if the service is updated with updating() method
    bool getIsUpdated()
    {
        return m_isUpdated;
    }

protected:
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
    virtual void info( std::ostream& _sstream )
    {
        _sstream << "TestService";
    }

    bool m_isUpdated;
};

/**
 * @brief   Test service implementation for composite
 */
class TestServiceImplementationComposite : public TestService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementationComposite)(::fwServices::ut::TestService) );
    TestServiceImplementationComposite() throw()
    {
    }
    virtual ~TestServiceImplementationComposite() throw()
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
        SLM_TRACE_FUNC();
    }
    virtual void updating() throw(::fwTools::Failed)
    {
        m_isUpdated = true;
    }
};

/**
 * @brief   Test service implementation for image
 */
class TestServiceImplementationImage : public TestService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementationImage)(::fwServices::ut::TestService) );
    TestServiceImplementationImage() throw()
    {
    }
    virtual ~TestServiceImplementationImage() throw()
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

};

} //namespace ut
} //namespace fwServices

#endif // __FWSERVICES_UT_TESTSERVICES_HPP__
