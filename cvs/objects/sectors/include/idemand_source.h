#ifndef _IDEMAND_SOURCE_H_
#define _IDEMAND_SOURCE_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*
* LEGAL NOTICE
* This computer software was prepared by Battelle Memorial Institute,
* hereinafter the Contractor, under Contract No. DE-AC05-76RL0 1830
* with the Department of Energy (DOE). NEITHER THE GOVERNMENT NOR THE
* CONTRACTOR MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
* LIABILITY FOR THE USE OF THIS SOFTWARE. This notice including this
* sentence must appear on any copies of this computer software.
*
* EXPORT CONTROL
* User agrees that the Software will not be shipped, transferred or
* exported into any country or used in any manner prohibited by the
* United States Export Administration Act or any other applicable
* export laws, restrictions or regulations (collectively the "Export Laws").
* Export of the Software may require some form of license or other
* authority from the U.S. Government, and failure to obtain such
* export control license may result in criminal liability under
* U.S. laws. In addition, if the Software is identified as export controlled
* items under the Export Laws, User represents and warrants that User
* is not a citizen, or otherwise located within, an embargoed nation
* (including without limitation Iran, Syria, Sudan, Cuba, and North Korea)
*     and that User is not otherwise prohibited
* under the Export Laws from receiving the Software.
*
* Copyright 2011 Battelle Memorial Institute.  All Rights Reserved.
* Distributed as open-source under the terms of the Educational Community
* License version 2.0 (ECL 2.0). http://www.opensource.org/licenses/ecl2.php
*
* For further details, see: http://www.globalchange.umd.edu/models/gcam/
*
*/


/*!
 * \file idemand_source.h
 * \ingroup Objects
 * \brief IDemandSource interface header file.
 * \author David Morrow, based on code by Josh Lurz
 */
 // GCAM-CDR
#include <vector>
#include <boost/core/noncopyable.hpp>

#include "util/base/include/inamed.h"
#include "util/base/include/istandard_component.h"
#include "util/base/include/data_definition_util.h"
#include <sectors\include\afinal_demand.h>
#include <sectors\include\CDR_final_demand.h>

class IInput;

// Need to forward declare the subclasses as well.
class DemandSource;
class ElasticDemandSource;
class OffsetDemandSource;
class AccumulatedDemandSource; 

/*!
 * \ingroup Objects
 * \brief This object is responsible for calculating demand for CDR.
 * \details This object can be added to a CDRFinalDemand object to create
 *          demand for CDR. The object is responsible for calculating how
 *          much demand to add. 
 * \author David Morrow, based on code by Josh Lurz
*/
class IDemandSource : public INamed, public IParsedComponent, private boost::noncopyable {
public:
    // Clone operator must be declared explicitly even though it is inherited
    // from IStandardComponent so that the return type can be changed. Since
    // this class is a subtype of IStandardComponent, this is legal and referred
    // to as a covariant return type.
    virtual IDemandSource* clone() const = 0;

    /*! \brief Returns whether the type of the object is the same as the passed
    *          in type.
    * \param aType Type to check the object's type against.
    * \return Whether the type of the object is the same as the passed in type.
    */
    virtual bool isSameType( const std::string& aType ) const = 0;

    /*! \brief Parse the data for this object starting at a given node.
    * \param aNode Root node from which to parse data.
    */
    virtual bool XMLParse( const xercesc::DOMNode* aNode ) = 0;

    /*! \brief Write data from this object in an XML format for debugging.
    * \param aPeriod Period for which to write data.
    * \param aOut Filestream to which to write.
    * \param aTabs Object responsible for writing the correct number of tabs.
    */
    virtual void toDebugXML( const int aPeriod, std::ostream& aOut, Tabs* aTabs ) const = 0;

    /*! \brief Complete the initialization of the demand source.
    * \param aRegionName Region name.
    * \param aParent Reference to a CDRFinalDemand object.
    */
    virtual void completeInit( const std::string& aRegionName, CDRFinalDemand& aParent );

    /*!
     * \brief Initialize the demand source for a given period.
     * \param aRegionName Region name.
     * \param aGDP Pointer to a GDP object.
     * \param aDemo Pointer to a Demographic object.
     * \param aPeriod Model period.
     * \param aParent Reference to a CDRFinalDemand object.
     */
    virtual void initCalc( const std::string& aRegionName, 
                           const GDP* aGDP, 
                           const Demographic* aDemographics, 
                           const int aPeriod, 
                           CDRFinalDemand& aParent );

    /*!
     * \brief Returns the name of the demand source.
     */
    virtual const std::string& getName() const;

    /*!
     * \brief Calculates the demand from this demand source.
     * \param aPeriod Model period.
     * \param aRegionName Region name.
     */
    virtual double getDemand( const int aPeriod, const std::string& aRegionName );

    /*!
     * \brief Reports whether the demand source is subject to a spending cap.
     * \details A CDRFinalDemand object can limit overall demand to ensure
     *          that aggregate spending on CDR does not exceed a certain 
     *          level. Some kinds of demand sources are exempt from that
     *          spending cap by default, bust most demand sources allow
     *          users to modify that setting. This function tells the
     *          CDRFinalDemand object whether to count demand from this
     *          demand source toward the demand subject to a spending cap.
     * \return Whether the demand source is subject to the spending cap. 
     */
    virtual const bool isCapped() const;

protected:

    DEFINE_DATA(
        /* Declare all subclasses of IDemandSource to allow automatic traversal of the
         * hierarchy under introspection.
         */
        DEFINE_SUBCLASS_FAMILY( IDemandSource, DemandSource,
            ElasticDemandSource, OffsetDemandSource,
            AccumulatedDemandSource )
    )
};

#endif // _IDEMAND_SOURCE_H_
