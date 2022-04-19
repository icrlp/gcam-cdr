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
 * \file CDR_capture_component.cpp
 * \ingroup Objects
 * \brief CDRCaptureComponent source file.
 * \author Josh Lurz, David Morrow
 */

#include "util/base/include/definitions.h"
#include <string>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include "util/base/include/xml_helper.h"
#include "technologies/include/CDR_capture_component.h"
#include "marketplace/include/marketplace.h"
#include "containers/include/iinfo.h"
#include "containers/include/scenario.h"
#include "util/logger/include/ilogger.h"
#include "containers/include/market_dependency_finder.h"
#include "functions/include/iinput.h"

using namespace std;

extern Scenario* scenario;

//! Constructor
CDRCaptureComponent::CDRCaptureComponent()
{
    mRemoveFraction = 0;
    mStorageCost = 0;
}

CDRCaptureComponent::~CDRCaptureComponent() {
}

CDRCaptureComponent* CDRCaptureComponent::clone() const {
    CDRCaptureComponent* clone = new CDRCaptureComponent();
    clone->copy( *this );
    return clone;
}

void CDRCaptureComponent::copy( const CDRCaptureComponent& aOther ) {
    mStorageMarket = aOther.mStorageMarket;
    mTargetGas = aOther.mTargetGas;
    mRemoveFraction = aOther.mRemoveFraction;
    mStorageCost = aOther.mStorageCost;
}

bool CDRCaptureComponent::isSameType( const string& aType ) const {
    return aType == getXMLNameStatic();
}

/*! \brief Get the XML node name in static form for comparison when parsing XML.
* \details This public function accesses the private constant string, XML_NAME.
*          This way the tag is always consistent for both read-in and output and
*          can be easily changed. The "==" operator that is used when parsing,
*          required this second function to return static.
* \note A function cannot be static and virtual.
* \author Josh Lurz, James Blackwood
* \return The constant XML_NAME as a static.
*/
const string& CDRCaptureComponent::getXMLNameStatic() {
    const static string XML_NAME = "CDR-capture-component";
    return XML_NAME;
}

const string& CDRCaptureComponent::getName() const {
    return getXMLNameStatic();
}

// Documentation inherits.
bool CDRCaptureComponent::XMLParse( const xercesc::DOMNode* node ) {
    /*! \pre Assume we are passed a valid node. */
    assert( node );

    const xercesc::DOMNodeList* nodeList = node->getChildNodes();
    for ( unsigned int i = 0; i < nodeList->getLength(); i++ ) {
        const xercesc::DOMNode* curr = nodeList->item( i );
        if ( curr->getNodeType() != xercesc::DOMNode::ELEMENT_NODE ) {
            continue;
        }
        const string nodeName = XMLHelper<string>::safeTranscode( curr->getNodeName() );
        if ( nodeName == "storage-market" ) {
            mStorageMarket = XMLHelper<string>::getValue( curr );
        }
        else if ( nodeName == "remove-fraction" ) {
            mRemoveFraction = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "storage-cost" ) {
            mStorageCost = XMLHelper<double>::getValue( curr );
        }
        else if ( nodeName == "target-gas" ) {
            mTargetGas = XMLHelper<string>::getValue( curr );
        }
        else {
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::ERROR );
            mainLog << "Unknown tag " << nodeName << " encountered while processing " << getXMLNameStatic() << endl;
        }
    }

    // TODO: Handle success and failure better.
    return true;
}

void CDRCaptureComponent::toDebugXML( const int aPeriod, ostream& aOut, Tabs* aTabs ) const {
    XMLWriteOpeningTag( getXMLNameStatic(), aOut, aTabs );
    XMLWriteElement( mStorageMarket, "storage-market", aOut, aTabs );
    XMLWriteElement( mRemoveFraction, "remove-fraction", aOut, aTabs );
    XMLWriteElement( mStorageCost, "storage-cost", aOut, aTabs );
    XMLWriteElement( mSequesteredAmount[aPeriod], "sequestered-amount", aOut, aTabs );
    XMLWriteClosingTag( getXMLNameStatic(), aOut, aTabs );
}

