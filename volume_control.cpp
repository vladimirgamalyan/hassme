#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include "volume_control.h"

// ��������������� ������� � �������� ��������� IAudioEndpointVolume
static IAudioEndpointVolume* getEndpointVolume()
{
	HRESULT hr;
	IMMDeviceEnumerator* pEnum = nullptr;
	IMMDevice* pDevice = nullptr;
	IAudioEndpointVolume* pVolume = nullptr;

	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator), (void**)&pEnum);
	if (FAILED(hr)) return nullptr;

	hr = pEnum->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
	if (FAILED(hr)) {
		pEnum->Release();
		return nullptr;
	}

	hr = pDevice->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&pVolume);

	pDevice->Release();
	pEnum->Release();

	if (FAILED(hr)) return nullptr;
	return pVolume;
}

// ��������/��������� ����
void muteAudio(bool mute)
{
	CoInitialize(NULL);
	IAudioEndpointVolume* pVolume = getEndpointVolume();
	if (pVolume) {
		pVolume->SetMute(mute, NULL);
		pVolume->Release();
	}
	CoUninitialize();
}

// ���������, ������� �� mute
bool isAudioMuted()
{
	BOOL muted = FALSE;
	CoInitialize(NULL);
	IAudioEndpointVolume* pVolume = getEndpointVolume();
	if (pVolume) {
		pVolume->GetMute(&muted);
		pVolume->Release();
	}
	CoUninitialize();
	return muted;
}

// ������������� ��������� (0.0f � 1.0f)
void setVolume(float level)
{
	CoInitialize(NULL);
	IAudioEndpointVolume* pVolume = getEndpointVolume();
	if (pVolume) {
		// clamp ��������
		if (level < 0.0f) level = 0.0f;
		if (level > 1.0f) level = 1.0f;
		pVolume->SetMasterVolumeLevelScalar(level, NULL);
		pVolume->Release();
	}
	CoUninitialize();
}

// �������� ������� ���������
float getVolume()
{
	float vol = 0.0f;
	CoInitialize(NULL);
	IAudioEndpointVolume* pVolume = getEndpointVolume();
	if (pVolume) {
		pVolume->GetMasterVolumeLevelScalar(&vol);
		pVolume->Release();
	}
	CoUninitialize();
	return vol;
}
