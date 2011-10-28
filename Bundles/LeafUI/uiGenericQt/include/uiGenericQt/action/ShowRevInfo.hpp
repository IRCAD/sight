/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIGENERICQT_UI_ACTION_SHOWACK_HPP_
#define _UIGENERICQT_UI_ACTION_SHOWACK_HPP_

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
 * @class   ShowRevInfo.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class UIGENERICQT_CLASS_API ShowRevInfo : public QObject, public ::fwGui::IActionSrv
{
    Q_OBJECT
public:

    typedef std::map<std::string, ::boost::filesystem::path> FindMapType;
    typedef std::set<std::string> BundleNameSet;
    typedef std::set<std::string> LibNameSet;
    typedef std::pair<BundleNameSet, LibNameSet> BundleLibPairType;
    typedef std::map<std::string, BundleLibPairType> RevMapType;

    fwCoreServiceClassDefinitionsMacro ( (ShowRevInfo)( ::fwGui::IActionSrv) ) ;

    UIGENERICQT_API ShowRevInfo() throw() ;

    UIGENERICQT_API virtual ~ShowRevInfo() throw() ;

protected:

    /**
     * @brief configure the action.
     * @verbatim
    <service type="::fwGui::IActionSrv" implementation="::uiGeneric::action::ShowRevInfo" autoComChannel="no" />
       @endverbatim
     */
    UIGENERICQT_API void configuring() throw( ::fwTools::Failed ) ;

    /// Starts action
    UIGENERICQT_API void starting() throw (::fwTools::Failed);

    /// Show the frame
    UIGENERICQT_API void updating() throw (::fwTools::Failed);

    /// Does nothing
    UIGENERICQT_API void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    /// Stops action
    UIGENERICQT_API void stopping() throw (::fwTools::Failed);

    UIGENERICQT_API void info(std::ostream &_sstream ) ;

private:

    const FindMapType findRevInfo(const ::boost::filesystem::path& findPath);
    void getRevMap(const FindMapType& findMap, RevMapType& revMap, bool isBundle = false);

    QTextDocument* generateReport();

protected slots:

    void saveReport();

};

} // namespace action
} // namespace uiGeneric

#endif /*_UIGENERICQT_ACTION_SHOWACK_HPP_*/
