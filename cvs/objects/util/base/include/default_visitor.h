#ifndef _DEFAULT_VISITOR_H_
#define _DEFAULT_VISITOR_H_
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
* \file default_visitor.h
* \ingroup Objects
* \brief DefaultVisitor class header file.
* \author Pralit Patel, Katherine Chung, Josh Lurz
*/

#include "util/base/include/ivisitor.h"
#include <string>

/*! \brief DefaultVisitor is an implementation of IVisitor which defines all
*          methods as empty.
* \details This is a convenience class used so that derived classes do not have
*          to implement all the methods of the IVisitor interface. 
*/
class DefaultVisitor : public IVisitor {
public:
    virtual ~DefaultVisitor(){}
    virtual void finish() const {}
    virtual void startVisitScenario( const Scenario* aScenario, const int aPeriod ){}
    virtual void endVisitScenario( const Scenario* aScenario, const int aPeriod ){}

    virtual void startVisitWorld( const World* aWorld, const int aPeriod ){}
    virtual void endVisitWorld( const World* aWorld, const int aPeriod ){}

    virtual void startVisitRegion( const Region* aRegion, const int aPeriod ){}
    virtual void endVisitRegion( const Region* aRegion, const int aPeriod ){}

    virtual void startVisitRegionMiniCAM( const RegionMiniCAM* aRegionMiniCAM, const int aPeriod ){}
    virtual void endVisitRegionMiniCAM( const RegionMiniCAM* aRegionMiniCAM, const int aPeriod ){}

    virtual void startVisitDemographic( const Demographic* aDemographic, const int aPeriod ){}
    virtual void endVisitDemographic( const Demographic* aDemographic, const int aPeriod ){}

    virtual void startVisitPopulation( const Population* aPopulation, const int aPeriod ){}
    virtual void endVisitPopulation( const Population* aPopulation, const int aPeriod ){}

    virtual void startVisitPopulationMiniCAM( const PopulationMiniCAM* aPopulation, const int aPeriod ){}
    virtual void endVisitPopulationMiniCAM( const PopulationMiniCAM* aPopulation, const int aPeriod ){}

    virtual void startVisitAgeCohort( const AgeCohort* aAgeCohort, const int aPeriod ){}
    virtual void endVisitAgeCohort( const AgeCohort* aAgeCohort, const int aPeriod ){}
    
    virtual void startVisitGender( const Gender* aGender, const int aPeriod ){}
    virtual void endVisitGender( const Gender* aGender, const int aPeriod ){}

    virtual void startVisitFemale( const Female* aFemale, const int aPeriod ){}
    virtual void endVisitFemale( const Female* aFemale, const int aPeriod ){}

    virtual void startVisitMale( const Male* aMale, const int aPeriod ){}
    virtual void endVisitMale( const Male* aMale, const int aPeriod ){}

    virtual void startVisitResource( const AResource* aResource, const int aPeriod ){}
    virtual void endVisitResource( const AResource* aResource, const int aPeriod ){}
    
    virtual void startVisitSubResource( const SubResource* aSubResource, const int aPeriod ){}
    virtual void endVisitSubResource( const SubResource* aSubResource, const int aPeriod ){}
    
    virtual void startVisitReserveSubResource( const ReserveSubResource* aSubResource, const int aPeriod ){}
    virtual void endVisitReserveSubResource( const ReserveSubResource* aSubResource, const int aPeriod ){}

    virtual void startVisitSubRenewableResource( const SubRenewableResource* aSubResource, const int aPeriod ){}
    virtual void endVisitSubRenewableResource( const SubRenewableResource* aSubResource, const int aPeriod ){}

    virtual void startVisitGrade( const Grade* aGrade, const int aPeriod ){}
    virtual void endVisitGrade( const Grade* aGrade, const int aPeriod ){}

    virtual void startVisitSector( const Sector* aSector, const int aPeriod ){}
    virtual void endVisitSector( const Sector* aSector, const int aPeriod ){}

    virtual void startVisitSubsector( const Subsector* aSubsector, const int aPeriod ){}
    virtual void endVisitSubsector( const Subsector* aSubsector, const int aPeriod ){}
    
    virtual void startVisitNestingSubsector( const NestingSubsector* aSubsector, const int aPeriod ){}
    virtual void endVisitNestingSubsector( const NestingSubsector* aSubsector, const int aPeriod ){}

    virtual void startVisitFinalDemand( const AFinalDemand* aFinalDemand, const int aPeriod ){}
    virtual void endVisitFinalDemand( const AFinalDemand* aFinalDemand, const int aPeriod ){}

    virtual void startVisitEnergyFinalDemand( const EnergyFinalDemand* aEnergyFinalDemand, const int aPeriod ){}
    virtual void endVisitEnergyFinalDemand( const EnergyFinalDemand* aEnergyFinalDemand, const int aPeriod ){}

    virtual void startVisitBuildingDemandSubsector( const BuildingDemandSubSector* aSubsector,
                                                    const int aPeriod ){}
    virtual void endVisitBuildingDemandSubsector( const BuildingDemandSubSector* aSubsector,
                                                  const int aPeriod ){}

    virtual void startVisitTechnology( const Technology* aTechnology, const int aPeriod ){}
    virtual void endVisitTechnology( const Technology* aTechnology, const int aPeriod ){}

