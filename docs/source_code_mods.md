
# Source Code Modifications

[GCAM-CDR 1.0](./README.md) introduces a number of changes to the source code, relative to GCAM 5.4. These changes are backward-compatible, meaning that XML inputs designed for GCAM 5.4 will work in exactly the same way when used with GCAM-CDR.

The most significant change is the creation of a `CDRFinalDemand` class, `DemandSource` classes, and a `DemandSourceFactory` to manage the creation of `DemandSource` objects. The behavior and configuration the `CDRFinalDemand` and `DemandSource` classes are documented in the page on [CDR Demand and Policies](./CDR_policies.md). For more detail, see the relevant files in the `../cvs/objects/sectors/source/` folder.

This page documents the handful of other changes that are necessary or helpful for making GCAM-CDR work. Trivial changes, such as changes to XML-parsing functions so that they recognize the `<CDR-final-demand>` input tag, are not covered here.

## Track regional emissions
GCAM-CDR makes it possible to track regional emissions _and make that information available to other parts of the model at the beginning of each iteration_. It does this by adding/subtracting emissions to a Trial Value Market, where available, using `CachedMarket` objects to avoid the performance penalty of having to look up the market each time. Since GCAM-CDR uses this feature to allow the `OffsetDemandSource` to know emissions at the beginning of the iteration, these markets are only created when and where users specify an Offset Demand Source. See `aghg.cpp` for details.    

## Allow renaming of CO<sub>2</sub> emissions
In GCAM-CDR, it is possible to give the CO2 emissions a name other than "CO2." In particular, GCAM-CDR gives the negative emissions from its new CDR technologies the name "CO2_CDR," which disconnects them from the "CO2" market. (In GCAM 5.4, attempting to do this will cause the model to add an additional `CO2` object to the technology, which leads to double-counting of the technology's emissions in some contexts.) Getting this to work required some minor changes to several different files:

|File|Change|Purpose|
|--|--|--|
|technology.cpp|Change the way the Technology checks for a missing `CO2` object during `completeIni()`|Allow a Technology to recognize a `CO2` object with a name other than `CO2`, while still adding a `CO2` object if there is none
|`aghg.h`  | Moved getXMLName() from protected to public | Enable other objects in the model to ask a GHG object whether it is a `CO2` or `nonCO2` GHG, so that the model can recognize `CO2` GHGs that are not named "CO2"
|`emissions_summer.cpp` | Check both the name and type of GHG | Enable the emissions summer to recognize CO2 emissions that are named something other than "CO2" (e.g., "CO2_CDR"). Without this, Hector will not see CO2 emissions with other names.


## CDR Capture Component
In GCAM 5.4, carbon capture components effectively shut themselves off when there is no carbon price by returning an extremely high price for carbon storage. The motivation for this behavior is to prevent CCS technologies from deploying in the absence of a policy incentive to do so. Since some forms of CDR use CCS, and users could create demand for CDR even in the absence of a carbon price, the `CDRCaptureComponent` will operate normally even without a carbon price. It is otherwise identical to a `StandardCaptureComponent`. See `technologes/source/CDR_capture_component.cpp` for details. 

## Endogenous supply curves for fractional secondary outputs 
Technologies that produce fractional secondary outputs produce secondary outputs at a variable rate, where the rate depends on the price of the good in question. In GCAM 5.4, users must exogenously specify the prices in the supply curve for that secondary output. GCAM-CDR makes it possible to scale the supply curve endogenously based on the price of a "reference good." For instance, a fractional secondary output can now be set to produce at 50% of its output ratio if the value (price) of the secondary output is 75% of the price of the reference good, 80% of its output ratio if the value of the secondary output is 100% of the price of the reference good, and 100% of its output ratio if the value of the secondary output is 200% of the price of the reference good. See `fractional_secondary_output.cpp` for details.     

## Allow negative demand
Markets count as solved if demand is negative, which happens when a secondary output brings demand below zero. GCAM-CDR does not typically depend on this functionality, but certain configurations make it possible. See `solution_info.cpp`, line 484. 

## Allow new final demand after calibration
GCAM 5.4 will not read in final demand for a final demand sector (e.g., industry) after 2015. This is because it assumes that demand has been set during the calibration period, and it automatically adjusts demand based on relevant elasticities, etc., in future periods. For final demand sectors that have zero demand until after 2015 (e.g., CDR), this poses a challenge. Although GCAM-CDR uses the `CDRFinalDemand` object to handle post-calibration final demand, and therefore does not use this feature, it does add functionality to enable users to set final demand after 2015, as long as final demand in that sector has previously been zero. See `energy_final_demand.cpp` for details. 

## Bug fixes 
There are two minor changes that amount to bug fixes. These bugs are generally not relevant to the behavior of GCAM 5.4, but did cause problems with input files from GCAM-CDR.

|File|Change|Purpose|
|--|--|--|
|`input_tax.cpp`|Check for 0 demand in the relevant sector when calculating demand for share-based policies | Avoid a divide-by-zero error
|`input_subsidy.cpp`|Check for 0 demand in the relevant sector when calculating demand for share-based policies | Avoid a divide-by-zero error

