# Changelog

v2.1.25 changes in progress (unreleased)
- new module: ChSel1 to select one channel from a polyinput
- bug fix: multiple minor changes to provide compatility with the GitHub Actions script
- bug fix: PokeMe not to mess up 5V and 6V values in case of "value only" mode
- new feature: PokeMe and Celei menu received some shortcut info text
- new feature: PokeMe has a new Alt-D alternative randomizer (adding only 1,2,3,4) to assist drum patterns
- bug fix: PokeMe extended mode sends to the relevant channel (i.e. 4 goes to channel 4 now)

v2.1.24 skipped changes

v2.1.23 changes 2024-04-06 (Zsolt Palotai release)
- bug fix: Rotatrig startup cleaned up
- bug fix: Channel Selector 2 display labels reviewed
- bug fix: Channel Selector 2 numeric shortcuts for top slider revised
- bug fix: Quant12 note calculation rewritten
- new feature: RouteSeq selection for a range in context menu
- new feature: SaveMeMono creates alternate pattern based on the number of steps set
- bug fix: RouteSeq timing issue
- new module: HighLow utility to switch between two pre-defined voltages
- new module: SimpleLFO with bleeding waves
- new feature: PokeMe context menu allows changing the 2, 3, 4 modes (set probability)
- new feature: PokeMe context menu allows changing the 7, 8, 9 modes (not to hit Nth time)
- new feature: PokeMe context menu allows syncing to the left (mother) module 
- new labels: Rubber mode 16 input labels are clarified
- new feature: Rubber mode 7, 8, 10 allows decreasing precision (see probability) between 6V and 7V
- new feature: Rubber got a new, a layered drum pattern (two-handed-drummer) mode and patch breaking rearrangement of older modes
- new feature: Rubber got a new, Bernoulli gate mode with multiple merged inputs

v2.1.22 changes 2023-10-13 (György Konrád release)
- new ranges added to MKnobs
- randomization of precision knob in Euclik is disabled
- programmer function (Euclidean) added to PokeMe reset input (see: polycable 0/len/hit/shift)
- new feature: clock sharing in RouteSeq 
- new feature: RouteSeq context menu fot an octave transposition option 
- new label: Slide16 received some input socket labels
- new module: Organic sequencer with crazy probability melodies
- new module: Rubber a collection of utilities I usually use (incomplete, there are placeholders for future functions)
- temporary bug fix: PokeMe extended mode sent strange voltage for grey (0) cells (this needs revisit)
- new feature: PokeMe got a new output mode ('values only') beside the 'triggers' and 'extended' mode; I needed it way to often to hide it into the 11th channel of the extended mode
- new feature: PokeMe context menu allows changing the 7, 8, 9 modes (hit only Nth time)
- bug fix: PokeMe reset behavior changed to play the first position
- new labels: EuClk has output labels for convience
- new feature: last voltages are reloading in Randvolt8

v2.1.21 changes (unreleased)
- new feature in PokeMe: probability weights are changed in step mode 2-4 from 95%-85%-75%-60% to 95%-80%-50% 
- new feature in PokeMe: step mode 5 is changed a random PW trigger (skip it, hit it with clock PW, hit it with 100% PW) to assist easier bass line creation
- some new labels to SaveMeMore 
- new feature: SeqP16 returns knob values if no polyphonic input is connected
- new module: RouteSeq switching between sequences

v2.1.20 changes 2023-02-18 (Libényi release)
- bug fix in Vulcan: row reference in tooltip text modified
- new hotkey: Vulcan focus switch can be toggled by 'F'
- new feature to Vulcan: option for extended outputs
- new layout to Vulcan: minor position fix of the grid
- new layout to Sequin: gained some HPs
- new layout to ChanSel6: lost some HPs
- new module PokeMe: another take on a 32-step trigger sequencer

v2.1.19 changes 2023-01-18
- new module NearMix: an almost passive mixer with 9-ins & 2-outs
- bug fix in Vulcan: rarity feature was messed up
- bug fix in Vulcan: pulse width calculation revised
- new layout to Ticklet: some labels added
- new feature in SaveMeMonoWide: randomization in the context menu
- new feature in Vulcan, SaveMeMono and SaveMeMonoWide: export to text file (to assist Voxglitch One Point/Zero modules)
- new feature in SaveMeMono and SaveMeMonoWide: import from (Voxglitch One Point/Zero) text files
- bug fix in SaveMeMore: the reverse output calculation revised
- bug fix in SaveMeMore: sync improved
- new feature in SaveMeMore: CV input to modify the START position
- new (half-baked) feature in Vulcan: tooltip boxes over the grid

v2.1.18 changes  2022-12-25
- new module added: Vulcan trigger sequencer
- new module added: Ticklet a simple clock divider

