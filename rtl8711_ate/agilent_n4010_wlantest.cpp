// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "agilent_n4010_wlantest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// Instrument_Interface properties

/////////////////////////////////////////////////////////////////////////////
// Instrument_Interface operations

CString Instrument_Interface::GetInstrumentVISAResourceString()
{
	CString result;
	InvokeHelper(0x76, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetInstrumentVISAResourceString(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x76, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 lpszNewValue);
}

BOOL Instrument_Interface::GetUseInternalLicense()
{
	BOOL result;
	InvokeHelper(0x77, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetUseInternalLicense(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x77, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL Instrument_Interface::GetDeleteAWGSequenceFiles()
{
	BOOL result;
	InvokeHelper(0x78, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetDeleteAWGSequenceFiles(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x78, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL Instrument_Interface::GetShowSplashScreen()
{
	BOOL result;
	InvokeHelper(0x79, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetShowSplashScreen(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x79, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL Instrument_Interface::GetUseExternalFrequencyReference()
{
	BOOL result;
	InvokeHelper(0xa1, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetUseExternalFrequencyReference(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xa1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

void Instrument_Interface::ActivateInstrument()
{
	InvokeHelper(0x7a, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::Dispose()
{
	InvokeHelper(0x7b, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetFrequency()
{
	double result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetFrequency(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double Instrument_Interface::GetTriggerLevel()
{
	double result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetTriggerLevel(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double Instrument_Interface::GetPowerRange()
{
	double result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetPowerRange(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

BOOL Instrument_Interface::GetMeasurementRunning()
{
	BOOL result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetCalibrationRunning()
{
	BOOL result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetSelfTestRunning()
{
	BOOL result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetAutorangeRunning()
{
	BOOL result;
	InvokeHelper(0xa3, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetTTLOutput()
{
	long result;
	InvokeHelper(0x1b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetTTLOutput(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

double Instrument_Interface::GetTriggerDelay()
{
	double result;
	InvokeHelper(0x23, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetTriggerDelay(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x23, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double Instrument_Interface::GetMaxPacketLength()
{
	double result;
	InvokeHelper(0x24, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetMaxPacketLength(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x24, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long Instrument_Interface::GetFramesUsed()
{
	long result;
	InvokeHelper(0x25, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetFramesUsed(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x25, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long Instrument_Interface::GetMaxSymbolsUsed()
{
	long result;
	InvokeHelper(0x26, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetMaxSymbolsUsed(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x26, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

BOOL Instrument_Interface::GetMirrorFrequency()
{
	BOOL result;
	InvokeHelper(0x27, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetMirrorFrequency(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x27, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

long Instrument_Interface::GetTriggerStyle()
{
	long result;
	InvokeHelper(0x28, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetTriggerStyle(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x28, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

BOOL Instrument_Interface::GetUseAlternateSyncSequence()
{
	BOOL result;
	InvokeHelper(0x30, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetUseAlternateSyncSequence(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x30, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

double Instrument_Interface::GetTriggerHoldoff()
{
	double result;
	InvokeHelper(0x81, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetTriggerHoldoff(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x81, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long Instrument_Interface::GetWindowType()
{
	long result;
	InvokeHelper(0xa4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetWindowType(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xa4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

void Instrument_Interface::Calibrate()
{
	InvokeHelper(0x29, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::QuickCal()
{
	InvokeHelper(0xb2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

long Instrument_Interface::SelfTest()
{
	long result;
	InvokeHelper(0x2b, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetChannel(long Format, long Channel)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x2c, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Format, Channel);
}

void Instrument_Interface::DUTChanged()
{
	InvokeHelper(0x31, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

VARIANT Instrument_Interface::Obj89607()
{
	VARIANT result;
	InvokeHelper(0x32, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
	return result;
}

VARIANT Instrument_Interface::Obj89601()
{
	VARIANT result;
	InvokeHelper(0x33, DISPATCH_METHOD, VT_VARIANT, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetBluetoothMode()
{
	InvokeHelper(0x34, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::SetWlanMode()
{
	InvokeHelper(0x35, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::Preset()
{
	InvokeHelper(0x2f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::Suspend()
{
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL Instrument_Interface::GetWtaVisible()
{
	BOOL result;
	InvokeHelper(0x2d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetWtaVisible(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x2d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL Instrument_Interface::GetVsaVisible()
{
	BOOL result;
	InvokeHelper(0x2e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetVsaVisible(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x2e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

BOOL Instrument_Interface::GetCalibrationNeeded()
{
	BOOL result;
	InvokeHelper(0x2a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetInstrumentOption2GHz()
{
	BOOL result;
	InvokeHelper(0x36, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetInstrumentOption5GHz()
{
	BOOL result;
	InvokeHelper(0x37, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetInstrumentOptionWaveformGenerator()
{
	BOOL result;
	InvokeHelper(0x38, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetInstrumentOptionFlexibleWaveformGenerator()
{
	BOOL result;
	InvokeHelper(0x39, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetInstrumentOptionBluetooth()
{
	BOOL result;
	InvokeHelper(0x3a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetReferenceLocked()
{
	BOOL result;
	InvokeHelper(0xa2, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211a_CFL_Initiate()
{
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211a_CFL_ForceInitiate()
{
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211a_CFL_Result()
{
	double result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211a_CFL_Overloaded()
{
	BOOL result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211a_EVM_Initiate()
{
	InvokeHelper(0xd, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211a_EVM_ForceInitiate()
{
	InvokeHelper(0xe, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211a_EVM_Result()
{
	double result;
	InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetIEEE80211a_EVM_ModulationFormat()
{
	long result;
	InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211a_EVM_Overloaded()
{
	BOOL result;
	InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211a_FrequencyError_Initiate()
{
	InvokeHelper(0x3b, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211a_FrequencyError_ForceInitiate()
{
	InvokeHelper(0x3c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211a_FrequencyError_Result()
{
	double result;
	InvokeHelper(0x3d, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211a_FrequencyError_Overloaded()
{
	BOOL result;
	InvokeHelper(0x3e, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211a_SpectralFlatness_Initiate()
{
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211a_SpectralFlatness_ForceInitiate()
{
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::IEEE80211a_SpectralFlatness_Amplitude(long i)
{
	double result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_R8, (void*)&result, parms,
		i);
	return result;
}

double Instrument_Interface::IEEE80211a_SpectralFlatness_Frequency(long i)
{
	double result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_R8, (void*)&result, parms,
		i);
	return result;
}

long Instrument_Interface::GetIEEE80211a_SpectralFlatness_NumResults()
{
	long result;
	InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_SpectralFlatness_Margin()
{
	double result;
	InvokeHelper(0x21, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211a_SpectralFlatness_Overloaded()
{
	BOOL result;
	InvokeHelper(0x22, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::AveragePower_Initiate()
{
	InvokeHelper(0x4f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetAveragePower_Result()
{
	double result;
	InvokeHelper(0x50, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetAveragePower_Overloaded()
{
	BOOL result;
	InvokeHelper(0x51, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::CwFrequencyOffset_Initiate()
{
	InvokeHelper(0xaf, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetCwFrequencyOffset_Result()
{
	double result;
	InvokeHelper(0xb0, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetCwFrequencyOffset_Overloaded()
{
	BOOL result;
	InvokeHelper(0xb1, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211a_Demod_Initiate()
{
	InvokeHelper(0x43, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211a_Demod_RmsEVM()
{
	double result;
	InvokeHelper(0x44, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_Demod_IQOffset()
{
	double result;
	InvokeHelper(0x46, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_Demod_FrequencyError()
{
	double result;
	InvokeHelper(0x45, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_Demod_GatedPower()
{
	double result;
	InvokeHelper(0x47, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211a_Demod_MeasureGatedPower()
{
	BOOL result;
	InvokeHelper(0x48, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_Demod_MeasureGatedPower(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x48, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

double Instrument_Interface::GetIEEE80211a_Demod_LeftGatedPowerTime()
{
	double result;
	InvokeHelper(0x49, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_Demod_LeftGatedPowerTime(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x49, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double Instrument_Interface::GetIEEE80211a_Demod_RightGatedPowerTime()
{
	double result;
	InvokeHelper(0x4a, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_Demod_RightGatedPowerTime(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x4a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

BOOL Instrument_Interface::GetIEEE80211a_Demod_Overloaded()
{
	BOOL result;
	InvokeHelper(0x4b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211a_Demod_BurstGood()
{
	BOOL result;
	InvokeHelper(0x4c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_Demod_SubcarrierSpacing()
{
	double result;
	InvokeHelper(0x4d, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_Demod_SubcarrierSpacing(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x4d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long Instrument_Interface::GetIEEE80211a_Demod_MeasurementOffset()
{
	long result;
	InvokeHelper(0x4e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_Demod_MeasurementOffset(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long Instrument_Interface::GetIEEE80211a_Demod_EqualizationType()
{
	long result;
	InvokeHelper(0xb3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_Demod_EqualizationType(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xb3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

void Instrument_Interface::IEEE80211a_SpectralMask_Initiate()
{
	InvokeHelper(0x82, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211a_SpectralMask_ForceInitiate()
{
	InvokeHelper(0x83, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::IEEE80211a_SpectralMask_Frequency(long i)
{
	double result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x84, DISPATCH_METHOD, VT_R8, (void*)&result, parms,
		i);
	return result;
}

double Instrument_Interface::IEEE80211a_SpectralMask_Amplitude(long i)
{
	double result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x85, DISPATCH_METHOD, VT_R8, (void*)&result, parms,
		i);
	return result;
}

double Instrument_Interface::GetIEEE80211a_SpectralMask_Margin()
{
	double result;
	InvokeHelper(0x86, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_SpectralMask_ReferenceLevelResult()
{
	double result;
	InvokeHelper(0x87, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetIEEE80211a_SpectralMask_NumSpectrumPoints()
{
	long result;
	InvokeHelper(0x88, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_SpectralMask_Span()
{
	double result;
	InvokeHelper(0x89, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_SpectralMask_Span(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x89, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double Instrument_Interface::GetIEEE80211a_SpectralMask_ReferenceLevel()
{
	double result;
	InvokeHelper(0x8a, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211a_SpectralMask_ReferenceLevel(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x8a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

BOOL Instrument_Interface::GetIEEE80211a_SpectralMask_Overloaded()
{
	BOOL result;
	InvokeHelper(0x8b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211b_EVM_Initiate()
{
	InvokeHelper(0x12, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211b_EVM_ForceInitiate()
{
	InvokeHelper(0x13, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211b_EVM_Result()
{
	double result;
	InvokeHelper(0x14, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetIEEE80211b_EVM_ModulationFormat()
{
	long result;
	InvokeHelper(0x15, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211b_EVM_Overloaded()
{
	BOOL result;
	InvokeHelper(0x16, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211b_FrequencyError_Initiate()
{
	InvokeHelper(0x3f, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211b_FrequencyError_ForceInitiate()
{
	InvokeHelper(0x40, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211b_FrequencyError_Result()
{
	double result;
	InvokeHelper(0x41, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211b_FrequencyError_Overloaded()
{
	BOOL result;
	InvokeHelper(0x42, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211b_CarrierSuppression_Initiate()
{
	InvokeHelper(0x17, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211b_CarrierSuppression_ForceInitiate()
{
	InvokeHelper(0x18, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211b_CarrierSuppression_Result()
{
	double result;
	InvokeHelper(0x19, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211b_CarrierSuppression_Overloaded()
{
	BOOL result;
	InvokeHelper(0x1a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211b_Demod_Initiate()
{
	InvokeHelper(0x52, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211b_Demod_RmsEVM()
{
	double result;
	InvokeHelper(0x53, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_Demod_PeakEVM()
{
	double result;
	InvokeHelper(0x54, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_Demod_IQOffset()
{
	double result;
	InvokeHelper(0x56, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_Demod_FrequencyError()
{
	double result;
	InvokeHelper(0x55, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_Demod_GatedPower()
{
	double result;
	InvokeHelper(0x57, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211b_Demod_MeasureGatedPower()
{
	BOOL result;
	InvokeHelper(0x58, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211b_Demod_MeasureGatedPower(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x58, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

double Instrument_Interface::GetIEEE80211b_Demod_LeftGatedPowerTime()
{
	double result;
	InvokeHelper(0x59, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211b_Demod_LeftGatedPowerTime(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x59, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double Instrument_Interface::GetIEEE80211b_Demod_RightGatedPowerTime()
{
	double result;
	InvokeHelper(0x5a, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211b_Demod_RightGatedPowerTime(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x5a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

BOOL Instrument_Interface::GetIEEE80211b_Demod_Overloaded()
{
	BOOL result;
	InvokeHelper(0x5b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211b_Demod_BurstGood()
{
	BOOL result;
	InvokeHelper(0x5c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetIEEE80211b_Demod_TrackPhase()
{
	BOOL result;
	InvokeHelper(0x5d, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211b_Demod_TrackPhase(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x5d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

long Instrument_Interface::GetIEEE80211b_Demod_MeasurementOffset()
{
	long result;
	InvokeHelper(0x5e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211b_Demod_MeasurementOffset(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x5e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

void Instrument_Interface::IEEE80211b_SpectralMask_Initiate()
{
	InvokeHelper(0x8c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::IEEE80211b_SpectralMask_ForceInitiate()
{
	InvokeHelper(0x8d, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::IEEE80211b_SpectralMask_Frequency(long i)
{
	double result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x8e, DISPATCH_METHOD, VT_R8, (void*)&result, parms,
		i);
	return result;
}

double Instrument_Interface::IEEE80211b_SpectralMask_Amplitude(long i)
{
	double result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x8f, DISPATCH_METHOD, VT_R8, (void*)&result, parms,
		i);
	return result;
}

double Instrument_Interface::GetIEEE80211b_SpectralMask_Margin()
{
	double result;
	InvokeHelper(0x90, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_SpectralMask_ReferenceLevelResult()
{
	double result;
	InvokeHelper(0x91, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetIEEE80211b_SpectralMask_NumSpectrumPoints()
{
	long result;
	InvokeHelper(0x92, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_SpectralMask_Span()
{
	double result;
	InvokeHelper(0x93, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211b_SpectralMask_Span(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x93, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double Instrument_Interface::GetIEEE80211b_SpectralMask_ReferenceLevel()
{
	double result;
	InvokeHelper(0x94, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetIEEE80211b_SpectralMask_ReferenceLevel(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x94, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

BOOL Instrument_Interface::GetIEEE80211b_SpectralMask_Overloaded()
{
	BOOL result;
	InvokeHelper(0x95, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::GatedSpectrum_Initiate()
{
	InvokeHelper(0x96, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

BOOL Instrument_Interface::GetGatedSpectrum_Overloaded()
{
	BOOL result;
	InvokeHelper(0x97, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetGatedSpectrum_NumMeasurementPoints()
{
	long result;
	InvokeHelper(0x9a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetGatedSpectrum_AveragingStyle()
{
	long result;
	InvokeHelper(0x9c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetGatedSpectrum_AveragingStyle(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x9c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long Instrument_Interface::GetGatedSpectrum_SpectrumWidth()
{
	long result;
	InvokeHelper(0x9b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetGatedSpectrum_SpectrumWidth(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x9b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

BOOL Instrument_Interface::GetGatedSpectrum_MeasureBandPower()
{
	BOOL result;
	InvokeHelper(0x9f, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetGatedSpectrum_MeasureBandPower(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x9f, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

double Instrument_Interface::GetGatedSpectrum_BandPower()
{
	double result;
	InvokeHelper(0xa0, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::AWG_LoadSegment(LPCTSTR WindowsFileName, LPCTSTR InstrumentFileName)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x5f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 WindowsFileName, InstrumentFileName);
}

void Instrument_Interface::AWG_LoadSequence(LPCTSTR WindowsFileName, LPCTSTR InstrumentFileName)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x6d, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 WindowsFileName, InstrumentFileName);
}

void Instrument_Interface::AWG_SaveSegment(LPCTSTR InstrumentFileName, LPCTSTR WindowsFileName)
{
	static BYTE parms[] =
		VTS_BSTR VTS_BSTR;
	InvokeHelper(0x60, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName, WindowsFileName);
}

void Instrument_Interface::AWG_PlayA(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency)
{
	static BYTE parms[] =
		VTS_BSTR VTS_R8 VTS_R8;
	InvokeHelper(0x61, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName, Level, OffsetFrequency);
}

void Instrument_Interface::AWG_PlayB(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency)
{
	static BYTE parms[] =
		VTS_BSTR VTS_R8 VTS_R8;
	InvokeHelper(0x62, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName, Level, OffsetFrequency);
}

void Instrument_Interface::AWG_PlayCustom(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency, long SampleRate)
{
	static BYTE parms[] =
		VTS_BSTR VTS_R8 VTS_R8 VTS_I4;
	InvokeHelper(0x63, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName, Level, OffsetFrequency, SampleRate);
}

void Instrument_Interface::AWG_SelectSequenceA(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency)
{
	static BYTE parms[] =
		VTS_BSTR VTS_R8 VTS_R8;
	InvokeHelper(0x6e, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName, Level, OffsetFrequency);
}

void Instrument_Interface::AWG_SelectSequenceB(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency)
{
	static BYTE parms[] =
		VTS_BSTR VTS_R8 VTS_R8;
	InvokeHelper(0x6f, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName, Level, OffsetFrequency);
}

void Instrument_Interface::AWG_SelectSequenceCustom(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency, long SampleRate)
{
	static BYTE parms[] =
		VTS_BSTR VTS_R8 VTS_R8 VTS_I4;
	InvokeHelper(0x70, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName, Level, OffsetFrequency, SampleRate);
}

void Instrument_Interface::AWG_Stop()
{
	InvokeHelper(0x66, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::AWG_TriggerSequence()
{
	InvokeHelper(0x64, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::AWG_CwTone(double Level, double Frequency)
{
	static BYTE parms[] =
		VTS_R8 VTS_R8;
	InvokeHelper(0x65, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 Level, Frequency);
}

void Instrument_Interface::AWG_DeleteSegment(LPCTSTR InstrumentFileName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x67, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName);
}

void Instrument_Interface::AWG_DeleteSequence(LPCTSTR InstrumentFileName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x71, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 InstrumentFileName);
}

unsigned long Instrument_Interface::GetAWG_FreeSegmentSpace()
{
	unsigned long result;
	InvokeHelper(0x68, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

unsigned long Instrument_Interface::GetAWG_FreeSequenceSpace()
{
	unsigned long result;
	InvokeHelper(0x69, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::AWG_CreateSequenceFile()
{
	InvokeHelper(0x73, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void Instrument_Interface::AWG_SequenceFile_AddSegment(LPCTSTR SegmentName, long Repetitions)
{
	static BYTE parms[] =
		VTS_BSTR VTS_I4;
	InvokeHelper(0x74, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 SegmentName, Repetitions);
}

void Instrument_Interface::AWG_SequenceFile_Save(LPCTSTR WindowsFileName)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x75, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 WindowsFileName);
}

BOOL Instrument_Interface::GetAWG_SourceActive()
{
	BOOL result;
	InvokeHelper(0x6b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetAWG_SequencePlaying()
{
	BOOL result;
	InvokeHelper(0x6c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

BOOL Instrument_Interface::GetAWG_HighPowerMode()
{
	BOOL result;
	InvokeHelper(0xae, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::SetAWG_HighPowerMode(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0xae, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 bNewValue);
}

CString Instrument_Interface::GetInstrumentVISAResourceName()
{
	CString result;
	InvokeHelper(0x7c, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString Instrument_Interface::GetInstrumentManufacturer()
{
	CString result;
	InvokeHelper(0x7d, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString Instrument_Interface::GetInstrumentModel()
{
	CString result;
	InvokeHelper(0x7e, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString Instrument_Interface::GetInstrumentSerialNumber()
{
	CString result;
	InvokeHelper(0x7f, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

CString Instrument_Interface::GetInstrumentFirmwareRevision()
{
	CString result;
	InvokeHelper(0x80, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211a_Autorange_Run()
{
	InvokeHelper(0xa5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211a_Autorange_PowerRange()
{
	double result;
	InvokeHelper(0xa6, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_Autorange_TriggerLevel()
{
	double result;
	InvokeHelper(0xa7, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

long Instrument_Interface::GetIEEE80211a_Autorange_MaxSymbolsUsed()
{
	long result;
	InvokeHelper(0xa8, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211a_Autorange_MaxPacketLength()
{
	double result;
	InvokeHelper(0xa9, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void Instrument_Interface::IEEE80211b_Autorange_Run()
{
	InvokeHelper(0xaa, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

double Instrument_Interface::GetIEEE80211b_Autorange_PowerRange()
{
	double result;
	InvokeHelper(0xab, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_Autorange_TriggerLevel()
{
	double result;
	InvokeHelper(0xac, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

double Instrument_Interface::GetIEEE80211b_Autorange_MaxPacketLength()
{
	double result;
	InvokeHelper(0xad, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}
