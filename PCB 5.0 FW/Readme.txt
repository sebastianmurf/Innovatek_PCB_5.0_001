PCB 2.0
T5.5 over T5.4 Dated: 12/19/25
1. AEK-3000 Power Supply recognition logic to run on lower AC voltage without alarm.
2. Manual pump functionality.
3. Manual power supply functionality.
4. Current setpoints added for pre-pure, pure, waste from setpoints branch.
4. Post-pure-shunt and pre-pure-shunt steps added to process operation.


T5.4 over T5.3 Dated: 05/06/2025 . 
1. DigitalOutput &= ~(1<<OUTPUT_SHUNT);	//OUTPUT_SHUNT On added on waste step

T5.3 over T5.2 Dated: 20/05/2025 . T5.1 creating restart whenever usb is connected so T5.2 created with step by step testing when this happens.
1. SwitchPAEvoltPure variable added.
2. Command to set SwitchPAEvoltPure added at 410.41. The value passed will be accomodated to one byte ranging 0.00 to 2.55
3. when system in pure, one more OR condition added in switching condition irrespective of FirstWasteCycle true or false:
    during waste run for at least 5sec and if PAE volt after 5 sec is greater than this user setpoint SwitchPAEvoltWaste then step switching will happen.
4. SWITCH_PAE_VOLT_PUREa memory location added 
5. Factory default SwitchPAEvoltPure & SwitchPAEvoltWaste set as 2.3V
6. all reporting section (command 350) deleted to accomodate additional variable & code.

T5.2 over T5.0 Dated: 29/03/2025 . T5.1 creating restart whenever usb is connected so T5.2 created with step by step testing when this happens.
1. PAE_PrepurifyStepVoltageSetpoint variable added. 
2. OptimizedPAE_CurrentForPrepureStep variable added.
3. PAE_PrepurifyStepVoltageSetpoint command added at 410.39.
4. Command to set SwitchPAEvoltWaste moved from at 410.35 to 410.38 as 410.35 & 410.36 slot is reserved for CIP_FAVORABLE_TIME.
5. During PrePurify step, PAE voltage is set as per PAE_PrepurifyStepVoltageSetpoint and current is set as per OptimizedPAE_CurrentForPrepureStep variable value.
6. PAE_PrepurifyStepVoltageSetpoint values that can be set is min = 0, max = 12 and default is 1.5
7. PAE_PurifyStepVoltageSetpoint default value changed from 1.9 to 2.5
8. PAE_WasteStepVoltageSetpoint default value changed from 1.9 to 2.5
9. OptimizedPAE_CurrentForPrepureStep command added at 410.40
10. variable WastePreShuntTimeInSec renamed as ShuntPostPureStepTimeInSec. Command to change is 412.0
11. variable WastePostShuntTimeInSec renamed as ShuntPostWasteStepTimeInSec. Command to change is 412.1
12. SHUNT_POST_PURE step added and system goes into this step after PURIFY step is over.
13. SHUNT_POST_WASTE step added and system goes into this step after HF_Waste step is over.
14. Code having WastePreShuntTimeInSec and WastePostShuntTimeInSec removed as no longer valid.

T5.0 over T4.9 Dated: 25/03/2025 . Flow chart confusing. Don't go according to flow chart. Change requirement is to switch from waste whenever PAE voltage during waste gets higher than user set value.
1. One byte SwitchPAEvoltWaste variable added.
2. Command to set SwitchPAEvoltWaste added at 410.35. The value passed will be accomodated to one byte ranging 0.00 to 2.55
3. when system in waste, one more OR condition added in switching condition irrespective of FirstWasteCycle true or false:
    during waste run for at least 5sec and if PAE volt after 5 sec is greater than this user setpoint SwitchPAEvoltWaste then step switching will happen.
4. Post waste next cycle waste current always start with WASTE_MAX_CURRENT: 
    OptimizedPAE_CurrentForWasteStep = WASTE_MAX_CURRENT;

T4.9 over T4.8 Dated: 06/02/2025 
1. BYPASS Logic removed from DiUse as Relay 2 is not Bypass valve in DiUse, its HFIV. In fact no Bypass in DiUse.

T4.8 over T4.7 Dated: 05/02/2025 
1. Instead of RawWaterTemperature, AtlasTempeature variable value is used to display in LCD.

T4.7 over T4.6 Dated: 04/02/2025 
1. PAE_PURE_STEP_VOLT_DEF_VAL changed from 17 to 19 i.e 1.7V to 1.9V
2. test code added to show the temperature calculation on dockligt. Real Temperature = Raw Temperature + offset

T4.6 over T4.5 Dated: 04/02/2025 
1. code from pressure sensor copied to Digital input for bypass and standby/run.

T4.5 over T4.4 Dated: 03/02/2025 
1. Start Timer of 5 sec was not turned on so the start was not happening. BUG FIXED.

T4.4 over T4.3 Dated: 02/02/2025 Guillermo change request
MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL changed from 500 to 0

T4.3 OVER T4.2
1.b-> if IP_START_SWT is high for more than 5 sec and the current process is in STAND_BY then goto PREPURIFY STEP. if IP_START_SWT is low for more than 5 sec then goto STAND_BY step.

T4.2 over T4.1 Sebastian change request
1.	Pressure Sensor Logic:
o	When the pressure reads -2.50 bar, the input states will start determining the start/stop process and bypass valve operation, rather than relying on the pressure sensor at that point.
2.	DI2 (BYP):
o	Functions completely independently and does not interfere with the normal process.
o	When the signal is high, activate the bypass valve relay.
o	When the signal is low, deactivate the bypass relay.
3.	DI4 (SIG):
o	Becomes the sole relay for the start/stop process.
o	When high, start the machine.
o	When low, stop the machine.
Please note that previously, DI1 was involved in the start/stop of the unit. This functionality must now be handled exclusively by DI4 for the start/stop process, and DI2 will control the bypass valve. There should be no interdependency between the relays moving forward.
Changes Made
1. if no pressure sensor installed
    a. if IP_BYPASS_SIG is high for more than WaitTimeB4BypassAlarmInSec, then alarm occurs and bypass relay operates.
    b. if IP_START_SWT is high for more than 5 sec and the current process is in IDLE then goto PREPURIFY STEP. if IP_START_SWT is low for more than 5 sec then goto IDLE step.
    
T3.9 over T3.8 Sebastian changes via video dated: 05/01/2025
ALL systems:
1. RawWaterTemperature is kept raw. Offset is only added at the time of showing or saving.

T3.8 over T3.7 Guillermo changes via mail dated: 20/11/2024
ALL systems:
1.	Start pressure default setpoint: change to 3.5 DONE
2.	Stop pressure default setpoint: change to 4.2 DONE
3.	Bypass pressure default setpoint: change to 3 DONE
4.	Max pure voltage default setpoint: change to 1.7    DONE
5.	Max waste voltage default setpoint: change to 1.7   DONE

F1(DIENTRY):
1.	F1(dientry) low flow waste alarm default setpoint: change to 1.0 DONE
2.	F1 (dientry) low flow pure alarm default setpoint: change to 3.0 DONE

MINI(DIUSE):
1.	Mini (DiUse) low flow waste alarm default setpoint: change to 0.5 DONE
2.	Mini (DiUse) low flow pure alarm default setpoint: change to 1.0  DONE

T3.7 over T3.6 Guillermo changes via mail dated: 14/11/2024
1. Remove the delay when opening the pure valve. This delay occurs every time you open the pure valve whether the system is in operation or manual mode. FOUND DELAY OF 1 SEC DURING POLARITY SWITCH RELAY 7 IN DIENTRY BUT NOT ON PURE VALVE OPRATION. DELAY REMOVED.
2. Set the default cycle count to 50 for the Zero EC alarm i.e SP_EC_ZERO_CYCLEa factory default setpoint from 5 to 50. DONE. 
3. Set the default internet connection mode to Auto-Switch mode. DEFAULT WAS ALREADY IN AUTO-SWITCH.
4. Set the default flow k factor to 1400. DONE CHANGED FROM 700 TO 1400
5.	Do not POWER CYCLE the system if there is a PAE error. Instead, show the alarm and continue normal operation. DONE

T3.6 over T3.5 Dated: 04.10.2024 bug fix
1. Auto "INSTANT FLOW" display value dependent upon unit set by user but Manual "FLOW" was in liter per min/100.
    Manual was also made like auto. FIXED.

T3.5 over T3.4 Dated: 29.09.2024 by Guillermo
1. For DiUse only, HF_WasteShuntTimeInSec configurable variable added.
    a. Configurable memory allocated in memory address 414, one byte allocated. Value can range from 0-255
    b. Command added at 412.2
    c. Factory default set to zero


T3.4 over T3.3 Dated: 28.09.2024
1. For DiUse only, WastePostShuntTimeInSec factory default = 0

T3.3 over T3.2 Dated: 25.09.2024
1. All PCB4.0 was creating issue during ethernet and wifi autoswitch as disabling serial port  before switching was missing in MOBILE_STAGE_SWITCH_TO_ETHERNET stage. Below code added to fix
    GPRS_UART->SCR &= ~0x10;//DISABLE SERIAL receive interrupt in order to prevent controller being hang due to serial Rx pin gets low and controller gets stuck in serial interrupt
            
T3.2 over T3.1 Dated: 25.09.2024
1. Due to T2.6 change, there was a bug When system goes to IDLE due to Zero EC alarm, in IDLE MIV, SHUNT relay remained ON. BUG FIXED.

T3.1 over T3.0 Dated: 23.09.2024
1. Relay 6 controls one of the coils on the contactor. We do not need to turn in that relay during HF_WASTE step for dientry.


T3.0 over T2.9 Dated: 23.09.2024
1. WastePreShuntTimeInSec and WastePostShuntTimeInSec variable introduced for changes made in T2.9->I1 configurable
2. WastePreShuntTimeInSec and WastePostShuntTimeInSec variable is a setpoint with default value 5 but not configurable via LCD nor report to server at the beginning of communication with server,
    can only be set via docklight or from server read/write command which is added at 412.0 for pre and 412.1 for post.

T2.9 over T2.8 Dated: 22.09.2024
I. Changes asked by Guillermo: Whatsapp 22.09.2024
    1. Disable relays #6 and #7 during the first and last 5 seconds of waste. Also disable the pae power supply. CHANGES DONE FOR RLY6, BUT RLY 7 IS ALL TIME DISABLE DURING WASTE 
    2. Relay #2 should not be switching on and off during Flush Module  command. ONCE MANUAL FLUSH IS COMMAND RLY 2 IS ON, LATER IT CAN GET OFF DUE TO PRESSURE BUT WILL NOT GET ON AGAIN IF PRESSURE IS BACK
    3. now that we know how to resolve the wifi dosconnection, disable the beep alarm. CODE DELETED

T2.8 over T2.7 Dated: 21.09.2024
1. Buz on for 25mS only instead of 50mS for every 300 Sec during alarm.
2. if there is no proper response from the server for more than 300 sec, buz will on for every even sec.

T2.7 over T2.6 Dated: 15.09.2024
1. Buz on for 50mS only instead of 200mS for every 300 Sec during alarm.

T2.6 over T2.5 Dated: 15.09.2024
Guillermo whatsapp Dated: 14.09.2024
Lets do the following changes:
1. Do not do a pcb reset if there is an ec zero alarm. DONE
2. show the alarm under alarms. ALREADY THERE
3. Go to IDLE if there is a zero ec alarm after 5 cycles. DONE
4. Make the cycles adjustable through setpoints. For example if we want to extend the cycles later, allow us. If we want it to be 10 or 200 allow us. FROM DOCKLIGHT ONLY
DOCKLIGHT COMMAND TO SET AND CHECK ZERO EC SET POINT AND ITS COUNTER
1. Read the setpoint value set in PCB = 1010,DBG,410.34=?]
2. Set the cycle count for Zero EC alarm = 1010,DBG,410.34=X]
    where X is the cycle count desired after which alarm is generated
