/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_ROITRAITS_HPP__
#define __FWDATA_ROITRAITS_HPP__

#include "fwData/Node.hpp"
#include "fwData/StructureTraits.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class   ROITraits
 * @brief   This class defines ROI traits
 * @author  IRCAD (Research and Development Team).
 * @date    2007-20011.
 */
class FWDATA_CLASS_API ROITraits : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ROITraits)(::fwData::Object), (()), ::fwTools::Factory::New< ROITraits >) ;

    fwGettersSettersDocMacro(Identifier, identifier, std::string, the ROIs identifier);

    fwGettersSettersDocMacro(MaskNode, maskNode, ::fwData::Node::wptr, the ROI mask node used for ROI);

    fwGettersSettersDocMacro(StructureTraits, structureTraits, StructureTraits::wptr, the associated structure traits);

    fwGettersSettersDocMacro(EvaluatedExp, evaluatedExp, StructureTraits::ROIExpression, the ROI evaluated expression);

protected :

    /**
     * @brief constructor
     */
    FWDATA_API ROITraits();

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~ROITraits() throw();

private:

    /// ROI identifier
    std::string m_identifier;

    /// mask node used for ROITraits
    ::fwData::Node::wptr m_maskNode;

    /// associated structure traits
    ::fwData::StructureTraits::wptr m_structureTraits;

    /// roi evalulated expression
    StructureTraits::ROIExpression m_evaluatedExp;


};

} // namespace fwData

#endif // __FWDATA_ROITRAITS_HPP__
