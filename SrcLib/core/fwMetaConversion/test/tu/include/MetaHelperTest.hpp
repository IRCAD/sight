#ifndef __FWMETACONVERSION_UT_METAHELPERTEST_HPP__
#define __FWMETACONVERSION_UT_METAHELPERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMetaConversion
{
namespace ut
{

class MetaHelperTest :  public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( MetaHelperTest );
    CPPUNIT_TEST( dataToMeta );
    CPPUNIT_TEST( metaToData );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void dataToMeta();
    void metaToData();

};


}  // namespace ut
}  // namespace fwMetaConversion



#endif // __FWMETACONVERSION_UT_METAHELPERTEST_HPP__
