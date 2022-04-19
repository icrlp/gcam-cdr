/*
* LEGAL NOTICE
* This computer software was prepared by the Institute for
* Carbon Removal Law and Policy through funding from the
* Alfred P. Sloan Foundation.
*
*/

/*!
* \file demand_source.cpp
* \ingroup Objects
* \brief Demand Source class source file.
* \author David Morrow
*/

#include <string>
#include <algorithm>

#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

#include "marketplace/include/marketplace.h" // GCAM-CDR
#include "util/base/include/definitions.h"
#include "util/base/include/xml_helper.h"
#include "util/base/include/ivisitor.h"
#include "containers/include/scenario.h"
#include "sectors/include/CDR_final_demand.h" // GCAM-CDR
#include "util/base/include/util.h" // GCAM-CDR
#include <sectors\include\sector_utils.h>

#include "sectors/include/demand_source.h"


using namespace std;
using namespace xercesc;

extern Scenario* scenario;

/*! \brief Constructor.
* \author David Morrow
*/
DemandSource::DemandSource()
{
    mCPriceName = "CO2";
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
const string& DemandSource::getXMLNameStatic() {
    static const string XML_NAME = "demand-source";
    return XML_NAME;
}

/*! \brief Returns the name of this particular demand source.
 */
const string& DemandSource::getName() const {
    return mName;
}

/*! \brief Set data members from XML input
*
* \author Josh Lurz, David Morrow
* \param node pointer to the current node in the XML input tree
*/
bool DemandSource::XMLParse( const DOMNode* aNode ) {

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
        else if ( nodeName == "demand" ) {
            XMLHelper<double>::insertValueIntoVector( curr, mDemand, modeltime );
        }
        else if ( nodeName == "require-C-price"  || nodeName == "require-c-price" ) {
            // mCPriceName gets cleared if user sets <require-c-price>0</require-c-price>.
            // Otherwise, mCPriceName gets set to the name attribute or remains set to
            // the default value that is specified in DemandSource::DemandSource(). 
            if ( !XMLHelper<bool>::getValue( curr ) ) {
                mCPriceName.clear(); // mCPriceName is now empty
            }
            else {
                string priceName = XMLHelper<string>::getAttr( curr, "name" );
                if ( priceName.empty() ) {
                    ILogger& mainLog = ILogger::getLogger( "main_log" );
                    mainLog.setLevel( ILogger::NOTICE );
                    mainLog << "CDR demand source \"" << mName << "\" is set to operate only "
                        << "when there is a positive carbon price, but no carbon price market "
                        << "was specified. Defaulting to \"CO2\"." << endl;
                }
                else {
                    mCPriceName = priceName;
                }
            }
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
void DemandSource::toDebugXML( const int aPeriod,
    ostream& aOut,
    Tabs* aTabs ) const
{
    XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs, mName );
    const Modeltime* modeltime = scenario->getModeltime();

    // write the xml for the class members.
    XMLWriteElement( mName, "name", aOut, aTabs );
    XMLWriteElement( mDemand[ aPeriod ], "demand", aOut, aTabs );
    XMLWriteElement( mCPriceName, "required-c-price-name", aOut, aTabs );

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
void DemandSource::completeInit( const string& aRegionName ) {

    if ( mName.empty() ) {
        mName = "exogenous-demand-source";
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
void DemandSource::initCalc( const string& aRegionName,
    const GDP* aGDP,
    const Demographic* aDemographics,
    const int aPeriod )
{
}

/*! \brief Get current demand from this demand source.
 *
 *  \details Return the exogenously specified demand from this
 *           source, but first check whether a carbon price
 *           is present and/or required to generate demand.
 *
 *  \author David Morrow
 *  \param aPeriod Model period.
 *  \param aRegionName Regional information object.
 *  \return double Quantity demanded
 */
double DemandSource::getDemand( const int aPeriod, const string& aRegionName ) {

    // If mCPriceName is set, we need to check that the carbon price is greater than zero.
    // If the carbon price isn't set above zero, then we do not add any demand.
    if ( !mCPriceName.empty() ) {
        Marketplace* marketplace = scenario->getMarketplace();
        double carbonPrice = marketplace->getPrice( mCPriceName, aRegionName, aPeriod, false );
        if ( carbonPrice < util::getTinyNumber() || carbonPrice == Marketplace::NO_MARKET_PRICE ) {
            return 0;
        }
    }

    // If we didn't return 0 above, it's safe to return the exogenously specified demand.
    return mDemand[ aPeriod ];
}
