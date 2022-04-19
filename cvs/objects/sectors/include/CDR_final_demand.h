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


#ifndef _CDR_FINAL_DEMAND_H_
#define _CDR_FINAL_DEMAND_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*!
 * \file CDR_final_demand.h
 * \ingroup Objects
 * \brief The CDRFinalDemand class header file.
 * \author David Morrow
 */

#include <vector>
#include <xercesc/dom/DOMNode.hpp>
#include "marketplace/include/cached_market.h" // GCAM-CDR
#include "sectors/include/afinal_demand.h"
#include "util/base/include/value.h"
#include "util/base/include/time_vector.h"

#include "sectors/include/demand_source.h"

 // Forward declarations
class GDP;
class Demographic;
class DemandSource;
class SpendingCap;

/*!
 * \ingroup Objects
 * \brief A class which manages final demand for CDR (or similar goods).
 *
 * \details Final demand for CDR works differently than for other goods.
 *          This object uses one or more DemandSource objects to
 *          manage final demand in a particular region. See the various
 *          types of DemandSource objects listed in the DemandSourceFactory.
 *
 */

class CDRFinalDemand : public AFinalDemand
{

public:
    static const std::string& getXMLNameStatic();

    CDRFinalDemand();

    virtual ~CDRFinalDemand();

    virtual bool XMLParse( const xercesc::DOMNode* aNode );

    virtual void toDebugXML( const int aPeriod,
        std::ostream& aOut,
        Tabs* aTabs ) const;

    virtual const std::string& getName() const;

    virtual void completeInit( const std::string& aRegionName,
        const IInfo* aRegionInfo );

    virtual void initCalc( const std::string& aRegionName,
        const GDP* aGDP,
        const Demographic* aDemographics,
        const int aPeriod );

    virtual void setFinalDemand( const std::string& aRegionName,
        const Demographic* aDemographics,
        const GDP* aGDP,
        const int aPeriod );


    virtual double getWeightedEnergyPrice( const std::string& aRegionName,
        const int aPeriod ) const;

    virtual void accept( IVisitor* aVisitor, const int aPeriod ) const;

protected:


    virtual const std::string& getXMLName() const;

    /* The SpendingCap has been disabled in this version. Uncomment the code block below to activate it.*/
    /*
    class SpendingCap {

    public:
        SpendingCap();

        static const std::string& getXMLNameStatic();
        virtual bool XMLParse( const xercesc::DOMNode* aNode );
        virtual void toDebugXML( const int aPeriod, std::ostream& aOut, Tabs* aTabs ) const;
        virtual const std::string& getName() const;
        virtual void completeInit( const std::string& aRegionName );

        virtual double getSpendingCap( const std::string& aRegionName, const GDP* aGDP, const int aPeriod );
        virtual const std::string& getGrowthLimitingMarket() const;

    protected:

        // The percentage of GDP for the base spending cap
        objects::PeriodVector<double> mGDPPercentage;

        // The name of the CO2 tax to use to adjust spending cap
        std::string mTaxName;

        // A multiplier to apply to the spending cap as the CO2 tax rises
        // relative to GDP per capita (PPP)
        double mMultiplier;

        // Maximum percentage of GDP for this spending cap
        double mMaxPercentage;

        // Name of the market that limits CDR growth.
        std::string mGrowthLimitingMarket;
    };
    */


    // Define data such that introspection utilities can process the data from this
    // subclass together with the data members of the parent classes.
    DEFINE_DATA_WITH_PARENT(
        AFinalDemand,

        // Basic features of the final demand objecct

        //! Name of the final demand and the good it consumes.
        DEFINE_VARIABLE( SIMPLE, "name", mName, std::string ),

        //! Name of the market region for CDR final demand.
        DEFINE_VARIABLE( SIMPLE, "market", mMarketRegion, std::string ),

        //! Maximum demand
        DEFINE_VARIABLE( SIMPLE, "max-demand", mMaxDemand, double ),

        //! An array of DemandSource objects to generate demand
        DEFINE_VARIABLE( ARRAY, "demand-sources", mDemandSources, std::vector<DemandSource*> ),

        //! A SpendingCap object
        // DRM: We don't expect or use multiple spending caps,
        // but using the PeriodVector avoids having to create a
        // new ParseContainerNode function.
        /* The SpendingCap has been disabled in this version. Uncomment the line below to activate it.*/
        //DEFINE_VARIABLE( ARRAY, "spending-cap", mSpendingCap, std::vector<SpendingCap*> ),

        //! Raw demand, independent of any constraints
        DEFINE_VARIABLE( ARRAY, "raw-demand", mRawDemand, objects::PeriodVector<double> ),

        //! State value necessary to use Marketplace::addToDemand
        DEFINE_VARIABLE( SIMPLE | STATE, "CDR-demand", mCDRDemand, Value )

    );

    //! A pre-located market which has been cached from the marketplace to get
    //! the price and add demands to.
    std::auto_ptr<CachedMarket> mCachedMarket;


};

#endif // _CDR_FINAL_DEMAND_H_
