/*
* LEGAL NOTICE
* This computer software was prepared by the Institute for
* Carbon Removal Law and Policy through funding from the
* Alfred P. Sloan Foundation.
*
*/

/*!
* \file demand_source_elastic.cpp
* \ingroup Objects
* \brief OffsetDemandSource class source file.
* \author David Morrow
*/

#include <string>
#include <algorithm>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "util/base/include/definitions.h"
#include "util/base/include/xml_helper.h"
#include "util/base/include/ivisitor.h"
#include "containers/include/iactivity.h"
#include "containers/include/iinfo.h"
#include "containers/include/market_dependency_finder.h"
#include "containers/include/scenario.h"
#include "sectors/include/CDR_final_demand.h" // GCAM-CDR
#include "util/base/include/util.h" // GCAM-CDR
#include <sectors\include\sector_utils.h>

#include "containers/include/gdp.h" // GCAM-CDR
#include "marketplace/include/marketplace.h"

#include "sectors/include/demand_source_offset.h"


using namespace std;
using namespace xercesc;

extern Scenario* scenario;

/*! \brief Constructor.
* \author David Morrow
*/
OffsetDemandSource::OffsetDemandSource() :
    DemandSource(),
    mOffsetFraction( 0.0 )
{
    mMaxOffset = util::getLargeNumber();
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
const string& OffsetDemandSource::getXMLNameStatic() {
    static const string XML_NAME = "offset-demand-source";
    return XML_NAME;
}

/*! \brief Set data members from XML input
*
* \author Josh Lurz, David Morrow
* \param node pointer to the current node in the XML input tree
*/
bool OffsetDemandSource::XMLParse( const DOMNode* aNode ) {

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
        else if ( nodeName == "market-name" ) {
            mOffsetMarketName = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "market-region" ) {
            mOffsetMarketRegion = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "offset-fraction" ) {
            XMLHelper<double>::insertValueIntoVector( curr, mOffsetFraction, modeltime );
        }
        else if ( nodeName == "max-offset" ) {
            mMaxOffset = XMLHelper<double>::getValue( curr );
        }
        // To enable tax avoidance, uncomment the following block of code.
        /*
        else if ( nodeName == "tax-avoidance" ) {
            mTaxAvoidanceStartYear = XMLHelper<int>::getAttr( curr, "start-year" );
            mTaxAvoidanceMarketName = XMLHelper<string>::getAttr( curr, "market-name" );
            mTaxAvoidancePriceAdjust = XMLHelper<double>::getAttr( curr, "price-adjust" );
        }
        */
        else {
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::WARNING );
            mainLog << "Unknown element " << nodeName
                << " encountered while parsing " << getXMLNameStatic() << endl;
        }
    }
    return true;
}

/*! \brief Write information useful for debugging to XML output stream
*
* Function writes market and other useful info to XML. Useful for debugging.
*
* \author Josh Lurz, David Morrow
* \param period model period
* \param out reference to the output stream
* \param aTabs A tabs object responsible for printing the correct number of tabs.
*/
void OffsetDemandSource::toDebugXML( const int aPeriod,
    ostream& aOut,
    Tabs* aTabs ) const
{
    XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs, mName );
    const Modeltime* modeltime = scenario->getModeltime();

    // write the xml for the class members.
    XMLWriteElement( mName, "name", aOut, aTabs );
    XMLWriteElement( mOffsetFraction[ aPeriod ], "offset-fraction", aOut, aTabs );
    XMLWriteElement( mOffsetMarketName, "offset-market", aOut, aTabs );
    XMLWriteElement( mOffsetMarketRegion, "offset-region", aOut, aTabs );
    XMLWriteElement( mMaxOffset, "max-offset", aOut, aTabs );

    XMLWriteClosingTag( getXMLNameStatic(), aOut, aTabs );
}

