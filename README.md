# TA-Regulator

## Intro

 A DIY Arduino consumption regulator build to use excess solar power for charging
 a storage heater – "thermal battery" or Thermal Accumulator (TA) – as a means to
 enhance PV self-consumption ratio.
 
 It is based on the Regulator sketch as developed by Juraj Andrássy:
 [https://github.com/jandrassy/Regulator](https://github.com/jandrassy/Regulator).
 Check his version first – it provides many additional modules and functionalities
 that are not included in this version and is excellently set up, allowing for
 case-by-case modification.        
 
 This TA Regulator version has been tuned for "zero export" PV restriction,
 basically demanding a different mechanism to estheblish how much PV excess
 power exists, as any excess is by default suppressed by the export limiter and
 therefore not visible as surplus to the meter. To establish this untapped PV,
 a small reference PV panel placed next to the PV array is used. It tells how
 much PV (W) is potentially available from the sun through the "insolPower"
 value, derived from the shortcut current of the panel. This functionality is
 set up in the InSol module.
 
 ModBus data from the PV inverter and smartmeter are then used to see how much
 much PV is currently generated and consumed. The difference tells the untapped
 excess PV, which is then "regulated" as heating power for the storage heater.
 
 TA Regulator features MQTT, through which the heating power can be set
 independently ("manual") from the automatically regulated power. If power is
 set through MQTT, automatic regulation is disabled.
 
 A "night charging" function has been integrated through which a given amount
 of kWh can be set for over-night accumulatiuon in the heater, using cheap night
 tariff. It is enabled/disabled over a long press of the button. The level of
 night charge is determined with a weather prediction algorithm. With sunny
 skies predicted, the charge level is lower, with worsening or overcast skies
 the charge level is brought up. This is done through the ChargeForecast module.
 
 Functionality for an OLED display (128 x 64 or 128 x 128) is included, as well
 as a Led Matrix screen. Screens are switched off during night, but can get
 activated with a short pressing of the button.
 
 ## CPU
 
 The platform used is ARDUINO_SAMD_MKRZERO with MKR Connector Carrier and ETH
 Shield. It can be made compatible with other platforms through by including
 platform specific instructions available from the original Regulator sketch.
 
 ## Hardware
 

<img src="https://elektropionir.rs/wp-content/uploads/2022/10/TA-regulator-scaled.jpg" width=75% height=75%>
Components used (from left-up to right-down):

* (top row)
  - solid state relay (SSR) to bypass dimmer
  - dimmer module with extra heatsink (RobotDyn)
  - EMI noise filter on 230V line output
  - cable connector block
* (middle row)
  - MOSFET driver for SSR
  - I2C hub (Grove)
  - DIN-current sensor
  - LED-bar (Grove)
  - DIN-power supply
  - EMI noise filter on 230V line input
* (bottom row)
  - Arduino MKR Zero/ETH on MKR carrier
  - speaker (Grove)
  - OLED display and LED Matrix display
  - ADC for reference mini PV panel (Grove)
  - 15 OHM 5W 'shortcut' resistor for mini PV panel

 ## Notable hardware mods

 Hardware modifications made: a larger heatsink is connected to the RobotDyn
 triac module (to prevent overheating), a snubber is connected to the triac
 module (to prevent blowing-out the triac), and EMI line filters are put on
 the incoming and outgoing power line (to prevent distortion on the AC line
 from the dimmer regulation).
 
 ## Revisions

 2023.11.22 – sketch updated with minor bugfixes and with logging functionality of local PV Insolation
