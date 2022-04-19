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
 * \file calibrate_resource_visitor.cpp
 * \ingroup Objects
 * \brief The CalibrateResourceVisitor class source file.
 * \author Kate Calvin
 */

#include "util/base/include/definitions.h"
#include <cassert>
#include "util/base/include/calibrate_resource_visitor.h"
#include "util/logger/include/ilogger.h"
#include "resources/include/subresource.h"
#include "resources/include/reserve_subresource.h"
#include "resources/include/renewable_subresource.h"
#include "resources/include/resource.h"
#include "resources/include/grade.h"
#include "technologies/include/technology_container.h"
#include "technologies/include/itechnology.h"

#include "marketplace/include/marketplace.h"
#include "containers/include/scenario.h"

using namespace std;

extern Scenario* scenario;

/*!
 * \brief Constructor
 * \param aRegionName Name of the region if starting the visiting below the
 *        region level.
 */
CalibrateResourceVisitor::CalibrateResourceVisitor( const string& aRegionName )
:mCurrentRegionName( aRegionName )
{
}

void CalibrateResourceVisitor::startVisitResource( const AResource* aResource,
                                                   const int aPeriod )
{
    mCurrentResourceName = aResource->getName();
}

void CalibrateResourceVisitor::endVisitResource( const AResource* aResource,
                                                 const int aPeriod )
{
    mCurrentResourceName.clear();
}

void CalibrateResourceVisitor::startVisitSubResource( const SubResource* aSubResource, const int aPeriod ) {
    // If calibration is active and a calibrated production quantity was read in, 
    // then we need to calculate the price adder needed to produce that quantity
    if( aSubResource->mCalProduction[ aPeriod ] != -1.0 && aPeriod > 0 ){
        // First, calculate the cumulative production.  This is equal to 
        // cumulative production in the previous period plus the calibrated
        // production times the timestep.  Note: this assumes constant production
        // in all years with in a timestep. This is necessary to prevent erratic
        // production in the future.

        double tempCumulProd = aSubResource->mCumulProd[ aPeriod - 1 ] + 
            aSubResource->mCalProduction[ aPeriod ] * scenario->getModeltime()->gettimestep( aPeriod );

        // Next, determine which grade of resource is produced to get to the
        // cumulative production needed.
        double temp_cumulative = 0.0;
        int gr_ind = 0;
        double gr_avail = 0.0;
        while ( temp_cumulative < tempCumulProd ) {
            gr_avail = aSubResource->mGrade[ gr_ind ]->getAvail();
            temp_cumulative += gr_avail;
            gr_ind++;
        }

        // Then, calculate the fraction of the next grade that will be produced
        double fractGrade = 0.0;
        if ( gr_avail > 0.0 ) {
            fractGrade = ( tempCumulProd - ( temp_cumulative - gr_avail ) )
                / gr_avail;
        }

        // Next, calculate the effective price.  This is the price needed
        // to produce the calibrated production quantity in this period.
        // mEffectivePrice = cost of the next highest grade - 
        // ( 1 - fractGrade )*( cost of higher grade - cost of lower grade )
        double low_cost = 0.0;
        if ( gr_ind > 0 ) {
            low_cost = aSubResource->mGrade[ gr_ind - 1 ]->getCost( aPeriod );
        }
        double tempEffectivePrice = aSubResource->mGrade[ gr_ind ]->getCost( aPeriod ) - 
            ( 1 - fractGrade ) * ( aSubResource->mGrade[ gr_ind ]->getCost( aPeriod ) - low_cost );

        double mktPrice = scenario->getMarketplace()->getPrice( mCurrentResourceName, 
                                                                mCurrentRegionName, 
                                                                aPeriod );
        
        ITechnology* currTech = aSubResource->mTechnology->getNewVintageTechnology( aPeriod );
        currTech->calcCost( mCurrentRegionName, mCurrentResourceName, aPeriod );
        double techCost = currTech->getCost( aPeriod );

        // Finally, calculate the price adder. This is the difference between the
        // effective price and the global price
        const_cast<SubResource*>( aSubResource )->mPriceAdder[ aPeriod ] = tempEffectivePrice - mktPrice + techCost;
    }
    else {
        // If no calibration, then set price adder to zero
        if ( aSubResource->mPriceAdder[ aPeriod ].isInited() && aSubResource->mPriceAdder[ aPeriod ] != 0 ) {
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::WARNING );
            mainLog << "User input value >"
                    << aSubResource->mPriceAdder[ aPeriod ]
                    << "< for price-adder variable in SubResource being re-set in period " << aPeriod << endl;
        }
        const_cast<SubResource*>( aSubResource )->mPriceAdder[ aPeriod ] = 0;
    }
}

