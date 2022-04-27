# BECCS Integration

Bioenergy with carbon capture and storage (BECCS) technologies produce electricity, liquid biofuels, or hydrogen as their primary output, but they produce CDR as a side effect of their normal operation.[^1] In GCAM-CDR, as in GCAM 5.4, BECCS technologies reside in the energy sector.

GCAM-CDR integrates BECCS technologies into the CDR market in two senses:

1. BECCS technologies compete directly with other CDR technologies to satisfy regional demand for CDR, using GCAM's normal mechanism for economic choice between competing technologies.
2. BECCS technologies are paid for their CDR services at a price determined by the CDR market, rather than at the carbon price. This price normally reflects the minimum payment required to incentivize the amount of BECCS that the CDR market demands.[^2]     

## Contrast with GCAM 5.4

In GCAM 5.4 (and earlier versions), BECCS technologies are subsidized for this CDR at the price of carbon. That is, for each ton of carbon that they sequester, they receive a subsidy equal to the carbon price.[^3] The total amount of CDR they provide (i.e., the amount of carbon they sequester) is proportional to their output of their primary good (e.g., electricity). That, in turn, is determined by the demand for the relevant energy carrier (e.g., electricity) and the BECCS technologies' share of the market for producing that good, which depends on their post-subsidy costs. The amount of BECCS in any given period is therefore largely determined by the carbon price, rather than by any explicit demand for CDR.

## Technical remarks on implementation

The integration of BECCS into the CDR market is accomplished via an intermediate market whose price is determined by GCAM's solution algorithm. This intermediate market is called `BECCS`, but it could also be thought of a market for BECCS-derived CDR credits.

All bioenergy technologies that use CCS produce a secondary output of `BECCS` that increases the supply of the `BECCS` market. The `BECCS` technology in the `CDR_regional` sector takes `BECCS` as an input. GCAM-CDR adjusts the price of the `BECCS` market until the amount of carbon sequestered by BECCS technologies matches the amount demanded of BECCS technologies by the CDR market. Because BECCS technologies also earn revenue from their primary output (e.g., electricity or liquid biofuels), this cost is often less than the cost of ["dedicated" CDR technologies](./CDR_technologies.md) like direct air capture.

When integrating BECCS technologies into the CDR market, GCAM-CDR also adds a `ctax-input` that reflects the amount of carbon sequestered by each technology in each period. This neutralizes the carbon-price subsidy that these technologies normally receive, so that their payment for sequestering carbon is determined by the CDR market, not the carbon price.  

\[[return to GCAM-CDR overview](./README.md)\]

### Notes
[^1]: BECCS technologies produce CDR as a byproduct because the carbon contained in their input fuels has been removed from the atmosphere through the growth of biomass. Therefore, when the CO<sub>2</sub> released by processing or consuming those fuels is captured and geologically sequestered, the net effect is to move CO<sub>2</sub> from the atmosphere into geological storage.

[^2]: The market that calculates this price has a built-in minimum price. As long as BECCS technologies are supplying at least as much CDR as the market demands of them, they will be paid at that minimum price. This creates the possibility for conditions in which BECCS technologies are paid for CDR at a higher rate than the market would demand. From a technical perspective, the minimum price helps GCAM's solution algorithm avoid difficulties with the CDR market, but the optimal level of the minimum price in the BECCS market remains an open question.

[^3]: Strictly speaking, the subsidy is applied at the point when biomass enters the energy system (e.g., as `regional biomass` or `regional corn for ethanol`), because that is the point at which GCAM "sees" negative emissions. Bioenergy technologies without CCS "pay back" that subsidy when they re-emit the carbon contained in their `regional biomass` or similar inputs. Since BECCS technologies sequester most of that carbon, rather than re-emitting it, they do not have to repay the subsidy. The net effect is equivalent to BECCS technologies getting paid to sequester carbon.

