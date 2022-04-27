# Overview of GCAM-CDR

GCAM-CDR is a variant of the [Global Change Analysis Model (GCAM)](http://www.globalchange.umd.edu/gcam/) [version 5.4](http://jgcri.github.io/gcam-doc/). GCAM-CDR extends GCAM by adding more technologies and policies for carbon dioxide removal (CDR). GCAM 5.4 is produced by the [Joint Global Change Research Institute (JGCRI)](http://www.globalchange.umd.edu/) at Pacific Northwest National Laboratories (PNNL). GCAM-CDR was produced by the [Institute for Carbon Removal Law and Policy](https://carbonremoval.info) at American University.

This documentation covers the main changes to GCAM 5.4 that are implemented in GCAM-CDR 1.0.  For documentation of all other features of GCAM 5.4, see JGCRI's documentation of GCAM itself. (Throughout this documentation, "GCAM 5.4" refers to JGCRI's official release of the model.)

_Documentation last updated April 27, 2022_

## Summary of new elements

GCAM-CDR introduces the following elements to GCAM:

- **New CDR technologies.** GCAM-CDR includes direct air capture (DAC), terrestrial enhanced weathering (TEW), and ocean enhanced weathering (OEW). It also includes all of the bioenergy with carbon capture and storage (BECCS) technologies that are included in GCAM 5.4. As of GCAM 5.4, GCAM-core also includes a native implementation of DAC. The variants included in GCAM-CDR are similar, but not exactly the same, to those in GCAM 5.4.
- **Finer control over CDR deployment, including CDR policy.** GCAM-CDR gives users finer control over CDR deployment and policy than GCAM 5.4 does. This includes control over the demand for CDR, the maximum rate of growth in the CDR sector, interregional trade in CDR, and the integration of BECCS and other CDR technologies.
- **More thorough separation of biofuels from fossil fuels.** To facilitate tracking and control of BECCS technologies, GCAM-CDR includes files that more thoroughly separate biofuels from fossil fuels in the energy system. In GCAM 5.4, liquid and gaseous biofuels are mixed with fossil fuels during upstream energy transformation. In GCAM-CDR, they are kept separate except in various end-use sectors.

GCAM-CDR also involves minor changes to other elements in the GCAM source code. These changes were made to enable new CDR technologies and policies, and they are implemented so as to avoid changing the model's behavior when using existing XML input files. The changes are visible in the commit history of this Github repository.
 
## Overview of documentation

If you want to jump right in, read the [Quick Start Guide for GCAM-CDR](./GCAM-CDR_quick_start_guide.md).

If you want to learn about GCAM-CDR in more detail, this documentation covers the following topics:

- **[New CDR technologies](./CDR_technologies.md).** An explanation of the various CDR technologies in GCAM-CDR.
- **[A new CDR sector](./CDR_supply_sector.md).** An overview of the CDR supply sector.
- **[CDR policies](./CDR_policies.md).** Detailed explanation of the various CDR policies  available in GCAM-CDR, especially policies for [configuring demand for CDR](./CDR_policies.md#configuring-demand-for-CDR). For a quicker introduction to these topics, see the [Quick Start Guide](./GCAM-CDR_quick_start_guide.md#CDR-policy-files).
- **[BECCS integration](./BECCS_integration.md).** An overview of GCAM-CDR's method for integrating bioenergy with CCS (BECCS) technologies into the CDR sector.
- **[Separation of biofuels from fossil fuels](bioenergy_separation.md).** A detailed explanation of the "bioseparation" files that partially disentangle liquid and gaseous biofuels from fossil fuels.
