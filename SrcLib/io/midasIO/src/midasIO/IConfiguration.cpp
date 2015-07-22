/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>

#include "midasIO/IConfiguration.hpp"

namespace midasIO
{

const IConfiguration::Privacy2NameType IConfiguration::s_PRIVACY_NAMES
    = ::boost::assign::map_list_of(PUBLIC, "Public")(PRIVATE, "Private")(PARENT, "Parent");

const IConfiguration::LicenseContainerType IConfiguration::s_LICENSES = ::boost::assign::map_list_of
                                                                            (PUBLIC_PDDL,           "Public (PDDL)")
                                                                            (PUBLIC_ODC_BY,
                                                                            "Public: Attribution (ODC-BY)")
                                                                            (PUBLIC_ODBL,
                                                                            "Public: Attribution, Share-Alike (ODBL)")
                                                                            (PRIVATE_ARR,
                                                                            "Private: All right reserved")
                                                                            (PUBLIC_CC_BY_3,
                                                                            "Public: Attribution (CC BY 3.0)")
                                                                            (PUBLIC_CC_BY_SA_3,
                                                                            "Public: Attribution, Share-Alike (CC BY-SA 3.0)")
                                                                            (PUBLIC_CC_BY_ND_3,
                                                                            "Public: Attribution, No Derivative Works (CC BY-ND 3.0)")
                                                                            (PUBLIC_CC_BY_NC_3,
                                                                            "Public: Attribution, Non Commercial, (CC BY-NC 3.0)")
                                                                            (PUBLIC_CC_BY_NC_SA_3,
                                                                            "Public: Attribution, Non Commercial, Share-Alike (CC BY-NC-SA 3.0)")
                                                                            (PUBLIC_CC_BY_NC_ND_3,
                                                                            "Public: Attribution, Non Commercial, No Derivative Works (CC BY-NC-ND 3.0)");

IConfiguration::IConfiguration()
{
    m_attrRootFolderName = "Patients";
    m_attrAppName        = "Default";
    m_attrPrivacy        = PRIVATE;
    m_attrDefaultLicense = PRIVATE_ARR;
}

} // namespace midasIO