3. Read how many cycle has passed of zero EC cycle = 1010,DBG,420.17=?]

T2.5 over T2.4 Dated: 06.09.2024
1. omitting 1 sec delay during polarity switch for DiUse only. 

T2.4 over T2.3 Dated: 20.08.2024
1. All changes made on DiEntry From T1.6 to T2.3 also implemented in DiUse in T2.4.

T2.3 over T2.2 Dated: 23.07.2024
1. While system in IDLE and user press ENTER, all PAE alarms are self resolved.
2. From LOGOUT mode to AUTO mode, all PAE alarms are self resolved.

T2.2 over T2.1 Dated: 30.06.2024
1.  All alarm reported by PAE is latched until user says its resolved via LCD or server. Even if PAE has resolved it by itself it is not marked as resolved until user says so.

T2.1 Dated: 29.06.2024
1.  First waste was not happening because of: after start signal system first goes to standby. In standby once enter is pressed goes to other step depending upon pressure.
    So in the standby step according to flow chart if the pressure is < start goto prepurify. In flow chart standby step is missing and once flush+shunt happens, system should complete the flush+shunt according to flow chart, but in actual it is not due to
    changes made in T2.0. Need to correct the flow chart for future reference.
2.  Duing IDLE step buzzer gets ON for 200ms every 5min. But within one sec it was turning ON for multiple times. Made once only.

T2.0 Dated: 28.06.2024
The system should always "listen" for a pressure drop (same as in the pure step) when it is in the Flush+Shunt step too, 
if Pressure < Pressure Start and the system is in standby or in the middle of Flush+Shunt, go to pre-purify and not to first waste 
as per the flow chart version 2 edited on 28-06-2024.

T1.9 Dated: 27.06.2024
If the pressure goes higher than 4.8 (stop pressure) during pure it goes to waste (Which is correct and it is suppose to be like this).
During waste if the pressure decreases and stabilizes inbetween 3.7 > PRESSURE < 4.8 (which is tank full condition) system goes to WAIT. 
System should not go to wait, it should complete the waste then flush then wait.
RESOLVED IN THIS VERSION.

T1.8
How to set start/finish waste time? 
o	Make a ratio between waste time / (Pure time + pre-pure time) 
o	Example: 80 s / 115 s = 0.69 
o	Start and finish waste time = pure time elapsed x 0.69  
DONE

T1.7
1. 14th May 2024 sebastian change that process should not stop in case of EC error, previously it use to go to IDLE but later it continues the process but didn't taken care about the polarity, that is why this loophole created. In the firmware T1.7, I have resolved the loophole.
2. whenever the automode is started first waste is always half of the total waste time.

T1.6
1.	Add live pressure screen in the Operations tab

T1.5
1. keep alarms tab at last of the line in operations screen
2. Finish with a Waste, Start with a Waste: For this feature FlushShuntPendingFlag introduced, 
    whenever shunt has to happen it will make this flag true and after waste step if this flag is true then goes to shunt step. 
    Once shunt step is over, this flag is turned false.
 
B4.T1.4
1. Bootloader changes as per relay logic.

T1.4 over R6.0 
1. Display Current and Voltage Live on the Operation Screens. -> Live current & Live Voltage Added. 29.05.2024
Implement code to continuously monitor and display real-time current and voltage values on the screen.
2. Relay logic toggled. 30.05.24
3. Finish with a Waste, Start with a Waste: 30.05.24
Update the firmware to ensure that the system finishes with a waste process before starting a new cycle with waste material. This sequencing helps in maintaining operational efficiency and prevents cross-contamination. The initial waste time should be set to 50% of the normal waste time to optimize the process.


S1.1 over R6.0 i.e S1.0 Dated: 05.09.2023 NTC 23.09.05 23:30 as per email dated 23.09.05 via Sebastian
#define PREPURE_POLARITY_SWTICH

R6.0 over R5.9 Dated: 05.09.2023 NTC 23.09.05 23:30 as per email dated 23.09.05 via Sebastian
1. No matter what type of contactor used (new or old), whenever OUTPUT_POLARITY relay is switching DO6_RELAY_ON i.e SHUNT relay will get on for 1sec.
2. Instead of strcpy(LcdLine2," 0005 ???? 5000 "); it has been converted to sprintf(LcdLine2," %04d ???? %04d",MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL, MAX_CYCLE_COUNT_FOR_CIP_MAX_VAL);
    so that whenever client ask for temp changes, it gets reflected on LCD too. Same logic has been implement in all the setpoint without decimal point.

R5.9 over R5.8 Dated: 17.01.2023 NTC 23.01.17 23:35
1. #if(CVS_CIP==CVS_CIP_NO)
    #define MAX_CYCLE_COUNT_FOR_CIP_DEF_VAL changed from 800 to 500

R5.8 over R5.7 Dated: 13.12.2022 NTC 22.12.13 17:35
1. Reduce the pure low flow & Waste low flow alarm minimum setpoints to 0 from all the model (DiUse,DiEntry,CFO). DONE.
2. Modify the zero EC alarm in such a way that it does not send the system to IDLE. It ONLY needs to show an alarm on both LCD and IoT. IT IS WORKING AS REQUESTED. I DON'T KNOW WHY THEY HAVE REQUESTED THE SAME.
3. Remove the Low-Level CIP alarm. DONE. NTC 22.12.23 18:40

R5.7 over R5.6 Dated: 20.09.2022 NTC 22.09.20 23:55
1. Relay 2 operation from docklight/server was not happening for CFO. Rectified.

R5.6 over R5.5 Dated: 19.08.2022 NTC 22.08.19 23:45
1. for CFO system, server was not able to read setpoint 410.9 and 410.10 RECTIFIED.

R5.5 over R5.4 Dated: 08.08.2022 NTC 22.08.08 23:45
1. Ppg_SetDuty(5,0);//this code moved within WAIT step
2. Ppg_SetDuty(5,0); added in HIGH_TEMP and HT_WAIT step.

R5.4 over R5.3 Dated: 23.06.2022 
1. Fixe the cycle count
2. Renaming
3. Set default set time and grace time

R5.3 over R5.2 Dated: 16.06.2022 

1. Code optimization NEEDS TESTING

Reduced from 

  109 264 bytes of readonly  code memory
   10 719 bytes of readonly  data memory
   10 507 bytes of readwrite data memory

To

   105 424 bytes of readonly  code memory
   10 415 bytes of readonly  data memory
   10 505 bytes of readwrite data memory
   
by deactivating unused not required commands.
2. CycleCounter increment moved from HF_WASTE to post PREPURIFY step.

R5.2 over R5.1 Dated: 08.06.2022 //NTC: 22.06.08 00:03
1. System found stuck at AT command. RESTART SYSTEM IF STUCK FOR MORE THAN 15 SEC
2. whatsapp change
[0:29 am, 08/06/2022] Voltea Guillermo: Spoke to the team, please
modify the pae self resolve logoc to the following:
Instead of trying once, try to self reosolve 3 times and after the 3rd time go to IDLE
[0:29 am, 08/06/2022] Voltea Guillermo: When system goes into IDLE, latch the alarm and show which alarm is it that is popping
3. Standard DiUse and DiEntry change the default pure ec target to 150. CFO and p3F2 change the default pure ec target to 50

below code is added in Main at the end of HF_WASTE step
#if((PRODUCT_TYPE==PRODUCT_DIUSE||PRODUCT_TYPE==PRODUCT_DIENTRY)&&(CVS_CIP==CVS_CIP_YES))
else if(CycleCounter >= MaxCycleCountForCIP)
{   CurrentProcessStep = CIP_DOSING_ON;
    CIP_DosingRunTimeInSec = 0;
}
#endif

R5.1 over R5.0 Dated: 31.05.2022
1. upper & lower limit of CIP.

R5.0 over R4.9 FOR Only DiUse and DiEntry MODEL 05.05.2022
1. ONLY do a CIP cleaning if the following 3 criteria are met:
A. The unit is in Standby mode.
B. The unit is within X% of its CIP cycles. For example: 20% of the CIP cycles. Add this percentage variable to setpoints.
C. The unit is within X minutes/hours of a set time. For example: set time is 12:00 within +\- 1 hour). Add set time and delay time variables to setpoints.
2. Issue of R4.9 that after Drying function is over alarm relay is ON untill system is send to idle. RESOLVED.
3. During DiEntry firmware upgrade, bypass relay keeps on toggling. hence these 2 functions PAE_OnOffRelayControl(); BypassOperation(); has been moved inside the FIRMWARE_NOT_AVAILABLE.

R4.9 over R4.8 FOR ALL MODEL 03.05.2022
1. Implement the Dry System user function:
        DiUse and CFO  Turn On relays J9, J10, J11, and J12.
        DiEntry  Turn On relays J9, J11, J12, and J16.

R4.8 over R4.7 FOR ALL MODEL 12.04.2022
1. if bypass pressure is set as zero means no bypass alarm is required. so before ALARM_OCCURED, AnalogPressureBypassLevel > 0 condition is checked. DONE
2. MAX_CYCLE_COUNT_FOR_CIP_MIN_VAL changed from 5 to 1
3. START_PRESSURE_DEF_VAL was different for NO_CVS = 4.0 and for WITH_CVS = 3.5. For this version and onward it is the same 4.0 for both With/without CVS. for DiUse/DiEntry/CFO.
4. START_PRESSURE_DEF_VAL was different for NO_CVS = 4.8 and for WITH_CVS = 5.5. For this version and onward it is the same 4.8 for both With/without CVS. for DiUse/DiEntry/CFO.

R4.7 over R4.6 FOR ALL MODEL 08.04.2022
1. PAE current logging was missing in LogRecord function. ADDED.
2. Relay name MOV replaced with WOV. done

R4.6 over R4.2/4.3/4.4/4.5 FOR ALL MODEL 11.03.2022
1. Change the default system password to 1010. Currently, it is 0000.
2. Adjust the minimum Bypass pressure setpoint to 0 instead of 0.5 BAR. 
3. If there is a zero EC error, ONLY send the alarm. Do not send the system to IDLE.
4. Rather than checking spot EC to check for a zero ec error, use average EC at the end of the Pure step.
5. Adjust the minimum Prepure time setpoint to 2 instead of 0 seconds.
6. Remove the TOG? characters. This will make space for the relay names.
7. Rename the SNT relay. Name it WASTE COIL. Attached is a photo.
8. Rename the POL relay. Name it PURE COIL. Attached is a photo.
9.Rename the PAE relay. Name it RECIRC. V. Attached is a photo.
10.Rename the BUZ relay. Name it BUZZER. Attached is a photo.
11. Live EC added in log memory.
12. PAEcurrent added in log memory.
13. Live EC added in server protocol.
14. PAEcurrent added in server protocol.
15. Reduce the timer on the Stat: Failed logic to no more than 5 Minutes. Previously failed msg was being displayed after 3 unsuccessful attemp, now failed msg is displayed after first attempt.


R4.5 over R4.3 FOR ALL MODEL DATED 16.03.2022
1. Pressure Averaging changed from 149 to 35 sample. On 149 sample it was taking 24sec, now with 35 sample it should take approx 5sec.

