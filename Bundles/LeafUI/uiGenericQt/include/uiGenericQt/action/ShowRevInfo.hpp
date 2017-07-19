/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIGENERICQT_ACTION_SHOWREVINFO_HPP__
#define __UIGENERICQT_ACTION_SHOWREVINFO_HPP__

#include <map>
#include <set>

#include <QObject>
#include <QTextDocument>

#include <boost/filesystem/path.hpp>

#include <fwGui/IActionSrv.hpp>

#include "uiGenericQt/config.hpp"

namespace uiGeneric
{

namespace action
{

/**
 * @brief   This action show the revision info for bundle and lib.
 * @class   ShowRevInfo
 */
class UIGENERICQT_CLASS_API ShowRevInfo : public QObject,
                                          public ::fwGui::IActionSrv
{
Q_OBJECT
public:

    typedef std::map<std::string, ::boost::filesystem::path> FindMapType;
    typedef std::set<std::string> BundleNameSet;
    typedef std::set<std::string> LibNameSet;
    typedef std::pair<BundleNameSet, LibNameSet> BundleLibPairType;
    typedef std::map<std::string, BundleLibPairType> RevMapType;

    fwCoreServiceClassDefinitionsMacro ( (ShowRevInfo)( ::fwGui::IActionSrv) );

    UIGENERICQT_API ShowRevInfo() noexcept;

    UIGENERICQT_API virtual ~ShowRevInfo() noexcept;

protected:

    /**
     * @brief configure the action.
     * @code{.xml}
       <service type="::fwGui::IActionSrv" impl="::uiGeneric::action::ShowRevInfo" autoConnect="no" />
       @endcode
     */
    UIGENERICQT_API void configuring();

    /// Starts action
    UIGENERICQT_API void starting();

    /// Show the frame
    UIGENERICQT_API void updating();

    /// Stops action
    UIGENERICQT_API void stopping();

    UIGENERICQT_API void info(std::ostream &_sstream );

private:

    const FindMapType findRevInfo(const ::boost::filesystem::path& findPath);
    void getRevMap(const FindMapType& findMap, RevMapType& revMap, bool isBundle = false);

    QTextDocument* generateReport();

protected Q_SLOTS:

    void saveReport();

};

} // namespace action
} // namespace uiGeneric

#endif /*__UIGENERICQT_ACTION_SHOWREVINFO_HPP__*/
