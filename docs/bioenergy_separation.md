# Bioenergy Separation

The bioenergy separation ("bioseparation") file more completely separates bioenergy from fossil energy through critical parts of the energy system, especially in energy transformation and distribution. This makes it possible to track BECCS more completely and affect its deployment more accurately than using the default set-up.

Note that the file does not disentangle bioenergy and fossil fuels until after the model completes its calibration to historical data, because the calibration data combines fossil fuels and biofuels. During the calibration periods (through 2015), the bioenergy separation files cause no changes in model behavior or output.

The bioenergy separation file does the following:

1. Turns off the `biomass gasification` subsector of the `gas processing` supply sector and the `biomass liquids` subsector of the `refining` supply sector.[^1] 

2. Creates new `biogas processing` and `biorefining` supply sectors, along with their associated technologies.

3. Creates new energy distribution supply sectors, subsectors, and technologies:
	-  Creates `biogas pipeline` and `wholesale biogas` supply sectors, along with their associated technologies.
	- Creates a new `delivered biogas` subsector in the `delivered gas` supply sector. (Technologies that use `delivered gas` therefore do not distinguish between fossil gas and biogas.)

 1. Creates a new `refined bioliquids industrial` sector.
 2. Creates a `refined bioliquids enduse` technology in the `refined liquids enduse` supply sectors. (Technologies that use `refined liquids enduse` therefore do not distinguish between fossil liquids and bioliquids.)

4. Creates new subsectors and technologies in the `elec_X` pass-through sectors that connect various electricity technologies to biogas and bioliquids supply sectors.
5. Creates new technologies in the `industrial energy use` and `industrial feedstocks` supply sectors. (But note that the `bio_sep_liquids_limits.xml` disables the use of bioliquids in the `industrial feedstocks` sector.)

Taken together, these changes induce non-negligible changes in primary energy consumption by fuel type, relative to GCAM 5.4. These changes are most noticeable in the refined liquids sector. To reduce these changes, some of the bioenergy technologies in various sectors have been weighted differently than in GCAM 5.4, but quantitative differences remain. Furthermore, this "bioseparation" creates some minor qualitative changes in the way the energy sector works. For example, in GCAM 5.4, a gas-fired power plant built in 2030 could gradually transition from fossil gas to biogas as the fraction of biogas in the system increases. With the bioenergy separation file, fossil gas-fired power plants and biogas-fired power plants are considered different technologies; a plant built in 2030 will still use the same fuel in 2050, regardless of the overall share of biogas in the energy system.

\[[return to GCAM-CDR overview](./README.md)\]

### Notes

[^1]: This happens after the last calibration period. It is accomplished by setting the share-weight for those subsectors to zero after the last calibration period, which means that the biofuel technologies continue to exist in the relevant sectors, but are never used.
 