R4.4 over R4.3 DATED 16.03.2022
1. Reduce the Buzzer time. Turn it ON for 200 milli seconds instead of 1 second, ON TEST was fluctuating. RESOLVED. FOR ALL MODEL
2. sending to Flush shunt from any step removed and only sent to flush shunt from prepure, pure, waste, hf_waste. FOR CFO ONLY

R4.3 over R4.2 FOR CFO MODEL 15.03.2022
1. ProcessStepRunSec was kept reinitialized to zero in BypassOperation function during SHUNT step, hence the process was stuck on it. RESOLVED.

R4.2 over R4.1 FOR ONLY CFO 09.03.2022
1. Make sure the system goes into Stand By mode any time the pressure sensor sees the Stop Pressure. Any time meaning any step in the process including the Waste and Prepure steps. Currently, system only goes to Stand By during the Pure step.
2. Change the Flush + Shunt time to 20 seconds.

R4.1 over R4.0 FOR ALL MODEL 04.03.2022
1. During wifi name and passord entry, first char at start and after entry of new char is always left arrow key.
2. if available char is available then only backspace key should work.

R4.0 over R3.9 FOR ALL MODEL
1. During wifi name and passord entry, if user press down button when space character is being blinking, backspace arrow will be displayed and upon enter press it will function as backspace.

R3.9 over R3.8 FOR CFO MODEL
1. WOV2 relay operation missing during prepure and pure. RECTIFIED.

R3.8 over R3.7 FOR CFO MODEL
1. next batch current calculation reverted back after guillermo whatsapp msg NTC:10.02.2022 15:30

R3.7 over R3.6 FOR ALL MODELS
1. PRESSURE FORMULA REVERTED BACK TO OLD LOGIC AS PER MY TEST INSTRUMENT. 

R3.6 over R3.5 
2. only for CFO, DiUse condition copied to CFO except relay logic. (like in LCD & communication)
1. only for DiUse, if pressure <= BypassLevel then BypassAlarmSecCounter = 0 and reset bypass alarm, if present.

R3.5 embedding CFO in R3.4
1. Debug message on docklight for pressure calculation disabled. FOR ALL MODEL
2. P6 for CFO
    a. Relay operation as per R2.1 . OK TESTED
    b. in waste step too, pump analog output voltage is generated as per pure traget flow.
    c. Avg EC is calculated from (Avg pure EC +Avg Waste EC)/2. and next current for pure is calculated. Waste current is same as pure current.
    d. Change the default time settings to the following:
        i.   Prepure time: 10
        ii.  Pure time: 80
        iii. Waste time: 90

R3.4 over R3.3 FOR ALL MODEL: 
1. Pressure formula changed according to Guillermo Email dated: 02.02.2022

R3.3 over R3.2 according to Guillermo Email dated: 27.01.2022
1. BYPASS Operation is executed even when the system is in logout mode. That means the relay will operate and an alarm will generate even in logout mode. FOR ALL MODEL.
2. For External NC bypass option, Bypass Relay is ON if pressure is less than bypass pressure setpoint. After Relay ON, Bypass alarm will occur after WaitTimeB4BypassAlarmInSecCounter > WaitTimeB4BypassAlarmInSec setpoint. ONLY FOR DIENTRY
3. For other bypass options, Bypass Relay is OFF if pressure is less than bypass pressure setpoint. After Relay Off, Bypass alarm will occur after WaitTimeB4BypassAlarmInSecCounter > WaitTimeB4BypassAlarmInSec setpoint. ONLY FOR DIENTRY
4. default Start pressure = 4.0 FOR ALL MODEL.
5. default stop pressure = 4.8 FOR ALL MODEL.
6. default bypass pressure = 3.5 FOR ALL MODEL.
7. ESC key is used to return to home screen from "Test WiFi" screen. FOR ALL MODEL.

R3.2 over R3.1 according to Guillermo Email dated: 15.01.2022
below bug of 3.1 resolved.
1d> If the EC zero error is not resolved after doing a PCB reset, then trigger the EC ZERO ERROR alarm and send the system to IDLE mode. NOT Functioning Properly  when system goes into IDLE mode after doing the PCB reset, it has been observed that an infinite flush-shunt occurs.  When a DiUse is in IDLE mode, NONE of the relays should be ON other than the buzzer(spare) every 5 mins.
When testing, I suggest you replicate the EC ZERO ERROR by disconnecting the EC probe.
2. WIFI NAME Limit: Change the limit on WIFI Name from 16 to 32 characters. DONE NOT Functioning Properly  When WIFI name is longer than 13 characters, the WIFI name address gets corrupted and the WIFI Password is added to the WIFI Name. As an example, look at the attached photos. WIFI Name should only be iphone12341111; however, the WIFI Password is added to it:iphone12341111zzzzzzzzzz12345678.
3. EC Limit: Change the limit on EC target from a max of 2000 to a max of 5000. DONE. Works.
4. Wait Name: Replace Wait with Stand By. DONE Incomplete  one of the screens is still showing Wait. The screen that shows the EC shows Wait instead of Stand By. A video has been sent to your WhatsApp.

R3.1 over R3.0 according to Guillermo Email dated: 22.12.2021
1. EC ZERO ERROR Self-Resolve Logic: The zero ec alarm has been activating on a few systems, both DiUse and DiEntry. It has been occurring at random times and root cause has not been yet identified. I have learnt through testing that a PCB reset will fix this error; therefore, we would like to implement this PCB reset as a temporary fix. Below is the logic:
    1a. If an EC Zero Error occurs, activate a counter, and start a cycle count. Name this cycle count  EC ZERO Cycles and make it a variable. As a default set it to 3 cycles. Add this variable to setpoints.
    1b. If the system runs for X cycles and the EC zero Error is still there, do a PCB reset and reset counter.
    1c. If the EC zero error is resolved before reaching X cycles, reset counter.
    1d. If the EC zero error is not resolved after doing a PCB reset, then trigger the EC ZERO ERROR alarm and send the system to IDLE mode.
    DONE NEEDS TESTING
2. WIFI NAME Limit: Change the limit on WIFI Name from 16 to 32 characters. DONE
3. EC Limit: Change the limit on EC target from a max of 2000 to a max of 5000. DONE.
4. Wait Name: Replace Wait with Stand By. DONE

R3.0 over R1.9 Dated: 08/12/2021 for all model
1. bootloader firmware updated. Upon firmware update using FOTA, Automatic Restore factory set disabled. 

R1.9 over R1.8 Dated: 02/12/2021 for all model
1. PCB will power cycle itself when there is a PAE comm alarm is registered.

R1.8 over R1.7 Dated: 08/11/2021 for all model
1. DiEntry:
    Change the preset Pure Flow Target to 5.3. DONE
    Change the preset Stop Pressure to 4.8. DONE
    Switch off the CVS CIP logic. Use the standard one only. NOT REQUIRED AS PREVIOUS VERSION IT WAS ALREADY SWITCHED OFF
DiUse:
    Switch off the CVS CIP logic. Use the standard one only. NOT REQUIRED AS PREVIOUS VERSION IT WAS ALREADY SWITCHED OFF

R1.7 over R1.5 Dated: 29/10/2021 for all model
1. shunt on for 250mS whenever polarity relay switches, only for old contactor version. Removed from new contactor version.
2. Instead of turning on the Shunt Relay, Turn ON the PAE Relay (Spare) during the Recirculation CIP step (during "CIP RECIRCULATE" on LCD). ONLY DIENTRY and SERIES, NOT DIUSE
3. During transition from pure to waste, first switch off whatever relay is suppose to be off during waste and give one sec delay then only switch on the remainging required relay

R1.6 over R1.5 Dated: 28/10/2021 for all model
1. half sec delay inbetween any two relay switching.

R1.5 over R1.4 Dated: 28/10/2021 for all model
1. PAE clock frequency reduced to half.

R1.4 over R1.1 Dated: 25/10/2021 for all model
1. PAE clock frequency reduced to half.

R1.1 over R1.0 Dated: 19/10/2021 for all model
1. switched to 10 bar pressure sensor

R1.0 over D9.9 Dated: 14/10/2021 for all model
S1.3 over S1.2 Dated: 14/10/2021 for Series model
1. After manual CIP PCB will return to Voltea Homepage with step as IDLE.

S1.2 over S1.1 Dated: 13/10/2021 for Series model
1. Data was not thrown to the server in S1.1. Rectified and tested.
2. Setpoint was not going beyond waste time. Rectified.
3. Default PAE Voltage for pure and waste changed to 3.6VDC

D9.9 over D9.8 Dated: 06/10/2021 for all model
1. on date time request to server, 410.33 setpoint send was missing. ADDED.

S1.1 over S1.0 Dated: 01/10/2021 for all model
1. setpoint added
    //setpoint id = 410.33 is WaitTimebB4BypassAlarmInSec
    #define WAIT_TIME_B4_BYPASS_ALARM_MIN_VAL 0
    #define WAIT_TIME_B4_BYPASS_ALARM_DEF_VAL 900
    #define WAIT_TIME_B4_BYPASS_ALARM_MAX_VAL 9999
2. Variable added 
    ui WaitTimeB4BypassAlarmInSec = 900;
    ui WaitTimeB4BypassAlarmInSecCounter;
3. Alarm occurs when WaitTimeB4BypassAlarmInSecCounter > WaitTimeB4BypassAlarmInSec
4. if server responds with REJECTED, pcb will switch off the ethernet/wifi untill reset.

D9.7 over D9.5 Dated: 21/09/2021 for all model
1. D9.6 reverted back to D9.5.
2. PressureOffset variable added.
3. PRESSURE_OFFSETa location added to non volatile memory partition.
4. PressureOffset variable setting command added at 420.16
5. PressureOffset variable cannot be more than 10% of PHYSICAL_VALUE_HIGH defined in GPIO.h. if found greater, then it will reinitialized to zero.

D9.6 over D9.5 Dated: 21/09/2021 for all model
1. New pressure measurement technique is off by 0.3bar with actual so reverted back to D9.4, that we formulated in excel.

D9.5 over D9.4 Dated: 20/09/2021 for all model
1. Voltage is converted to mA first as we are measuring 4-20mA. Later mA is converted to Physical Quantities as defined in GPIO.h file. NEEDS TESTING.
2. TotalPureVolume added to periodic save page and retrieved from periodic save page whenever the system restarts, previously it was from log memory.

D9.4 over D9.3 Dated: 17/09/2021 for all model
1. In IDLE state if power cycle then all relay used to be ON. This happened after adding relay control feature from server side. RECTIFIED.
2. WiFi bug resolved. Needs more testing.

D9.3 over D9.2 Dated: 15/09/2021 for all model
1. ServerNoResponseCounter = 0; once the connection gets successful so that upon one failure it do not disconnect.

D9.2 over D9.1 Dated: 15/09/2021 for all model
1. DigitalOutput = PrevDigitalOutput = 0xFFFF;//for manual control from server while PCB in IDLE step. line code added whenever PCB goes to IDLE mode to switch off all relays at the begining.

D9.1 over D9.0 Dated: 15/09/2021 for all model
1. In WiFi communication, if connection no response from server for more then 3 times then wifi module will go to reconnection step.

D9.0 over D8.9 Dated: 12/09/2021 for all model
1. with FOTA tested.

D8.9 over D8.8 Dated: 09/09/2021 for all model
1. Bug found on server communication that - delay for receiving complete packet before execution was there in debug communication ON mode. RECTIFIED.

