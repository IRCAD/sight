/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_ACTION_SINITNEWSERIES_HPP__
#define __UIMEDDATAQT_ACTION_SINITNEWSERIES_HPP__

#include "uiMedDataQt/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwMedData/Series.hpp>

namespace uiMedDataQt
{
namespace action
{
/**
 * @brief   This action allows to update ::uiMedDataQt::editor::SSelector UI in order to add new series.
 * @note The action adds an empty series in each patient/study group to select where the series must be added.
 */
class UIMEDDATAQT_CLASS_API SInitNewSeries : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (SInitNewSeries)(::fwGui::IActionSrv) );

    /// Constructor
    UIMEDDATAQT_API SInitNewSeries();

    /// Destructor
    UIMEDDATAQT_API virtual ~SInitNewSeries() noexcept;

protected:

    /// Starts service.
    virtual void starting() override;

    /// Stops service. Does nothing.
    virtual void stopping() override;

    /**
     *
     * @code{.xml}
       <service uid="initSeriesUid" impl="::uiMedDataQt::action::SInitNewSeries" type="::fwGui::IActionSrv"
     * autoConnect="yes">
         <seriesDB>seriesDB_UID</seriesDB>
         <institution>My great company</institution>
       </service>
       @endcode
     * \b seriesDB : UID of series DB used to add new series for each patient/study group
     * \b institution (not mandatory) : default value to be set for new series equipment's institution
     */
    virtual void configuring() override;

    /// Adds the fake series in series DB.
    virtual void updating() override;

    virtual void info( std::ostream& _sstream ) override;

private:

    /// UID of referring seriesDB
    std::string m_seriesDBId;

    /// Default value for Dicom series attribute "Institution".
    std::string m_defaultInstitution;
};

} // namespace action
} // namespace uiMedDataQt

#endif // __UIMEDDATAQT_ACTION_SINITNEWSERIES_HPP__

