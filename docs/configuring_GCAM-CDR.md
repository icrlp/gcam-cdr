# Configuring GCAM-CDR
This page provides a detailed explanation of how to configure [GCAM-CDR](./README.md), including a discussion of important options for customizing scenarios. The configuration of CDR demand and policy is discussed separately on the [CDR Policies](./CDR_policies.md) page.

For a shorter introduction, see the [Quick Start Guide](./GCAM-CDR_quick_start_guide.md).

## Configuration Files
Most scenarios run with GCAM-CDR will include all of the following files:

```xml
<!-- Bioseparation files -->
<Value name="bioseparation">../input/gcamdata/xml/bio_sep_final_final_final27.xml</Value>
<Value name="bioenergy_water">../input/gcamdata/xml/electricity_water_coefs_bio.xml</Value>
<Value name="bioliquids_limits">../input/gcamdata/xml/bio_sep_liquids_limits.xml</Value>

<!-- CDR technology files -->
<Value name="cdr_supplysector">../input/gcamdata/xml/CDR.xml</Value>
<Value name="cdr_growth_limit">../input/gcamdata/xml/CDR_growth_limit.xml</Value><!-- optional -->
<Value name="cdr_trade">../input/gcamdata/xml/CDR_traded.xml</Value>
<Value name="cdr_dac">../input/gcamdata/xml/DAC.xml</Value>
<Value name="waste_heat">../input/gcamdata/xml/waste_heat_endogenous.xml</Value>
<Value name="cdr_tew">../input/gcamdata/xml/TEW.xml</Value>
<Value name="cdr_tew_limit">../input/gcamdata/xml/limit_land.xml</Value>
<Value name="cdr_oew">../input/gcamdata/xml/OEW.xml</Value>
<Value name="cdr_oew_limit">../input/gcamdata/xml/OEW_shipping.xml</Value>
<Value name="cdr_nonenergy">../input/gcamdata/xml/CDR_costs.xml</Value>
<Value name="cdr_resources">../input/gcamdata/xml/resources_CDR.xml</Value>

<!-- BECCS integration files -->
<Value name="beccs_integration">../input/gcamdata/xml/BECCS_integration.xml</Value>
<Value name="beccs_countersubsidy">../input/policy/CDR/ignore_BECCS_in_CO2_constraint.xml</Value><!-- only needed when using a carbon/GHG constraint -->
``` 

It is helpful to think about these files in several groups.

### Bioseparation Files

These files more thoroughly separate liquid and gaseous biofuels from fossil fuels in GCAM's model of the energy system. These files are entirely compatible with GCAM 5.4. They are not strictly necessary to run GCAM-CDR, but the BECCS integration files will not work properly without them.

file | description
-----|--------------
`bio_sep_final_final_final27.xml` | Improves on GCAM's separation of bioenergy, especially BECCS, from fossil energy
`electricity_water_coefs_bio.xml` | Adds water usage for electricity technologies created in the `bio_sep` file (optional, but recommended)
`bio_sep_liquids_limits.xml` | Prohibits liquid biofuels from being used to generate electricity or to manufacture industrial feedstocks (optional, but recommended)

One of the key motivations for the bioseparation files is that in GCAM 5.4, biogas and refined bioliquids (i.e., gaseous and liquid fuels made from biomass) are mixed together with fossil fuels in the energy transformation sector. When those mixed fuels are used with CCS (e.g., in the electricity sector), some amount of CO<sub>2</sub> is permanently removed from the atmosphere, but the model cannot easily "see" how much CDR has occurred and therefore cannot incorporate that CDR into the CDR sector or affect it through CDR policy. The bioseparation files resolve this problem by keeping biofuels and fossil fuels separate, except for end-use sectors in which CCS is not used. 

Note that the `bio_sep_liquids_limits.xml` implies slightly different behavior for industrial feedstocks than is implied by GCAM 5.4's `liquids_limits.xml` file. The GCAM 5.4 file requires a certain fraction of refined liquids used for industrial feedstocks to come from refined petroleum. The GCAM-CDR file simply prohibits the use of refined bioliquids from being used in industrial feedstocks. This is because the permanence of carbon sequestration in industrial feedstocks varies by feedstock, and so allowing bioliquids inputs to the industrial feedstock sector would exaggerate CDR via that route to a large but uncertain degree.

### CDR Technology Files

These files define the basic CDR sector and technologies. They define technologies for direct air capture (DAC), terrestrial enhanced weathering (TEW), and ocean enhanced weathering (OEW). With the exception of `OEW_shipping.xml`, which is not strictly essential, the files are entirely compatible with GCAM 5.4.

file | description
-----|--------------
`CDR.xml` | Defines regional CDR supply sectors
`CDR_growth_limit.xml` | Constrains the growth of CDR to prevent unrealistically large values in early periods (optional)
`CDR_traded.xml` | Enables interregional trade in CDR (optional, but recommended)
`DAC.xml` | Defines technologies for high-heat (solvent-based) direct air capture (DAC) and low-heat (sorbent-based) DAC
`waste_heat_endogenous.xml` | Defines the waste heat resource used by `DAC_sorbent`
`TEW.xml` | Defines the terrestrial enhanced weathering technology
`limit_land.xml` | Defines the `cropland_TEW` resource used to limit TEW
`OEW.xml` | Defines the ocean enhanced weathering technology
`OEW_shipping.xml` | Defines the `OEW-shipping` resource used to limit OEW (optional, but recommended)
`CDR_costs.xml` | Adds non-energy costs for primary CDR technologies
`resources_CDR.xml` | Defines new resources used by CDR technologies; also sets their carbon coefficients