v2.1.17 changes 2022-12-25
- bugfix in SaveMeMore: DEV label removed and latch button layout unified
- bugfix in SaveMeMore: reverse output was messed upper
- bugfix in SaveMeMore: startup calculation is greatly improved
- bugfix in SaveMeMore: precision was enabled for the outputs
- new feature in SaveMeMore: rhythm can be delayed by a knob
- new output on Quant12 for the sampling trigger
- new layout to FromTo, TrigPrec1, TrigPRec2: gained some HPs
- new layout to MKnob: lost some HPs

v2.1.16 2022-12-01
- renamed EuclkExp to EuclkMore
- changed EuclkExp startup value for the mixed output and added NXOR
- new shortkey in PushMe and PushMeMore: Alt+number delays hits of a zone by a step
- feature added to PushMe and PushMeMore context menu: pulse width can be set to 100% or "as-clocked"
- feature added to PushMe and PushMeMore context menu: limit the precision settings to adding or skipping
- minor change in lights redrawing in PushMe and PushMeMore
- new features in ChanSel6: logic features, new mono output modes are added to the context menu (OR, NOR, solo XOR, solo NXOR, odd XOR, odd NXOR)
- new shortkey in ChanSel2: 1-9 keys change the upper channel selection slider (no shortkey for 10-16 channels though)
- new layout for SaveMeMono: it put on some HPs :)
- new module added: SaveMeMore an expander to add additional reading heads to SaveMeMono
- bugfix in Sequin to save the quantization mode
- 
v2.1.15 2022-10-30
- changed Sheep with a mild mutation settings on startup 
- changed Quant12 poly output to reflect transposition
- bug fix in Quant12 in the quantized transposition mode
- changed RotaTrig CV stepper behaviour to consider SLOT settings
- potential bug fix in RotaTrig to avoid unitialized inputVolt
- new module added: Euclk (basic Euclidean sequencer)
- new expander: EuclkExp an extension for adding outputs to Sheep module

v2.1.14 2022-09-29
- new expander module: SheepMore an extension for adding outputs to Sheep module
- bug fix with clock division of PushMeMore (random sequences & multi-trigger for others)
- new feature in PushmeSeq & PushmeMore (precision settings in context menu); I couldn't resist :)
- new feature in PushmeSeq, Celei, Celesta, Sheep (step calculation isn't affected by bypassing)
- minor bug fix in Celei for reset V levels
- new reversed output on FromTo
- bug fix in Sheep on the startup display

v2.1.13 2022-09-18
- new module added: Sheep (stepped voltage again)
- new expander module: PushMeMore an extension for adding tracks to PushMeSeq module
- adding first step indicator LED to SaveMeMono and SaveMeMonoWide to see the value of first step (I like drumpatterns with a first-hit-start)
- label fix in Quant12, and adding a trigger indicator LED
- clock indicator level set to 2.0V (instead of the previously coded 0.2V :> ) in Celei, Celesta, FromTo, PushMeSeq, Quant12, QuantET, RandVolt10, RandVolt8, RotaTrig, Sequin
- bug fix in RotaTrig CV step mode (0-10V)
- label fix and heavy code renovation in PushMe (Zone 6 labels corrected)
- minor enhancement of RandVolt8 (fixed voltages are from 0-7V and variable by the knob/switch)

v2.1.12 2022-08-20
- modifying descriptions and labels in QuantET, PushMe
- patch breaking change in QuantET MODE the order of options
- layout change of swapped sockets in QuantET
- new module added as RotaTrig (routing)
- new module added as FromTo (theMatrix stepper)
- new module added as Quant12 (quantizer)

v2.1.11 2022-08-10
- new module added as QuantET with variable equally distributed scales
- bug fix in Randvolt8 to show uni/bi switch values properly
- modifying description labels in SaveMeMono and SaveMeMonoWide

v2.1.10 2022-08-01
- new module added as RandVolt8 providing stepped/fixed voltages 
- new shortkey for Pushme to randomize all buttons ('R') including exclusive dice for the 3x16 mode and the 1x48 random mode
- bug fix in ChanSel2 to display channel number or CV value depending poly input availabilty
- new modules added as SaveMeMono (2HP) & SaveMeMonoWide (6HP) providing random voltages and 256 slots for saving a stepped voltage sequence
- significant enhancement to SeqP16 by adding the option to repeat certain steps

v2.1.9 2022-07-23
- contains a TRP1 bugfix
- contains a ChanSel 2 upgrade to snap the sliders if input is connected
- changed PushMe sequence modes (1x16+4x8 became 1x16+3x8+1x8 where the last track is random)
- new shortkeys for Pushme to record hits and clear zones
- new gate syncing feature in the context menu of Enveloops
- basic quantization is added to the context menu of Celei and Celesta

v2.1.8 2022-07-14
- contains NearMess mixer and Sequin sequencer

V2.1.7 2022-07-01
- contains Celesta and Celei sequencers
