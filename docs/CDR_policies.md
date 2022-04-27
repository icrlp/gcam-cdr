# CDR Demand and Policies
This file documents various ways to configure CDR-related demand, policies, and controls in [GCAM-CDR 1.0](./README.md).

## Overview
In GCAM-CDR, as in GCAM 5.4, demand for CDR is driven by CDR-related policy. In GCAM 5.4, the primary CDR-related policy is the carbon price: BECCS technologies are paid at the carbon price for each ton of carbon it sequesters, and direct air capture (DAC), when activated, deploys in response to the carbon price. GCAM-CDR introduces a much wider range of CDR policies and a greater degree of control over CDR deployment.

For the purposes of GCAM-CDR, CDR-related policies can be divided into several types: those that [create demand for CDR](#creating-demand-for-CDR), those that control [trade in CDR](#trade-in-CDR), and those that manage the interactions between CDR and emissions policies (e.g., carbon taxes and carbon constraints).

## Creating Demand for CDR 
In GCAM, demand for all goods and services derives from demand for "final" goods, such as food, residential and commercial buildings (and energy to run them), industrial output, transportation, etc. GCAM forecasts demand for these final goods based on historical demand combined with projections of population, per capita income, income-elasticity and price-elasticity for the goods, and projected prices of inputs. The demand for final goods then determines demand for intermediate goods, such as electricity or liquid fuels, which determines demand for primary goods, such as coal, biomass, or oil.

Because demand for CDR works very differently than demand for these other goods, GCAM-CDR introduces a different mechanism for creating demand for CDR. Users can specify one or more "Demand Sources" in any region, which induces demand in different ways. GCAM-CDR 1.0 includes four different kinds of Demand Sources, which are summarized in the following table. 

Type of Demand Source | Description | Sample Use Case
-----|------------|------------------
**Exogenous** | Explicitly sets demand for a certain quantity of CDR, measured in millions of metric tons of carbon (MtC) removed. | Modeling separate targets for emissions abatement and CDR (e.g., modeling a regional net-zero target in which gross CO<sub>2</sub> emissions fall to 100 MtC, and 100 MtC worth of CDR neutralizes the remaining emissions)
**Elastic** | Sets demand for an endogenously-calculated quantity of CDR, which rises and falls with the carbon price. Users can configure the relationship between the carbon price and demand for CDR. | Modeling a case in which CDR demand is assumed to rise with climate policy ambition, as measured by the carbon price
**Offset** | Sets demand for CDR in each period as a fraction or multiple of emissions of a particular greenhouse gas, such as CO<sub>2</sub>, in that period. Users can configure the Demand Source to offset only emissions from particular sectors and/or technologies. | Modeling a net-zero policy without separate targets; or neutralizing emissions from specific sectors
**Accumulated** | Tracks demand that went unsatisfied in earlier periods because of limited CDR capacity, and then gradually adds that demand back into the CDR market over time once CDR capacity has expanded. (Note that this Demand Source must be used together with at least one other Demand Source. It will not generate demand for CDR on its own.) | Modeling a scenario in which a country or sector commits to removing all of its historical emissions

Users can combine Demand Sources within the same region (e.g., to set a base level of demand plus additional demand that rises with the carbon price), as illustrated in the examples below.

## Configuring CDR demand

To configure CDR demand in a particular region, users must include a `CDR-final-demand` tag in that region, and the `CDR-final-demand` tag must contain at least one Demand Source. The following examples illustrate the configuration of different kinds of Demand Sources in the `USA` region.

### Configuring Exogenous demand sources

 The simplest way to configure CDR demand is to set demand to a specific level.
```xml
<region name="USA">
  <CDR-final-demand name="CDR">	      
	<demand-source>
	  <demand year="2025" fillout="1">500</demand> <!-- Adds 500 MtC of demand in every period from 2025 onward. -->
	</demand-source>
  </CDR-final-demand>
</region>
``` 

Demand can also be specified for each period, as shown below. Note that if the purpose of specifying demand for each period is to limit the growth of CDR, another option is to set demand to the maximum value in all periods and [use a growth-limiting file to constrain growth](./configuring_GCAM-CDR.xml#Controlling-the-Growth-of-the-CDR-Sector).
```xml
<region name="USA">
  <CDR-final-demand name="CDR">	        
	<demand-source>
	  <demand year="2025">500</demand>
	  <demand year="2030">600</demand>
	  <demand year="2035">700</demand>
	  <demand year="2040">800</demand>
	  <demand year="2045">900</demand>
	  <demand year="2050">1000</demand>
	  <demand year="2055">1100</demand>
	  <demand year="2060">1200</demand>
	  <demand year="2065">1300</demand>
	  <demand year="2070">1400</demand>
	  <demand year="2075" fillout="1">1500</demand> <!-- The fillout="1" parameter extends this through all future periods. -->
	</demand-source>	
  </CDR-final-demand>
</region>
```
Note that **by default, exogenous demand sources will not produce any CDR demand unless there is a carbon price in the region.** Users can easily turn this off by setting the `require-C-price` parameter to `false`, as shown below. 
```xml
<region name="USA">
  <CDR-final-demand name="CDR">	      
	<demand-source>
	  <demand year="2025" fillout="1">500</demand>
	  <require-C-price>0</require-C-price>
	</demand-source>
  </CDR-final-demand>
</region>
``` 

The `require-C-price` parameter can also be used to change the name of the CO<sub>2</sub> market on which CDR demand depends, as shown below.

```xml
<region name="USA">
  <CDR-final-demand name="CDR">	      
	<demand-source>
	  <demand year="2025" fillout="1">500</demand>
	  <require-C-price name="CO2_LTG">1</require-C-price>
      <!-- The optional "name" attribute tells the model the
           name of the tax market to check for a carbon price.
           The default is "CO2". --> 
	</demand-source>
  </CDR-final-demand>
</region>
``` 

## Configuring Elastic demand sources

An Elastic demand source makes CDR demand rise (or fall) with the carbon price. With an elastic demand source, demand for CDR is calculated as a function of the carbon price:

Demand<sub>CDR</sub> = maxDemand / ( 1 + _e_<sup>( (-1 \* steepness ) / ( carbonPrice - midpoint ) )</sup> 

This equation produces an "s-curve," so that CDR demand will rise slowly at first in relation to the carbon price; then rise more quickly as the carbon price gets higher; and eventually level off at some maximum value, once the carbon price gets high enough.

The simplest way to create elastic demand is as follows:

```xml
<region name="USA">
  <CDR-final-demand name="CDR">	      
	<elastic-demand-source> 		<!-- Create an elastic demand source using the default settings. -->
	  <max-demand>1000</max-demand> <!-- Set the maximum demand from this source to 1000 MtC. -->
	</elastic-demand-source>
  </CDR-final-demand>
</region>
```

The default settings for the elastic demand source are given in the table below.

Setting | Default Value | Notes
----------|-------------|------
`max-demand` | 0 | _**No demand will be added unless the user specifies a `max-demand`.**_
`steepness` | 0.005 | _Higher values lead to a steeper s-curve: demand rises more slowly at first, but then rises more quickly as the carbon price approaches the `midpoint`._
`midpoint` | _500_ | _The carbon price at which CDR demand will equal half of `max-demand`._
`min-price` | _0_ | _The minimum carbon price that must be met before any demand for CDR is added._

The following example shows how to modify the shape of the s-curve and prevent any CDR demand until the carbon price reaches a certain level.


```xml
<region name="USA">
  <CDR-final-demand name="CDR">	      
	<elastic-demand-source> 		
	  <max-demand>10000</max-demand>  <!-- Set the maximum demand to 10,000 MtC. -->
	  <steepness>0.1</steepness>      <!-- Increase the steepness of the curve. -->
	  <midpoint>2000</midpoint>		  <!-- Demand reaches 5,000 MtC at a carbon price of $2000 per ton of carbon. -->
	  <min-price>100</min-price>      <!-- If the carbon price is below $100/ton, do not add any CDR demand. -->
	</elastic-demand-source>
  </CDR-final-demand>
</region>
```
### Configuring  Offset demand sources

An Offset Demand Source sets CDR demand equal to a fraction (or multiple) of the net emissions of a specific greenhouse gas in a specific region, subject to an optional quantity cap.[^1] Because of the way that GCAM handles carbon accounting, **the Offset Demand Source should be configured carefully when deviating from GCAM-CDR's typical configuration of CDR technologies and BECCS.** 

A simple use of an Offset Demand Source can be configured as follows. In this example, if net emissions of CO<sub>2</sub> in the USA region equal 100 MtC (ignoring removals via CDR[^2]), this demand source will add 100 MtC of demand for CDR in the USA region; if net emissions of CO<sub>2</sub> equal 200 MtC, it will add 200 MtC of demand for CDR. 

```xml
<region name="USA">
	<CDR-final-demand name="CDR">
		<offset-demand-source> <!-- Default to offsetting CO2 in USA. -->
			<offset-fraction year="2025" fillout="1">1</offset-fraction> <!-- Try to offset 100% of gross CO2 emissions from USA. -->
		</offset-demand-source>
	</CDR-final-demand>
</region>
```

A more complete picture of the default settings is given by this example:

```xml
<region name="USA">
	<CDR-final-demand name="CDR">
		<offset-demand-source>
			<offset-fraction year="2025" fillout="1">1</offset-fraction>
			<market-name>CO2</market-name> <!-- Default: CO2 -->
			<market-region>USA</market-region> <!-- Defaults to containing region. -->
			<max-offset>1000000</max-offset> <!-- Default maximum is 1,000,000 MtC. -->
		</offset-demand-source>
	</CDR-final-demand>
</region> 
```

The following example illustrates how to offset both CO<sub>2</sub> and a non-CO<sub>2</sub> greenhouse gas. The `offset-fraction` is typically set to the CO<sub>2</sub>-equivalent value of the relevant non-CO<sub>2</sub> gas. Note, however, that because of difference in the length of time that CO<sub>2</sub> and non-CO<sub>2</sub> gases remain in the atmosphere, avoiding emissions of non-CO<sub>2</sub> gases has different long-term climate implications than removing an amount of CO<sub>2</sub> equal to the CO<sub>2</sub>-equivalent value of the gas. 

```xml
<region name="USA">
	<CDR-final-demand name="CDR">
		<offset-demand-source name="CO2_offset">
			<offset-fraction year="2025" fillout="1">1</offset-fraction>
		</offset-demand-source>
		<offset-demand-source name="CH4_offset">
		   <offset-fraction year="2025" fillout="1">5.7272</offset-fraction> <!-- Remove 5.7272 tC for each tCH4 emitted. -->
		   <market-name>CH4</market-name> <!-- This names the GHG to offset. --> 
		</offset-demand-source>
	</CDR-final-demand>
</region>
```

Note that **emitters continue to pay the carbon price on their emissions**, even if emissions are offset. The "offsets" created by the Offset demand source are therefore not "carbon offsets" in the typical use of the term. Instead, this set-up simulates a situation in which countries or sectors commit to offsetting their residual emissions but do not allow emitters to avoid carbon taxes by paying for offsets instead.

During development of GCAM-CDR 1.0, a "tax avoidance" feature was tested in which, after a specified year, the effective price of carbon was set to the price of CDR, simulating situations in which emitters could avoid taxes by paying for CDR instead. This often led to extensive model failures and/or extremely implausible results. Given the non-negligible loss in performance required to make this feature available, it is turned off in GCAM-CDR 1.0. The relevant code remains, commented out, in the GCAM-CDR source code. Users wishing to explore the implications of allowing emitters to pay for CDR instead of carbon taxes can therefore re-enable the feature by editing and recompiling the GCAM-CDR source code. With careful configuration, reasonable scenarios could likely be produced using the feature. 

### Configuring Accumulated demand sources
An Accumulated Demand Source keeps track of CDR demand that went unsatisfied in earlier periods because the CDR sector could not meet demand, and then gradually adds that accumulated demand back into the market in later periods. More specifically, at the beginning of each period, the demand source checks for unsatisfied demand from the previous period, adds it to a stock of accumulated demand, and then adds a user-specified fraction of that accumulated demand back into the market. If any of that extra demand again goes unsatisfied, it is added back into the stock of accumulated demand at the beginning of the next period.

The Accumulated Demand Source relies on the `unsatisfied CDR demand` technology used to constrain the growth of CDR. That technology must be modified slightly when using an Accumulated Demand Source, so that GCAM-CDR can track the amount of CDR demand that goes unsatisfied, as shown in the following example.

```xml
<region name="USA">
	<CDR-final-demand name="CDR">
		<accumulated-demand-source>        <!-- Create the demand source. -->
			<market-name>unsatisfiedCDRdemand</market-name>   <!-- Specify the name of the market used to track unsatisfied demand. -->
			<fraction-to-satisfy>0.1</fraction-to-satisfy>    <!-- OPTIONAL. Specifies what fraction of unsatisfied demand to add in each period. Defaults to 0.1. -->
		</accumulated-demand-source>
	</CDR-final-demand>
	<ghgpolicy name="unsatisfiedCDRdemand">                   <!-- Create a placeholder tax market for tracking unsatisfied demand. -->
		<market>USA</market>
		<fixedTax year="1975" fillout="1">0</fixedTax>        <!-- We don't want this tax market to affect prices at all. -->
	</ghgpolicy>
</region>
<global-technology-database>
	<location-info sector-name="CDR_regional" subsector-name="unsatisfiedDemand">
		<technology name="unsatisfied CDR demand">
			<!-- We add one unit of demand to the unsatisfiedCDRdemand
				 tax market for each unit of output from this technology.
				 Since GCAM *subtracts* secondary outputs from demand,
				 we set the output ratio to -1 rather than 1. -->
			<period year="1975">
				<secondary-output name="unsatisfiedCDRdemand">
					<output-ratio>-1</output-ratio>
				</secondary-output>
			</period>
			<period year="1990">
				<secondary-output name="unsatisfiedCDRdemand">
					<output-ratio>-1</output-ratio>
				</secondary-output>
			</period>
			. . . <!-- shortened for brevity; see input/policy/CDR/CDR_accumulate_demand.xml for full definition -->
			<period year="2100">
				<secondary-output name="unsatisfiedCDRdemand">
					<output-ratio>-1</output-ratio>
				</secondary-output>
			</period>
		</technology>
	</location-info>
</global-technology-database>
```
The optional `fraction-to-satisfy` parameter determines what fraction of the stock of accumulated demand gets added to the market in each period. A lower value will spread the accumulated demand out over more periods. The default value is 0.1.  

## Trade in CDR
GCAM-CDR allows CDR services to be traded between regions, using an Armington-style trading system like [the one that GCAM 5.4 uses for trade in agricultural products](http://jgcri.github.io/gcam-doc/details_trade.html#armington-style-trade). Users can easily disable trade by removing `CDR_traded.xml` from the configuration file. They can also customize how trade works by editing that file.

The CDR trading system amounts to a global (or multi-region) market for carbon removal "credits," so that demand for CDR in one region can be satisfied by CDR carried out in some other region(s). By default, all final demand for CDR in each region is initially transferred to the global market. (This represents an assumption of "zero home-bias," meaning that buyers in each region have no preference for domestically sourced CDR.) Global CDR demand is then allocated between regions based on the price of CDR in each region and a weighting that reflects each region's capacity for CDR. In GCAM-CDR 1.0, these period-by-period weightings were calculated as each region's share of global GHG emissions in each period in a Reference scenario. The intuition behind that weighting is that GHG emissions in a Reference scenario reflect a combination of population, wealth, energy production/consumption, industrial output, and agricultural output (which is relevant for capacity to conduct enhanced weathering on croplands).

Note that unless trade is disabled, users can easily set global CDR demand by adding demand to any single region. For example, when trade is enabled, adding 2 GtC of CDR demand to the `USA` region will result in 2 GtC of CDR output globally, with the output distributed between regions by the trading mechanism.

## CDR and Emissions Policies
Setting prices or constraints on greenhouse gas emissions is the simplest way to set climate policy in GCAM. It remains essential in GCAM-CDR. When designing scenarios in GCAM-CDR, it is important to consider the relationship between CDR and emissions policies.

**The default behavior in GCAM-CDR is that CDR technologies are not paid at the carbon price**, and **modifying this default behavior is likely to cause problems.** That is, CDR technologies do not and should not receive a payment equal to the carbon price for each ton of carbon they sequester. The model will generally not work as intended if this default behavior is modified: doing so will distort the price signals in the CDR market and, in particular, distort competition between BECCS and other CDR technologies. Demand for CDR, including BECCS, is driven by [CDR policies, as described above](#configuring-CDR-demand), not directly by the carbon price itself.

On a technical level, the default behavior is accomplished through two mechanisms:
1. Non-BECCS CDR technologies include a `<CO2 name="CO2_CDR"/>` tag, instead of the normal `<CO2 name="CO2"/>` tag. This means that the climate model sees their negative emissions as removing CO<sub>2</sub> from the atmosphere, but they are not directly connected to the `CO2` market, which handles carbon prices and constraints.
2. BECCS technologies, once integrated into the CDR market, are configured to "pay back" the subsidy they receive for sequestering CO<sub>2</sub> captured from the air by biomass, just as bioenergy technologies without CCS do. In place of that subsidy BECCS technologies are paid for CDR at a price determined by the CDR market.

**If a scenario requires [linking the `CO2_CDR` market to a policy market](http://jgcri.github.io/gcam-doc/policies.html#linked-markets), such as the `CO2` or `GHG` market, then the `<price-adjust>` parameter should be set to 0.**

With that caveat in mind, CDR can stand in various relationships to emissions policies in GCAM-CDR. In some cases, emissions policies and CDR policies are more or less independent. For example, when emissions are discouraged through a fixed tax and CDR is incentivized through an Exogenous Demand Source, the model manages emissions reductions and CDR independently. In other cases, however, emissions policies and CDR policies interact. This is especially likely when emissions policies rely on a carbon constraint or greenhouse gas constraint, such as a net-zero target. Several such cases are discussed below.

### Setting separate targets for abatement and CDR
With the default configuration in GCAM-CDR, a constraint on CO<sub>2</sub> will not count negative emissions from CDR toward the satisfaction of that constraint. For example, if a user specifies a global CO<sub>2</sub> constraint of 500 MtC in 2050, and CO<sub>2</sub> emissions are at 1,000 MtC in 2050, the model will not recognize the constraint as satisfied _even if there are at least 500 MtC removed via DAC, OEW, and TEW in 2050_.[^3] 

This makes it easy to set separate targets for emissions abatement and CDR, as some experts recommend to policymakers ([McLaren et al. 2019](https://www.frontiersin.org/articles/10.3389/fclim.2019.00004/full); [Lee et al. 2021](https://iopscience.iop.org/article/10.1088/1748-9326/ac1970/meta#references)).[^4] Simply set a carbon/GHG constraint in the normal way, and then set demand for CDR separately, and the two policies will need to be satisfied independently. For example, setting a constraint in which CO<sub>2</sub> declines linearly from a peak in 2025 to 500 MtC in 2050, plus [a CDR policy that calls for 500 MtC of CDR in every period](./CDR_policies#configuring-exogenous-demand-sources), will result in a net-zero policy with separate targets for emissions abatement and CDR. The following code snippet illustrates how to do this:

```xml
<region name="USA">
  <ghgpolicy name="CO2">
    <market>global</market>
    <constraint year="2025">12000</constraint>
    <constraint year="2030">9700</constraint>
    <constraint year="2035">7400</constraint>
    <constraint year="2040">5100</constraint>
    <constraint year="2045">2800</constraint>
    <constraint fillout="1" year="2050">500</constraint>
  </ghgpolicy>
  <CDR-final-demand>
    <demand-source>
      <demand fillout="1" year="2025">500</demand> <!-- This will get distributed globally via trade. -->
    </demand-source>
  </CDR-final-demand>
</region>
<region name="Africa_Eastern">
  <ghgpolicy name="CO2">
    <market>global</market> <!-- Include this region in the global constraint. -->
  </ghgpolicy>
  <!-- No CDR-final-demand tag is needed in other regions for this set-up. -->
</region>
  ... <!-- Add other regions. -->
```  
Another option is set to a constraint on net-emissions (including CDR) and then link the `CO2_CDR` market to the `CO2` market. This requires _removing_ the `ignore_BECCS_in_CO2_constraint.xml` file from the configuration, and adding a policy file like the one sketched below:

```xml
<region name="USA">
  <ghgpolicy name="CO2">
    <market>global</market>
    <constraint year="2025">12000</constraint>
    <constraint year="2030">9600</constraint>
    <constraint year="2035">7200</constraint>
    <constraint year="2040">4900</constraint>
    <constraint year="2045">2400</constraint>
    <constraint fillout="1" year="2050">0</constraint>
  </ghgpolicy>
  <CDR-final-demand>
    <demand-source>
      <demand fillout="1" year="2025">500</demand> <!-- This will get distributed globally via trade. -->
    </demand-source>
  </CDR-final-demand>
  <linked-ghg-policy name="CO2_CDR">
    <price-adjust fillout="1" year="1975">0.0</price-adjust> <!-- Keep this at 0! -->
    <demand-adjust fillout="1" year="1975">1.0</demand-adjust>
    <market>global</market>
    <linked-policy>CO2</linked-policy>
    <price-unit>1990$/tC</price-unit>
    <output-unit>MtC</output-unit>
  </linked-ghg-policy>
</region>
<region name="Africa_Eastern">
  <ghgpolicy name="CO2">
    <market>global</market>
  </ghgpolicy>
  <linked-ghg-policy name="CO2_CDR">
    <price-adjust fillout="1" year="1975">0.0</price-adjust> <!-- Keep this at 0! -->
    <demand-adjust fillout="1" year="1975">1.0</demand-adjust>
    <market>global</market>
    <linked-policy>CO2</linked-policy>
    <price-unit>1990$/tC</price-unit>
    <output-unit>MtC</output-unit>
  </linked-ghg-policy>
</region>
  ... <!-- Add other regions. -->
```  

The net effect here is to count negative emissions from BECCS and other CDR technologies toward the satisfaction of the constraint. Because there is only 500 MtC of demand for CDR, however, the model will be forced to satisfy the policy by bringing net emissions from all other sectors down to 500 MtC by 2050. Note, though, that **this successfully models separate targets only if the net-emissions constraint is designed to account for the negative emissions from CDR.** Otherwise, the scenario could involve a moral hazard effect, as explained in the next subsection. 

### Moral hazard
The term "moral hazard," in relation to CDR, refers to the idea that deploying CDR could slow the reduction in gross emissions of CO<sub>2</sub> and/or other greenhouse gasses. Configuring GCAM-CDR incorrectly can unintentionally induce a strong moral hazard effect in which each ton of carbon removed via CDR leads to an extra ton of fossil carbon being emitted into the atmosphere. (This also means that the potential impact of moral hazard could be studied by deliberately configuring GCAM-CDR to induce a moral hazard effect of varying strengths.)

In general, whenever negative emissions from CDR are linked to an emissions constraint in GCAM-CDR, the model will project additional fossil carbon emissions equal to the amount of CDR, relative to what it would project without that CDR demand. The linkage occurs when (1) a carbon constraint is imposed and (2) the `ignore_BECCS_in_CO2_constraint.xml` file is removed and/or the `CO2_CDR` market is linked to the carbon constraint through a `linked-ghg-policy`. For example, comparing model results from a scenario with a net-zero constraint to results from a scenario with _the same_ net-zero constraint _with CDR included in the constraint_ will result in additional fossil emissions equal to the negative emissions from CDR. CDR may appear to "make it easier" to achieve the constraint because it lowers the carbon price, but this is only because the constraint on gross emissions is much weaker under that configuration.

This is why **the default behavior in GCAM-CDR is not to link negative emissions from CDR to emissions constraints,** and why it is recommended that users think carefully about the relationship between their emissions constraints and CDR demand.

\[[return to GCAM-CDR overview](./README.md)\]

### Notes

 [^1]: To calculate net emissions, the Offset Demand Source creates a new market (called, e.g., `CO2-offset-demand` or `CH4-offset-demand`). Any technology that emits/removes the specified greenhouse gas adds its net emissions to the demand for that market, and the `offset demand source` is set equal to the demand for that market. Because the new CDR technologies CO<sub>2</sub> emissions are named `CO2_CDR`, they are ignored by that market, even though they are still recognized as CO<sub>2</sub> emissions by the climate model. This is why the Offset Demand Source ignores the removals via the new CDR technologies. In order to get the Offset Demand Source to ignore removals via BECCS, the `CDR_offset_CO2.xml` file adds a secondary output to the BECCS technology in the `CDR_regional` sector that increases demand in the `CO2-offset-demand` market, counteracting the BECCS technologies' initial reduction in demand for offsets. That way, the demand for BECCS' CDR is routed through the CDR market, rather than incorporated silently into the calculation of net emissions.
 [^2]: See note 1 on how the Offset Demand ignores removals via CDR. When creating new offset demand XML files, users should pay close attention to the handling of removals from BECCS. Failure to adjust the `CO2-offset-demand` market will cause double counting of removals via BECCS, as those removals are counted first in the net emissions calculations and again in the satisfaction of demand for CDR.
[^3]: GCAM 5.4's default behavior is to count negative emissions from BECCS toward the carbon constraint. Because BECCS technologies emit a fraction of the CO<sub>2</sub> in their biofuels back into the atmosphere, they need to be integrated into the main `CO2` market. The `ignore_BECCS_in_CO2_constraint.xml` file in GCAM-CDR excludes BECCS technologies' sequestered carbon from the `CO2` market without excluding the carbon that they emit. Thus, if the `ignore_BECCS_in_CO2_constraint.xml` file is _removed_ from the configuration file, then the negative emissions from BECCS technologies _will_ be counted toward the satisfaction of the constraint, as it is in GCAM 5.4.
[^4]: For a contrasting view, see ([Smith 2021](https://www.nature.com/articles/s43247-021-00095-w)), but note that Smith's suggestions require more care to implement in GCAM and GCAM-CDR because of the models' default reliance on technology-neutral carbon pricing and net-emissions constraints.
