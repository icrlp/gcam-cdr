#ifndef _ELASTIC_DEMAND_SOURCE_H_
#define _ELASTIC_DEMAND_SOURCE_H_
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
* \file demand_source_elastic.h
* \ingroup Objects
* \brief The ElasticDemandSource class header file.
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
* \brief This class represents a source of elastic final demand for some good.
*
* \details In GCAM-CDR, demand sources are attached to a CDRFinalDemand
*          object to generate demand for CDR. An elastic final demand source
*          generates demand that is elastic with respect to a carbon price,
*          meaning that demand will rise or fall as the carbon price
*          rises or falls.
*
* \author David Morrow
*/

class ElasticDemandSource : public DemandSource
{

public:
    ElasticDemandSource();
    virtual ~ElasticDemandSource();

    static const std::string& getXMLNameStatic();
    virtual bool XMLParse( const xercesc::DOMNode* aNode );
    virtual void toDebugXML( const int aPeriod, std::ostream& aOut, Tabs* aTabs ) const;
    virtual void completeInit( const std::string& aRegionName );

    virtual double getDemand( const int aPeriod, const std::string& aRegionName );

protected:

    DEFINE_DATA_WITH_PARENT(
        DemandSource,

        //! Maximum allowable demand from this source
        DEFINE_VARIABLE( SIMPLE, "max-demand", mMaxDemand, double ),

        //! Steepness of the demand s-curve
        DEFINE_VARIABLE( SIMPLE, "steepness", mSteepness, double ),

        //! Midpoint of the demand s-curve
        DEFINE_VARIABLE( SIMPLE, "midpoint", mMidpoint, double ),

        //! The name of the tax to which this demand source responds
        DEFINE_VARIABLE( SIMPLE, "tax-name", mTaxName, std::string ),

        //! Minimum price at which to add demand
        DEFINE_VARIABLE( SIMPLE, "min-price", mMinPrice, double ),

        //! The period in which to activate this demand source
        DEFINE_VARIABLE( SIMPLE, "start-period", mStartPeriod, int ),

        //! The year in which to activate this demand source
        DEFINE_VARIABLE( SIMPLE, "start-year", mStartYear, int )

    )

};

// Inline function definitions.
inline ElasticDemandSource::~ElasticDemandSource() {}

#endif // _ELASTIC_DEMAND_SOURCE_H_