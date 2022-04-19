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
 * \file CDR_final_demand.cpp
 * \ingroup Objects
 * \brief CDRFinalDemand class source file.
 * \author David Morrow
 */

#include <string>
#include <algorithm>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "util/base/include/definitions.h"
#include "util/base/include/xml_helper.h"
#include "util/base/include/configuration.h"
#include "util/base/include/model_time.h"
#include "util/base/include/ivisitor.h"
#include "containers/include/iactivity.h"
#include "containers/include/iinfo.h"
#include "containers/include/gdp.h" // GCAM-CDR
#include "containers/include/scenario.h"
#include "marketplace/include/marketplace.h"
#include "containers/include/market_dependency_finder.h"
#include "sectors/include/CDR_final_demand.h" // GCAM-CDR
#include "util/base/include/util.h" // GCAM-CDR
#include <sectors\include\sector_utils.h>
#include "sectors/include/demand_source_factory.h"
#include "sectors/include/demand_source.h"


using namespace std;
using namespace xercesc;

extern Scenario* scenario;

/*! \brief Constructor.
*/
CDRFinalDemand::CDRFinalDemand() :
    mRawDemand( 0.0 )
{
    // Set default that effectively places
    // no limit on output .
    mMaxDemand = util::getLargeNumber();
}

/*! \brief Destructor.
*/
CDRFinalDemand::~CDRFinalDemand() {
}

const string& CDRFinalDemand::getXMLName() const {
    return getXMLNameStatic();
}

/*! \brief Get the XML node name in static form for comparison when parsing XML.
*
* This public function accesses the private constant string, XML_NAME. This way
* the tag is always consistent for both read-in and output and can be easily
* changed. The "==" operator that is used when parsing, required this second
* function to return static.
* \note A function cannot be static and virtual.
* \author Josh Lurz, James Blackwood
* \return The constant XML_NAME as a static.
*/
const string& CDRFinalDemand::getXMLNameStatic() {
    const static string XML_NAME = "CDR-final-demand";
    return XML_NAME;
}

const string& CDRFinalDemand::getName() const {
    return mName;
}

// Read in information from the XML input file.
bool CDRFinalDemand::XMLParse( const DOMNode* aNode ) {

    assert( aNode );

    // get the name attribute.
    mName = XMLHelper<string>::getAttr( aNode, "name" );

    // get all child nodes.
    DOMNodeList* nodeList = aNode->getChildNodes();
    const Modeltime* modeltime = scenario->getModeltime();

    // loop through the child nodes.
    for ( unsigned int i = 0; i < nodeList->getLength(); ++i ) {
        DOMNode* curr = nodeList->item( i );
        const string nodeName = XMLHelper<string>::safeTranscode( curr->getNodeName() );

        if ( nodeName == "#text" ) {
            continue;
        }
        else if ( nodeName == "market" ) {
            mMarketRegion = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "max-demand" ) {
            mMaxDemand = XMLHelper<double>::getValue( curr );
        }
        else if ( DemandSourceFactory::isOfType( nodeName ) ) {
            parseContainerNode( curr, mDemandSources, DemandSourceFactory::create( nodeName ).release() );
        }
        // The Spending Cap feature has been disabled.
        //else if ( nodeName == SpendingCap::getXMLNameStatic() ) {
        //    parseContainerNode( curr, mSpendingCap, new SpendingCap() );
        //}
        else {
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::WARNING );
            mainLog << "Unknown element " << nodeName
                << " encountered while parsing " << getXMLName() << " in CDRFinalDemand" << endl;
        }
    }
    return true;
}

// Write output to the debug file.
void CDRFinalDemand::toDebugXML( const int aPeriod,
    ostream& aOut,
    Tabs* aTabs ) const
{
    XMLWriteOpeningTag( getXMLName(), aOut, aTabs, mName );
    const Modeltime* modeltime = scenario->getModeltime();

    // write the xml for the class members.
    XMLWriteElement( mMarketRegion, "market", aOut, aTabs );
    XMLWriteElement( mCDRDemand, "final-demand", aOut, aTabs );
    XMLWriteElement( mRawDemand[aPeriod], "raw-demand", aOut, aTabs );
    XMLWriteElement( mMaxDemand, "max-demand", aOut, aTabs );

    // The Spending Cap feature has been disabled.
    //if ( mSpendingCap.size() > 0 ) {
    //    mSpendingCap[ 0 ]->toDebugXML( aPeriod, aOut, aTabs );
    //}

    for ( unsigned int i = 0; i < mDemandSources.size(); ++i ) {
        mDemandSources[ i ]->toDebugXML( aPeriod, aOut, aTabs );
    }

    XMLWriteClosingTag( getXMLName(), aOut, aTabs );
}