D8.8 over D8.7 Dated: 09/09/2021 for all model
1. Relay control command and in IDLE step, PrevOutput & current output is matched. for manual control from server while PCB in IDLE step.
2. log when step exits is disabled. Now log when only step is entered is enable. Prevously both was enable.
3. if wifi stays in this stage MOBILE_STAGE_WIFI_STATION_MODE_TCP_CLIENT > 30 sec, goto MOBILE_STAGE_ROUTING_DECISION_MAKING
4. MOBILE_STAGE_WIFI_STATION_MODE_TCP_CLIENT stage every 2 sec it will query the router.

D8.7 over D8.6 Dated: 04/09/2021 for all model
1. if wifi is successful to connect router then it will stay in wifi mode even if server is down. Auto switch may only happens when wifi is not able to connect to router.
2. if ethernet doesn't receive any response for 3 attempts, then only auto switch may happen

D8.6 over D8.5 Dated: 02/09/2021 for all model
1. Default Pressure setpoints for DiEntry changed to: Bypass:3.5, Start:4.0, Stop:5.0. 
2. Forced PAE_I2C_STOP used upon power reset.

D8.5 over D8.4 Dated: 01/09/2021 for all model
1. LAST_STEP_B4_POWER_OFFa memory location added.
2. LAST_RUN_SEC_B4_POWER_OFFa memory location added.
3. Periodic save is done every 10 sec.
4. Upon RESTART, instead of Log memory, LAST_STEP_B4_POWER_OFFa & LAST_RUN_SEC_B4_POWER_OFFa location is read and step and runsec is initialized, so that max loss of time is 9 sec.
5. TEST CODE: if received byte lenght is zero, xCommandFlag is cleared.

D8.4 over D8.3 Dated: 31-08-2021 for all model
1. in all previous version log memory pointer was updated every after 15 logs available. Due to which upon power cycle the system use to restart some previous step but not last step before power cycle. Now upon every log generated, log pointer is updated. Disadvantage of this is memory life will decrease. Since no logs will be generated untill PCB receives date/time, if next power cycle is before PCB receives date/Time then PCB will start from the latest log available. 

D8.3 over D8.2 Dated: 31-08-2021 for all model
1. If start command from server when system is in Auto-IDLE, if multiple command is received, PCB is restarting the Process from Prepurify again and again. Rectified.

D8.2 over D8.1 Dated: 31-08-2021 for all model
1. Test firmware to find out why COMM -> blank appears in ethernet connection instead of ACK. 
    Trial 1. poweroff cycle to only ethernet manually by using D8.1 -> Result Failed
    Trial 2. poweroff cycle along with buffer flush made in D8.2 -> Result Unknown yet until next time its pop up.
    
D8.1 over D8.0 Dated: 28-08-2021 for all model
1. DigitalOutput value given and received as per inverted or non-inverted logic. No matter which model ON=0, OFF=1.
2. alarm reset command added at 411.255
28-08-2021 for only CVS
3. D6.5  Dated: 12-05-2021 point 3 : If the system is not going into Wait Mode and it needs to do a CIP cleaning, the system can only run up to 55 extra cycles before the CIP cleaning is forced. This condition, removed from CVS program. As CVS system, the cleaning process is decided only if system goes to wait step.

D8.0 over D7.9 Dated: 27-08-2021 for all model
1. if mode change command is received, mode will be changed only if the current mode is different than the existing mode, so that it does not fluctuates upon multiple command is received.
2. if the setpoint is not available for any perticular version, Reading or writing the unavailable value will return 0.
3. changes according to Guillermo mail FOR DIENTRY Dated:27/08/2021
    change "sip dosing on" to "cip dosing" for 110 only once 
    change "CIP dosing off" to "cip recirculation" for 900sec only once 
    requested change "cip pulse ON" to "CIP flush" for 180sec but instead "CIP flush" is already there so changed in "CIP FLUSH"

D7.9 over D7.8 Dated: 24-08-2021 for all model
1. MobileStageWaitSec made for 10sec.


D7.8 over D7.7 Dated: 24-08-2021 for all model
1. pressure sensor formula changed for 10bar sensor. as per below input: not tested, yet to confirm from voltea.
Pressure in bar	||      Voltage measured in docklight
        0	                    1438
        1	                    2112
        2	                    2719
        3	                    3269
        4	                    3885
        5	                    4471
        


D7.7 over D7.6 Dated: 24-08-2021 for CVS model
1. Just for CVS model, factory default for start pressure changed to 3.5, stop pressure changed to 5.5, CIP pressure changed to 5.0

D7.6 over D7.5 Dated: 24-08-2021 for all
1. if Prefered Communication Media is set to NONE then system will log without validating date/time from server.

D7.5 over D7.4 Dated: 24-08-2021 for all
1. Low flow was sending the system to wait instead of idle. Rectified. ok tested.
2. Pressure formula float structure change as D7.4 pressure was not accurate. Need to test.
3. Just for testing log sec changed from 60 to 10 second.

D7.4 over D7.3 Dated: 20-08-2021 for all
1. if MobileDebugFlag is TRUE, server command response sent back to server is shown in docklight. 
2. Instead of AtlasConductivity, LastFiveCyclePureAverageConductivityAverage is been logged and send to server.
3. system will go to wait at the end of waste, if there is low flow alarm (purify and/or waste).
4. PAE_OperationAsPerModeAndProcessStep function, else part of if(Mode == MODE_AUTO) is added to switch off the PAE in LOGOUT MODE.
5. default factory set changed, BYPASS_OPTION_DEF_VAL -> BYPASS_EXTERNAL_NC.
6. pressure sensor changed from 6bar to 10bar.

D7.3 over D7.2 Dated: 20-08-2021 for all
1. PAE voltage setting for DiEntry added.
2. PAE voltage max allowed changed from 2.0 to 12.0 for both.
3. Factory reset, max min value, updated as required by server

D7.2 over D7.1 Dated: 04-08-2021 for all
1. Recording Log disabled until current date/time not received either from server or docklight or from LCD. 04/08/2021 ok tested
2. CommunicationMediaPrefered also added in Flash so that it remembers even after restart. 04/08/2021 ok tested.
3. In any mode (even in logout, hardware test etc etc), PCB will log record periodically. 06/08/2021 OK TESTED.
4. Logging record is made dynamic. Meaning record time interval is made 60 sec, if any step/input/output change occurs wihin 60sec it gets recorded too. 06/08/2021 ok tested.
5. When any step is over, it gets recorded. 06/08/2021 ok tested.
6. Groupt Digital input debounce taken care. 06/08/2021 ok tested.
7. server/docklight Relay command added at 326.1 to 326.9 06/08/2021 ok tested.
8. % recovery and Mode added in PT_TABLE and is sent to server appended at the end. 06/08/2021 ok tested.

D7.1 over D7.0 Dated: 07-07-2021 for all
1. Time docklight format as per server requirement. 07/07/2021 ok tested
2. Date docklight format as per server requirement. 07/07/2021 ok tested
3. Mode switching command added to 420.0. 07/07/2021 ok tested.
4. password field on command use, for docklight its password, but for server its command stamp id. 01/08/2021 ok Tested

D7.0 over D6.9 Dated: 01-07-2021 for all
1. Spare relay is being used for buzzer which will turn on for 1 sec every 5 min when system is in idle. For this functionality, Bootloader has to be modified hence bootloader version has been updated to B2.

D6.9 over D6.8 Dated: 23-06-2021 for all
1. "2," Prefix for all command response from PCB so as to match server communication protocol. DATED: 23-06-2021
2. alarm code in decimal format.
3. in docklight log debug added input and output status.

D6.8 over D6.7 Dated: 22-06-2021 for all
1. bug on compairing current pressure and set cip pressure rectified and resolved.

D6.7 over D6.6 Dated: 22-06-2021 for all
1. for pressure greater than start and less than stop, goto WAIT condition from all other step except SHUNT & WAIT changed to goto WAIT from all operation step i.e PREPURIFY, PURIFY, WASTE & HF_WASTE.

F2.D6.6 over F1.D6.6 Dated: 19-06-2021 for both
1. BYPASS LEVEL PRESSURE CHANGED FROM 3.0 TO 3.5 FOR DIENTRY. DATED: 19-06-2021
2. TIMER BEFORE CIP START (CIP_WaitTimeInSec) & AnalogPressureCipLevel SETPOINT ADDED.
3. WAIT_BEFORE_CIP_START step added.
4. made this new logic firmware as F2

PCB 2.0
D6.6 over D6.5 Dated: 31-05-2021 for both
1. After server packet header, line feed missing. DONE
2. Pkt_Type added in packet header after SOP.
3. HMI CRC computed and tested. DONE
4. DIENTRY pure and waste current differentiated using conditional formating in constants.c file. Dated: 18-06-2021

D6.5  Dated: 12-05-2021 for both
1. water temp, cabinet temp, pressure, anaout voltage, input & output added into log memory
2. CIP will now occur ONLY when the system goes into Wait Mode.
3. If the system is not going into Wait Mode and it needs to do a CIP cleaning, the system can only run up to 55 extra cycles before the CIP cleaning is forced.
4. Once the CIP cleaning is done, both the standard and 55 extra cycles counters will be reset.

D6.4  Dated: 26-02-2021 for both
1. dientry shunt relay will on, no matter which contactor.
2. if version has H3 then it is for old contactor, if version has H4 it is for new contactor.

D6.3 Dated: 24-02-2021 for both
1. Made for old contactor.

D6.2 Dated: 23-02-2021 for both
2. Change the dynamic default setpoint from .1 to 0.05 on both DiEntry and DiUse. DONE
3. Give the temperature compensation task to the Atlas EC chip. DONE
4. Change the flow k-factor default value from 181 to 700. DONE

D6.1 Dated: 22-02-2021 for both
1. My compensated conductivity removed and Atlas compensated conductivity included.

D6.0 Dated: 15-02-21 both
1. Relay state changed as per Guillermo email.

D5.9 Dated: 10-02-21 specially for dientry
1. max pressure allowed to set via setpoint is changed from 5.9 to 4.8 bar
2. Instead of stop pressure start pressure was used in setpoint. RECTIFIED.

D5.8 Dated: 09-02-21 specially for dientry
1. Here Active means OFF for Internal & External NO and ON for External NC.

D5.7 Dated: 09-02-21 specially for dientry
1. if pressure is < bypass pressure i.e 3.0bar, Bypass relay is active else inactive. Here Active means ON for Internal & External NO and OFF for External NC.


D5.6 Dated: 09-02-21 specially for dientry
1. For DiEntry, Setpoint for start/stop/bypass limit changed. 
    Previously it was:
        Pressure <= 3.0 -> start
        Pressure <= 3.7 -> bypass
        Pressure >= 4.8 -> stop
    Now, in this version
        Pressure <= 3.0 -> bypass
        Pressure <= 3.7 -> start
        Pressure >= 4.8 -> stop 

Both D5.5 Dated: 09-02-21
1. Decimal point forgotten in D5.4 so fixed in this version.

Both D5.4 Dated: 09-02-21
1. pressure for start/stop/bypass added on the screen with limit and default factory set.

Both D5.3 Dated: 08-02-21
1. without pae error if the step goes to hf_waste then even if there was pae error in prev cycle it can be consider that pae error was resolved after flush+shunt so clear the PaeFailCounter.
                                
Both D5.2 Dated: 05-02-21
1. All other steps except PREPURIFY, PURIFY, WASTE, HF_WASTE , PAE is set to Disable output mode.
2. PAE relay switching action is removed. Now PAE is also a free Digital Output.
3. In case of any PAE alarm, system will go to FLUSH+SHUNT, then to WAIT, then to PREPURIFY. Even in PREPURIFY PAE error is not solved then system will go to IDLE.

