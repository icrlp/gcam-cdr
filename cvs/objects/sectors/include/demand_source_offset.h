#ifndef _OFFSET_DEMAND_SOURCE_H_
#define _OFFSET_DEMAND_SOURCE_H_
#if defined(_MSC_VER)
#pragma once
#endif

/*
* LEGAL NOTICE
* This computer software was prepared by the Institute for
* Carbon Removal Law and Policy through funding from the
* Alfred P. Sloan Foundation.
*
*/



/*!
* \file demand_source_offset.h
* \ingroup Objects
* \brief The OffsetDemandSource class header file.
* \author David Morrow
*/

#include <vector>
#include <xercesc/dom/DOMNode.hpp>
#include <map>
#include <memory>
#include <list>

#include "util/base/include/time_vector.h"
#include "sectors/include/demand_source.h"
#include "sectors/include/afinal_demand.h"
#include "sectors/include/CDR_final_demand.h"

// Forward declarations
class ILogger;
class GDP;
class IInfo;
class Demographic;


/*!
* \ingroup Objects
* \brief This class represents a source of final demand that calibrates
*        demand for some good, such as CDR, to the demand in some
*        other market, such as the CO2 market.
*
* \details In GCAM-CDR, demand sources are attached to a CDRFinalDemand
*          object to generate demand for CDR. An OffsetDemandSource
*          allows you to set demand equal to a fraction of multiple of
*          the emissions of a specific GHG. 
*
* \author David Morrow
*/

class OffsetDemandSource : public DemandSource
{

public:
    OffsetDemandSource();
    virtual ~OffsetDemandSource();

    static const std::string& getXMLNameStatic();
    virtual bool XMLParse( const xercesc::DOMNode* aNode );
    virtual void toDebugXML( const int aPeriod, std::ostream& aOut, Tabs* aTabs ) const;
    virtual void completeInit( const std::string& aRegionName );
 
    virtual double getDemand( const int aPeriod, const std::string& aRegionName );

protected:

    DEFINE_DATA_WITH_PARENT(
        DemandSource,

        //! The fraction of the market demand to offset
        DEFINE_VARIABLE( ARRAY, "offset-fraction", mOffsetFraction, objects::PeriodVector<double> ),

        //! The name of the market from which to calculate demand.
        DEFINE_VARIABLE( SIMPLE, "offset-market-name", mOffsetMarketName, std::string ),

        //! The name of the market region for the offset market.
        DEFINE_VARIABLE( SIMPLE, "offset-market-region", mOffsetMarketRegion, std::string ),

        /* Tax avoidance has been disabled. Uncomment the following lines to enable it. */
        //! The year in which emitters can begin avoiding GHG taxes by offsetting emissions, if any
        //DEFINE_VARIABLE( SIMPLE, "tax-avoidance-start-year", mTaxAvoidanceStartYear, int ),
        
        //! The name of the market containing the price of offsets
        //DEFINE_VARIABLE( SIMPLE, "tax-avoidance-market-name", mTaxAvoidanceMarketName, std::string ),

        //! Price adjustment for non-CO2 GHG tax avoidance
        //DEFINE_VARIABLE( SIMPLE, "tax-avoidance-price-adjust", mTaxAvoidancePriceAdjust, double ),

        //! Maximum allowable demand from this source
        DEFINE_VARIABLE( SIMPLE, "max-offset", mMaxOffset, double )
 

    )

};

// Inline function definitions.
inline OffsetDemandSource::~OffsetDemandSource() {}

#endif // _OFFSET_DEMAND_SOURCE_H_