    virtual void startVisitAgProductionTechnology( const AgProductionTechnology* aTechnology, const int aPeriod ){}
    virtual void endVisitAgProductionTechnology( const AgProductionTechnology* aTechnology, const int aPeriod ){}

    virtual void startVisitBaseTechnology( const BaseTechnology* aBaseTechnology, const int aPeriod ){}
    virtual void endVisitBaseTechnology( const BaseTechnology* aBaseTechnology, const int aPeriod ){}

    virtual void startVisitConsumer( const Consumer* aConsumer, const int aPeriod ){}
    virtual void endVisitConsumer( const Consumer* aConsumer, const int aPeriod ){}

    virtual void startVisitNationalAccount( const NationalAccount* aNationalAccount, const int aPeriod ){}
    virtual void endVisitNationalAccount( const NationalAccount* aNationalAccount, const int aPeriod ){}

    virtual void startVisitInput( const IInput* aInput, const int aPeriod ){}
    virtual void endVisitInput( const IInput* aInput, const int aPeriod ){}

    virtual void startVisitExpenditure( const Expenditure* aExpenditure, const int aPeriod ){}
    virtual void endVisitExpenditure( const Expenditure* aExpenditure, const int aPeriod ){}

    virtual void startVisitMiniCAMInput( const MiniCAMInput* aInput, const int aPeriod ){}
    virtual void endVisitMiniCAMInput( const MiniCAMInput* aInput, const int aPeriod ){}

    virtual void startVisitOutput( const IOutput* aOutput, const int aPeriod ){}
    virtual void endVisitOutput( const IOutput* aOutput, const int aPeriod ){}

    virtual void startVisitGHG( const AGHG* aGHG, const int aPeriod ){}
    virtual void endVisitGHG( const AGHG* aGHG, const int aPeriod ){}

    virtual void startVisitMarketplace( const Marketplace* aMarketplace, const int aPeriod ){}
    virtual void endVisitMarketplace( const Marketplace* aMarketplace, const int aPeriod ){}

    virtual void startVisitMarket( const Market* aMarket, const int aPeriod ){}
    virtual void endVisitMarket( const Market* aMarket, const int aPeriod ){}

    virtual void startVisitGDP( const GDP* aGDP, const int aPeriod ){}
    virtual void endVisitGDP( const GDP* aGDP, const int aPeriod ){}

    virtual void startVisitLandNode( const LandNode* aLandNode, const int aPeriod ){}
    virtual void endVisitLandNode( const LandNode* aLandNode, const int aPeriod ){}

    virtual void startVisitLandLeaf( const LandLeaf* aLandLeaf, const int aPeriod ){}
    virtual void endVisitLandLeaf( const LandLeaf* aLandLeaf, const int aPeriod ){}

    virtual void startVisitLandUseHistory( const LandUseHistory* aLandUseHistory, const int aPeriod ){};
    virtual void endVisitLandUseHistory( const LandUseHistory* aLandUseHistory, const int aPeriod ){};

    virtual void startVisitCarbonCalc( const ICarbonCalc* aCarbonCalc, const int aPeriod ){}
    virtual void endVisitCarbonCalc( const ICarbonCalc* aCarbonCalc, const int aPeriod ){}

    virtual void startVisitClimateModel( const IClimateModel* aClimateModel, const int aPeriod ){}
    virtual void endVisitClimateModel( const IClimateModel* aClimateModel, const int aPeriod ){}
	
    virtual void startVisitTranSubsector( const TranSubsector* aTranSubsector, const int aPeriod ) {}
    virtual void endVisitTranSubsector( const TranSubsector* aTranSubsector, const int aPeriod ) {}

    virtual void startVisitTranTechnology( const TranTechnology* aTranTechnology, const int aPeriod ){}
    virtual void endVisitTranTechnology( const TranTechnology* aTranTechnology, const int aPeriod ){}

    virtual void startVisitNodeInput( const NodeInput* aNodeInput, const int aPeriod ){}
    virtual void endVisitNodeInput( const NodeInput* aNodeInput, const int aPeriod ){}

    virtual void startVisitGCAMConsumer( const GCAMConsumer* aGCAMConsumer, const int aPeriod ){}
    virtual void endVisitGCAMConsumer( const GCAMConsumer* aGCAMConsumer, const int aPeriod ){}

    virtual void startVisitBuildingNodeInput( const BuildingNodeInput* aBuildingNodeInput, const int aPeriod ){}
    virtual void endVisitBuildingNodeInput( const BuildingNodeInput* aBuildingNodeInput, const int aPeriod ){}

    virtual void startVisitBuildingServiceInput( const BuildingServiceInput* aBuildingServiceInput, const int aPeriod ){}
    virtual void endVisitBuildingServiceInput( const BuildingServiceInput* aBuildingServiceInput, const int aPeriod ){}
    
    virtual void startVisitFoodDemandInput( const FoodDemandInput* aFoodDemandInput, const int aPeriod ){};
    virtual void endVisitFoodDemandInput( const FoodDemandInput* aFoodDemandInput, const int aPeriod ){};

    virtual void startVisitNoEmissCarbonCalc( const NoEmissCarbonCalc* aNoEmissCarbonCalc, const int aPeriod ){}
    virtual void endVisitNoEmissCarbonCalc( const NoEmissCarbonCalc* aNoEmissCarbonCalc, const int aPeriod ){}
};

#endif // _DEFAULT_VISITOR_H_
