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
* \file market_tax.cpp
* \ingroup Objects
* \brief MarketTax class source file. (Originally called GHGMarket)
* \author Sonny Kim
*/

#include "util/base/include/definitions.h"
#include "marketplace/include/market_tax.h"

using namespace std;

///! Constructor
MarketTax::MarketTax( const MarketContainer* aContainer ) :
  MarketRES( aContainer ) {
}

void MarketTax::toDebugXMLDerived( ostream& out, Tabs* tabs ) const {
}

IMarketType::Type MarketTax::getType() const {
    return IMarketType::TAX;
}

double MarketTax::getDefaultPrice() const {
    const double DEFAULT_PRICE = 5;
    return DEFAULT_PRICE;
}

//! The supply in MarketTax is the constraint, it should not be removed by calls to nullSupply
void MarketTax::nullSupply() {
}
