#ifndef _ACCUMULATED_DEMAND_SOURCE_H_
#define _ACCUMULATED_DEMAND_SOURCE_H_
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
* \brief This class represents a source of final demand for some good
*        where unsatisfied demand can carry forward to future periods.
*
* \details In GCAM-CDR, demand sources are attached to a CDRFinalDemand
*          object to generate demand for CDR. An AccumulatedDemandSource
*          allows demand to accumulate if it is not satisfied in
*          earlier periods, and then get added back into the market
*          in later periods.
*
* \author David Morrow
*/

class AccumulatedDemandSource : public DemandSource
{

public:
    AccumulatedDemandSource();
    virtual ~AccumulatedDemandSource();
    static const std::string& getXMLNameStatic();
    bool XMLParse( const xercesc::DOMNode* aNode );
    virtual void toDebugXML( const int aPeriod, std::ostream& aOut, Tabs* aTabs ) const;
    virtual void completeInit( const std::string& aRegionName );
    virtual void initCalc( const std::string& aRegionName, const GDP* aGDP, const Demographic* aDemographics, const int aPeriod );

    virtual double getDemand( const int aPeriod, const std::string& aRegionName );

protected:

    DEFINE_DATA_WITH_PARENT(
        DemandSource,

        //! The name of the market for unsatisfied demand
        DEFINE_VARIABLE( SIMPLE, "unsatisfied-demand-name", mMarketName, std::string),

        //! The name of the market region for the unsatisfied demand market
        DEFINE_VARIABLE( SIMPLE, "unsatisfied-demand-region", mMarketRegion, std::string),

        //! The amount of accumulated unsatisfied demand
        DEFINE_VARIABLE( ARRAY, "accumulated-demand", mAccumulatedDemand, double ),

        //! The fraction of unsatisfied demand to try to fill in each period.
        DEFINE_VARIABLE( SIMPLE, "fraction-to-satisfy", mFractionToSatisfy, objects::PeriodVector<double> ),

        //! The demand in the current period.
        DEFINE_VARIABLE( SIMPLE, "current-demand", mCurrentDemand, double )

    )

};

// Inline function definitions.
inline AccumulatedDemandSource::~AccumulatedDemandSource() {}

#endif // _ACCUMULATED_DEMAND_SOURCE_H_