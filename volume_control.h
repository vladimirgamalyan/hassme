#pragma once

// ��������/��������� ��������� ���� (mute/unmute)
void muteAudio(bool mute);

// ���������, ������� �� ������ mute
bool isAudioMuted();

// ������������� ��������� (0.0f � 1.0f)
void setVolume(float level);

// �������� ������� ��������� (0.0f � 1.0f)
float getVolume();
