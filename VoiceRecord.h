#ifndef ZVOICERECORD_H
#define ZVOICERECORD_H
#include "stdafx.h"
class CVoiceBase  
{
public:
    
    CString m_result;
    MMRESULT res;
    enum 
    {
        SPS_8K=8000,
        SPS_11K=11025,
        SPS_22K=22050,
        SPS_44K=44100
    };
    enum
    {
        CH_MONO=1,
        CH_STEREO=2
    };

    char* buffer;
    WAVEHDR WaveHeader;
    WAVEFORMATEX PCMfmt;

    void SetFormat(DWORD nSamplesPerSec,  
            WORD  wBitsPerSample,WORD  nChannels);
    BOOL CopyBuffer(LPVOID lpBuffer, DWORD ntime);
    CString GetLastError();
    void GetMMResult(MMRESULT res);
    void DestroyBuffer();
    BOOL PrepareBuffer(DWORD ntime);
    
    CVoiceBase();
    virtual ~CVoiceBase();
};

class CVoiceRecording : public CVoiceBase  
{
public:
    void RecordFinished();
    BOOL IsOpen();
    BOOL Close();
    BOOL Open();    
    BOOL Record();
    
    HWAVEIN hWaveIn;
    
    CVoiceRecording();
    virtual ~CVoiceRecording();
	static void CALLBACK VoiceWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};

class CVoicePlaying : public CVoiceBase  
{
public:
    void PlayFinished();
    BOOL IsOpen();
    BOOL Close();
    BOOL Open();
    BOOL Play();

    HWAVEOUT hWaveOut;

    CVoicePlaying();
    virtual ~CVoicePlaying();
	
};
#endif