#ifndef _DEMAND_SOURCE_H_
#define _DEMAND_SOURCE_H_
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
* \file demand_source.h
* \ingroup Objects
* \brief The Demand Source class header file.
* \author David Morrow
*/

#include <vector>
#include <xercesc/dom/DOMNode.hpp>
#include <map>
#include <memory>
#include <list>
#include <boost/core/noncopyable.hpp>

#include "util/base/include/ivisitable.h"
#include "util/base/include/iparsable.h"
#include "util/base/include/inamed.h"
#include "util/base/include/object_meta_info.h"
#include "util/base/include/time_vector.h"
#include "util/base/include/value.h"
#include "util/base/include/data_definition_util.h"


#include "sectors/include/afinal_demand.h"
#include "sectors/include/CDR_final_demand.h"

// Forward declarations
class ILogger;
class GDP;
class IInfo;
class Demographic;

// Need to forward declare the subclasses as well.
class ElasticDemandSource;
class OffsetDemandSource;
class AccumulatedDemandSource;

/*!
* \ingroup Objects
* \brief This class represents a generic source of final demand for CDR or another good.
*
* \details In GCAM-CDR, demand sources are attached to a CDRFinalDemand 
*          object to generate demand for CDR. This demand source allows
*          users to exogenously set demand for CDR in each period.
*
* \author David Morrow
*/

class DemandSource: public INamed,
                    public IParsable
{
    friend class DemandSourceFactory;

public:
    DemandSource();
    virtual ~DemandSource();

    static const std::string& getXMLNameStatic();
    virtual bool XMLParse( const xercesc::DOMNode* aNode );
    virtual const std::string& getName() const;
    virtual void toDebugXML( const int aPeriod, std::ostream& aOut, Tabs* aTabs ) const;
    virtual void completeInit( const std::string& aRegionName );
    virtual void initCalc( const std::string& aRegionName, const GDP* aGDP, const Demographic* aDemographics, const int aPeriod );

    virtual double getDemand( const int aPeriod, const std::string& aRegionName );

protected:

    DEFINE_DATA(
        DEFINE_SUBCLASS_FAMILY( DemandSource, ElasticDemandSource,
                                OffsetDemandSource, AccumulatedDemandSource ),

        //! Name of the demand sources.
        DEFINE_VARIABLE( SIMPLE, "name", mName, std::string ),

        //! Quantity of CDR demanded in each period
        DEFINE_VARIABLE( ARRAY, "demand", mDemand, objects::PeriodVector<double> ),

        //! Whether this demand source operates only when there is a carbon price
        DEFINE_VARIABLE( SIMPLE, "c-price-name", mCPriceName, std::string )

    );

};

// Inline function definitions.
inline DemandSource::~DemandSource() {}

#endif // _DEMAND_SOURCE_H_