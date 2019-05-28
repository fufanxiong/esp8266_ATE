#ifndef INSTRUMENT_INTERFACE //如果没有定义POINT_H这个宏，则编译以下代码
#define INSTRUMENT_INTERFACE   //定义宏 ，避免此后重复编译


// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// Instrument_Interface wrapper class

class Instrument_Interface : public COleDispatchDriver
{
public:
	Instrument_Interface() {}		// Calls COleDispatchDriver default constructor
	Instrument_Interface(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	Instrument_Interface(const Instrument_Interface& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:
	bool state;

// Operations
public:
	CString GetInstrumentVISAResourceString();
	void SetInstrumentVISAResourceString(LPCTSTR lpszNewValue);
	BOOL GetUseInternalLicense();
	void SetUseInternalLicense(BOOL bNewValue);
	BOOL GetDeleteAWGSequenceFiles();
	void SetDeleteAWGSequenceFiles(BOOL bNewValue);
	BOOL GetShowSplashScreen();
	void SetShowSplashScreen(BOOL bNewValue);
	BOOL GetUseExternalFrequencyReference();
	void SetUseExternalFrequencyReference(BOOL bNewValue);
	void ActivateInstrument();
	void Dispose();
	double GetFrequency();
	void SetFrequency(double newValue);
	double GetTriggerLevel();
	void SetTriggerLevel(double newValue);
	double GetPowerRange();
	void SetPowerRange(double newValue);
	BOOL GetMeasurementRunning();
	BOOL GetCalibrationRunning();
	BOOL GetSelfTestRunning();
	BOOL GetAutorangeRunning();
	long GetTTLOutput();
	void SetTTLOutput(long nNewValue);
	double GetTriggerDelay();
	void SetTriggerDelay(double newValue);
	double GetMaxPacketLength();
	void SetMaxPacketLength(double newValue);
	long GetFramesUsed();
	void SetFramesUsed(long nNewValue);
	long GetMaxSymbolsUsed();
	void SetMaxSymbolsUsed(long nNewValue);
	BOOL GetMirrorFrequency();
	void SetMirrorFrequency(BOOL bNewValue);
	long GetTriggerStyle();
	void SetTriggerStyle(long nNewValue);
	BOOL GetUseAlternateSyncSequence();
	void SetUseAlternateSyncSequence(BOOL bNewValue);
	double GetTriggerHoldoff();
	void SetTriggerHoldoff(double newValue);
	long GetWindowType();
	void SetWindowType(long nNewValue);
	void Calibrate();
	void QuickCal();
	long SelfTest();
	void SetChannel(long Format, long Channel);
	void DUTChanged();
	VARIANT Obj89607();
	VARIANT Obj89601();
	void SetBluetoothMode();
	void SetWlanMode();
	void Preset();
	void Suspend();
	BOOL GetWtaVisible();
	void SetWtaVisible(BOOL bNewValue);
	BOOL GetVsaVisible();
	void SetVsaVisible(BOOL bNewValue);
	BOOL GetCalibrationNeeded();
	BOOL GetInstrumentOption2GHz();
	BOOL GetInstrumentOption5GHz();
	BOOL GetInstrumentOptionWaveformGenerator();
	BOOL GetInstrumentOptionFlexibleWaveformGenerator();
	BOOL GetInstrumentOptionBluetooth();
	BOOL GetReferenceLocked();
	void IEEE80211a_CFL_Initiate();
	void IEEE80211a_CFL_ForceInitiate();
	double GetIEEE80211a_CFL_Result();
	BOOL GetIEEE80211a_CFL_Overloaded();
	void IEEE80211a_EVM_Initiate();
	void IEEE80211a_EVM_ForceInitiate();
	double GetIEEE80211a_EVM_Result();
	long GetIEEE80211a_EVM_ModulationFormat();
	BOOL GetIEEE80211a_EVM_Overloaded();
	void IEEE80211a_FrequencyError_Initiate();
	void IEEE80211a_FrequencyError_ForceInitiate();
	double GetIEEE80211a_FrequencyError_Result();
	BOOL GetIEEE80211a_FrequencyError_Overloaded();
	void IEEE80211a_SpectralFlatness_Initiate();
	void IEEE80211a_SpectralFlatness_ForceInitiate();
	double IEEE80211a_SpectralFlatness_Amplitude(long i);
	double IEEE80211a_SpectralFlatness_Frequency(long i);
	long GetIEEE80211a_SpectralFlatness_NumResults();
	double GetIEEE80211a_SpectralFlatness_Margin();
	BOOL GetIEEE80211a_SpectralFlatness_Overloaded();
	void AveragePower_Initiate();
	double GetAveragePower_Result();
	BOOL GetAveragePower_Overloaded();
	void CwFrequencyOffset_Initiate();
	double GetCwFrequencyOffset_Result();
	BOOL GetCwFrequencyOffset_Overloaded();
	void IEEE80211a_Demod_Initiate();
	double GetIEEE80211a_Demod_RmsEVM();
	double GetIEEE80211a_Demod_IQOffset();
	double GetIEEE80211a_Demod_FrequencyError();
	double GetIEEE80211a_Demod_GatedPower();
	BOOL GetIEEE80211a_Demod_MeasureGatedPower();
	void SetIEEE80211a_Demod_MeasureGatedPower(BOOL bNewValue);
	double GetIEEE80211a_Demod_LeftGatedPowerTime();
	void SetIEEE80211a_Demod_LeftGatedPowerTime(double newValue);
	double GetIEEE80211a_Demod_RightGatedPowerTime();
	void SetIEEE80211a_Demod_RightGatedPowerTime(double newValue);
	BOOL GetIEEE80211a_Demod_Overloaded();
	BOOL GetIEEE80211a_Demod_BurstGood();
	double GetIEEE80211a_Demod_SubcarrierSpacing();
	void SetIEEE80211a_Demod_SubcarrierSpacing(double newValue);
	long GetIEEE80211a_Demod_MeasurementOffset();
	void SetIEEE80211a_Demod_MeasurementOffset(long nNewValue);
	long GetIEEE80211a_Demod_EqualizationType();
	void SetIEEE80211a_Demod_EqualizationType(long nNewValue);
	void IEEE80211a_SpectralMask_Initiate();
	void IEEE80211a_SpectralMask_ForceInitiate();
	double IEEE80211a_SpectralMask_Frequency(long i);
	double IEEE80211a_SpectralMask_Amplitude(long i);
	double GetIEEE80211a_SpectralMask_Margin();
	double GetIEEE80211a_SpectralMask_ReferenceLevelResult();
	long GetIEEE80211a_SpectralMask_NumSpectrumPoints();
	double GetIEEE80211a_SpectralMask_Span();
	void SetIEEE80211a_SpectralMask_Span(double newValue);
	double GetIEEE80211a_SpectralMask_ReferenceLevel();
	void SetIEEE80211a_SpectralMask_ReferenceLevel(double newValue);
	BOOL GetIEEE80211a_SpectralMask_Overloaded();
	void IEEE80211b_EVM_Initiate();
	void IEEE80211b_EVM_ForceInitiate();
	double GetIEEE80211b_EVM_Result();
	long GetIEEE80211b_EVM_ModulationFormat();
	BOOL GetIEEE80211b_EVM_Overloaded();
	void IEEE80211b_FrequencyError_Initiate();
	void IEEE80211b_FrequencyError_ForceInitiate();
	double GetIEEE80211b_FrequencyError_Result();
	BOOL GetIEEE80211b_FrequencyError_Overloaded();
	void IEEE80211b_CarrierSuppression_Initiate();
	void IEEE80211b_CarrierSuppression_ForceInitiate();
	double GetIEEE80211b_CarrierSuppression_Result();
	BOOL GetIEEE80211b_CarrierSuppression_Overloaded();
	void IEEE80211b_Demod_Initiate();
	double GetIEEE80211b_Demod_RmsEVM();
	double GetIEEE80211b_Demod_PeakEVM();
	double GetIEEE80211b_Demod_IQOffset();
	double GetIEEE80211b_Demod_FrequencyError();
	double GetIEEE80211b_Demod_GatedPower();
	BOOL GetIEEE80211b_Demod_MeasureGatedPower();
	void SetIEEE80211b_Demod_MeasureGatedPower(BOOL bNewValue);
	double GetIEEE80211b_Demod_LeftGatedPowerTime();
	void SetIEEE80211b_Demod_LeftGatedPowerTime(double newValue);
	double GetIEEE80211b_Demod_RightGatedPowerTime();
	void SetIEEE80211b_Demod_RightGatedPowerTime(double newValue);
	BOOL GetIEEE80211b_Demod_Overloaded();
	BOOL GetIEEE80211b_Demod_BurstGood();
	BOOL GetIEEE80211b_Demod_TrackPhase();
	void SetIEEE80211b_Demod_TrackPhase(BOOL bNewValue);
	long GetIEEE80211b_Demod_MeasurementOffset();
	void SetIEEE80211b_Demod_MeasurementOffset(long nNewValue);
	void IEEE80211b_SpectralMask_Initiate();
	void IEEE80211b_SpectralMask_ForceInitiate();
	double IEEE80211b_SpectralMask_Frequency(long i);
	double IEEE80211b_SpectralMask_Amplitude(long i);
	double GetIEEE80211b_SpectralMask_Margin();
	double GetIEEE80211b_SpectralMask_ReferenceLevelResult();
	long GetIEEE80211b_SpectralMask_NumSpectrumPoints();
	double GetIEEE80211b_SpectralMask_Span();
	void SetIEEE80211b_SpectralMask_Span(double newValue);
	double GetIEEE80211b_SpectralMask_ReferenceLevel();
	void SetIEEE80211b_SpectralMask_ReferenceLevel(double newValue);
	BOOL GetIEEE80211b_SpectralMask_Overloaded();
	void GatedSpectrum_Initiate();
	BOOL GetGatedSpectrum_Overloaded();
	// method 'GetGatedSpectrum_Frequencies' not emitted because of invalid return type or parameter type
	// method 'GetGatedSpectrum_Powers' not emitted because of invalid return type or parameter type
	long GetGatedSpectrum_NumMeasurementPoints();
	long GetGatedSpectrum_AveragingStyle();
	void SetGatedSpectrum_AveragingStyle(long nNewValue);
	long GetGatedSpectrum_SpectrumWidth();
	void SetGatedSpectrum_SpectrumWidth(long nNewValue);
	BOOL GetGatedSpectrum_MeasureBandPower();
	void SetGatedSpectrum_MeasureBandPower(BOOL bNewValue);
	double GetGatedSpectrum_BandPower();
	void AWG_LoadSegment(LPCTSTR WindowsFileName, LPCTSTR InstrumentFileName);
	void AWG_LoadSequence(LPCTSTR WindowsFileName, LPCTSTR InstrumentFileName);
	void AWG_SaveSegment(LPCTSTR InstrumentFileName, LPCTSTR WindowsFileName);
	void AWG_PlayA(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency);
	void AWG_PlayB(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency);
	void AWG_PlayCustom(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency, long SampleRate);
	void AWG_SelectSequenceA(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency);
	void AWG_SelectSequenceB(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency);
	void AWG_SelectSequenceCustom(LPCTSTR InstrumentFileName, double Level, double OffsetFrequency, long SampleRate);
	void AWG_Stop();
	void AWG_TriggerSequence();
	void AWG_CwTone(double Level, double Frequency);
	void AWG_DeleteSegment(LPCTSTR InstrumentFileName);
	void AWG_DeleteSequence(LPCTSTR InstrumentFileName);
	unsigned long GetAWG_FreeSegmentSpace();
	unsigned long GetAWG_FreeSequenceSpace();
	// method 'GetAWG_LoadedSegments' not emitted because of invalid return type or parameter type
	// method 'GetAWG_LoadedSequences' not emitted because of invalid return type or parameter type
	void AWG_CreateSequenceFile();
	void AWG_SequenceFile_AddSegment(LPCTSTR SegmentName, long Repetitions);
	void AWG_SequenceFile_Save(LPCTSTR WindowsFileName);
	BOOL GetAWG_SourceActive();
	BOOL GetAWG_SequencePlaying();
	BOOL GetAWG_HighPowerMode();
	void SetAWG_HighPowerMode(BOOL bNewValue);
	CString GetInstrumentVISAResourceName();
	CString GetInstrumentManufacturer();
	CString GetInstrumentModel();
	CString GetInstrumentSerialNumber();
	CString GetInstrumentFirmwareRevision();
	void IEEE80211a_Autorange_Run();
	double GetIEEE80211a_Autorange_PowerRange();
	double GetIEEE80211a_Autorange_TriggerLevel();
	long GetIEEE80211a_Autorange_MaxSymbolsUsed();
	double GetIEEE80211a_Autorange_MaxPacketLength();
	void IEEE80211b_Autorange_Run();
	double GetIEEE80211b_Autorange_PowerRange();
	double GetIEEE80211b_Autorange_TriggerLevel();
	double GetIEEE80211b_Autorange_MaxPacketLength();
};


#endif