/*! \brief Complete the initialization
*
* This routine is only called once per model run
*
* \author David Morrow
* \param aRegionName Name of a geopolitical model region.
* \param aRegionInfo Regional information object.
* \warning markets are not necessarily set when completeInit is called
*/
void CDRFinalDemand::completeInit( const string& aRegionName,
    const IInfo* aRegionInfo )
{
    // Set some default values.
    if ( mName.empty() ) {
        mName = "CDR";
    }
    if ( mMarketRegion.empty() ) {
        ILogger& CDRLog = ILogger::getLogger( "CDR_log" );
        CDRLog.setLevel( ILogger::DEBUG );
        CDRLog << "No market region set for " << getXMLName() << " in " << aRegionName
            << ". Defaulting to " << aRegionName << "." << endl;
        mMarketRegion = aRegionName;
    }

    // For regions outside the market region, add a dependency
    // between this region and the market region. This enables
    // GCAM to place this final demand in the global ordering
    // even though there is no matching supply sector in 
    // this region.
    if ( aRegionName != mMarketRegion ) {
        Marketplace* marketplace = scenario->getMarketplace();
        MarketDependencyFinder* depFinder = marketplace->getDependencyFinder();
        depFinder->addDependency( getName(), aRegionName, getName(), mMarketRegion, true );
    }

    // Initialize demand sources.
    for ( unsigned int i = 0; i < mDemandSources.size(); ++i ) {
        mDemandSources[i]->completeInit( aRegionName );
    }

}

/*! \brief Perform any initializations needed for each period.
*
* Any initializations or calculations that only need to be done once per period
* (instead of every iteration) should be placed in this function.
*
* \author David Morrow
* \param aRegionName Name of a geopolitical region
* \param aGDP GDP information object
* \param aDemographics Demographic information object
* \param aPeriod Model period
*/
void CDRFinalDemand::initCalc( const string& aRegionName,
    const GDP* aGDP,
    const Demographic* aDemographics,
    const int aPeriod )
{

    // Create the cached market to improve efficiency of setFinalDemand.
    Marketplace* marketplace = scenario->getMarketplace();
    mCachedMarket = marketplace->locateMarket( mName, mMarketRegion, aPeriod );

    //setMaxDemand( aRegionName, aPeriod ); // calculate maximum demand for this period

    // Run initCalc for all demand sources.
    for ( unsigned int i = 0; i < mDemandSources.size(); ++i ) {
        mDemandSources[i]->initCalc( aRegionName, aGDP, aDemographics, aPeriod );
    }

}