/*! \brief Complete the initialization
*
* This routine is only called once per model run
*
* \author David Morrow
* \param aRegionInfo Regional information object.
* \warning markets are not necessarily set when completeInit is called
*/
void OffsetDemandSource::completeInit( const string& aRegionName ) {

    Marketplace* marketplace = scenario->getMarketplace();
    const Modeltime* modeltime = scenario->getModeltime();
    ILogger& CDRlog = ILogger::getLogger( "CDR_log" );
    CDRlog.setLevel( ILogger::NOTICE );


    if ( mName.empty() ) {
        mName = getXMLNameStatic();
    }

    // Set a default value for the market name; warn user
    if ( mOffsetMarketName.empty() ) {
        mOffsetMarketName = "CO2";
        CDRlog << "Could not find a market for calculating offsets. Defaulting to " << mOffsetMarketName << endl;
    }

    // Inherit the market region name.
    if ( mOffsetMarketRegion.empty() ) {
        mOffsetMarketRegion = aRegionName;
    }

    // Prevent negative values for mOffsetFraction
    // Values >1 are allowable in case user wants to offset more than 100% of a market
    for ( int i = 0; i < modeltime->getmaxper(); ++i ) {
        mOffsetFraction[i] = max( mOffsetFraction[i], 0.0 );
    }

    // The CO2-offset-demand markets correspond to geopolitical regions. If there is a global
    // or multi-region trade in CDR credits, we also want to create a single market
    // We want to create a market to track demand for offsets across the market region.
    if ( marketplace->createMarket( aRegionName, mOffsetMarketRegion, mOffsetMarketName + "-offset-demand", IMarketType::TRIAL_VALUE ) ) {
        // Set info for the new markets.
        IInfo* marketInfo = marketplace->getMarketInfo( mOffsetMarketName + "-offset-demand", aRegionName, 0, true );
        marketInfo->setString( "price-unit", "MtC" );
        marketInfo->setString( "output-unit", "MtC" );
        for ( unsigned int per = 1; per < modeltime->getmaxper(); ++per ) {
            marketplace->setMarketToSolve( mOffsetMarketName + "-offset-demand", aRegionName, per );
        }
    }
    MarketDependencyFinder* depFinder = marketplace->getDependencyFinder();
    depFinder->addDependency( "CDR", aRegionName, mOffsetMarketName + "-offset-demand", aRegionName );

    // To enable tax avoidance, uncomment the following block of code.
    /* 

    // If tax avoidance is enabled, create a CO2_effective (or equivalent) market.
    if ( mTaxAvoidanceStartYear > 0 ) {

        if ( mTaxAvoidanceMarketName.empty() ) {
            mTaxAvoidanceMarketName = "CDR";
        }

        if ( mTaxAvoidancePriceAdjust == 0 ) {
            mTaxAvoidancePriceAdjust = 1;

            CDRlog.setLevel( ILogger::DEBUG );
            CDRlog << "Tax avoidance price-adjust not found or set to zero in " << aRegionName << "; defaulting to 1." << endl;
        }

        ILogger& mainLog = ILogger::getLogger( "main_log" );
        mainLog.setLevel( ILogger::WARNING );
        mainLog << "Creating markets to enable tax avoidance in " << aRegionName << " when offsetting emissions with CDR..." << endl
                << "Expect warnings about the dangers of 'linking to different markets over time'. Ignore these." << endl;
        marketplace->createLinkedMarket( aRegionName, mOffsetMarketRegion, 
                                         mOffsetMarketName + "_effective", mTaxAvoidanceMarketName, 
                                         modeltime->getyr_to_per( mTaxAvoidanceStartYear ) );

        IInfo* marketInfo = marketplace->getMarketInfo( mOffsetMarketName + "_effective", aRegionName, 0, true );
        // Set the units of tax and emissions for reporting.
        marketInfo->setString( "price-unit", "1990$/kg" );
        marketInfo->setString( "output-unit", "MtC" );
        // Set the price/demand adjustments into the linked market info object for
        // retrieval by that market object.
        for ( int per = 0; per < modeltime->getmaxper(); ++per ) {
            IInfo* currMarketInfo = marketplace->getMarketInfo( mOffsetMarketName + "_effective", aRegionName, per, true );
            currMarketInfo->setDouble( "price-adjust", mTaxAvoidancePriceAdjust );
            currMarketInfo->setDouble( "demand-adjust", 0 );
        }
        // Add a dependency between this and the linked market and include a dummy
        // activity to ensure the dependecy finder has an activity to traverse
        depFinder->addDependency( mOffsetMarketName, aRegionName, mOffsetMarketName + "_effective", aRegionName, false );
        depFinder->resolveActivityToDependency( aRegionName, mOffsetMarketName + "_effective",
            new DummyActivity(), new DummyActivity() );
    }
    */

}

/*! \brief Get current demand from this demand source.
 *
 *  \details Calculate the demand from this demand source
 *           as a fraction/multiple of the demand in the 
 *           relevant market.
 *
 *  \param aPeriod Model period.
 *  \param aRegionName Regional information object.
 *  \return double Quantity demanded
 */
double OffsetDemandSource::getDemand( const int aPeriod, const string& aRegionName ) {

    Marketplace* marketplace = scenario->getMarketplace();
    double offsetDemand = (mOffsetFraction[aPeriod] == 0) ? 0
        : marketplace->getPrice( mOffsetMarketName + "-offset-demand", aRegionName, aPeriod ) * mOffsetFraction[aPeriod];
    return max( 0.0, min( offsetDemand, mMaxOffset ) );
}