Both D5.1 Dated: 04-02-21
1. If there is any PAE error for more than 5 sec during PREPURIFY or PURIFY or WASTE or HF_WASTE then system will go to SHUNT.
2. During SHUNT PAE output voltage and current is disabled and at every 15sec interval PAE output relay is switch ON/OFF.
3. Process will continue as it should be (SHUNT -> PREPURIFY -> SHUNT), but if PAE error occurance is more than 75sec still not resolved, then PCB will restart.

Both D5.0 Dated: 02-02-21
1. DiEntry & DiUse Manual CIP seperated.
2. At the end of CIP, next step is decided upon pressure switch, pressure sensor analog value and manual or auto operation.
3. WASTE_VALUE_ALARM_FLAG as ALARM RESOLVED at the CIP end if done through automode.

Both D4.9 Dated: 02-02-21
1. Manual CIP added.
2. Factory setting for both rectified and checked.
3. Display menu adjusted as per old released version.

DiEntry P4.B1.H3.F1.D4.8 Dated: 01-02-21
1. AnaPressureAvgScanCounter max count increased from 100 to 150

DiEntry P4.B1.H3.F1.D4.7 Dated: 01-02-21
1. From prepurify to purify step, pump will start from lowest speed and gradually increases.

DiEntry P4.B1.H3.F1.D4.6 Dated: 01-02-21
1. AnaPressureAvgScanCounter max count increased from 50 to 100

DiEntry P4.B1.H3.F1.D4.5 Dated: 29.01.21
1. 3.5 bar slab changed to 3.7
2. BYpass valve on/off opposite than prev version if pressure sensor is connected.

DiUse   P3.B1.H3.F1.D4.3 Dated: 29.01.21
DiEntry P4.B1.H3.F1.D4.3 Dated: 29.01.21
1. D3.1 Pressure logic change, in SHUNT step, if pressure is inbetween low and high limit, system should not go to wait until SHUNT step is over. 

DiUse P3.B1.H3.F1.D4.2 Dated: 28.01.21
DiUse P4.B1.H3.F1.D4.1 Dated: 28.01.21
1. D3.1 Pressure logic, pressure reaches highest point goto step, changed from WAIT to SHUNT.

DiUse P3.B1.H3.F1.D4.1 Dated: 28.01.21
1. in purify step, if pressure is > 3.5 then go to shunt with zero pump analog output voltage.

D4.0 Dated: 28.01.21 
1. Raw temperature, current flow and total flow is received from the child processor.
2. flow K factor is applied in mother processor. No need to pass the K factor to child.
3. Child processor response to total flow from mother, the total flow counter is reset at child processor.
4. Frequency of asking the parameter value from mother to child has been reduced.

D3.1 Dated: 26.01.21 
1. DiUse Pressure logic simulated and tested.
2. DiEntry Pressure logic simulated and tested.

D3.0 Dated: 24.01.21 
1. DiUse 2.6 and DiEntry 2.5 is merged together.
2. DiUse 3.0 if pressure sensor is not working and value is less than 0 bar then Digital input is considered to alarm.
3. if(I2C_ComErrorCounter > 250) PCB will restart.

P2.6 over P2.3 Dated: 20.01.21
1. if wifi/ethernet debug command is off, Delay of 100ms Added before sending any command to wifi/ethernet/server. Without this delay,
    wifi test was showing FAILED everytime. The minimum delay required is not tested but with 100ms it is working well.
    
D2.3  over D2.2 Dated: 20.11.28
1. spare serial port working: 
    i. COMMAND 200 IS CONFIGURED TO TRANSMIT DATA FROM PCB AND CAN BE VIEWED IN DOCKLIGHT CONNECTED TO SPARE SERIAL PORT WITH 115200 bps.
    ii. Whatever is transmitted through docklight connected to spare serial port with 115200bps is viewed in docklight connected to usb serial port.
    iii. In testing it is found that, if serial port is pressed hard, point i is not functioning properly along with freezing HMI serial port too.
2. wifi_test_fail_cntr variable added to attempt 3 connection test attempt before showing fail message. Fail message time taken is approx 3-4 min

D2.2  over D2.1 Dated: 20.11.12
1. "0000,DBG,23.0=?]" command added for reading AnaPressureAval from command. 

D2.1 20.10.27
1. Wifi SSID name entering option through LCD and Keypad within Userfunction. VIEW & CHANGE option logic implemented.
2. Wifi PASSWORD entering option through LCD and Keypad within Userfunction. VIEW & CHANGE option logic implemented.
3. Wifi SSID changed to WiFi Network.
4. Wifi Network and password change UP & DOWN key made circular for ASCII code.
5. Saved successfull message for 3 sec after save is done.
6. Test WiFi option added in user functions.

D2.0 20.10.27
1. Download option for reboot record added in command. Command 350.7 which will give only reboot records. OK TESTED.

D1.9 20.10.26
1. Wifi SSID name entering option through LCD and Keypad within Userfunction. OK TESTED
2. Wifi PASSWORD entering option through LCD and Keypad within Userfunction. OK TESTED

D1.8 20.10.06
1. DIFarm changed to DIUse.
2. Atlas replacement data reading code is optimised. viz break from loop if garbage is received. wait Delay reduced from 5 to 2
3. In automode 2 command to read temp & flow combined and used only one command with extension on 123 command.

D1.7 20.10.05
1. Manual mode output remains on that same state untill and unless device is switch off or put the system into auto mode.

D1.6 20.09.30 
1. Watch dog activated
2. Refresh watch dog in fuction DelayInMilliSec added in code.

D1.5 20.09.30 
1. Never goes to idle.

D1.4 20.09.30 
1. Watchdog removed.

D1.3 20.09.30 
1. Watchdog timer deactivated, software reset processor code disabled.

D1.2 20.08.27 
1. WDT TEST 1: STARTED ON 20.08.27 20:00, NO RESET DETECTED TILL NOW
2. ACTIVATE CONDUCTIVITY PROBE ONES AUTO MODE STARTS. DONE
3. TOTAL FLOW NOT RECEIVED FROM CHILD BOARD. Done
4. K value for flow is by default 720. If needs to be changed send command {306=K} at project init
5. RESET issue at waste 70/75 & IDLE ENTER/57 & IDLE ENTER/185 & IDLE ENTER/313 . SOLVED    

D1.1 20.02.18 19:47
1. Temperature instead of 201 command used 123 command and used voltea formula, but error is higher than my measurement w.r.t hanna.


T3.0 OVER T2.9 Dated: 26.08.2019
1. Shunt Step, OUTPUT_HFIV set OFF.
2. N.E instead of FUL in CIP_LOW_LEVEL_ALARM_FLAG display section.
3. mmu.c line no 1656 delay removed as the download all was taking much time. I added this delay so as to resolve hang issue due to laptop buffer full.

T2.9 OVER T2.8 Dated: 11.08.2019
1. sensor activate command is fired after every log saved.
2. every 5 sec after temperature range error, recalibrate at last known good temperature.

T2.8 OVER T2.7 Dated: 09.08.2019
1. ShuntSec stored in Flash memory.
2. Reset Factory set command will reset it to default 40

T2.7 OVER T2.6 Dated: 05.08.2019
1. ShuntSec issue with LCD was conflicting with the ShuntSec variable added. Rectified

T2.6 OVER T2.5 Dated: 30.07.2019
1. ShuntSec variable added with by default value 40sec and docklight command 420.15 added to change the value. Change is only on RAM.

T2.5 OVER T2.4 Dated: 18.07.2019
1. the format on the display realigned in case of Flow Comm Err.
2. decimal point taken care during Waste Current calculation.

T2.4 OVER R4.4 Dated: 14.07.2019
1. NO alarm for the low flow waste alarm when it is set to 0.0 OR N/A instead of value.
2. Add in alarm to detect failed communication in ATLAS flow chip. Failed communication is due to change of baud rate.
   The alarm should be triggered if the PCB stops communicating with atlas flow chip for more than 30 seconds.
3. PAE current calculation debug formula added in docklight display.

T2.1 OVER T2.0 Dated: 16.05.2019
1. setpoint saved in Flash so that on power loss , PCB remembers the volume setting.
2. Litter converted to Litre

T2.0 RELEASED OVER R4.3 Dated: 16.05.2019
1. Flash memory alocated for flow and volume unit selection for gallon or Litter
2. Variable added in RAM for flow and volume unit selection for gallon or Litter
3. Display as per for flow and volume unit selection for gallon or Litter in setpoint
4. Display on LCD for flow and volume unit selection for gallon or Litter as set by user in setpoint
5. Reset factory set command initialize the VOLUME setpoint to GALLON.

R4.3 RELEASED OVER T1.9 DATED: 03.05.2019

T1.9 over T1.8 Dated: 10.04.2019
1. 5 cycle average recovery displayed on the LCD screen similar to dientry D6.5
2. 5 cycle average pure EC displayed on the LCD screen similar to dientry D6.5
3. Email Setting - OUTGOING MAIL SERVER PORT NUMBER should be hard set to 587

T1.7 over T1.6 Dated: 26.02.2019
1. Mail attempt max to 5.

T1.6 over T1.5 Dated: 20.02.2019
1. Timing to wait for response from SMTP server before initiating another round for email shoot doubled w.r.t T1.5 in order to overcome multiple email.

T1.5 over T1.4 Dated: 15.02.2019
1. Timing to wait for response from SMTP server before initiating another round for email shoot doubled in order to overcome multiple email.

T1.4 over T1.3 Dated: 08.02.2019
1. Total pure volume in gallon bug fixed.
2. LOW EC ERR changed to ZERO EC ERR in lcd alarm
3. Enter press on idle step resolves Zero EC error automatically.

T1.3 over T1.2 Dated: 06.02.2019
1. gal instead of liter and gal/m instead of lit/m

T1.2 over T1.1 Dated: 10.01.2019
1. Instead of Low Pressure alarm, low EC alarm replaced in alarm display list.

T1.1 over T1.0 Dated: 09.01.2019
1. Zero EC limit set to < 5uS

T1.1 over D9.9 Dated: 08.01.2019
1. Zero EC limit set to < 1uS

D9.9 over D9.8 Dated: 07.01.2019
1. timer added with start stop as per email dated: 4/1/2019
2. Add zero EC alarm to list of alarm. Zero Ec alarm is trigged only if 
    the Ec reading is zero for 30 seconds. If zero EC alarm is triggered put the system on idle at the end of cycle.
3. Second counter in high temperature made to upcount.

D9.8 over D9.7 Dated: 03.01.2019
1. If GSM time < PCB programmed Time. GSM is restarted.

D9.8 over D9.7 Dated: 03.01.2019
1. DEBUG_TEMPERATURE_COMPENSATED_EC_CALCULATION activated.

D9.7 OVER D9.6 Dated:14.12.2018
1. Low flow alarm to be resolved only when user press enter to go from idle to wait step.

D9.6 OVER D9.5 Dated:14.12.2018
1. Remove flow check . Low flow purify or low flow waste alarm triggering put the system in idle after completing the cycle.
2. SMS on low flow on purify and waste.

D9.5 OVER D9.4 Dated:14.12.2018
1. Flow Check average flow decimal point fixed.

D9.4 OVER D9.3 Dated:13.12.2018
1. Low flow pure and waste alarm fixed as in DiEntry.

D9.3 OVER D9.2 Dated:07.12.2018
1. Disable SMS of temperature out of range alarm.

D9.2 OVER D9.1 Dated:28.11.2018
1. if temperature out of range for 120 sec i.e 60 sec after alarm, PCB will reset.

D9.1 OVER R3.9 Dated:28.11.2018
1. TempAtWhichCali value hard coded to 25 degree.

