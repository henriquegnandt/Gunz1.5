#include "stdafx.h"
#include "VoiceRecord.h"

CVoiceBase::CVoiceBase()
{
	//defualt settings
   	PCMfmt.cbSize=0;
	PCMfmt.wFormatTag=WAVE_FORMAT_PCM;
	PCMfmt.nChannels=1;
	PCMfmt.nSamplesPerSec=8000;
	PCMfmt.wBitsPerSample=8;
	PCMfmt.nBlockAlign=1;
	PCMfmt.nAvgBytesPerSec=8000;

	buffer=NULL;
}

CVoiceBase::~CVoiceBase()
{
	if (buffer!=NULL)
	{
		delete [] buffer;
		buffer=NULL;
	}
}


BOOL CVoiceBase::PrepareBuffer(DWORD ntime)
{
   if (buffer!=NULL)
   {
	   delete [] buffer;
	   buffer=NULL;
   }
   
   DWORD length=PCMfmt.nSamplesPerSec*PCMfmt.nChannels
	          *PCMfmt.wBitsPerSample*ntime/8;

   buffer=new char[length];
   if (buffer==NULL)
 	{
 		return FALSE;
 	}

   WaveHeader.lpData=buffer;
   WaveHeader.dwBufferLength=length;
   WaveHeader.dwBytesRecorded=0;
   WaveHeader.dwUser=0;
   WaveHeader.dwFlags=0;
   WaveHeader.reserved=0;
   WaveHeader.lpNext=0;

   return TRUE;
}

void CVoiceBase::DestroyBuffer()
{
	if (buffer!=NULL)
	{
		delete [] buffer;
		buffer=NULL;
	}
}

void  CVoiceBase::GetMMResult(MMRESULT res)
{
	switch (res)
	{
	case MMSYSERR_ALLOCATED: 
		m_result="Specified resource is already allocated.";
		break;
		
	case MMSYSERR_BADDEVICEID:
		m_result="Specified device identifier is out of range.";
		break;
		
	case MMSYSERR_NODRIVER:
		m_result="No device driver is present. ";
		break;
		
	case MMSYSERR_NOMEM:
		m_result="Unable to allocate or lock memory. ";
		break;
		
	case WAVERR_BADFORMAT:
		m_result="Attempted to open with an unsupported waveform-audio format.";
		break;
		
	case WAVERR_UNPREPARED:
		m_result="The buffer pointed to by the pwh parameter hasn't been prepared. ";
		break;
		
	case WAVERR_SYNC:
		m_result="The device is synchronous but waveOutOpen was called"
			"without using the WAVE_ALLOWSYNC flag. ";
		break;
		
	case WAVERR_STILLPLAYING:
		m_result="The buffer pointed to by the pwh parameter is still in the queue.";
		break;
		
	case MMSYSERR_NOTSUPPORTED:
		m_result="Specified device is synchronous and does not support pausing. ";
		break;
		
	case MMSYSERR_NOERROR:
		break;
		
	default:
		m_result="Unspecified error";
	}
}

CString CVoiceBase::GetLastError()
{
	return m_result;
}

BOOL CVoiceBase::CopyBuffer(LPVOID lpBuffer,DWORD ntime)
{
	DWORD length=PCMfmt.nSamplesPerSec*PCMfmt.nChannels
		*PCMfmt.wBitsPerSample*ntime/8;
	memcpy(buffer, lpBuffer,length );
	return TRUE;
}

void CVoiceBase::SetFormat( DWORD nSamplesPerSec,  WORD  wBitsPerSample,WORD  nChannels)
{
	
   	PCMfmt.cbSize=0;
	PCMfmt.wFormatTag=WAVE_FORMAT_PCM;
	PCMfmt.nChannels=nChannels;
	PCMfmt.nSamplesPerSec=nSamplesPerSec;
	PCMfmt.wBitsPerSample=wBitsPerSample;
	PCMfmt.nBlockAlign=nChannels*wBitsPerSample/8;
	PCMfmt.nAvgBytesPerSec=nSamplesPerSec*nChannels*wBitsPerSample/8;
}

CVoicePlaying::CVoicePlaying()
{
	hWaveOut=NULL;
}