/*! \brief Set the final demand for service into the marketplace.
*
* \detail Sets the demand for CDR based on the included DemandSources.
* 
* \author David Morrow
* \param string& aRegionName region name.
* \param GDP* aGDP object.
* \param Demographic* aDemographicss.
* \param aPeriod Model aPeriod
*/
void CDRFinalDemand::setFinalDemand( const string& aRegionName,
    const Demographic* aDemographics,
    const GDP* aGDP,
    const int aPeriod )
{

    const Modeltime* modeltime = scenario->getModeltime();

    // Uncomment the following line to prevent demand for CDR during calibration.
    // if ( aPeriod <= modeltime->getFinalCalibrationPeriod() ) { return; }

    Marketplace* marketplace = scenario->getMarketplace();

    // Get the logger
    ILogger& CDRLog = ILogger::getLogger( "CDR_log" );
    CDRLog.setLevel( ILogger::NOTICE );

    // Reset demand at the beginning of each iteration.
    double finalDemand = 0;

    // Iterate over this region's demandSource objects 
    // to get total demand from this region.
    for ( unsigned int i = 0; i < mDemandSources.size(); ++i ) {
        finalDemand += mDemandSources[i]->getDemand( aPeriod, aRegionName );
    }

    mRawDemand[aPeriod] = finalDemand;

    // Enforce quantity-based cap on demand.
    finalDemand = min( finalDemand, mMaxDemand );

    /* The Spending Cap feature has been disabled.
    // Apply the spending cap, if there is one.
    if ( mSpendingCap.size() > 0 ) {
        double CDRprice = marketplace->getPrice( mName, mMarketRegion, aPeriod, false );
        CDRprice *= ( 2.212 * 1e9 ); // convert from 1975$/kg to 1990$/Mt 

        // Check for CDR growth limiter and adjust CDR price as necessary.
        double growthLimiterPrice = marketplace->getPrice( mSpendingCap[0]->getGrowthLimitingMarket(), mMarketRegion, aPeriod, false );
        if ( growthLimiterPrice != Marketplace::NO_MARKET_PRICE ) {
            IInfo* marketInfo = marketplace->getMarketInfo( mSpendingCap[0]->getGrowthLimitingMarket(), aRegionName, 0, true );
            double minPrice = marketInfo->getDouble( "lower-bound-supply-price", false );
            if ( growthLimiterPrice >= minPrice ) {
                // DRM: This is an approximation! It will always
                // underestimate the true cost of CDR. Crucially, it assumes
                // a price on the order of $10/kg for the unsatisfied CDR demand 
                // technology that is used to limit CDR growth.
                CDRprice /= 100; 
            }
        }
        
        // Apply the spending cap.
        double spendingCapValue = mSpendingCap[0]->getSpendingCap( aRegionName, aGDP, aPeriod ) * 1e6; // convert from 1990$M to 1990$
        finalDemand = min( finalDemand, spendingCapValue / CDRprice );
    }
    */

    if ( finalDemand > util::getSmallNumber() ) {
        // Add demand to the market.
        mCDRDemand = finalDemand;
        mCachedMarket->addToDemand( mName, mMarketRegion, mCDRDemand, aPeriod, true );
    }

}

// Copied from NegativeEmissionsFinalDemand
// DRM: Omitting this causes an error in region_minicam.cpp
// So while I'm not actually sure what it does, we seem to need it.
double CDRFinalDemand::getWeightedEnergyPrice( const string& aRegionName,
    const int aPeriod ) const
{
    return 0;
}


// Documentation is inherited.
void CDRFinalDemand::accept( IVisitor* aVisitor,
    const int aPeriod ) const
{
    aVisitor->startVisitFinalDemand( this, aPeriod );
    aVisitor->endVisitFinalDemand( this, aPeriod );
}


//*******************************************************
//*******************************************************
//******* THE SPENDING CAP FEATURE HAS BEEN DISABLED. ***
//******* DRM: I have the left the code intact in     ***
//******* anyone wants to enable it again, but it's   ***
//******* unnecessary and a little buggy.             ***
//*******************************************************
//*******************************************************


/**
 * Definitions of SpendingCap class member functions.
 */
