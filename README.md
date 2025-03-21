# Yolow

Missing a few simple module from my VCV Rack patches I decided to code a bit. But I would also like to share them with you. If you successfully use any of them in your project or you like it please let me know. Your advices and [support](https://paypal.me/salix0210?country.x=HU&locale.x=hu_HU) would be also appreciated.  

<img width="720" alt="image" src="https://user-images.githubusercontent.com/34127628/205014176-79ed0271-2c23-4ab3-877c-225fb91d3d13.png">

|Module|Short desc.|
| --- | --- |
|[ChSel1](#ChSel1)|Select your favourite channel from a polyphonic input|
|[ChSel2](#ChSel2)|Select upto 2 of your favourite channels from a polyphonic input|
|[ChSel6](#ChSel6)|Select upto 6 of your favourite channels from a polyphonic input|
|[ChSel8](#ChSel8)|Select upto 8 of your favourite channels from a polyphonic input|
|[PolyCopy10](#PolyCopy10)|Polyphonic merge with normalized input sockets in 2HP|
|[PolyCopy16](#PolyCopy16)|Polyphonic merge with normalized input sockets in 4HP|
|[PolyExp8](#PolyExp8)|Polyphonic split in 2HP|
|[AbacSeq](#AbacSeq)|Variable trigger sequencer with randomization and pattern chaining|
|[Celesta](#Celesta)|Three 8 step CV sequencers|
|[Celei](#Celesta)|Single 8 step CV sequencer|
|[Rubber](#Rubber):heart:|Barebone looking sequencer and more|
|[PokeMe](#PokeMe):heart:|Simple single track trigger sequencer with upto 32 steps|
|[PushMeSeq](#PushMeSeq)|Variable trigger sequencer|
|[PushMeMore](#PushMeMore)|Expander module with additional tracks for PushMeSeq|
|[Vulcan](#Vulcan)|Simple 8-track trigger sequencer with 16-steps|
|[Euclk](#Euclk)|Basic Euclidean sequencer|
|[EuclkMore](#EuclkMore)|Expander module with additional tracks for Euclk|
|[SeqP5](#SeqP5)|4 step sequential switch to choose from 5 inputs|
|[RouteSeq](#RouteSeq)|An 8 step sequential switch with option to a 128 steps pattern|
|[MKnob](#MKnob)|Simple 4 channel fix voltage provider (4xmono & 1xpoly)|
|[RandVolt8](#RandVolt8)|Provider of fix (7V, 6V, 5V, etc) or random stepped voltage with range settings|
|[FromTo](#FromTo)|Provides stepped voltage in a range|
|[RotaTrig](#RotaTrig)|Distributes incoming clock in various ways|
|[RotaTrig](#Ticklet)|Clock modifier module with a polyphonic 8-channel output of divided clock pulses|
|[SaveMeMono](#SaveMeMono)|S&H that saves upto 256 stepped voltage|
|[SaveMeMonoWide](#SaveMeMonoWide)|S&H that saves upto 256 stepped voltage|
|[SaveMeMore](#SaveMeMore)|Expander module with additional tracks for SaveMeMono|
|[Sheep](#Sheep)|Provides random stepped voltages or gates with a lot of flexibility|
|[SheepMore](#SheepMore)|Expander module with additional CV and gates outputs for Sheep|
|[Slide6](#Slide6)|Simple 6 channel fix voltage provider with adjustable range (1xpoly)|
|[Slide16](#Slide16)|A 16 channel fix voltage provider (1xpoly); with simple 16 step sequencer (1xmono)|
|[NearMess](#NearMess)|An almost passive VCA and mixer, 2-to-9|
|[NearMix](#NearMix):heart:|An almost passive VCA and mixer, 9-to-2|
|[TrigPrec1](#TrigPrec1)|Trigger sequence randomizer|
|[TrigPrec2](#TrigPrec2):heart:|Trigger sequence randomizer|
|[TrigPrec6](#TrigPrec6)|Trigger sequence randomizer|
|[Enveloop](#Enveloop)|Looping envelope generator|
|[QuantET](#QuantET)|Quantizer with variable equal distribution scales|
|[Quant12](#Quant12)|Quantizer with transpose and VCA|
|[tXOr](#tXOr)|Clock divider with XOR output and CV inputs|
|[9lives](#9lives)|Clock multiplier, clock randomizer, trigger sequencer, CV sequencer|
|[Sequin](#Sequin):zzz:|*Deprecated.* Queen of Sequencers|
|[SeqP16](#SeqP16):zzz:|*Deprecated.* 4 step sequential switch to choose from 16 channels of a polyphonic input|
|[tHex](#tHex):zzz:|*Deprecated.* A 16 step trigger sequencer with random options|
|[PolyExp10](#PolyExp10):zzz:|*Deprecated.* Polyphonic split in 2HP|
|[RandVolt10](#RandVolt10):zzz:|*Deprecated.* Provider of fix (10V, 9V, 8V, etc) or random stepped voltage|

Deprecated modules will be removed from the next major release.

-----

### ChSel1
Select your favourite channel from a polyphonic input (see PIN) with the push buttons.

### ChSel2
Select your favourite channel from a polyphonic input (see PIN). The upper section returns a single monophonic output as defined by the slider. The lower section works the same way but the input at the bottom allows you to modify the slider on the fly. This transforms the small 2HP module into a simple 16-step sequencer!

<img width="272" alt="image" src="https://user-images.githubusercontent.com/34127628/156900013-2fd81ae0-16ac-40fd-adf2-582d2e2ae03a.png">

If there's no PIN connected then fixed voltages are sent to the output. The upper slider reacts to pressing 1-9 numeric keys.

### ChSel6
Select upto 6 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 6 in the context menu. Similarly to the suggestion [here](https://github.com/dbgrande/GrandeModular/issues/4). But you can use the context menu to select from various alternative mono outputs (like OR, XOR, NXOR).

### ChSel8
Select upto 8 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 8 in the context menu. It's only 2HP!

If there's no PIN connected then fixed voltages are sent to the output. Unfortunately the knob labels don't properly reflect the behaviour. 

-----

### PolyCopy10
A simple merge module with ten normalized inputs. As suggested [here](https://github.com/dbgrande/GrandeModular/issues/3). 

### PolyCopy16
A simple merge module with sixteen normalized inputs.  

<img width="250" alt="image" src="https://user-images.githubusercontent.com/34127628/156899500-237c10e8-6976-4130-9836-cf5a6047b056.png">

### PolyExp8
A simple 2HP split module it returns either channel 1-8 or 9-16 a polyphonic input. 

-----

### AbacSeq

AbacSeq is a simple trigger sequencer. It is upto 32 steps and directs each clock signal to one of the outputs (Track 1, 2, or 3). There is a way to set up unique pattern chain. I.e. the image below shows that the pattern length is 4 steps and the pattern chain is AABC. In this case A pattern plays slot 1-4, B pattern plays slot 5-8, and C is for 9-12. As an additional feature you can set some randomization for each of the patterns. I.e. 30% on A means that the sequencer most likely will send the gate as programmed but there is a 30% chance that the signal appears on one of the other outputs.   

<img width="185" alt="image" src="https://github.com/user-attachments/assets/18f92d0d-b119-4098-8326-52de4568dc9a" />

### Celesta
Celesta is a unquantized note sequencer based on [Andre_M's idea](https://community.vcvrack.com/t/ideas-for-any-interested-developers/17697?u=firolfo) in the VCV Community forum. Practically it's a module with three 8 step sequencers (A, B, C). Pattern length is adjustable. However sequencer B advances to the next step only when A has finished. Similary sequencer C takes the next step only if B has completed its pattern.

As an additional feature is that every step has a "precision" switch. Setting it to 
- `0` "mr perfect": the note is always played as expected
- `1` “missed notes”: occasionally plays another note from the same sequence
- `2` “sticky fingers”: occasionally keeps playing the previous note

On the left you find a “precision” knob to influence how often these anomalies happen. Available keyboard shortcuts:
- `i`: alternate values for all note knobs
- `r`: random values for all note knobs and step lengths

### Celei
Celei is the smaller sister of Celesta. Basically it's a simple unquantized note sequencer upto 8 steps with OCT (octave) and RNG (range) settings. A unique feature that you can get a clock signal once a selected step is active. (Setting it to step #1 would return a common EOC signal.) An external transpose (TRANSP) input is also accepted. Special thanks to Andre_M for all the contribution!

Keyboard shortcuts are available:
- `1-8`: alternate value for the selected note knob
- `i`: alternate values for all note knobs
- `r`: random values for all note knobs and step lengths
- `u`: move knob values upwards
- `d`: move knob values downwards
- `c`/`v`: change between 'clock mode' and 'voltage mode'

<img width="500" alt="image" src="https://user-images.githubusercontent.com/34127628/177008993-9b384b4c-4ca0-4ae8-92a4-8ce2eed897db.png">

### Rubber (still in progress)

Rubber is collection of utility modules I tend to use regularly. It has 4 inputs, 4 outputs, a button and a mode selector.
- Mode #0 - A, B, C, D inputs are sent to SUM, AVG, MIN, MAX CV outputs
- Mode #1 - A, B, C, D inputs are sent to ABS, +5V, inverted, and SIN outputs
- Mode #2 - Quantized random CVs
- Mode #3 - Basic quantizer. Incoming voltages of input A-D will be returned on the output sockets as quantized.
- Mode #4 - Advanced quantizer for 1 input.
- Mode #5 - Merge 4 inputs into a polyphonic output.
- Mode #6 - Split a polyphonic input into 4 solo outputs.
- Mode #7 - Linear drum pattern. (Uses one output at a time.) Pressing the button generates a repeating random 4-channel drum pattern. 
Input A is reset, B is the clock, and C is allows changing the length of the pattern. (I.e. 1.6v = 16 steps, 2.5v = 25 steps.) 
Input D is tricky. 
  * >=8V forces a random output selection
  * 7.00V-7.99V uses the same pattern but with an alternative output selection
  * 6.00V-6.99V uses the same pattern with 100%-1% probability
  * 5.00V-5.99V mutes all outputs
  * 4.00V-4.99V forces all gates to ouput D
  * 3.00V-3.99V forces all gates to ouput C
  * 2.00V-2.99V forces all gates to ouput B
  * 1.00V-1.99V forces all gates to ouput A
  * 0.00V-0.99V returns the regular output pattern
  * <0.00V returns a specific, redialable pattern variation
- Mode #8 - Layered drum pattern. (Uses two outputs at a time.)
Input D offers options are similar to mode #7.
- Mode #9 - Phasor drums (+-5V CV to hits).
- Mode #10 - Looped random CV. Pressing the button generates a repeating random 4-channel pattern of random CVs. 
Input A is reset, B is the clock, and C is allows changing the length of the pattern. (I.e. 1.6v = 16 steps, 2.5v = 25 steps.) 
Input D offers options are similar to mode #7. The outputs provide the same pattern but in different ranges. (0v to 1v, -1v to 1v, 0v to 10v, -5v to 5v.)
- Mode #11 - Bernoulli random triggers. The weight of output A can be set in the menu or by CV.
- Mode #12 - Mono to poly. Rotates an incoming gate or CV among the 4 channels of the polyphonic output. 
- Mode #13-#16 - empty slots

### PokeMe

PokeMe is a 32-step single track trigger sequencer. There is a reset and clock input and a monophonic output. Although it is a fairly simple sequencer but there are a few additional tricks available. The context menu includes a `choke` function and selection between two `pulse width` modes.

By default triggers can be entered to the cell grid by mouse. Shift-Left-Click and Ctrl-Left-Click allows changing of the trigger modes. Various colors of the cells represent these different behaviors:

- `0` (grey) blank
- `1` (green) 100% probability hit
- `2` (yellow) 90% probability hit
- `3` (yellow) 80% probability hit
- `4` (yellow) 50% probability hit
- `5` (orange) gate or trigger or nothing (I think) 
- `6` (purple) trigger or double hit or nothing (I think) 
- `7` (blue) only every 2nd 
- `8` (blue) only every 3rd
- `9` (blue) only every 4th
- `10` (red) restart

Short keys from Ctrl-1 to Ctrl-9 can also be used to set the trigger mode of the next click. Ctrl-X is dedicated to entering a "restart" cell. Paramters for mode 2-4 and 7-9 can be set in the context menu. 

An additional feature is that placing multiple PokeMe modules next to each other the clock and reset signals are shared. I wouldn't group too many of them but a 3-4 track sequencer can be built easily.

<img width="271" alt="image" src="https://user-images.githubusercontent.com/34127628/214388300-552c0034-40c3-4233-9ab2-eb136b885f8c.png">

By default the module provides gates but using the context menu it is possible the get distinct voltages or a polyphonic `Extended output`. Additional channels of the polyphonic output include counter of the current step (0-31V), the step value as voltage (0-9V) and a trigger is also sent to the relevant channel (i.e. `5` will send a trigger to the 5th channel). 

<img width="252" alt="image" src="https://user-images.githubusercontent.com/34127628/215619594-f328d0e0-e4ca-455b-8580-07338b1f0b45.png">

### PushMeSeq
It is a variable trigger sequencer. It expects monophonic clock and reset input. Simple 'push and play' kind of sequencer with an additional knob on setting the mode. The available modes are:

- `0`: picks randomly from 48 steps (monophonic output)
- `1`: 1x48 steps (with monophonic output)
- `2`: 1x32 + 1x16 steps (with 2-channel polyphonic output)
- `3`: 3x16 steps (with 3-channel polyphonic output)
- `4`: 2x16 + 2x8 steps (with 4-channel polyphonic output)
- `5`: 1x16 + 3x8 + 1x8 steps (with 5-channel polyphonic output)
- `6`: 5x8 + 1x8 steps (with 6-channel polyphonic output)

In modes `5` and `6` the last sequence provides random gates. 

Keyboard shortcuts are also available:
- `i`: inverts all buttons values
- `1-6`: randomizes a certain zone
- `Shift-1` to `Shift-6`: clears a certain zone
- `Alt-1` to `Alt-6`: delays hits of a zone
- `r`: common dice function with unique algorhythm for sequence mode `0` and `3`
- `x`: records a hit to the first sequence
- `c`: records a hit to the second sequence (if available)
- `v`: records a hit to the third sequence (if available)
- `b`: records a hit to the fourth sequence (if available)
- `n`: records a hit to the fifth sequence (if available)
- `m`: records a hit to the sixth sequence (if available)
- `Alt-R`: special dice mode designed for the 1x48 random mode

![image](https://user-images.githubusercontent.com/34127628/177716736-c3e5015c-f4cc-47f2-a232-451239b25a2f.png)

### PushMeMore
An expander for PushMeSeq. It adds additional tracks, it can clone the sequence of the 'mother module' and divide its clock signal. If you feel that it's too perfect you can always check the context menu and reduce the precision. 

### Vulcan
Simple 16-steps trigger sequencer with 8-tracks. Advanced usage allows monophonic or 8 channel polyphonic clock and reset inputs. All tracks has a separated output and a knob for division of the main clock. Tracks have a switch to change behaviour between 'full (100%) length' gate or 'clock width' gate. 

<img width="643" alt="image" src="https://user-images.githubusercontent.com/34127628/208769332-5f8d3db4-22ef-4a8b-8e41-2fb22c7cd8ee.png">

Available trigger modes (also presented by the colorful squares):
- `0 Blank`
- `1 Trigger` always played
- `2 Probability A` probability value is applied to the trigger; i.e. 90% PROB A means that the trigger is mostly hit
- `3 Probability B` probability value is applied to the trigger; i.e. 50% PROB B means that about half of the triggers are missed
- `4 + Rarity A` trigger is played only the Nth time
- `5 - Rarity A` trigger is missed the Nth time
- `6 + Rarity B` trigger is played only the Nth time
- `7 - Rarity B` trigger is missed the Nth time
- `8 Rarity and Probabiltiy A` trigger is played only the Nth time with a probability condition
- `9 Rarity and Probabiltiy B` trigger is played only the Nth time with a probability condition
- `10 Restart` it resets the track to the first step

Mouse functions:
- `Left-Click` onto the grid changes the relevant step between blank (empty) and the selected kind of trigger
- `Shift-Left-Click` onto the grid changes the relevant trigger to the next mode (i.e. Probability A  will be changed to Probability B)
- `Ctrl-Left-Click` onto the grid changes the relevant trigger to the previous mode (i.e. Probability B  will be changed to Probability A)

Keyboard shortcuts are also available:
- `1` to `8` randomizes the relevant track with triggers (according to the denstiy and mode settings)
- `Shift-1` to `Shift-9` sets the random density settings from 10% to 90%
- `Ctrl-1` to `Ctrl-9` selects some of the mode settings (option `0` and `10` are ignored here)
- `f` to switch the 'focus' switch

There is a switch available for change between one-shot and looping sequencer mode. Another switch allows turning off the highlights for the current step.

There one more unique feature that you can enable in the context menu: 'extended output'. By default all outputs sends the trigger sequence only. But enabling this feature the ouput becomes polyphonic and 
- `channel 12` shows the number of step (e.g. 0V-15V represents step 1-16)
- `channel 11` shows the current value of the step (e.g. 0V-9V represenst the trigger modes)
- 'channel 1' to 'channel 9' sends a trigger is current step value is relevant (e.g. step value is 1 then channel 2 sends a trigger, step value is 2 then channel 3 sends a trigger)  

### Euclk
I found [Euclidean rhythms](https://en.wikipedia.org/wiki/Euclidean_rhythm) fascinating ever since my high school math teacher, Mr. Tamás Kötél introduced me to their musical importance. There are many fantastic implementation of this concept in VCV but I thought it's worth for me to give a try. 

Euclk is a single track sequencer with clock (CLK) and reset (RST) inputs. There is a knob for the pattern length (LEN), hits (HIT), shift (SHF) paramters. There is also a precision (PRC) settings available for instant randomization. The two outputs provide the calculated trigger sequence and its inverted variation.

If you need more complextiy I suggest checking out docB [N3](https://library.vcvrack.com/dbRackSequencer/N3) or the mighty Frozen Wasteland [Quad Algorithmic Rhythm Generator](https://library.vcvrack.com/FrozenWasteland/QuadAlgorithmicRhythm).

### EuclkMore
An expander for Euclk. It uses the clock and reset signal of Euclk and adds an additional track. With the LEN and HIT CV inputs (0-10V) you can modify parameters on the fly. There is one more big knob added to select the behaviour of the MXD output. It can be the inverted or a random sequence but it also allows you to choose a logical combination with mother module.  

### SeqP5
A tricky 4-step sequencer that returns one of the 5 input sources according to the knob selections. To advance the step you need a clock (see CLK). Red LEDs indicate the active step, yellow LEDs indicate the currently selected input. Setting any of the knobs to zero will randomly forward one of the input sources. <-- See a VCV selection demo here: [demo/Demo_SeqP5.vcvs](demo/Demo_SeqP5.vcvs).-->

### RouteSeq 
It is a versatile sequencer that can act as a switch. The right column of knobs allows you to set the time spent at a ccertain step. Hence you can create an 8-steps pattern or a 128-steps pattern, or anything in between. If a polyphonic input is provided then the left knobs allow you to select one of the channels and the module will return that. Without providing a polyphonic input the left column of knobs allow you to dial specific CVs. (See the context menu for alternative ranges.) 

But you better give it a try instead of waiting for a proper operational manual. See a VCV selection demo here: [demo/Demo_RouteSeq_with_BogAudio.vcvs](demo/Demo_RouteSeq_with_BogAudio.vcvs).

One more thing: if you put multiple RouteSeq modules next to each other then they share the clock of the very left one! 

-----

### MKnob
A simple fix voltage provider module. The context menu offers
- Quantize: `nope` / `octave` / `notes`
- Range: `0V to 10V`, `-10V to 10V`, `0V to 1V`, or `-5V to 5V`
- Poly channels: limiting polyphonic output from 1 to 4 channels if needed

If no poly output or quantization is needed then go for [VCV 8vert](https://library.vcvrack.com/Fundamental/8vert) instead. It's the standard module to provide fix voltages.  
 
### RandVolt8
Similar to RandVolt10 but with 8 outputs only. It has no context menu but some range options (lower and upper limit voltage) are on the panel. 
- Without a clock input: it provides eight fix voltages between in the valid range.
- With a clock input: it provides random voltages. Practically a S&H unit with internal noise source.

See a VCV selection demo here: [demo/Demo_RandVolt8.vcvs](demo/Demo_RandVolt8.vcvs).

### FromTo
This module provides stepped voltage in a specific range (FROM and TO) according the defined division (DIV). (E.g. if FROM=1V, TO=5V, DIV=4 then the OUT will provide 1V, 2V, 3V, and 4V.) The module was specificially created to assist me playing with docB's [TheMatrix](https://library.vcvrack.com/dbRackSequencer/TheMatrix). See a VCV selection demo here: [demo/Demo_FromTo.vcvs](demo/Demo_FromTo.vcvs).

The OUT is monophonic by default but a switch allows you to have multiple copies of the same CV on multiple channels of a polyphonic OUT. It can be useful sometimes: 

<img width="448" alt="image" src="https://user-images.githubusercontent.com/34127628/190145507-bd9f99be-d265-48eb-89b6-03a6de239660.png">

### RotaTrig
This module sends the incoming clock randomly, or sequentially to the outputs. The number of outputs can be set between 1 and 6 (see SLOTS). Note that priority is given to the output that has received an enabling (ENBL) trigger.

Feeding a looping LFO (0-10V) to the CLK socket you can also send the voltages semi-randomly to the outputs. Like this:

<img width="714" alt="image" src="https://user-images.githubusercontent.com/34127628/187549668-e54ad8a6-10b0-48e2-a6de-fd032893bbd9.png">

### Ticklet
A simple clock division module with the option of 8 monopohonic clock output and an 8-channel polyphonic clock output.

### SaveMeMono
Tricky little module to save and return upto 256 voltages. Source input can be external (see LFO) or internal noise. If internal noise is selected then voltages can be set to be gates (0V or 10V), bi (-5V to 5V), or uni (0v to 10V). The recorded values can be shifted to left (i.e. previous) or right (i.e. next) slot. There's a tiny yellow LED indicating the voltage of the first step. The output is provided 3 ways:
- `normal` order
- `reverse` order
- `random` order. 

An example to record a simple melody using your keyboard:  
<img width="297" alt="image" src="https://user-images.githubusercontent.com/34127628/209512719-32ceb994-969a-43dd-8f37-c2c3c4ac7b7e.png">

If you like the concept but are not fan of this module then check out [PdArray Array](https://library.vcvrack.com/PdArray/Array).

### SaveMeMonoWide
Similar to SaveMeMono but all options are available from the panel. I love it. 

### SaveMeMore
An expander module for SaveMeMono. Not compatible with SaveMeMonoWide (yet)! It adds an additional read-head to the 256 element CV sequence stored in SaveMeMono and uses its clock.

### Sheep
A module to provide stepped voltage. A mutating 32-steps map is read and sent to the output. The map is a sequence of 0-1V values amplified by the range knob on the top. Appart from the common clock (CLK) and reset (RST) inputs there are also knobs available:
- RND knob changes/mutates the values of the map
- MUT switch defines if the mutation is 'permanent' or 'temporary'
- INC knob changes the distance of steps
- DIV knob allows clock division
- STR knob defines the first step for the reading head
- END knob defines the last step of the reading

There's also a pretty rich context menu
- Restart mode: `always` jumps back to the very first step but `on reset` allows more creative use of the INC knob 
- Voltage mode: allows CV output or gates (see also hotkey `g`)
- Number of output channels: allows to create a polyphonic output with differently mutated variations of the same signal
- Link start and end knobs: if `linked` the number of steps remain unchanged by turning one of the knobs

Keyboard shortcuts are available:
- `1-4`: randomizes a column in the 'CV map'
- `p`: randomly changes the color of the 'CV map' 
- `r`: randomizes all values for the 'CV map' (0-1V)
- `g`: allows CV output or gates (based on comparison to 0.5V, 0.75V, 0.9V)

### SheepMore
An expander module to add more outputs to Sheep. They share the same 'green field' (see 'map of CVs'). Additional available sockets:
- CV pure (0-1v CV provided by the reading head of Sheep)
- CV inverted (0-1V inverted CV of the previous socket)
- CV amplified (CV of the 1st socket but amplified by 'Voltage range' value of Sheep)
- CV inverted amplified
- CV modified (the mutated CV, unique for each expander modules)
- CV inverted modified
- GATE pure (knob of the expander defines the level compared the current CV to)
- GATE inverted (inverted of the previous socket)
- GATE modified (the mutated gates, unique for each expander modules)
- GATE inverted modified

<img width="535" alt="image" src="https://user-images.githubusercontent.com/34127628/192959193-db908873-ac8b-4a56-b46c-a7fda5d75c50.png">

### Slide6
A module to provide a six channel polyphonic output with the fixed voltages set by the six sliders. The two knobs (see MIN & MAX) on the top defines the range of voltages. The context menu offers
- Quantize: `nope` / `octave` / `notes`
- Poly channels: limiting polyphonic output from 1 to 6 channels if needed

### Slide16
A module to provide a sixteen channel polyphonic output with the fixed voltages set by the six sliders. The two knobs (see MIN & MAX) on the top defines the range of voltages. 

The module can also be used as a 16-step sequencer and return the melody as a monophonic CV (see OUT at the bottom). To step from slider to slider you can use a clock (see STEP input) or alternatively you can use a 0V-10V CV (see JUMP). 

Some keyboard shortcuts are enabled and the context menu offers
- Quantize: `nope` / `octave` / `notes`
- Poly channels: limiting polyphonic output from 1 to 16 channels if needed

The 'poly channels' setting doesn't influence the sequence created by the JUMP input. But it does influence the number of steps available with the STEP input.

### NearMess
It is a very simple mixer that will combine two monophonic inputs (A and B) according to fixed ratios. This module is a tribute to my Takab [Nearness](https://www.modulargrid.net/e/takaab-nearness) Eurorack module that I dearly loved for it's simplicity and practicality.  

<img width="363" alt="image" src="https://user-images.githubusercontent.com/34127628/178104155-491bdf4e-58f1-40fb-af54-ab3bfbafdba2.png">

### NearMix
The little sister of NearMess. But this module has nine inputs and two outputs. The inputs are weighted so changing the socket for the cable the signal in the mix can be more or less dominant. No mixer can be simpler than this.

-----

### TrigPrec1
It's a simple trigger precision ruiner. Probability can be set by a knob or CV input. Probability allows to skip triggers from the input sequence (see IN socket) and/or to add a few triggers from the clock (see CLK socket on the top). I also would like to draw your attention to the similarly useful [HetrickCV Probability](https://library.vcvrack.com/HetrickCV/Probability) module!

### TrigPrec2
It's a dual trigger precision ruiner. Each knob define the probability to skip triggers from the input sequence (see socket under the knob) and/or to add a few triggers from the clock (see CLK on the top). The module follows the concept discussed [here](https://github.com/clone45/voxglitch/issues/74). 

### TrigPrec6
It's the bigger brother of TrigPrec2 with six inputs and outputs. Each knob defines the probability to skip triggers from the input sequence (see socket left from the knob) and/or to add a few triggers from the clock (see CLK on the top). 

-----

### Enveloop
This is an ADR (attack, decay, release) envelope generator with built in VCA. Separate knobs are available for A, D and R settings. The range can be set by a switch on the top, and the starting voltage for A is also definable. At the bottom there is a gate, a noise and an audio input allowing modification of the controll voltages. 

![image](https://user-images.githubusercontent.com/34127628/180173786-a125ed24-0193-4825-aabd-8e79c0603454.png)

The context menu of the module provides two gate modes:
- single cycle: one gate, one wave
- or syncing: one gate restarts the waves

The module was inspired by the mighty [Arido](https://solcitomusica.blogspot.com/p/virtuales.html) VST of Solcitomusic. It was also motivating that I found only a few envelope generators with a built in VCA in the VCV Library. 

<img width="298" alt="image" src="https://user-images.githubusercontent.com/34127628/173998298-82dfa826-579d-4bc5-b02e-202e10c019b4.png">

### QuantET

Quantizer with variable equal distribution scales. Following the comments of [this](https://community.vcvrack.com/t/whats-your-favorite-musical-scale/17970/40) VCV Community Forum and reading a bit about the history of the [12 equal temperament](https://en.wikipedia.org/wiki/12_equal_temperament) I decided to create a barebone quantizer that allows variable [ET](https://en.wikipedia.org/wiki/Equal_temperament). (Including the famous [19ET](https://en.wikipedia.org/wiki/19_equal_temperament), [53ET](https://en.wikipedia.org/wiki/53_equal_temperament), and others.)

The module is less of a faithful recreation of historical scales than a quick experiment of various divisions. It includes a VCA also for the noise input and some other common features I usually use. There are another [great ET/EDO implementations](https://library.vcvrack.com/?query=equal+division&brand=&tag=&license=) in the Library.

### Quant12

Quantizer with some commonly used features like trigger and transpose. This module replicates the functions of a bunch of module that I usually pair with my quantizers. This module has become one of my most used modules. I really like it. Some features:

- TRIG knob changes the probability of catching the trigger input (i.e. setting to 50% means that about half of the incoming triggers are ignored)
- NOISE knob is an attenuator of the noise input
- TRANSP knob is an attenuator of the same input

 If no input is provided for
 - TRIG then it simply continously calculates the quantized voltage
 - NOISE then it automatically uses an internal noise in the 0V to 1V range
 - TRANSP then it is ignored

Please note that depending on the MODE switch the TRANSP input signal can be completely ignored, or added to the quantized CV (i.e. dynamically detune), or quantized together with the NOISE input (i.e. transpose the melody).

As of the outputs 
- left top: provides the enabled notes in a polyphonic output; note that with proper settings the TRANSP also transposes these values (read: a chord is provided)
- right top: provides a single note as calculated from NOISE (and possibly TRANSP)
- bottom middle: provides a trigger every time the TRIG input signal is used  

### tXOr
This is a clock modification module. Each knob sets the division of the clock signal (see CLK). The output is the XOR (exclusive OR) signal of the modified clocks. The concept is similar to JWM's [1Pattern](https://library.vcvrack.com/JW-Modules/1Pattern) but the tXOr inputs allow dynamic modification of the knobs. 

### 9lives
First of all you obviously need a clock (CLK). The module can be used as 

- a buffer: CLK signal is sent unchanged to THR (through).
- a simple clock modifier: CLK input is multiplied by the MUL knob and sent to the relevant output (see MUL at the bottom).
- a fix voltage provider: CVY returns the CV of yellow slider of the currently selected block (similarly CVW is the white slider, CVG is the green slider)
- a complex clock modifier: LIFE returns the clock signal multipled by the yellow slider value. Please note that the white slider acts as a probability setting and may force the clock to be quiet. The green slider serves as a multiplcation probability by forcing a single pulse or a multiplied rhythm.
- a super complex clock modifier: a 0V-10V CV (see JUMP) allows you to dynamically select one of the blocks. Hence the yellow, white, green parameters are changing accordingly.
- a 16-step sequencer: a 0V-10V CV (see JUMP) allows you to dynamically select one of the blocks. Hence the CVY, CVW, CVG outputs will provide a changing stepped CV.

The active block is also indicated by the yellow LEDs on the top. Some keyboard shortcuts are enabled to select blocks and randomize parameters.

The concept of the module is based on Sha#Bang! Modules [Stochastic Sequencer Grid](https://library.vcvrack.com/Sha-Bang-Modules/StochSeqGrid) but the implementation is greatly simplified (clock, layout, operation).

-----

### Sequin
This module is *deprecated*:zzz:. I just don't find it useful any longer. Rubber is much better as general sequencer. Queen of Sequencers: slim, smooth, beautiful but noone really understands her. It delivers a CV sequence according to a clock but the pulse width (PW), notes can be modified by external LFOs. You can also enable or disable any clock pulse by an external CV (ENBL) or set the probability (PRB) for instant excitement. A basic quantization function is available in the context menu.

<img width="736" alt="image" src="https://user-images.githubusercontent.com/34127628/178427481-bc32b7a7-b5de-41bc-845c-4e7e6c22b7fb.png">

<img width="809" alt="image" src="https://user-images.githubusercontent.com/34127628/178427526-c4b0a55a-09ae-4452-90e1-8baa10a67ff5.png">

<img width="809" alt="image" src="https://user-images.githubusercontent.com/34127628/178427580-93929a80-1dbd-4be7-b3ec-12e1b89990e7.png">

<img width="809" alt="image" src="https://user-images.githubusercontent.com/34127628/178428799-2d8b313c-cc95-4329-a937-dd83713b903d.png">

<img width="808" alt="image" src="https://user-images.githubusercontent.com/34127628/178428839-246ea34c-fb59-4874-b48f-7c7f58ee6bc2.png">

### SeqP16
This module is *deprecated*:zzz:. RouteSeq replaces it with more steps and features. Anyway SeqP16 was a tricky 4-step sequencer that returns one of the 16 channels of a polyphonic input (POLY IN). To advance the step a clock (see CLK) is needed. Red LEDs indicate the active step. Setting any of the knobs to zero will randomly select one of the input channels. (Recommended reading: [musical dice game](https://en.wikipedia.org/wiki/Musikalisches_Würfelspiel).)

### tHex 
This module is *deprecated*:zzz:. [Nibble Sequencer](https://library.vcvrack.com/CountModula/NibbleTriggerSequencer) of Count Modula is built on the [same concept](https://github.com/countmodula/VCVRackPlugins/issues/89) but provides better features.

<img width="85" alt="image" src="https://user-images.githubusercontent.com/34127628/156899830-e201945d-faca-43bf-bc53-e8606f2f13d3.png">

### PolyExp10
This module is *deprecated*:zzz:. It is replaced by PolyExp8. A simple 2HP split module it returns the first ten channels of a polyphonic input. 
Optionally the returned channels can be set to `first 10` or `last 10` in the context menu.

### RandVolt10
This module is *deprecated*:zzz:. It is replaced by RandVolt8.
- Without a clock input: it provides ten fix voltages (1V-10V).
- With a clock input: it provides random voltages. Practically a S&H unit with internal noise source.

Optionally the range can be set to `0V to 10V` or `-5V to 5V` in the context menu.

<img width="225" alt="image" src="https://user-images.githubusercontent.com/34127628/156899586-15b8dd43-4d7e-4e67-98d1-7b67e0b63bbd.png">