void CalibrateResourceVisitor::startVisitReserveSubResource( const ReserveSubResource* aSubResource, const int aPeriod ) {
    if( aSubResource->mCalReserve[ aPeriod ].isInited() ){
        double prevCumul = aPeriod > 0 ? aSubResource->mCumulProd[ aPeriod - 1 ] : 0.0;
        
        // First, calculate the cumulative production.  This is equal to
        // cumulative production in the previous period plus the calibrated
        // production times the timestep.  Note: this assumes constant production
        // in all years with in a timestep. This is necessary to prevent erratic
        // production in the future.
        
        double tempCumulProd = prevCumul + aSubResource->mCalReserve[ aPeriod ];
        
        // Next, determine which grade of resource is produced to get to the
        // cumulative production needed.
        double temp_cumulative = 0.0;
        int gr_ind = 0;
        double gr_avail = 0.0;
        while ( temp_cumulative < tempCumulProd && gr_ind < aSubResource->mGrade.size() ) {
            gr_avail = aSubResource->mGrade[ gr_ind ]->getAvail();
            temp_cumulative += gr_avail;
            gr_ind++;
        }
        
        // not enough in supply curve to meet calibration
        if( gr_ind == aSubResource->mGrade.size() ) {
            const_cast<ReserveSubResource*>( aSubResource )->mPriceAdder[ aPeriod ] = aSubResource->mGrade[ gr_ind - 1 ]->getCost( aPeriod ); // TODO?
            return;
        }
        
        // Then, calculate the fraction of the next grade that will be produced
        double fractGrade = 0.0;
        if ( gr_avail > 0.0 ) {
            fractGrade = ( tempCumulProd - ( temp_cumulative - gr_avail ) )
            / gr_avail;
        }
        
        // Next, calculate the effective price.  This is the price needed
        // to produce the calibrated production quantity in this period.
        // mEffectivePrice = cost of the next highest grade -
        // ( 1 - fractGrade )*( cost of higher grade - cost of lower grade )
        double low_cost = 0.0;
        if ( gr_ind > 0 ) {
            low_cost = aSubResource->mGrade[ gr_ind - 1 ]->getCost( aPeriod );
        }
        double tempEffectivePrice = aSubResource->mGrade[ gr_ind ]->getCost( aPeriod ) -
            ( 1 - fractGrade ) * ( aSubResource->mGrade[ gr_ind ]->getCost( aPeriod ) - low_cost );
        
        double mktPrice = scenario->getMarketplace()->getPrice( mCurrentResourceName,
                                                                mCurrentRegionName,
                                                                aPeriod );
        
        ITechnology* currTech = aSubResource->mTechnology->getNewVintageTechnology( aPeriod );
        currTech->calcCost( mCurrentRegionName, mCurrentResourceName, aPeriod );
        double techCost = currTech->getCost( aPeriod );
        
        // Finally, calculate the price adder. This is the difference between the
        // effective price and the global price
        const_cast<ReserveSubResource*>( aSubResource )->mPriceAdder[ aPeriod ] = tempEffectivePrice - mktPrice + techCost;
    }
    else if( aPeriod == 0 ) {
        const_cast<ReserveSubResource*>( aSubResource )->mPriceAdder[ aPeriod ] = -util::getLargeNumber();
    }
    else {
        // If no calibration, then set price adder to zero
        if ( aSubResource->mPriceAdder[ aPeriod ].isInited() && aSubResource->mPriceAdder[ aPeriod ] != 0 ) {
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::WARNING );
            mainLog << "User input value >"
                    << aSubResource->mPriceAdder[ aPeriod ]
                    << "< for price-adder variable in SubResource being re-set in period " << aPeriod << endl;
        }
        const_cast<ReserveSubResource*>( aSubResource )->mPriceAdder[ aPeriod ] = 0;
    }
}