CVoicePlaying::~CVoicePlaying()
{
	if (IsOpen())
		Close();
}

BOOL CVoicePlaying::Play()
{
	res=waveOutPrepareHeader (hWaveOut,&WaveHeader,sizeof(WAVEHDR));
    GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
		return FALSE;
		
	res=waveOutWrite(hWaveOut,&WaveHeader,sizeof(WAVEHDR));	
    GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
		return FALSE;
	else
		return TRUE;
}

bool CALLBACK VoiceWaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	
	CVoicePlaying* pVoice=(CVoicePlaying*) dwInstance;
 	if (uMsg==WOM_DONE)
	{
			
		pVoice->res=waveOutUnprepareHeader(pVoice->hWaveOut, &pVoice->WaveHeader, sizeof(WAVEHDR));
		pVoice->GetMMResult(pVoice->res);
		pVoice->PlayFinished();
		
		if (pVoice->res!=MMSYSERR_NOERROR)
			return FALSE;
		else
			return TRUE;
	}

	return TRUE;
}

BOOL CVoicePlaying::Open()
{
	if (IsOpen())
		return FALSE;
	
	res=waveOutOpen (&hWaveOut,WAVE_MAPPER,&PCMfmt,(DWORD) VoiceWaveOutProc,(DWORD) this, CALLBACK_FUNCTION);
	GetMMResult(res);
	
	if (res!=MMSYSERR_NOERROR)
	{
		hWaveOut=NULL;
		return FALSE;
	}
	else
		return TRUE;
}

BOOL CVoicePlaying::Close()
{
	//res=waveOutReset(hWaveOut);
	//GetMMResult(res);
	//if (res!=MMSYSERR_NOERROR)
	//	return FALSE;
	
	res=waveOutClose(hWaveOut);
	GetMMResult(res);

	if (res!=MMSYSERR_NOERROR)
		return FALSE;
	else
		return TRUE;
}

BOOL CVoicePlaying::IsOpen()
{
  if(hWaveOut!=NULL)
	 return TRUE;
  else
     return FALSE;
	

}


void CVoicePlaying::PlayFinished()
{
	//write your own handler here

	//or simply create your own class and override this virtual function
}

CVoiceRecording::CVoiceRecording()
{
	hWaveIn=NULL;	
}

CVoiceRecording::~CVoiceRecording()
{
	if (IsOpen())
		Close();
}

BOOL CVoiceRecording::Record()
{
	res=waveInPrepareHeader(hWaveIn,&WaveHeader,sizeof(WAVEHDR));
	GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
		return FALSE;


	res=waveInAddBuffer(hWaveIn,&WaveHeader,sizeof(WAVEHDR));
	GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
		return FALSE;
		
	res=waveInStart(hWaveIn) ;
	GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
		return FALSE;
	else
		return TRUE;
	
		
}

void CALLBACK CVoiceRecording::VoiceWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
		if (uMsg==WIM_DATA)
		{
			CVoiceRecording* pVoice=(CVoiceRecording*) dwInstance;
			pVoice->res=waveInUnprepareHeader(pVoice->hWaveIn, &pVoice->WaveHeader, sizeof(WAVEHDR));
			pVoice->GetMMResult(pVoice->res);
			pVoice->RecordFinished();
		}
}

BOOL CVoiceRecording::Open()
{
	if (IsOpen())
		return FALSE;
	res=waveInOpen(&hWaveIn, (UINT) WAVE_MAPPER, &PCMfmt,  (DWORD)&VoiceWaveInProc, (DWORD) this, CALLBACK_FUNCTION);
	GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
	{
		hWaveIn=NULL;
		return FALSE;
	}
	else
		return TRUE;
}

BOOL CVoiceRecording::Close()
{
	//res=waveInReset(hWaveIn);
	//GetMMResult(res);
	//if (res!=MMSYSERR_NOERROR)
	//	return FALSE;
		
	res=waveInClose (hWaveIn);
	GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
		return FALSE;
	else
		return TRUE;
}

BOOL CVoiceRecording::IsOpen()
{
	if (hWaveIn!=NULL)
		return TRUE;
	else
		return FALSE;
}


void CVoiceRecording::RecordFinished()
{
	//write your handler here

	//or create your own classes that derived from this class
	//and override this virtual function
}