/*
 // Constructor. Sets some defaults.
CDRFinalDemand::SpendingCap::SpendingCap() :
    mGDPPercentage( 0 )
{
    mTaxName = "CO2";
    mMultiplier = 0;
    mMaxPercentage = 0.1;
    mGrowthLimitingMarket = "CDR-growth-limiter";
}

const string& CDRFinalDemand::SpendingCap::getXMLNameStatic() {
    static const string XML_NAME = "spending-cap";
    return XML_NAME;
}

bool CDRFinalDemand::SpendingCap::XMLParse( const DOMNode* aNode ) {

    assert( aNode );

    // get all child nodes.
    DOMNodeList* nodeList = aNode->getChildNodes();
    const Modeltime* modeltime = scenario->getModeltime();

    // loop through the child nodes.
    for ( unsigned int i = 0; i < nodeList->getLength(); ++i ) {
        DOMNode* curr = nodeList->item( i );
        const string nodeName = XMLHelper<string>::safeTranscode( curr->getNodeName() );

        if ( nodeName == "#text" ) {
            continue;
        }
        else if ( nodeName == "gdp-percentage" ) {
            XMLHelper<double>::insertValueIntoVector( curr, mGDPPercentage, modeltime );
        }
        else if ( nodeName == "ctax-name" ) {
            mTaxName = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "multiplier" ) {
            mMultiplier = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "max-percentage" ) {
            mMaxPercentage = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "growth-limiter" ) {
            mGrowthLimitingMarket = XMLHelper<string>::getValue( curr );
        }
        else {
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::WARNING );
            mainLog << "Unknown element " << nodeName
                << " encountered while parsing " << getXMLNameStatic() << endl;
        }
    }
    return true;
}

// Write output to the debug file.
void CDRFinalDemand::SpendingCap::toDebugXML( const int aPeriod,
    ostream& aOut,
    Tabs* aTabs ) const
{
    XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs, getXMLNameStatic() );
    const Modeltime* modeltime = scenario->getModeltime();

    // write the xml for the class members.
    XMLWriteElement( mGDPPercentage[aPeriod], "gdp-percentage", aOut, aTabs );
    XMLWriteElement( mMaxPercentage, "max-percentage", aOut, aTabs );
    XMLWriteElement( mTaxName, "tax-name", aOut, aTabs );
    XMLWriteElement( mMultiplier, "cap-multiplier", aOut, aTabs );
    XMLWriteElement( mGrowthLimitingMarket, "growth-limiting-market", aOut, aTabs );

    XMLWriteClosingTag( getXMLNameStatic(), aOut, aTabs );
}

const string& CDRFinalDemand::SpendingCap::getName() const {
    return getXMLNameStatic();
}

void CDRFinalDemand::SpendingCap::completeInit( const string& aRegionName ) {

    const Modeltime* modeltime = scenario->getModeltime();

    ILogger& mainLog = ILogger::getLogger( "main_log" );
    mainLog.setLevel( ILogger::WARNING );
    ILogger& CDRlog = ILogger::getLogger( "CDR_log" );
    CDRlog.setLevel( ILogger::DEBUG );

    for ( int per = 0; per < modeltime->getmaxper(); ++per ) {

        // Warn if GDP percentage is set to zero after calibration ends.
        if ( mGDPPercentage[per] == 0 && per > modeltime->getFinalCalibrationPeriod() ) {
            CDRlog << "Spending cap created in " << aRegionName
                << " with a cap of 0% of GDP. This will disable some or all sources of demand for CDR in "
                << aRegionName << endl;
        }

        // Ensure that gdp-percentage does not max-percentage.
        if ( mGDPPercentage[per] > mMaxPercentage ) {

            mainLog << "Read-in GDP percentage for CDR spending cap in " << aRegionName
                << " exceeds maximum allowable percentage of " << mMaxPercentage * 100
                << "%. Spending cap reduced to " << mMaxPercentage * 100 << "% of GDP. "
                << "Use <max-percentage> to set a higher maximum allowable percentage." << endl;
            mGDPPercentage[per] = mMaxPercentage;
        }

    }

}
*/

/*! \brief Calculates the value of the spending cap for CDR in aRegionName during aPeriod.
 *  \details The spending cap is specified as a percentage of regional GDP. Optionally,
 *           users can make the cap elastic with respect to the carbon price by setting
 *           mMultiplier > 0. In that case, scale up the spending cap based on the ratio
 *           of the carbon price to regional GDP per capita. The intuition here is that
 *           regions with more stringent carbon prices are willing to spend a larger
 *           share of their income on CDR.
 *
 *  \param aRegionname
 *  \param aGDP
 *  \param aPeriod
 *  \return double The value of the spending cap in millions of 1990$
 *  \author David Morrow
 */
 /*
double CDRFinalDemand::SpendingCap::getSpendingCap( const std::string& aRegionName, const GDP* aGDP, const int aPeriod ) {

    double GDP = aGDP->getGDP( aPeriod );

    // Initialize priceGDPRatio to zero
    // and recalculate it if mMultiplier > 0
    double priceGDPRatio = 0;
    if ( mMultiplier > 0 ) {
        // Recalculate priceGDPRatio on the ratio of the CO2 price to GDP per capita.
        Marketplace* marketplace = scenario->getMarketplace();
        double CO2price = marketplace->getPrice( mTaxName, aRegionName, aPeriod, false );
        if ( CO2price != Marketplace::NO_MARKET_PRICE ) {
            double GDPpc = aGDP->getPPPGDPperCap( aPeriod );
            priceGDPRatio = CO2price / (GDPpc * 10); // multiplty denominator by ten, not 1000, because of conversion to percentage
        }
    }
    double rawCap = GDP * (mGDPPercentage[aPeriod] + (priceGDPRatio * mMultiplier)); // scale up mGDPPercentage based on ratio of C price to GDPpc

    return min( rawCap, GDP * mMaxPercentage ); // ensure spending cap can't exceed maximum allowable percentage of GDP
}

const string& CDRFinalDemand::SpendingCap::getGrowthLimitingMarket() const {
    return mGrowthLimitingMarket;
}

*/