Most users will not need to modify most of these files, unless they want to change the parameterization of particular CDR technologies or add new ones. 

Some users will want to remove, modify, or replace the `CDR_growth_limit.xml` file, as the rate of growth of CDR is an important variable that remains highly uncertain. See the section on Controlling the Growth of the CDR Sector (below) for details.

### BECCS Integration Files

These files connect BECCS technologies to the CDR market. GCAM-CDR will work without them, but BECCS technologies will operate independently of other CDR technologies and CDR demand.

file | description
-----|--------------
`BECCS_integration.xml` | Enables BECCS technologies to compete directly in the CDR market and get paid through that market and eutralizes the carbon-price-based subsidy that BECCS technologies normally receive  
`ignore_BECCS_in_CO2_constraint.xml` | Excludes negative emissions via BECCS from any CO2/GHG constraint policy (optional)

Including the `BECCS_integration.xml` file will make it so that BECCS technologies are paid just enough to induce them to supply the CDR that the markets/policymakers demand, rather than getting paid at the carbon price. This reflects the default assumption in GCAM-CDR that because BECCS technologies are competing with other CDR technologies, they will not enjoy the enormous economic rents involved in being paid many hundreds or even thousands of dollars for each ton of carbon they sequester.

Note that the `ignore_BECCS_in_CO2_constraint.xml` file is only relevant for scenarios that include a constraint-based carbon policy that targets CO<sub>2</sub> (e.g., a `ghgpolicy` that uses `<constraint>` tags rather than `<fixedTax>` tags). When using such a constraint, the `ignore_BECCS_in_CO2_constraint.xml` file is needed if and only if you wish to set separate targets for emissions abatement and CDR. See the [CDR Policies](./CDR_policies.md) page for details.  

## Creating CDR Demand
Adding new CDR sectors and technologies **will not, by itself, induce any CDR in a scenario.**[^1] The configuration file must also include one or more CDR policy files, such as `input/policy/CDR/CDR_exo_2GtC.xml`.  See the [CDR Demand and Policies](./CDR_policies.md) page for details. 

## Controlling the Growth of the CDR Sector

One crucial question about CDR is the rate at which the CDR sector can grow. This is highly uncertain. In most sectors in GCAM, the growth of new technologies, such as BECCS, is endogenously limited by competition with existing technologies. Because CDR is in its own sector, users must take steps to ensure that new CDR technologies do not grow at an unrealistic rate. This can be done in two main ways:

- **Set demand in a way that rises gradually.** By configuring CDR demand using exogenous, offset, or elastic demand sources, it is possible to set total demand for CDR to grow gradually. For example, including `CDR_elastic_demand.xml` will induce a gradual rise in CDR demand, assuming a gradually rising carbon price.
- **Use a "growth-limiting" file.** GCAM-CDR includes several `CDR_growth_limit` files that constrain the growth of CDR output. Specifically, these files impose a quantity-based cap on the combined output of CDR technologies. This cap gradually rises over time, reflecting assumptions about the maximum feasible rate of growth of the CDR sector. Any CDR demand above that cap is allocated to a placeholder technology (`unsatisfied CDR demand`) that consumes no real inputs and removes no carbon.[^2] Users can easily modify the rate of growth by editing the XML files. See the XML files themselves to see how they work and how to modify them to customize the growth constraints. 

\[[return to GCAM-CDR overview](./README.md)\]

### Notes

[^1]: Some BECCS technologies may still operate because of the carbon price, depending on the details of the configuration. In a standard GCAM-CDR configuration, however, any BECCS output will be minimal without CDR-specific policies.
[^2]: The file does this by adapting a technique developed by JGCRI for their implementation of direct air capture. The file adds an `unsatisfied CDR demand` technology to the `CDR_regional` sector. That technology is expensive enough that it will not be used unless a tax is applied to the "real" CDR technologies. The file then adds a `policy-portfolio-standard` constraint on "real" CDR, whose price is passed on to the CDR technologies as a tax. GCAM-CDR adjusts that tax until the combined output of the "real" CDR technologies falls below that constraint; any remaining demand is allocated to the `unsatisfied CDR demand` technology. Note that, because of this set-up, users should take care in interpreting certain results when using a `CDR_growth_limit.xml` file. For instance, the model may report 2GtC worth of CDR output, even though 1.95 GtC of that output is from the `unsatisfied CDR demand` technology. Users should look at the outputs from the "real" CDR technologies in the `CDR_regional` sector to find the actual output of CDR. Similarly, the model will report that the technologies in the `CDR_regional` sector are much more expensive than usual, but this is misleading. Users should look at the prices of the relevant markets (e.g., the `DAC` market) to get the cost of DAC. 