void CDRCaptureComponent::completeInit( const string& aRegionName,
    const string& aSectorName )
{
    // Add the storage market as a dependency of the sector. This is because
    // this sector will have to be ordered first so that the total demand and
    // price for storage are known.
    scenario->getMarketplace()->getDependencyFinder()->addDependency( aSectorName,
        aRegionName,
        mStorageMarket,
        aRegionName );

    // Default the target gas to CO2.
    if ( mTargetGas.empty() ) {
        mTargetGas = "CO2";
    }
}

void CDRCaptureComponent::initCalc( const string& aRegionName,
    const string& aSectorName,
    const string& aFuelName,
    const int aPeriod )
{
}

/**
 * \details Storage cost is only valid for mTargetGas ("CO2").
 * Unlike standard storage components, getStorageCost does not
 * return a very large number if there is no carbon price. This
 * allows technologies with CDRCaptureComponents to operate at
 * reasonable prices even when there is no carbon price.
 * \param aRegionName
 * \param aGHGName
 * \param aPeriod
 * \return storage cost
 */
double CDRCaptureComponent::getStorageCost( const string& aRegionName,
    const string& aGHGName,
    const int aPeriod ) const
{
    // First check if this component can capture the gas.
    // Storage cost is only valid for mTargetGas (currently CO2).
    if ( aGHGName != mTargetGas ) {
        return 0;
    }

    // Check if there is a market for storage.
    double storageMarketPrice = scenario->getMarketplace()->getPrice( mStorageMarket,
        aRegionName,
        aPeriod, false );

    double storageCost;
    if ( storageMarketPrice == Marketplace::NO_MARKET_PRICE ) {
        // There is no carbon market. Use the read-in cost.
        storageCost = mStorageCost;
    }
    else {
        // Use the market cost.
        storageCost = storageMarketPrice;
    }
    return storageCost;
}

/**
 * \details Has a valid remove fraction for the target gas only (currently CO2).
 * \param aGHGName
 * \return remove fraction
 */
double CDRCaptureComponent::getRemoveFraction( const string& aGHGName ) const {
    return aGHGName == mTargetGas ? mRemoveFraction : 0;
}

/**
 * \details Calculate sequestered amount for all gases, but do not add
 *  to market demand if gas is not mTargetGas.
 * \param aRegionName
 * \param aGHGName
 * \param aTotalEmissions
 * \param aPeriod
 * \return emissions sequestered
 */
double CDRCaptureComponent::calcSequesteredAmount( const string& aRegionName,
    const string& aGHGName,
    const double aTotalEmissions,
    const int aPeriod )
{
    // Calculate the amount of sequestration.
    // Note the remove fraction is only greater than zero if the current GHG matches
    // the target gas of this capture component.
    double removeFrac = getRemoveFraction( aGHGName );
    double sequestered = 0.0;

    // Add the demand to the marketplace.
    if ( removeFrac > 0 ) {
        sequestered = removeFrac * aTotalEmissions;
        mSequesteredAmount[aPeriod] = sequestered;
        // set sequestered amount as demand side of carbon storage market
        Marketplace* marketplace = scenario->getMarketplace();
        marketplace->addToDemand( mStorageMarket, aRegionName, mSequesteredAmount[aPeriod], aPeriod,
            false );
    }
    return sequestered;
}

/**
 * \param aGHGName
 * \param aGetGeologic
 * \param aPeriod
 * \return sequestered amount
 */
double CDRCaptureComponent::getSequesteredAmount( const string& aGHGName,
    const bool aGetGeologic,
    const int aPeriod ) const
{
    // Only return emissions if the type of the sequestration equals is geologic.
    if ( aGetGeologic && aGHGName == mTargetGas ) {
        return mSequesteredAmount[aPeriod];
    }
    return 0;
}

void CDRCaptureComponent::adjustInputs( const string& aRegionName,
    std::vector<IInput*>& aInputs,
    const int aPeriod ) const
{
    // CDR capture components do not need to adjust input costs or efficiencies.
    // These should already be factored into modeled inputs and costs.
}