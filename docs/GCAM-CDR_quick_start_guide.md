# Quick Start Guide for GCAM-CDR

This file provides basic instructions for getting started with [GCAM-CDR](./README.md). The documentation assumes that you are already familiar with running [GCAM 5.4](http://jgcri.github.io/gcam-doc/index.html). If you are not familiar with it, start with the [User Guide in the GCAM 5.4 documentation](http://jgcri.github.io/gcam-doc/user-guide.html).

## Downloading GCAM-CDR

If you are running Windows, the easiest way to get started with GCAM-CDR is to download the Windows release package.

If you are running Windows and want to add GCAM-CDR to an existing installation of GCAM 5.4, you can just download the source code and copy extract the following files into the corresponding folders on your installation:

 - `exe/gcam-cdr.exe`
 - `exe/run-gcam-cdr.bat`
 - `exe/configuration_CDR.xml`
 - all of the (non-zipped) XML files in `input/gcamdata/xml`
 - all of the files in `input/policy/CDR`
 - `input/solution/cal_broyden_config.xml`
 - `output/queries/Main_queries.xml`

Depending on the configuration of your computer, you may also need to download some of the DLL files in the `exe` folder. If so, your computer will tell you that the file is missing when you try to run `run-gcam-cdr.bat`.

If you are not running Windows or would like to further modify the GCAM-CDR source code, you will need to download the source code and compile it yourself. See [JGCRI's documentation on compiling GCAM](http://jgcri.github.io/gcam-doc/gcam-build.html), noting that GCAM-CDR already has the source code for [Hector](http://jgcri.github.io/gcam-doc/hector.html) included in the appropriate folders. 

## Running Your First Scenario

Once you have downloaded and (if necessary) compiled gcam-cdr.exe, just double-click `run-gcam-cdr.bat` to run a preconfigured scenario in GCAM-CDR. You may wish to [customize your scenario first](#configuring-gcam-cdr).

## Inspecting Your Results

The easiest way to view the results from GCAM-CDR is with [ModelInterface](http://jgcri.github.io/gcam-doc/user-guide.html#modelinterface). For best access to CDR-specific results, download [the `Main_queries.xml` file from this respository](../misc/Main_queries.xml) and save it to the `/outputs/queries/` folder in your GCAM installation. Open (or reopen) the output database in ModelInterface, and you will find a set of CDR-related queries at the very bottom of the query list. Some other queries have been modified to reflect GCAM-CDR's reorganization of the biofuels in the energy sector, but they are entirely backward compatible with GCAM 5.4.

## Configuring GCAM-CDR

GCAM-CDR is highly customizable. You can customize your scenarios by modifying `configuration_CDR.xml` and/or the associated XML input files.
 
The [`configuration_CDR.xml` file](../exe/configuration_CDR.xml) contains all of the standard XML inputs for GCAM 5.4, as well as a number of XML files specific to GCAM-CDR. A few of the standard files, such as `liquids_limits.xml`, have been commented out because they are not normally needed with GCAM-CDR.

### Basic GCAM-CDR Files

Most scenarios run with GCAM-CDR will include all of the following files:

```xml
<!-- Bioseparation files -->
<Value name="bioseparation">../input/gcamdata/xml/bio_sep_final_final_final27.xml</Value>
<Value name="bioenergy_water">../input/gcamdata/xml/electricity_water_coefs_bio.xml</Value>
<Value name="bioliquids_limits">../input/gcamdata/xml/bio_sep_liquids_limits.xml</Value>

<!-- CDR technology files -->
<Value name="cdr_supplysector">../input/gcamdata/xml/CDR.xml</Value>
<Value name="becccs_rampup">../input/gcamdata/xml/CDR_growth_limit.xml</Value>
<Value name="cdr_trade">../input/gcamdata/xml/CDR_traded.xml</Value>
<Value name="cdr_dac">../input/gcamdata/xml/DAC.xml</Value>
<Value name="waste_heat">../input/gcamdata/xml/waste_heat_endogenous.xml</Value>
<Value name="cdr_ew">../input/gcamdata/xml/TEW.xml</Value>
<Value name="cdr_ew_limit">../input/gcamdata/xml/limit_land.xml</Value>
<Value name="cdr_oae">../input/gcamdata/xml/OEW.xml</Value>
<Value name="oew_limit">../input/gcamdata/xml/OEW_shipping.xml</Value>
<Value name="cdr_nonenergy">../input/gcamdata/xml/CDR_costs.xml</Value>
<Value name="cdr_resources">../input/gcamdata/xml/resources_CDR.xml</Value>

<!-- BECCS integration files -->
<Value name="beccs_integration">../input/gcamdata/xml/BECCS_integration.xml</Value>
``` 

It is helpful to think about these files in several groups: the [bioseparation files more thoroughly separate biofuels from fossil fuels](./bioenergy_separation.md) in GCAM's model of the energy system; the CDR technology files add new [CDR sectors](./CDR_supply_sector.md) and [CDR technologies](./CDR_technologies.md) to the model; and the [BECCS integration files](./BECCS_integration.md) enable BECCS technologies to compete directly with new CDR technologies. For details, see the documentation on [Configuring GCAM-CDR](./configuring_GCAM-CDR.md) page.


## CDR policy files

Some sample policy files are discussed below. **For most uses, you can include just one of these files.** You can easily modify the files to produce custom scenarios. You can also combine them. For instance, including both `CDR_exo_2GtC.xml` and `CDR_offset_CO2.xml` will set CDR demand equal to net CO~2~ emissions (ignoring removals via CDR) plus 2 GtC.

See the XML files themselves and/or the page on [CDR Policies](./CDR_policies.md) for details on how to use these files.

file | description
-----|------------
`CDR_exo_2GtC.xml` | Creates 2 GtC of global demand for CDR, which will be distributed via trade as long as `CDR_traded.xml` is included
`CDR_exo_regional.xml` | Creates 2 GtC of global demand for CDR, explicitly distributed across regions
`CDR_exo_per_period.xml` | Explicitly sets global demand for CDR in each period, specifying rapid growth and slow decline
`CDR_offset_CO2.xml` | Creates CDR demand equal to all residual CO~2~ emissions
`CDR_offset_GHG.xml` | Creates CDR demand equal to all residual GHG emissions
`CDR_elastic_demand.xml` | Creates CDR demand that rises and falls with the carbon price

\[[return to GCAM-CDR overview](./README.md)\]
