/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_COMPOSITE_CTIMAGESTORAGEDEFAULTCOMPOSITE_HPP__
#define __FWDICOMIOFILTER_COMPOSITE_CTIMAGESTORAGEDEFAULTCOMPOSITE_HPP__

#include "fwDicomIOFilter/composite/IComposite.hpp"
#include "fwDicomIOFilter/config.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace composite
{

/**
 * @brief Default filter used to read CTImageStorage SOP class.
 */
class FWDICOMIOFILTER_CLASS_API CTImageStorageDefaultComposite : public IComposite
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (CTImageStorageDefaultComposite)(IComposite),
                                            (()), ::fwDicomIOFilter::factory::New< CTImageStorageDefaultComposite > );

    /// Constructor
    FWDICOMIOFILTER_API CTImageStorageDefaultComposite(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~CTImageStorageDefaultComposite();

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const;

protected:

    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;
};

} // namespace composite
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_COMPOSITE_CTIMAGESTORAGEDEFAULTCOMPOSITE_HPP__ */
