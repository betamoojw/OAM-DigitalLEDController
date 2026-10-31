#pragma once


#define paramDelay(time) (uint32_t)( \
            (time & 0xC000) == 0xC000 ? (time & 0x3FFF) * 100 : \
            (time & 0xC000) == 0x0000 ? (time & 0x3FFF) * 1000 : \
            (time & 0xC000) == 0x4000 ? (time & 0x3FFF) * 60000 : \
            (time & 0xC000) == 0x8000 ? ((time & 0x3FFF) > 1000 ? 3600000 : \
                                         (time & 0x3FFF) * 3600000 ) : 0 )
                                             
#define ETS_ModuleId_NONE 0
#define ETS_ModuleId_BASE 1
#define ETS_ModuleId_UCT 2
#define ETS_ModuleId_APP 3
#define MAIN_FirmwareName "Digital LED Controller"
#define MAIN_OpenKnxId 0xAF
#define MAIN_ApplicationNumber 1
#define MAIN_ApplicationVersion 1
#define MAIN_ApplicationEncoding iso-8859-15
#define MAIN_ParameterSize 124
#define MAIN_MaxKoNumber 227
#define MAIN_OrderNumber "OFM-DigitalLEDController"
#define BASE_ModuleVersion 23
#define UCT_ModuleVersion 5
#define APP_ModuleVersion 1
// Parameter with single occurrence


#define BASE_StartupDelayBase                     0      // 2 Bits, Bit 7-6
#define     BASE_StartupDelayBaseMask 0xC0
#define     BASE_StartupDelayBaseShift 6
#define BASE_StartupDelayTime                     0      // 14 Bits, Bit 13-0
#define     BASE_StartupDelayTimeMask 0x3FFF
#define     BASE_StartupDelayTimeShift 0
#define BASE_HeartbeatDelayBase                   2      // 2 Bits, Bit 7-6
#define     BASE_HeartbeatDelayBaseMask 0xC0
#define     BASE_HeartbeatDelayBaseShift 6
#define BASE_HeartbeatDelayTime                   2      // 14 Bits, Bit 13-0
#define     BASE_HeartbeatDelayTimeMask 0x3FFF
#define     BASE_HeartbeatDelayTimeShift 0
#define BASE_Timezone                             4      // 5 Bits, Bit 7-3
#define     BASE_TimezoneMask 0xF8
#define     BASE_TimezoneShift 3
#define BASE_CombinedTimeDate                     4      // 1 Bit, Bit 2
#define     BASE_CombinedTimeDateMask 0x04
#define     BASE_CombinedTimeDateShift 2
#define BASE_SummertimeAll                        4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeAllMask 0x03
#define     BASE_SummertimeAllShift 0
#define BASE_SummertimeDE                         4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeDEMask 0x03
#define     BASE_SummertimeDEShift 0
#define BASE_SummertimeWorld                      4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeWorldMask 0x03
#define     BASE_SummertimeWorldShift 0
#define BASE_SummertimeKO                         4      // 2 Bits, Bit 1-0
#define     BASE_SummertimeKOMask 0x03
#define     BASE_SummertimeKOShift 0
#define BASE_TimezoneCustom                       5      // char*, 63 Byte
#define     BASE_TimezoneCustomLength 63
#define BASE_Latitude                            69      // float (4 Byte)
#define BASE_Longitude                           73      // float (4 Byte)
#define BASE_Diagnose                            78      // 1 Bit, Bit 7
#define     BASE_DiagnoseMask 0x80
#define     BASE_DiagnoseShift 7
#define BASE_Watchdog                            78      // 1 Bit, Bit 6
#define     BASE_WatchdogMask 0x40
#define     BASE_WatchdogShift 6
#define BASE_ReadTimeDate                        78      // 1 Bit, Bit 5
#define     BASE_ReadTimeDateMask 0x20
#define     BASE_ReadTimeDateShift 5
#define BASE_HeartbeatExtended                   78      // 1 Bit, Bit 4
#define     BASE_HeartbeatExtendedMask 0x10
#define     BASE_HeartbeatExtendedShift 4
#define BASE_InternalTime                        78      // 1 Bit, Bit 3
#define     BASE_InternalTimeMask 0x08
#define     BASE_InternalTimeShift 3
#define BASE_ManualSave                          78      // 3 Bits, Bit 2-0
#define     BASE_ManualSaveMask 0x07
#define     BASE_ManualSaveShift 0
#define BASE_PeriodicSave                        79      // 8 Bits, Bit 7-0
#define BASE_Info1LedFunc                        80      // 16 Bits, Bit 15-0
#define BASE_Info2LedFunc                        82      // 16 Bits, Bit 15-0
#define BASE_Info3LedFunc                        84      // 16 Bits, Bit 15-0
#define BASE_DefaultLedFunc                      86      // 1 Bit, Bit 7
#define     BASE_DefaultLedFuncMask 0x80
#define     BASE_DefaultLedFuncShift 7
#define BASE_Dummy                               109      // uint8_t
#define BASE_ModuleEnabled_UCT                   110      // 1 Bit, Bit 6
#define     BASE_ModuleEnabled_UCTMask 0x40
#define     BASE_ModuleEnabled_UCTShift 6
#define BASE_ModuleEnabled_APP                   110      // 1 Bit, Bit 5
#define     BASE_ModuleEnabled_APPMask 0x20
#define     BASE_ModuleEnabled_APPShift 5

