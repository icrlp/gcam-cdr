/*
* LEGAL NOTICE
* This computer software was prepared by the Institute for
* Carbon Removal Law and Policy through funding from the
* Alfred P. Sloan Foundation.
*
*/

/*!
* \file demand_source_accumulated.cpp
* \ingroup Objects
* \brief AccumulatedDemandSource class source file.
* \author David Morrow
*/

#include <string>
#include <algorithm>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "util/base/include/definitions.h"
#include "util/base/include/xml_helper.h"
#include "util/base/include/ivisitor.h"
#include "containers/include/scenario.h"
#include "sectors/include/CDR_final_demand.h" // GCAM-CDR
#include "util/base/include/util.h" // GCAM-CDR
#include <sectors\include\sector_utils.h>

#include "marketplace/include/marketplace.h"

#include "sectors/include/demand_source.h"
#include "sectors/include/demand_source_accumulated.h"


using namespace std;
using namespace xercesc;

extern Scenario* scenario;

/*! \brief Constructor.
* \author David Morrow
*/
AccumulatedDemandSource::AccumulatedDemandSource() :
    DemandSource(),
    mFractionToSatisfy( 0.5 )
{
    mCurrentDemand = 0;
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
const string& AccumulatedDemandSource::getXMLNameStatic() {
    static const string XML_NAME = "accumulated-demand-source";
    return XML_NAME;
}

/*! \brief Set data members from XML input
*
* \author Josh Lurz, David Morrow
* \param node pointer to the current node in the XML input tree
*/
bool AccumulatedDemandSource::XMLParse( const DOMNode* aNode ) {

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
            mMarketName = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "market-region" ) {
            mMarketRegion = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "fraction-to-satisfy" ) {
            XMLHelper<double>::insertValueIntoVector( curr, mFractionToSatisfy, modeltime );
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

/*! \brief Write information useful for debugging to XML output stream
*
* Function writes market and other useful info to XML. Useful for debugging.
*
* \author Josh Lurz, David Morrow
* \param period model period
* \param out reference to the output stream
* \param aTabs A tabs object responsible for printing the correct number of tabs.
*/
void AccumulatedDemandSource::toDebugXML( const int aPeriod,
    ostream& aOut,
    Tabs* aTabs ) const
{
    XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs, mName );
    const Modeltime* modeltime = scenario->getModeltime();

    // write the xml for the class members.
    XMLWriteElement( mName, "name", aOut, aTabs );
    XMLWriteElement( mMarketName, "unsatisfied-demand-market-name", aOut, aTabs );
    XMLWriteElement( mMarketRegion, "unsatisfied-demand-market-region", aOut, aTabs );
    XMLWriteElement( mAccumulatedDemand, "accumulated-demand", aOut, aTabs );
    XMLWriteElement( mFractionToSatisfy[ aPeriod ], "fraction-to-satisfy", aOut, aTabs );
    XMLWriteElement( mCurrentDemand, "current-demand", aOut, aTabs );

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
void AccumulatedDemandSource::completeInit( const string& aRegionName ) {

    ILogger& CDRlog = ILogger::getLogger( "CDR_log" );
    CDRlog.setLevel( ILogger::DEBUG );

    if ( mName.empty() ) {
        mName = getXMLNameStatic();
    }

    // Set a default value for the market that tracks unsatisfied demand.
    if ( mMarketName.empty() ) {
        mMarketName = "unsatisfiedCDRDemand";
        CDRlog << "The accumulated demand source, " << mName << ", in " << aRegionName
            << " was not given a name for market that tracks unsatisfied demand for CDR. "
            << "The default value of " << mMarketName << " was used." << endl;
    }

    // Default to the current region for the market that tracks unsatisfied demand.
    if ( mMarketRegion.empty() ) {
        mMarketRegion = aRegionName;
        CDRlog << "The accumulated demand source, " << mName << ", in " << aRegionName
            << " was not given a name for a market region for the market that tracks "
            << " unsatisfied demand for CDR. Defaulting to the demand source's own region, " << aRegionName << endl;
    }

    // Prevent negative values for mFractionToSatisfy.
    const Modeltime* modeltime = scenario->getModeltime();
    ILogger& mainLog = ILogger::getLogger( "main_log" );
    mainLog.setLevel( ILogger::WARNING );
    for ( unsigned int per = 0; per < modeltime->getmaxper(); ++per ) {
        if ( mFractionToSatisfy[ per ] < 0 ) {
            mFractionToSatisfy[ per ] = 0;
            mainLog << "fraction-to-satisfy < 0 in period " << per << " in " << aRegionName
                    << ". Reset to 0." << endl;
        }
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
void AccumulatedDemandSource::initCalc( const string& aRegionName,
    const GDP* aGDP,
    const Demographic* aDemographics,
    const int aPeriod )
{
    // Skip this during period 0.
    if ( aPeriod > 0 ) {

        Marketplace* marketplace = scenario->getMarketplace();
        // Check that the market exists.
        double price = marketplace->getPrice( mMarketName, mMarketRegion, (aPeriod - 1), false );
        if ( price != Marketplace::NO_MARKET_PRICE ) {
            // Add any unsatisfied demand from the previous period to mAccumulatedDemand.
            mAccumulatedDemand += marketplace->getDemand( mMarketName, mMarketRegion, (aPeriod - 1) );
            // Move the appropriate amount of demand from mAccumulatedDemand
            // to mCurrentDemand. If some of that demand goes unmet in this
            // period, it will get added back into mAccumulatedDemand at the
            // start of the next period.
            mCurrentDemand = mAccumulatedDemand * mFractionToSatisfy[ aPeriod ];
            mAccumulatedDemand -= mCurrentDemand;
        }

    }
}

/*! \brief Get current demand from this demand source.
 *
 *  \details The amount of accumulated demand to add into
 *           the market is calculated in initCalc(). This
 *           function just reports that amount to the 
 *           CDRFinalDemand object.
 *
 *  \author David Morrow
 *  \param aPeriod Model period.
 *  \param aRegionName Regional information object.
 *  \return double Quantity demanded
 */
double AccumulatedDemandSource::getDemand( const int aPeriod, const string& aRegionName ) {
    return mCurrentDemand;
}