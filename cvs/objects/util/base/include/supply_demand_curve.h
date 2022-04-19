#ifndef _SUPPLY_DEMAND_CURVE_H_
#define _SUPPLY_DEMAND_CURVE_H_
#if defined(_MSC_VER)
#pragma once
#endif

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
* \file supply_demand_curve.h
* \ingroup Objects
* \brief The SupplyDemandCurve class header file.
* \author Josh Lurz
*/

#include <functional>
#include <vector>
#include <string>

class ILogger;
class Market;
class World;
class Marketplace;
class SolutionInfoSet;

/*!
* \ingroup Objects
* \brief A class which defines a single supply and demand curve. 
* \author Josh Lurz
*/

class SupplyDemandCurve {
public:
    SupplyDemandCurve( int aMarketNumber, const std::string& aMarketName );
    ~SupplyDemandCurve();

    void calculatePoints( const std::vector<double>& aPrices, SolutionInfoSet& aSolnSet, World* aWorld,
                          Marketplace* aMarketplace, const int aPeriod, bool aIsPricesRelative );

    // Legacy version
    void calculatePoints( const int aNumPoints, SolutionInfoSet& aSolnSet, World* aWorld,
                          Marketplace* aMarketplace, const int aPeriod );
    
    void print( std::ostream& aOut ) const;
    void printCSV( std::ostream& aOut, int period, bool aPrintHeader ) const;
    
private:
    //! Index to the market which the curve is calculating for.
    int mMarketNumber;
    
    //! The name of the market which the curve is calculating for.
    const std::string& mMarketName;

/*!
* \ingroup Objects
* \brief A class which defines a single supply and demand point. 
* \author Josh Lurz
*/

class SupplyDemandPoint
{
   
public:
   SupplyDemandPoint( const double aPrice, const double aDemand, const double aSupply, const double aFx );
   double getPrice() const;
   void print( std::ostream& aOut ) const;
   
/*!
* \brief Binary comparison operator used for SavePoint pointers to order by increasing price. 
* \author Josh Lurz
*/  
   struct LesserPrice : public std::binary_function<SupplyDemandPoint*,SupplyDemandPoint*,bool>
   {
      //! Operator which performs comparison. 
      bool operator()( const SupplyDemandPoint* lhs, const SupplyDemandPoint* rhs ) const
      {   
         return lhs->getPrice() < rhs->getPrice();
      }
   };

   private:
      double mPrice; //!< Fixed Price
      double mDemand; //!< Demand at the price.
      double mSupply; //!< Supply at the price.
      double mFx; //!< F(x) at the price.
   };
   
   std::vector<SupplyDemandPoint*> mPoints; //!< Vector of points.

};

#endif // _SUPPLY_DEMAND_CURVE_H_