1.R3.9 RELEASED OVER R3.8 Dated:16.11.2018
1. Temperature compensated EC calculation deactivated from being shown on docklight NOT worked in R3.8, hence rectified.

R3.8 RELEASED OVER R3.7 Dated:16.11.2018
1. Temperature compensated EC calculation deactivated from being shown on docklight NOT worked in R3.7, hence rectified.
2. PAE current calculation code reactivated from being shown on docklight.

R3.7 RELEASED OVER R3.6 Dated:16.11.2018
1. Temperature compensated EC calculation deactivated from being shown on docklight.
2. PAE current calculation code deactivated from being shown on docklight.

R3.6 RELEASED OVER D9.0 Dated:16.11.2018

D9.0 over D8.9
1. Previously Signal Strength was used to be checked only prior to server communication but from version D8.2 we added feature to check signal 
   strength in between at interval of 1 min, processor was keep checking signal strength only and was not allowing other GSM functionality. RECTIFIED.

D8.9 over D8.8
1. Percentage Recovery saved in periodic save page so that once system restarts, PCB will remember the %recovery. DONE.
2. when check gsm SS if SS<required it never checks SMS too untill SS>required. This should not happen. RECTIFIED.

D8.8 over D8.6
1. Temperature probe activate whenever mode goes to auto and hybernate whenever mode goes to logout.

D8.6 over D8.5
1. Instead of recalibration, Atlas temperature out of range alarm added.

D8.5 over D8.4
1. EC compensation formula broken down to step by step with debug

D8.4 over D8.3
1. EC compensation formula changed as per chris mail dated 07.11.2018

D8.3 over D8.2
1. Since EC is in form of *100. Multiplication factor rectified to 0.204 as per EC

D8.2 over D8.1
1. issue with temp compensated EC activation resolved.
2. GSM signal strenght read frequency set to 1 min.

D8.1 over D8.0 or R3.5
1. Activate temperature compensation
2. Addition of filter of range 0-100 degrees as in order to eliminate garbage temperature reading.
3. If the temperature reading is above 100 degrees or below 0 degrees perform temperature compensation using the last known real temperature value recorded.
4. If the garbage temperature reading is recorded for more than 60 seconds then recalibrate the temperature probe with the last real value recorded by the temperature module.
5. The new temperature calibration should be effective for the upcoming cycle and not for the live cycle
6. Additional display on the docklight to show the temperature compensation calculated values.
7. Signal strength for the GSM through command

D8.0 over D7.9
1. Default Cycle count for cip changed to 432.

D7.9 over D7.8
1. no email for any alarm. done
2. filter pae volt > 12V becoz pae cannot supply greater than 12V. done

D7.8 over D7.7
1. Temperature compensated Conductivity disabled.

D7.7 over R3.4
1. %recovery formula corrected.
2. Gprs long command delay activated.
3. EmailInitializationCompleteFlag introduced so that if email is not initialize then don't attemp to send email.
4. PAE sample collection before averaging increased from 5 to 50 due to 655.35 value being received in 5 sample collection scheme.
5. Delay of 10 time unit added during temperature probe activation soon after conductivity probe activation.
6. Last cycle Pure Avg Conductivity has issue. Resetting accumulator and counter after pure step concept used.

R3.4 RELEASED OVER D3b3 Dated:24.09.2018
changes: Step 12: waste valve check step removed.

R3.3 RELEASED OVER D7b6 Dated:20.07.2018

D7.6 over D7.5
Diuse software change-: DT7d5 to D7.6

Disable the alarm through sms
The user should be able to get daily status if the status sms is send from user
All the alarm should be send through emails.
The exit screen shows-:
Exit Process Y/N , but

we want it changed to Exit Process N/Y on screen

5. Start stop logic according to version T7d5-:

The system stop operating when the stop signal is active and then perform Flush shunt for 40 seconds and go to wait till start signal is active.
------------------------------------------------------------------------------------------------------------------------------------------------------
D7.5 over R3.2
1. Remove low flow waste and purify alarm from sms list. (No sms should be send when the alarm is triggered).
2. PAE communication is attempted only when it is powered ON via relay.
3. PAE is switched off Instead of disabling in all other step except PPFY,PUFY,WASTE, HF_WASTE. 
4. Target EC setting should be observed first on the setpoint and the rest of the set point after Target EC.
5. Change the logic for start stop operation-:
    --------------------------------------------------------------------------------------------------------------------------------------------------
   | Bypass | Signal | Operation  | Comments
   | 24 V   | 24 V   | Start      | Latch the start signal when the bypass has 24 volts and signal is 24 volts. These both condition should satisfy.
   |        |        |            |
   | 24V/0V | 0 V    | Stop       | It doesnt matter if the Bypass has 24 V or 0 V once the Signal has 0 Volts the operation should stop and go to waste, shunting and then wait for Start Signal at Bypass
    --------------------------------------------------------------------------------------------------------------------------------------------------
6. Bypass alarm condition of entire waste step duration is deactivated.
7. WOV valve to open on SHUNT step.

R3.2 over R3.1
1. From Purify to Waste step in absence of start signal, pump output volteage set to zero.


R3.1 over R3.0
1. set point for CIP runtime alarm set to 1000s
2. No matter how and from which step, if it goes to IDLE step pump output voltage set to zero.

R3.0 released over D7.1

D7.1 over D7.0
1. Setpoint for Max CIP run time changed as per edgar mail dated: 12 June 2018 i.e 28 cycle multiplied by 6sec = 168sec.
  101 840 bytes of readonly  code memory
   10 816 bytes of readonly  data memory
   10 263 bytes of readwrite data memory
   
D7.0 over D6.7
1. Setpoint changed as per edgar mail dated: 12 June 2018

D6.7
1. Instead of average of all 5 sec EC, only pure step average EC is taken for average calculation.
  101 840 bytes of readonly  code memory
   10 816 bytes of readonly  data memory
   10 263 bytes of readwrite data memory
   
D6.6
1. Temperature compensated EC programmed as compile time configurable on AtlasTC.h. OK TESTED.

D6.5
1. Fota issue with R2.5 and D5.2 and above rectified.
2. Instead of Average EC, latest completed Average Pure EC being displayed in main screen.
3. Daily Record average EC will not include zero reading EC from this version and above.
4. To accomodate changes, serial code easy migration code removed thinking all old PCB has been migrated to new serial code required for FOTA, after FOTA success.

D6.4 over D6.3
1. WV check time range 10 to 60. Factory default 40. flow average is not consider from 0 to 7th sec.
  102 152 bytes of readonly  code memory
   10 722 bytes of readonly  data memory
   10 215 bytes of readwrite data memory
WITH SINGLE DELAY FUNCTION
  102 188 bytes of readonly  code memory
   10 722 bytes of readonly  data memory
   10 215 bytes of readwrite data memory

D6.3 over D6.2
1. Logic on average flow calculation has been modified, as maximum flow is found to be within 3 sec of beginning of "waste valve check" stage hence beginning 3 sec flow is not counted on average flow calculation for this step only.
2. Number of samples taken before average current flow is chosen from the most repeating ones, has been reduced from 25 to 5, because to take 25 sample it was taking almost 5 sec which was not satisfying waste valve check minimum setpoint. 

D6.2
1. Lcd Menu idle time up-counter fix.
2. Lcd Menu all capital letter. done
  102 168 bytes of readonly  code memory
   10 722 bytes of readonly  data memory
   10 215 bytes of readwrite data memory

D6.1
1. Lcd Menu wait and idle time fix.
  102 140 bytes of readonly  code memory
   10 730 bytes of readonly  data memory
   10 215 bytes of readwrite data memory

D6.0 over D5.9
1. LCD menu and display parameter changes as per email dated: 8/05/2018. DONE
2a. The system should stop purifying immediately and jump to waste when the stop signal is given to the system. DONE
2b. The system should additionally wait for 30 seconds after waste step is complete as to resume operation. NO NEED TO WAIT
2c. The system should remain in wait till the stop signal changes to start signal. SAME AS ALL OTHER VERSIONS
2d. As soon as the signal changes to start the unit should start operating. SAME AS ALL OTHER VERSIONS
3. Upon PCB restart it will not send SMS to all those alarm for which SMS has been disabled.

D5.9 over R2.7M
1. Alarms  Yes or No only  Open. PROGRAMMED. OK TESTED
2. Front screen, replace CIP countdown with Average EC  Open PROGRAMMED. OK TESTED
3. Total Pure Volume resettable  Open  (Put it under Setpoints if possible) PROGRAMMED. OK TESTED
4. CIP count, place lower down the screen listing  Open ( Put it after the Average Pure EC screen) PROGRAMMED . OK TESTED
5. Display % recovery  Open  (Put it after Total Volume, see calculation below). OK TESTED
6. Display total waste  Open  (Put it after % recovery). OK TESTED
7. Display total treated   (Put it after total waste). OK TESTED
8. Password protect the EC target  Open. PROGRAMMED. OK TESTED

Calculation for % recovery:
(Avg. Pure Flow * Purify Time) / (Purify Time * Avg. Pure Flow + Waste Time * Avg. Waste Flow + Prepure Time * Avg. Prepure Flow) * 100

Sample calculation:
Avg Pure Flow : 2.91
Avg Waste Flow : 1.13
Avg Prepure : 1.98
Purify Time = 110
Waste Time = 80
Prepure Time = 10

% Recovery  = (2.91*110) / (110*2.91 + 80*1.13 + 10*1.98)*100
% Recovery = 74.39%

R2.7M - Modified by disabling T2a9 functionality
1. Default timing changed as 
    CIP Step 1A      : 4s
    CIP Step 1       : 117s
    MAX CIP PRT      : 930s

T2a9 over R2.9
1. Log changed to 2 sec for debug.
2. Average flow of waste valve check step is displayed at the end of waste valve check step for debug.

R2.8 over R2.7
1. ResetAverageFlow(); Function added because previous stage average was hampering the current stage especially waste value check.
2. Since Log is being recorded in 5sec. Some alarm was getting generated and getting resolved within 5sec hence only resolved sms was being sent and during resolved sms sent, alarm code includes current existing alarm so voltea was thinking that SMS is being sent for those alarm for which SMS was not supposed to sent. RESOLVED BY USING AlarmFIFO function.

R2.7 over R2.6
1. averaging logic changed to accurate one on average pure volt, average waste volt and average EC.
2. Alarm on daily log was getting inserted only 1 byte but while sending reading 3 byte hence giving missmatch. RECTIFIED.

R2.6 over R2.5
1. Daily log table alarm field remained 1 byte but it has been extended to 3 byte, hence in email alarm value was comming out random. RECTIFIED.
2. CycleCounter Display Garbage character limited to Values that can be displayed.
3. CycleCounter value can be set from docklight for testing purpose.

R2.5 over R2.4
1. Password set to 40 character. DONE
2. Update SMTP settings with the following default values. DONE
    Username:                          0993b74745796f945e8778994619ad6f
    Password:                           98dd545c5932ca902e74cba4debd1fb5
    Sender address:                data@voltea.com
    outgoing smtp server:    in-v3.mailjet.com
    outgoing smtp port:        25
3. CIP pulse step on memory to remember the time elapses even during system restart. (same change on D221) DONE.
4. Disable the SMS notification for 80h and the 0h that comes after the restart of the PAE during the 80h alarm. DONE

D5.2 
1. PCB will factory reset to D5.1 default setting whenever restarts. Only for testing purpose. And soon later removed after creating D5.2

R2.4
Released version or D5.1

D5.1 over 5.0
1.Update the factory setpoint settings for the following
CIP Step 1A : 4s. done
CIP Step 1 : 117s done
Max CIP P.R.T : 930 done
2.Enable shunt during the entire duration of CIP done
3. shunt (PVR) relay shunting time increased to 250ms from 50 ms
4. manual option of operating PVR relay added.

