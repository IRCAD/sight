/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <json_spirit/json_spirit_writer_template.h>

#include "IoMidasTest.hpp"

#include <fwServices/registry/ServiceFactory.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/Acquisition.hpp>

#include <midasIO/IResponseHandler.hpp>

#include "ioMidas/SModelSeriesWriter.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioMidas::ut::IoMidasTest );

namespace ioMidas
{
namespace ut
{

void IoMidasTest::setUp()
{
}

//------------------------------------------------------------------------------

void IoMidasTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

::midasIO::IConfiguration::sptr createConfigurationTest(
    const std::string& version, const std::string& appname, const std::string& url)
{

    ::midasIO::IConfiguration::sptr config;

    config = ::midasIO::configurationFactory::New(version);
    CPPUNIT_ASSERT(config);
    config->setAppName(appname);
    config->setUrl("http://code.google.com/p/fw4spl/midas");
    return config;
}

//------------------------------------------------------------------------------

void IoMidasTest::checkCreateConfigurationTest()
{
    const std::string version = "3.2.6";
    const std::string appname = "Default";
    const std::string url("http://code.google.com/p/fw4spl/midas");

    ::midasIO::IConfiguration::sptr config;

    config = createConfigurationTest(version, appname, url);
    CPPUNIT_ASSERT_MESSAGE("Url not equal", config->getUrl() == url );
    CPPUNIT_ASSERT_MESSAGE("Workspace not equal", config->getAppName() == appname);
}

//------------------------------------------------------------------------------

void IoMidasTest::checkGetTest()
{
    const std::string CREFRESPSTAT    = "ok";
    const std::string CREFRESPCODE    = "0";
    const std::string CREFRESPMESSAGE = "Hello";
    const std::string CREFRESPDATA    = "\"apikey\":\"cbd37826afeb77d20f4834383efff254\" ";
    const std::string CREFRESPAPIKEY  = "cbd37826afeb77d20f4834383efff254";

    std::string CRESPAPIKEY = "{\"stat\":\"ok\",\"code\":\"0\",\"message\":\"Hello\",\"data\":{\"apikey\"";
    CRESPAPIKEY += ":\"cbd37826afeb77d20f4834383efff254\"}}";

    ::midasIO::IResponseHandler::sptr resp = ::midasIO::responseHandlerFactory::New("3.2.6");
    resp->setJsonData(CRESPAPIKEY);
    ::midasIO::IResponseHandler::ObjectType obj = resp->get();

    CPPUNIT_ASSERT(obj.find("stat")!= obj.end());
    CPPUNIT_ASSERT_EQUAL(CREFRESPSTAT, obj["stat"].get_str());

    CPPUNIT_ASSERT(obj.find("code")!= obj.end());
    CPPUNIT_ASSERT_EQUAL(CREFRESPCODE, obj["code"].get_str());

    CPPUNIT_ASSERT(obj.find("message")!= obj.end());
    CPPUNIT_ASSERT_EQUAL(CREFRESPMESSAGE, obj["message"].get_str());

    ::midasIO::IResponseHandler::ObjectType objectData = resp->getObjectData();

    CPPUNIT_ASSERT(objectData.find("apikey")!= objectData.end());
    CPPUNIT_ASSERT_EQUAL(CREFRESPAPIKEY, objectData["apikey"].get_str());
}

//------------------------------------------------------------------------------

void IoMidasTest::checkStatFieldTest()
{
    std::string CRESPAPIKEY = "{\"stat\":\"ok\",\"code\":\"0\",\"message\":\"\",\"data\":{\"apikey\"";
    CRESPAPIKEY += ":\"cbd37826afeb77d20f4834383efff254\"}}";

    ::midasIO::IResponseHandler::sptr resp = ::midasIO::responseHandlerFactory::New("3.2.6");
    resp->setJsonData(CRESPAPIKEY);
    CPPUNIT_ASSERT_EQUAL(true, resp->isSuccess());

    std::string CRESPAUTHENTIFICATIONWRONG = "{\"stat\":\"fail\",\"message\":\"Unable to authenticate.";
    CRESPAUTHENTIFICATIONWRONG += "Please check credentials.\",\"code\":-150}";
    resp->setJsonData(CRESPAUTHENTIFICATIONWRONG);
    CPPUNIT_ASSERT_EQUAL(false, resp->isSuccess());

}

//------------------------------------------------------------------------------

void IoMidasTest::checkGetMessageTest()
{
    const std::string CREFMSG("Unable to authenticate.Please check credentials.");
    std::string CRSP = "{\"stat\":\"fail\",\"message\":\"Unable to authenticate.";
    CRSP += "Please check credentials.\",\"code\":-150}";

    ::midasIO::IResponseHandler::sptr resp = ::midasIO::responseHandlerFactory::New("3.2.6");
    resp->setJsonData(CRSP);
    CPPUNIT_ASSERT_EQUAL(CREFMSG, resp->getErrorMsg());
}

//------------------------------------------------------------------------------

void IoMidasTest::checkSetMessageTest()
{
    const std::string CREFMSG("Here the message.");
    std::string CRSP = "{\"stat\":\"ok\",\"code\":\"0\",\"message\":\"\",\"data\":{\"apikey\"";
    CRSP                                  += ":\"cbd37826afeb77d20f4834383efff254\"}}";
    ::midasIO::IResponseHandler::sptr resp = ::midasIO::responseHandlerFactory::New("3.2.6");
    resp->setJsonData(CRSP);
    resp->setErrorMsg(CREFMSG);
    CPPUNIT_ASSERT_EQUAL(CREFMSG, resp->getErrorMsg());
}

//------------------------------------------------------------------------------

void IoMidasTest::checkGetDataTest()
{
    const std::string CREFMODEL = "Community";

    std::string r = "{\"stat\":\"ok\",\"code\":\"0\",\"message\":\"\",\"data\":[{\"_model\":\"Community\",";
    r += "\"ModelLoader\":{},\"_key\":\"community_id\",\"saved\":true,\"community_id\":\"1\",\"name\":\"Developper\",";
    r += "\"description\":\"\",\"creation\":\"2013-07-01 15:04:47\",\"privacy\":\"0\",\"folder_id\":\"19\",";
    r += "\"publicfolder_id\":\"20\",\"privatefolder_id\":\"21\",\"admingroup_id\":\"1\",\"moderatorgroup_id\":\"2\",";
    r += "\"membergroup_id\":\"3\",\"can_join\":\"1\",\"view\":\"23\",\"uuid\":";
    r += "\"51d17e6f65e0be9dd3625ce92da245d4488016ef118b6\"},{\"_model\":\"Community\",\"ModelLoader\":{},\"_key\":";
    r += "\"community_id\",\"saved\":true,\"community_id\":\"2\",\"name\":\"PM\",\"description\":\"\",\"creation\":";
    r += "\"2013-07-18 16:38:30\",\"privacy\":\"1\",\"folder_id\":\"38\",\"publicfolder_id\":\"39\",";
    r += "\"privatefolder_id\":\"40\",\"admingroup_id\":\"4\",\"moderatorgroup_id\":\"5\",\"membergroup_id\":\"6\",";
    r += "\"can_join\":\"0\",\"view\":\"2\",\"uuid\":\"51e7fde68650a181a10da39e5f3b8794b7b3bb19080e9\"}]}";

    ::midasIO::IResponseHandler::sptr resp = ::midasIO::responseHandlerFactory::New("3.2.6");
    resp->setJsonData(r);

    ::midasIO::IResponseHandler::ArrayType array = resp->getArrayData();

    ::midasIO::IResponseHandler::ArrayType::iterator it = array.begin();
    ::midasIO::IResponseHandler::ObjectType& obj        = it->get_obj();

    CPPUNIT_ASSERT(obj.find("_model")!= obj.end());
    CPPUNIT_ASSERT_EQUAL(CREFMODEL, obj["_model"].get_str());
}

//------------------------------------------------------------------------------

void IoMidasTest::checkConfigurationTest()
{
    const std::string url("http://code.google.com/p/fw4spl/midas");
    const std::string appName("Default");
    const std::string privacy("Private");
    const std::string rootFolder("Patients");

    // Create Config 1
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr urlCfg = ::fwRuntime::EConfigurationElement::New("url");
    urlCfg->setValue(url);

    ::fwRuntime::EConfigurationElement::sptr appNameCfg = ::fwRuntime::EConfigurationElement::New("appName");
    appNameCfg->setValue(appName);

    ::fwRuntime::EConfigurationElement::sptr privacyCfg = ::fwRuntime::EConfigurationElement::New("privacy");
    privacyCfg->setValue(privacy);

    ::fwRuntime::EConfigurationElement::sptr rootFolderCfg = ::fwRuntime::EConfigurationElement::New("rootFolder");
    rootFolderCfg->setValue(rootFolder);

    srvCfg->addConfigurationElement(urlCfg);
    srvCfg->addConfigurationElement(appNameCfg);
    srvCfg->addConfigurationElement(privacyCfg);
    srvCfg->addConfigurationElement(rootFolderCfg);

    // Create and execute service
    ::fwData::Acquisition::sptr pAcq = ::fwData::Acquisition::New();

    const std::string srvType("::io::IWriter");
    const std::string srvImpl("::ioMidas::SModelSeriesWriter");

    ::fwServices::IService::sptr srv
        = ::fwServices::registry::ServiceFactory::getDefault()->create( srvType, srvImpl );
    CPPUNIT_ASSERT(srv);
    ::fwServices::OSR::registerService( pAcq, srv );
    srv->setConfiguration(srvCfg);

    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start());

    CPPUNIT_ASSERT_NO_THROW(srv->stop());
    ::fwServices::OSR::unregisterService( srv );

}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace ioMidas