// Zeitbasis
#define ParamBASE_StartupDelayBase                    ((knx.paramByte(BASE_StartupDelayBase) & BASE_StartupDelayBaseMask) >> BASE_StartupDelayBaseShift)
// Zeit
#define ParamBASE_StartupDelayTime                    (knx.paramWord(BASE_StartupDelayTime) & BASE_StartupDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_StartupDelayTimeMS                  (paramDelay(knx.paramWord(BASE_StartupDelayTime)))
// Zeitbasis
#define ParamBASE_HeartbeatDelayBase                  ((knx.paramByte(BASE_HeartbeatDelayBase) & BASE_HeartbeatDelayBaseMask) >> BASE_HeartbeatDelayBaseShift)
// Zeit
#define ParamBASE_HeartbeatDelayTime                  (knx.paramWord(BASE_HeartbeatDelayTime) & BASE_HeartbeatDelayTimeMask)
// Zeit (in Millisekunden)
#define ParamBASE_HeartbeatDelayTimeMS                (paramDelay(knx.paramWord(BASE_HeartbeatDelayTime)))
// Zeitzone
#define ParamBASE_Timezone                            ((knx.paramByte(BASE_Timezone) & BASE_TimezoneMask) >> BASE_TimezoneShift)
// Empfangen über
#define ParamBASE_CombinedTimeDate                    ((bool)(knx.paramByte(BASE_CombinedTimeDate) & BASE_CombinedTimeDateMask))
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeAll                       (knx.paramByte(BASE_SummertimeAll) & BASE_SummertimeAllMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeDE                        (knx.paramByte(BASE_SummertimeDE) & BASE_SummertimeDEMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeWorld                     (knx.paramByte(BASE_SummertimeWorld) & BASE_SummertimeWorldMask)
// Sommerzeit ermitteln durch
#define ParamBASE_SummertimeKO                        (knx.paramByte(BASE_SummertimeKO) & BASE_SummertimeKOMask)
// POSIX TZ-String
#define ParamBASE_TimezoneCustom                      (knx.paramData(BASE_TimezoneCustom))
#define ParamBASE_TimezoneCustomStr                   (knx.paramString(BASE_TimezoneCustom, BASE_TimezoneCustomLength))
// Breitengrad
#define ParamBASE_Latitude                            (knx.paramFloat(BASE_Latitude, Float_Enc_IEEE754Single))
// Längengrad
#define ParamBASE_Longitude                           (knx.paramFloat(BASE_Longitude, Float_Enc_IEEE754Single))
// Diagnoseobjekt anzeigen
#define ParamBASE_Diagnose                            ((bool)(knx.paramByte(BASE_Diagnose) & BASE_DiagnoseMask))
// Watchdog aktivieren
#define ParamBASE_Watchdog                            ((bool)(knx.paramByte(BASE_Watchdog) & BASE_WatchdogMask))
// Bei Neustart vom Bus lesen
#define ParamBASE_ReadTimeDate                        ((bool)(knx.paramByte(BASE_ReadTimeDate) & BASE_ReadTimeDateMask))
// Erweitertes "In Betrieb"
#define ParamBASE_HeartbeatExtended                   ((bool)(knx.paramByte(BASE_HeartbeatExtended) & BASE_HeartbeatExtendedMask))
// InternalTime
#define ParamBASE_InternalTime                        ((bool)(knx.paramByte(BASE_InternalTime) & BASE_InternalTimeMask))
// Manuelles speichern
#define ParamBASE_ManualSave                          (knx.paramByte(BASE_ManualSave) & BASE_ManualSaveMask)
// Zyklisches speichern
#define ParamBASE_PeriodicSave                        (knx.paramByte(BASE_PeriodicSave))
// Info1
#define ParamBASE_Info1LedFunc                        (knx.paramWord(BASE_Info1LedFunc))
// Info2
#define ParamBASE_Info2LedFunc                        (knx.paramWord(BASE_Info2LedFunc))
// Info3
#define ParamBASE_Info3LedFunc                        (knx.paramWord(BASE_Info3LedFunc))
// 
#define ParamBASE_DefaultLedFunc                      ((bool)(knx.paramByte(BASE_DefaultLedFunc) & BASE_DefaultLedFuncMask))
// 
#define ParamBASE_Dummy                               (knx.paramByte(BASE_Dummy))
// UCT
#define ParamBASE_ModuleEnabled_UCT                   ((bool)(knx.paramByte(BASE_ModuleEnabled_UCT) & BASE_ModuleEnabled_UCTMask))
// APP
#define ParamBASE_ModuleEnabled_APP                   ((bool)(knx.paramByte(BASE_ModuleEnabled_APP) & BASE_ModuleEnabled_APPMask))

#define BASE_KoHeartbeat 1
#define BASE_KoTime 2
#define BASE_KoDate 3
#define BASE_KoDateTime 4
#define BASE_KoIsSummertime 5
#define BASE_KoManualSave 6
#define BASE_KoDiagnose 7

// In Betrieb
#define KoBASE_Heartbeat                           (knx.getGroupObject(BASE_KoHeartbeat))
// Uhrzeit
#define KoBASE_Time                                (knx.getGroupObject(BASE_KoTime))
// Datum
#define KoBASE_Date                                (knx.getGroupObject(BASE_KoDate))
// Uhrzeit/Datum
#define KoBASE_DateTime                            (knx.getGroupObject(BASE_KoDateTime))
// Sommerzeit aktiv
#define KoBASE_IsSummertime                        (knx.getGroupObject(BASE_KoIsSummertime))
// Speichern
#define KoBASE_ManualSave                          (knx.getGroupObject(BASE_KoManualSave))
// Diagnose
#define KoBASE_Diagnose                            (knx.getGroupObject(BASE_KoDiagnose))



#define APP_RequestXInterval                    114      // 8 Bits, Bit 7-0
#define APP_NightlightDuration                  115      // uint8_t
#define APP_NightlightMode                      116      // 2 Bits, Bit 7-6
#define     APP_NightlightModeMask 0xC0
#define     APP_NightlightModeShift 6
#define APP_NightlightTargetBrightness          117      // uint8_t
#define APP_DefaultBrightness                   118      // uint8_t
#define APP_DefaultPalette                      119      // uint8_t
#define APP_DefaultEffect                       120      // uint8_t
#define APP_DefaultEffectSpeed                  121      // uint8_t
#define APP_DefaultEffectIntensity              122      // uint8_t

// WLED Statusabfrage
#define ParamAPP_RequestXInterval                    (knx.paramByte(APP_RequestXInterval))
// Nachtlichtdauer (Minuten)
#define ParamAPP_NightlightDuration                  (knx.paramByte(APP_NightlightDuration))
// Nachtlichtmodus
#define ParamAPP_NightlightMode                      ((knx.paramByte(APP_NightlightMode) & APP_NightlightModeMask) >> APP_NightlightModeShift)
// Nachtlicht Zielhelligkeit
#define ParamAPP_NightlightTargetBrightness          (knx.paramByte(APP_NightlightTargetBrightness))
// Standardhelligkeit
#define ParamAPP_DefaultBrightness                   (knx.paramByte(APP_DefaultBrightness))
// Standardpalette
#define ParamAPP_DefaultPalette                      (knx.paramByte(APP_DefaultPalette))
// Standardeffekt
#define ParamAPP_DefaultEffect                       (knx.paramByte(APP_DefaultEffect))
// Effektgeschwindigkeit
#define ParamAPP_DefaultEffectSpeed                  (knx.paramByte(APP_DefaultEffectSpeed))
// Effektintensität
#define ParamAPP_DefaultEffectIntensity              (knx.paramByte(APP_DefaultEffectIntensity))

#define APP_KoChDimSw 201
#define APP_KoChDimRel 202
#define APP_KoChDimAbs 203
#define APP_KoChDimState 204
#define APP_KoChDimValue 205
#define APP_KoChScene 206
#define APP_KoChSceneState 207
#define APP_KoChRed 208
#define APP_KoChRedValue 209
#define APP_KoChGreen 210
#define APP_KoChGreenValue 211
#define APP_KoChBlue 212
#define APP_KoChBlueValue 213
#define APP_KoChRGB 214
#define APP_KoChRGBValue 215
#define APP_KoChNightlightSw 216
#define APP_KoChNightlightState 217
#define APP_KoChPreset 218
#define APP_KoChPresetValue 219
#define APP_KoChPalette 220
#define APP_KoChPaletteValue 221
#define APP_KoChEffectId 222
#define APP_KoChEffectIdValue 223
#define APP_KoChEffectSpeed 224
#define APP_KoChEffectSpeedValue 225
#define APP_KoChEffectIntensity 226
#define APP_KoChEffectIntensityValue 227

// Schalten
#define KoAPP_ChDimSw                             (knx.getGroupObject(APP_KoChDimSw))
// Relativ dimmen
#define KoAPP_ChDimRel                            (knx.getGroupObject(APP_KoChDimRel))
// Helligkeit
#define KoAPP_ChDimAbs                            (knx.getGroupObject(APP_KoChDimAbs))
// Helligkeitsstatus
#define KoAPP_ChDimState                          (knx.getGroupObject(APP_KoChDimState))
// Helligkeitswert
#define KoAPP_ChDimValue                          (knx.getGroupObject(APP_KoChDimValue))
// Szene
#define KoAPP_ChScene                             (knx.getGroupObject(APP_KoChScene))
// Szenenstatus
#define KoAPP_ChSceneState                        (knx.getGroupObject(APP_KoChSceneState))
// Rot
#define KoAPP_ChRed                               (knx.getGroupObject(APP_KoChRed))
// Rot Status
#define KoAPP_ChRedValue                          (knx.getGroupObject(APP_KoChRedValue))
// Green
#define KoAPP_ChGreen                             (knx.getGroupObject(APP_KoChGreen))
// Green Status
#define KoAPP_ChGreenValue                        (knx.getGroupObject(APP_KoChGreenValue))
// Blau
#define KoAPP_ChBlue                              (knx.getGroupObject(APP_KoChBlue))
// Blau Status
#define KoAPP_ChBlueValue                         (knx.getGroupObject(APP_KoChBlueValue))
// RGB
#define KoAPP_ChRGB                               (knx.getGroupObject(APP_KoChRGB))
// RGB Status
#define KoAPP_ChRGBValue                          (knx.getGroupObject(APP_KoChRGBValue))
// Nachtlicht
#define KoAPP_ChNightlightSw                      (knx.getGroupObject(APP_KoChNightlightSw))
// Nachtlicht Status
#define KoAPP_ChNightlightState                   (knx.getGroupObject(APP_KoChNightlightState))
// Preset
#define KoAPP_ChPreset                            (knx.getGroupObject(APP_KoChPreset))
// Preset Status
#define KoAPP_ChPresetValue                       (knx.getGroupObject(APP_KoChPresetValue))
// Palette
#define KoAPP_ChPalette                           (knx.getGroupObject(APP_KoChPalette))
// Palette Status
#define KoAPP_ChPaletteValue                      (knx.getGroupObject(APP_KoChPaletteValue))
// Effect
#define KoAPP_ChEffectId                          (knx.getGroupObject(APP_KoChEffectId))
// Effect Status
#define KoAPP_ChEffectIdValue                     (knx.getGroupObject(APP_KoChEffectIdValue))
// Effect Speed
#define KoAPP_ChEffectSpeed                       (knx.getGroupObject(APP_KoChEffectSpeed))
// Effect Speed Status
#define KoAPP_ChEffectSpeedValue                  (knx.getGroupObject(APP_KoChEffectSpeedValue))
// Effect Intensity
#define KoAPP_ChEffectIntensity                   (knx.getGroupObject(APP_KoChEffectIntensity))
// Effect Intensity Status
#define KoAPP_ChEffectIntensityValue              (knx.getGroupObject(APP_KoChEffectIntensityValue))



// Header generation for Module 'BASE_KommentarModule'

#define BASE_KommentarModuleCount 0
#define BASE_KommentarModuleModuleParamSize 0
#define BASE_KommentarModuleSubmodulesParamSize 0
#define BASE_KommentarModuleParamSize 0
#define BASE_KommentarModuleParamOffset 124
#define BASE_KommentarModuleCalcIndex(index, m1) (index + BASE_KommentarModuleParamOffset + _channelIndex * BASE_KommentarModuleCount * BASE_KommentarModuleParamSize + m1 * BASE_KommentarModuleParamSize)




// enumeration types


#ifdef MAIN_FirmwareRevision
#ifndef FIRMWARE_REVISION
#define FIRMWARE_REVISION MAIN_FirmwareRevision
#endif
#endif
#ifdef MAIN_FirmwareName
#ifndef FIRMWARE_NAME
#define FIRMWARE_NAME MAIN_FirmwareName
#endif
#endif