D5.0 over R2.3
1. shunt (PVR) relay shunting time decreased to 50ms from 1sec

R2.3
1. D4.9 and R2.2 merged

D4.9
1. PVR relay has no functionality after adding PAE power control through PUMP relay, hence PVR relay on during power up made OFF.
2. PVR relay get ON everytime for 1 sec before NVR relay switches.

R2.2
1. disable the sms alert for alarm 0X8000h

R2.1
1. disable the sms alert for alarm 0X20000h and 0x40000h. 0x20000h is the atlas i2c alarm, 0x40000 is the initial zero EC alarm.
2. PAE waste and pure max current reversed to 6600 & 4200 amp respectively, same as D4.5.

//D4.8
1. Temperature compensated conductitivity DEACTIVATED.

D4.7
1. Max current for waste limited to 5940 and pure to 3780

D4.6
1. Temperature compensated conductitivity
  102 044 bytes of readonly  code memory
   10 712 bytes of readonly  data memory
   10 235 bytes of readwrite data memory

D4.5
1. If Atlas I2C error persist for 200 reading, log as alarm. If it persist for 250 times then software restart.
2. DPRT removed from Reporting

D4.3
1. PCB will consider PAE less than 0.5 volt and less than 0.5 Amper current as zero. 
	Not successfull on testing as PAE is taking more than 50 sec to settle down. CONCEPT REJECTED.

D4.2
1. Time taken to settle the current and voltage to zero is shown on docklight

D4.1
1. Whenever Relay has to toggle (ON to OFF or OFF to ON), first command PAE to zero current & voltage, then switch relay and them final command as per process. 

D4.0 is extended version of R2.0
1. I2C comm error is logged only when 250 attempt read fails continuously.
2. PAE status error is logged only when 10 slot of pae status error buffer have same value.
3. SAVED msg in display in automode target EC changed.

D3.9
1. Make data2@voltea.com as by default email receiptant.

D3.8
1. More smooth flow by max min adjustment.
2. Default timezone set to GMT.
3. In Report Conductivity changed to EC.
4. Restart to DiUse removed and Restart goes to actual step that was before restarting.
5. Serial No in Email was reading from wrong address.
6. Instead of IMEI serial No are used in SMS, Email and FOTA communication, where ever possible as Serial No is now more reliable.
7. Instead of RAM corrupt restart, RAM reload with restart concept used.
8. One Second Delay before reading IMEI which was rectified long ago found to be deactivated in current version. Find working of OneDrive.

D3.7
1. Instead of Alarm whole message, only alarm code is sent via SMS to active user. This is done just because of more than 5 alarm active then it may exceed  the SMS character limit of 160 and may cause issue.
2. PeriodicSave function was getting recursive. RECTIFIED by placing after LogRecord Funtion within main while loop.
3. ReadAtlasConductivity() function during CIP was creating RAM CORRUPT. hence conductivity is read on those step which is less than WAIT. After that
	no RAM CORRUPT found, atleast in CIP process.
	
  102 416 bytes of readonly  code memory
   10 923 bytes of readonly  data memory
   10 174 bytes of readwrite data memory

D3.6
1. Dynamic Pump Control with successive appromixation technique. DONE
2. SMS Report will have device version at the begining. DONE

R1.9 Which is D3.6 with D3.5 with my pump control
1. Dynamic Pump Control with formula disabled and my logic enabled. 

D3.5
1. instead of TPV/LAST CYCLE VOL. ONLY TPV. DONE
2. in AUTO_0_1 unit changed from uS to uS/cm. DONE
3. AUTO_0_3 removed from auto step. If req^ in future then activate in up/down key code. DONE
4. instead of L/M changed it to L/min. DONE.

D3.4
1. Serial No in Device Info-> Serial Number was showing incorrect value. RECTIFIED.
2. Fota Page Request code was disabled during code optimization, hence fota was not working for version D3.0 to D3.3. RECTIFIED.
3. Dynamic Pump control was going zero once it reached to its maximum limit becoz of unit diff. RECTIFIED.

  103 556 bytes of readonly  code memory
   11 643 bytes of readonly  data memory
   10 143 bytes of readwrite data memory

D3.3
1. Dynamic flowmeter sampling formula corrected.
2. sample constants changed to 7 instead of 11
3. max & min value of flow p factor canged to (0.04 <= Pump P.Factor <= 2.00)

D3.2
1. if serial number is not valid, upon stating process Display page to be moved to Device Info->Serial No as the test team is not able figure out why
	system is not running.
2. Serial Info moved to Security Register (One time write only).
3. 5sec loging DONE
4. Zero EC 18th binary loc DONE
5. bypass back to bypass DONE