void CalibrateResourceVisitor::startVisitSubRenewableResource( const SubRenewableResource* aSubResource, const int aPeriod ) {
    // If calibration is active and a calibrated production quantity was read in, 
    // then we need to calculate the price adder needed to produce that quantity
    if( aSubResource->mCalProduction[ aPeriod ] > 0 && aPeriod > 0 ){
        double calProduction = aSubResource->mCalProduction[ aPeriod ];

        /*!
         * \warning This routine will fail when using a gdp supply elasticity.
         */
        assert( aSubResource->gdpSupplyElasticity == 0 );

        // Next, determine which grade of resource is produced to get to the
        // annual production needed.
        // Note that renewable resources behave differently than depletable in that
        // if your price is equal to the cost of Grade i you get all the available
        // of Grade i as apposed that which is available in Grade i-1
        int gradeIndex = 0;
        double currGradeAvail = 0.0;
        while ( currGradeAvail < calProduction && gradeIndex < aSubResource->mGrade.size() ) {
            // For renewable resources, getAvail() returns the fraction of the maxSubResource
            // that can be produced.  So, total production is getAvail() * maxSubResource
            ++gradeIndex;
            currGradeAvail = aSubResource->mGrade[ gradeIndex ]->getAvail() * aSubResource->getMaxAnnualSubResource( aPeriod );
        }

        if( gradeIndex == aSubResource->mGrade.size() ) {
            // The calibrated production is greater than the max value of the supply curve.
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::WARNING );
            mainLog << "Calibration value for " << mCurrentResourceName << " in " << mCurrentRegionName
                    << " is greater than the top of the supply curve." << endl;
            const_cast<SubRenewableResource*>( aSubResource )->mPriceAdder[ aPeriod ] = 0;
            return;
        }

        // gradeIndex must be greater than zero since we do not attempt to calibrate
        // unless the calibration production is greater than zero.
        assert( gradeIndex > 0 );

        // Then, calculate the fraction of the next grade that will be produced
        double prevGradeAvail = aSubResource->mGrade[ gradeIndex - 1 ]->getAvail();
        double fractGrade = ( calProduction - prevGradeAvail )
                / ( currGradeAvail - prevGradeAvail );

        // Next, calculate the effective price.  This is the price needed
        // to produce the calibrated production quantity in this period.
        double lowCost = aSubResource->mGrade[ gradeIndex - 1 ]->getCost( aPeriod );
        double highCost = aSubResource->mGrade[ gradeIndex ]->getCost( aPeriod );
        double tempEffectivePrice = lowCost + 
            ( fractGrade ) * ( highCost - lowCost );


        double mktPrice = scenario->getMarketplace()->getPrice( mCurrentResourceName, 
                                                                mCurrentRegionName, 
                                                                aPeriod );
        
        ITechnology* currTech = aSubResource->mTechnology->getNewVintageTechnology( aPeriod );
        currTech->calcCost( mCurrentRegionName, mCurrentResourceName, aPeriod );
        double techCost = currTech->getCost( aPeriod );

        // Finally, calculate the price adder. This is the difference between the
        // effective price and the global price
        const_cast<SubRenewableResource*>( aSubResource )->mPriceAdder[ aPeriod ] = tempEffectivePrice - mktPrice + techCost;
    }
    else {
        // If no calibration, then set price adder to zero
        if ( aSubResource->mPriceAdder[ aPeriod ].isInited() && aSubResource->mPriceAdder[ aPeriod ] != 0 ) {
            ILogger& mainLog = ILogger::getLogger( "main_log" );
            mainLog.setLevel( ILogger::WARNING );
            mainLog << "User input value >"
                    << aSubResource->mPriceAdder[ aPeriod ]
                    << "< for price-adder variable in SubRenewableResource being re-set in period " << aPeriod << endl;
        }
        const_cast<SubRenewableResource*>( aSubResource )->mPriceAdder[ aPeriod ] = 0;
    }
}
