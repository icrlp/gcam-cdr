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
* \brief ElasticDemandSource class source file.
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

#include "containers/include/gdp.h" // GCAM-CDR
#include "marketplace/include/marketplace.h"

#include "sectors/include/demand_source_elastic.h"


using namespace std;
using namespace xercesc;

extern Scenario* scenario;

/*! \brief Constructor.
* \author David Morrow
*/
ElasticDemandSource::ElasticDemandSource() :
    DemandSource()
{
    mSteepness = 0.005;  // default value roughly calibrated to GCAM 5.4
    mMidpoint = 500;    // default value roughly calibrated to GCAM 5.4
    mStartPeriod = -1;
    mStartYear = -1;
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
const string& ElasticDemandSource::getXMLNameStatic() {
    static const string XML_NAME = "elastic-demand-source";
    return XML_NAME;
}

/*! \brief Set data members from XML input
*
* \author Josh Lurz, David Morrow
* \param node pointer to the current node in the XML input tree
*/
bool ElasticDemandSource::XMLParse( const DOMNode* aNode ) {

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
        else if ( nodeName == "max-demand" ) {
            mMaxDemand = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "steepness" ) {
            mSteepness = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "midpoint" ) {
            mMidpoint = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "min-price" ) {
            mMinPrice = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "tax-name" ) {
            mTaxName = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "start-period" ) {
            mStartPeriod = XMLHelper<int>::getValue( curr );
        }
        else if ( nodeName == "start-year" ) {
            mStartYear = XMLHelper<int>::getValue( curr );
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
void ElasticDemandSource::toDebugXML( const int aPeriod,
    ostream& aOut,
    Tabs* aTabs ) const
{
    XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs, mName );
    const Modeltime* modeltime = scenario->getModeltime();

    // write the xml for the class members.
    XMLWriteElement( mName, "name", aOut, aTabs );
    XMLWriteElement( mStartPeriod, "start-period", aOut, aTabs );
    XMLWriteElement( mSteepness, "s-curve-steepness", aOut, aTabs );
    XMLWriteElement( mMidpoint, "s-curve-midpoint", aOut, aTabs );

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
void ElasticDemandSource::completeInit( const string& aRegionName ) {

    if ( mName.empty() ) {
        mName = getXMLNameStatic();
    }

    const Modeltime* modeltime = scenario->getModeltime();
    ILogger& CDRlog = ILogger::getLogger( "CDR_log" );
    CDRlog.setLevel( ILogger::DEBUG );

    // Ensure that mStartPeriod is set appropriately
    if ( mStartPeriod == -1 ) {
        if ( mStartYear == -1 ) {
            mStartPeriod = modeltime->getFinalCalibrationPeriod() + 1; // default to first post-calibration period

            CDRlog << "No starting date set for elastic demand source in " << aRegionName
                << ". Defaulting to first post-calibration period (period " << mStartPeriod
                << "). " << endl;
        }
        else {
            mStartPeriod = modeltime->getyr_to_per( mStartYear ); // convert starting year to starting period 
        }
    }

    // mStartPeriod can't be during calibration.
    if ( mStartPeriod < modeltime->getFinalCalibrationPeriod() ) {
        mStartPeriod = modeltime->getFinalCalibrationPeriod() + 1;

        ILogger& mainLog = ILogger::getLogger( "main_log" );
        mainLog.setLevel( ILogger::WARNING );
        mainLog << "Starting year cannot be a calibration period in elastic demand source \"" << mName
            << "\". start-period reset to first post-calibration period (period " << mStartPeriod
            << "." << endl;
    }

    // Set default tax name and give a warning.
    if ( mTaxName.empty() ) {
        mTaxName = "CO2";

        CDRlog.setLevel( ILogger::NOTICE );
        CDRlog << "No tax-name specified in elastic-demand-source in "
            << aRegionName << ". Defaulting to CO2." << endl;
    }

    // Warn if no maximum demand is provided.
    if ( mMaxDemand == 0 ) {
        ILogger& mainLog = ILogger::getLogger( "main_log" );
        mainLog.setLevel( ILogger::WARNING );
        mainLog << "No maximum demand is set for elastic CDR demand source \"" << mName << "\" in " << aRegionName << ". "
            << "Defaulting to 0. This demand source will not produce any demand." << endl;
    }

}


/*! \brief Calculates CDR demand based on the carbon price.
 *
 *  \detail Looks up the carbon price and, if it's high enough,
 *          calculates a CDR demand based on a logistic function
 *          that takes the carbon price as an input.
 *
 *  \author David Morrow
 */
double ElasticDemandSource::getDemand( const int aPeriod, const string& aRegionName ) {

    // If we're before the earliest allowed starting period,
    // just return 0 demand and skip the rest.
    if ( aPeriod < mStartPeriod ) {
        return 0;
    }

    // Look up the current CO2 price
    Marketplace* marketplace = scenario->getMarketplace();
    double CO2Price = marketplace->getPrice( mTaxName, aRegionName, aPeriod, false );

    // Calculate demand for this period
    if ( CO2Price == Marketplace::NO_MARKET_PRICE || CO2Price == 0 || CO2Price <= mMinPrice ) {
        // DRM: we have separate conditions for CO2Price == 0 || CO2Price <= mMinPrice
        // to ensure that the model produces demand if (mMinPrice > 0 && CO2Price == mMinPrice)
        // but produces no demand if ( mMinPrice == 0 && CO2Price == mMinPrice ). 
        return 0; // If there's no carbon price or it's too low, there's no demand
    }
    else {
        // This logistic function creates an s-curve that produces minimal CDR output
        // at very low carbon prices and rises fairly quickly around the midpoint.
        return mMaxDemand / (1 + exp( (-1 * mSteepness) * (CO2Price - mMidpoint) ));
    }

    // Ensure that demand stays within the allowed range
    return min( mMaxDemand, mDemand[aPeriod] );
}