a. Command.c //Temperature probe Active/Hybernate setting -> CopyBuff was used instead of PasteBuff
b. Command.c //Atlas Temperature Calibration Register -> CopyBuff was used instead of PasteBuff
c. Command.c -> case 712: {	//Calibration Request Register -> CopyBuff was used instead of PasteBuff
d. Command.c -> case 606: { //Conductivity Probe Active/Hybernate Function  -> CopyBuff was used instead of PasteBuff
e. Command.c -> case 608: {	//Probe Type register   -> CopyBuff was used instead of PasteBuff
f. Command.c -> case 610: {	//Calibration Register   -> CopyBuff was used instead of PasteBuff
g. Command.c -> case 614: {	//Calibration Request Register  -> CopyBuff was used instead of PasteBuff
h. Command.c -> case 616: {	//Temperature compensation Register  -> CopyBuff was used instead of PasteBuff
i. mmu.c -> In daily log table download, intead of daily alarm field, second table alarm field was being used. CORRECTED.

  103 688 bytes of readonly  code memory
   11 263 bytes of readonly  data memory
   10 175 bytes of readwrite data memory
	
D3.1
1. During Automode, if PCB switched off, then PCB does not activate the Atlas Conductivity module and Atlas Conductivity module remains in hybernate state. 
	Activation code added before going to automode.
2. PAE Dynamic control enabled.
 102 388 bytes of readonly  code memory
   11 235 bytes of readonly  data memory
   10 191 bytes of readwrite data memory

D3.0
1. Code optimization done.
2. ATLAS_I2C_COMM_ERROR alarm was getting merged with LOW PRESSURE ALAARM. Hence PCB was wrongly resolving it looking at ATLAS_I2C_COMM_ERROR. SEPERATED AFTER CHRIS REPORT.

  102 812 bytes of readonly  code memory
   11 187 bytes of readonly  data memory
   10 207 bytes of readwrite data memory


D2.9
1. Low flow (pure & waste) alarm setpoint was used to show on lcd and edit but never used for alarm generation due to code merging of arun. RECTIFIED
2. PUMP DYNAMIC CONTROL ON EVERY FLOW METER CURRENT FLOW CHANGE ON PURIFY STAGE
3. 20 blank space on email at the start.
4. slow down PAE by twice.
5. Message where RAM corrupt found.
6. StepsBeforeSwitchingPolarity function disabled.
7. Atlas I2C communication Error Added in alarm.

  104 980 bytes of readonly  code memory
   11 543 bytes of readonly  data memory
   10 245 bytes of readwrite data memory


D2.8
1. Before switching polarity, PAE is set to 0 volt 0 current, and it will display status as well as time taken to settle down on docklight.
  104 616 bytes of readonly  code memory
   11 519 bytes of readonly  data memory
   10 253 bytes of readwrite data memory

D2.7
1. Alarm increased to 3 byte from 2 byte in Database.
2. LOW PRESSURE ALARM added.
3. FLOW_CHECK with ID=15 step added.
4. CIP pump runtime added in database and in all reports.

  104 304 bytes of readonly  code memory
   11 519 bytes of readonly  data memory
   10 252 bytes of readwrite data memory

D2.6
1. PUFY on LCD changed to PURE.
2. PAE enable function enabled. Dynamic Control Disabled. Manual control enabled.

D2.5
1. PAE enable function disabled

D2.4
1. alarm code in report download remain 1 byte in all previous version, updated to 2 byte in D2.4 onwards
2. monthly and yearly report disabled

 D2.2
 1. Instead of 25 sample of average EC logged into memory, average pure EC is being logged.
 2. bypass alarm has been assigned to pure EC being zero.
 
 R1.8
  104 312 bytes of readonly  code memory
   11 479 bytes of readonly  data memory
   10 291 bytes of readwrite data memory
1. get alarm command deactivated
2. 325 command code added which will return current status of the system.

D1.8
  104 088 bytes of readonly  code memory
   11 547 bytes of readonly  data memory
   10 291 bytes of readwrite data memory

1. DisplayPage option was 150 but the variable was assigned as char which range from -128 to +127 hence it was mal functioning. Converted to UC.

D1.7
Edgar debug firmware

R1.5
D1.6 IS REALEASED AS R1.5

D1.6
  104 088 bytes of readonly  code memory
   11 547 bytes of readonly  data memory
   10 291 bytes of readwrite data memory

After using VariableUnsignedInt2UserInput function
  104 088 bytes of readonly  code memory
   11 547 bytes of readonly  data memory
   10 291 bytes of readwrite data memory

after using UserInput2UnsignedInt function 
after removing variable KeyTimeOut
  105 748 bytes of readonly  code memory
   11 703 bytes of readonly  data memory
   10 291 bytes of readwrite data memory

  106 300 bytes of readonly  code memory
   11 775 bytes of readonly  data memory
   10 298 bytes of readwrite data memory

  106 332 bytes of readonly  code memory
   11 839 bytes of readonly  data memory
   10 298 bytes of readwrite data memory
   
  106 284 bytes of readonly  code memory
   11 839 bytes of readonly  data memory
   10 298 bytes of readwrite data memory

1. Arrow on User Function tab corrected.
2. CIP dosing sec is saved into memory so that if system restart during CIP then system remembers from where to start doing CIP.
3. option to reset the counter when they refill the CIP bottle : It will display FUL in alarm status if refilled.

D1.5
  106 284 bytes of readonly  code memory
   11 839 bytes of readonly  data memory
   10 298 bytes of readwrite data memory

  106 380 bytes of readonly  code memory
   11 839 bytes of readonly  data memory
   10 298 bytes of readwrite data memory
   
  106 626 bytes of readonly  code memory
   11 981 bytes of readonly  data memory
   10 298 bytes of readwrite data memory

1. unditional Jump to step command reactivated.
2. Some code optimization done in HMI part and command part.

R1.3
  106 926 bytes of readonly  code memory
   12 005 bytes of readonly  data memory
   10 298 bytes of readwrite data memory

1. PAE comm error at bit 8 of alarm[0] reg
2. PAE status reg value to be in alarm[1] reg

R1.2
106 954 bytes of readonly  code memory
 11 521 bytes of readonly  data memory
 10 292 bytes of readwrite data memory
   
1. PAE send cmd debug message disabled by default. PAE & ATLAS I2C error will be shown on docklight if there is an error.
2. Serial number generation command changed to 1980.
3. Server side debug enabled at server 0. if sever 0 is enabled, device will send every thing that is debug enable to server 0 as well.
	Once debug is done, server has to send "[2,0000,DBG,520.0=0*975]" to device to exit from debug mode, else it will try to connect server 0
	again and again and it will not try any other server untill server 0 is disabled.
	Once in debug mode, device will not detect call or SMS untill connection is closed either by force close or exit debug command.
4. Divide by 100 in Current flow in download all data was missing. DONE
	

R1.1
1. CIP end flush default was 1800s, now should be 180s
2. LoadSetting was switching off the PVR relay which was made ON during initialization, so UpdateOutput code removed from LoadSetting code.
3. Changes made on Watch dog timer version T1.1 has been reversed from 0X00ffffFF to 0X006fffFF.

T1.1
1. Watch dog timer has been increased from 0X006fffFF to 0X00ffffFF
2. LoadSetting was switching off the PVR relay which was made ON during initialization, so UpdateOutput code removed from LoadSetting code.

D1.8
1. blank record will not be sent upon download and email.

D1.7
1. email with YYYY in year.

D1.6
1. Last cycle Waste Volume has been shifted to periodic save database. 

D1.5
1. Default factory set values changed as per spec.sheet v1_7.2

D1.4
1. PVR relay everytime ON after disabling the output during powerON.
2. Sec diff for log calculation was error. Variable type changed from uc to struct tm.

D1.3
1. Display log on docklight date format changed to DayOfTheWeek-20YY/MM/DD. 
2. Debug display added for total flow > 0.5 litter per sec.

D1.2
1. Display log on docklight date format changed to DayOfTheWeek-YY/MM/DD.

D1.1
1. PAE comm error msg not being displayed in version D1.0 so rectified it.

D1.0
1. CurrentPAEvolt is the median of 5 sample.
2. PAE communication error being displayed in Docklight code activated.

R4.9
1. if current calculation result becomes negative, since ui datatype is used, it converts it to positive which was becoming over the limit, 
	hence highest current possible was being supplied to module. Now if any of the calculation result becomes negative then both the pure and
	waste current of next cycle is set to zero.
2. "TOT.WASTE VOLUME" changed to " LAST WASTE VOL " in LCD display.

R4.8
1. LastCyclePureAverageConductivity reading upon restart had an error on addressing HSB. RECTIFIED.
2. Data mode to command mode delay in mobile function returned to 1000 ms, as in some mail some part of body was missing.

R4d7
1. On restart due to hang, flowmeter doesn't restart and will give the total flow as it is in flowmeter
	but controller thinks that it too has restarted and will give the reading from zero onwards which spikes up the total flow value,
	hence clear the flowmeter reading to sync with controller.
2. If need to log and log doesn't happening then, msg will be shown on docklight if log view is enabled.

R4.7
1. meadian technique to filter out conductivity extreame values.
2. conductivity average has taken wide range (10 < average no of sample < 250)
3. Response wait timing in GSM module decreased.
4. low flow alarm that was deactivated in R4d6, activated again.

R4d6
1. meadian technique to filter out flowmeter extream values.
2. deactivated low flow alarms.

R4.6
1. Watch dog timer has been increased from 0X005fffFF to 0X006fffFF

R4.5
1. LastCyclePureAverageConductivity will be saved in periodic save page. If system restarts at waste stage LastCyclePureAverageConductivity will 
	be read and will be used in formula for current calculation. DONE
2. 10 milli second delay has been added in continuous write to PAE register during EnablePAEoutput section.

R4.4
1. If in case controller restart at purify stage, AverageConductivityFlag remained FALSE because of default initialization, which should be true
	upon restart at purify stage. DONE.
2. OPTIMIZED_PAE_CURRENT_FOR_PURE_STEPa & OPTIMIZED_PAE_CURRENT_FOR_WASTE_STEPa shifted to periodic save page in MMU.

R4.3
1. while deactivating the PAE calculation display in docklight, the real calculation got deactivated. Hence activated again. DONE

R4.2
1. Averaging the Conductivity with concept of accumulate 25 samples and Divide at the end of every 25 sample accumulation. DONE

R4.1
1. MobileRoutine that was deactivated on R4.0, activated again.

R4.0
1. sender name has been omitted from email section. DONE

R3.9
1. Instead of total flow , use total flow in pure stage only. DONE.
2. When setpoint value gets changed and its saved, Saved is displayed for few millsecond to one second on LCD. DONE

R3.8
1. Averaging the flow with concept of accumulate 25 samples and Divide at the end of every 25 sample accumulation. DONE
2. Change password command added on docklight. DONE
3. CD0N = CDOF PARALLEL ACTION TO BE DISABLED.
4. CIP_LOW_LEVEL_ALARM_FLAG alarm msg was missing in SMS. DONE
5. instead of imei use serial no. DONE. both are included in email.
6. leakage alarm goes to idle. upon enter clear the alarm. DONE. but not tested

R3.7
1. Conductivity calibration in simple buttons. DONE. but not tested

R3.6
1. Email Username setting was gettting disturbed during reset factroy set command. DONE
2. All alarm was getting Off once user logout from the system. DONE. 
3. Alarm trigger, email to all registered mail id to the system if admin email ID is present. DONE
4. Flow filter: Instant change in flow greater than 30 liter per min discarded. Change in total flow greater than 0.5 liter per sec is discarded. DONE but not able to test.
5. alarm with date of alarm generation. DONE

R3.5
1. Max CIP RunTime alarm introduced. The generated Alarm Clearing will be from alarm section in automode.
2. Bypass alarm is checked for whole cycle at waste stage only. Alarm is generated if Bypass Sig remains High for whole time during waste stage and is cleared if it remains low for whole time during waste stage. DONE.
3. cycle counter garbage value "X95" or "X89" on display was due to unconditional stage shift. if cycle counter is 0 and heading towards CIP, before CIP is complete if forced to branch to stage earlier than HF WASTE which will lead to negative value, hence found display like this. Upon rectification Cycle counter will never go negative. DONE.

R3.4
1. Alarm indication at right bottom '!'. DONE
2. Once the CIP cycle is over, variable has to be reinitialized to its original process. DONE. Unconditional Jump case has to be tested. 
3. Logout and restart, process starts automatically sometimes. Variable initialization issue. DONE
4. Waste valve alarm to go on when flow > 0.3. DONE BUT NOT TESTED.
5. Alarm in binary format to display in LCD but will remain in Hex in Email. DONE. But this feature will allow you to have max of 8 alarm now and in future. If the number of alarms gets increased by 8 we have to return to our hex format.

R3.3
1. Deactivated parallel process of auto and download and date change email feature as system was misbehaving.
2. Jump to step command added.

R3.2
1. Total flow clear feature added. Same Command "CLEAR Flowmeter Reading" will reset atlas as well as PCB flowmeter recorded value to zero.
2. Serial HMI buffer overflow condition handled.
3. Introduced self healing feature: 
		a. if automode timing is not correctly displayed in LCD and not resolved within 10 sec then system will re initialize that part of code again within 10 more sec without affecting the process.

R3.1
1. During flowmeter initialization, first clear k factor before setting k factor into the module. DONE.
2. Make K factor for flowmeter as setpoint. DONE
3. Log on docklight screen activate/deactivated by command, by default deactivated. DONE
4. CIP process timing rectified.
5. code optimization during startup STARTUP SECTION has been done.

R3.0.3
1. During restart and if system starts up in prepurify or purify or waste or hf waste, PAE should be supplied with associated volt/current.

R3.0.2
1. Relay status at each step as per chris 2nd email dated 03/12/17. DONE

R3.0.1
1. Relay status at each step as per chris email dated 03/12/17. DONE
2. Current calculation mistake corrected and activated. chris manual control deativated. DONE
3. Email programmed with personal email id and tested but need to test with voltea email id. DONE
4. Command to yearly download via usb added. DONE
5. Command to request PCB to send email daily, monthly, yearly data to admin added. DONE
6. Automatic emailing system once UTC date/month/year change to admin added. DONE


R3.0
1. PUMP relay is always disabled  should be enabled for all steps except pure. DONE ok tested
2. Upon PowerON, device on logout mode. Press enter to automode-idle stage. Press enter to goto wait stage. Wait stage will check
	input signal to go to prepurify. DONE ok tested
3. Alarms DONE ok tested
4. added user functions in the menu and flush implemented. DONE ok tested

R2.9
1. PVR linked with PAE800 power supply.
2. flowmeter total flow gets reset to zero from atlas module on power on, so during powerup device checks the latest rec and reads its last recorded total flow. Now onwards whenever device reads atlas total flow it adds powerup flow to the total flow before using it
3. second table time sync
4. PUMP relay made off during purify step
5. if power on restart, the process restart from last periodic save time step & process second. Data of max 5 min may be lost.

R2.8
1. If cycle setpoint gets changed via any means(sms, hmi, usb) then if remaining cycle is higher than cycle, match the remaining with setpoint.
R2.7
1. Restore Factory Default making all relay to go ON. DONE ok tested.
2. HF waste setpoint default value error. DONE ok tested
3. P.Max Volt default value error. DONE ok tested
4. W.Max Volt default value error. DONE ok tested
5. Pure EC Target Default value error. DONE ok tested
6. W_Value default value and its change error. DONE ok tested
7. PAE output voltage set to zero every time device restart. (Problem: it takes almost 5 sec to give command)


R2.6
1.  Sensor read/Conductivity-Please change to no decimal points, and change unit to uS/cm DONE. ok tested
2.  Setpoints/Volt P-factor is linked to T.Conductivity DONE ok tested
3. Setpoints/T.Conductivity is linked to w_value DONE ok tested
4. Setpoints/w_value is not linked to anything DONE ok tested
5.  Sensor read/Flow doesnt update automatically, and doesnt update at all when Timebase is set to M. Flow timebase should be M. DONE ok tested
6.  Issues with flowmeter calibration  flow is not giving reasonable values. NEEDS MORE TIME
o   K factor is 840pulses/L, timebase in M, output in L/min. CALIBRATED AT OFFICE
7.  How do I calibrate temperature? At the moment, it reads 20.4 when it is 20.6C (so it looks ok)
8. How do I calibrate EC? Reads 1014 when at 1413uS/cm. This is an unacceptable margin!
9.       Remember that Target Conductivity (T.Conductivity) is the same as Outlet Conductivity (Pure EC Target), the variable we already have in the setpoints table!
o   This means that we can change the Pure EC Target both in setpoints and under the automatic process DONE

P1.B1.H1.F1.R2.5
·         Setpoints  please update setpoints as outlined in the table in previous email
o   Pump P-factor default value incorrect DONE
o   Dyna P-factor default value & max value incorrect DONE
o   P.Cycle Volt name and default value incorrect DONE
o   W.Cycle Volt name and default value incorrect DONE
o   V. P-factor isnt linked to anything. What is this used for? I was not aware that we need a P-factor for voltage. DONE
o   T.Conductivity shouldnt exist, please use Pure EC Target setpoint we already have for the current calculation DONE
o   W_factor is not linked to anything DONE
·         Automatic Process
o   PAE is outputting 12V all the time, even when not in automatic process
o   Instant Flow does not display anything DONE. ok tested
o   Avg Pure EC
§  doesnt have a right arrow DONE
§  has too many decimal points (should be no DP) DONE
§  doesnt show Average Pure EC, instead it shows instant pure EC DONE
o   Automatic process crashes randomly on step-to-step transitions and stops operation. This is unacceptable  at the moment I have not managed to run a complete cycle

R2.5
1. The code changed in R2.4 for GPRS PWRKEY to remain in low state untill AT command response had issues as GPRS_GPIO_INTI was again making the PWRKEY high. RESOLVED

R2.4
1. Auto process sensor functionality implemented.

R2.3
1. 4 setpoint added
2. sensor current value activated in sensor read within hardware testing page.

Testing version R0.1
1. OK TESTED TILL SERIAL.
2. OUTPUT 8 IS NOT ASSIGNED TO ANY DO PIN YET.
3. 2 DI INPUT IS NOT WORKING YET.
4. SERIAL OUT 1 TRACK HAS BEEN CUT.

FIRST VERSION R1.0
started coding




