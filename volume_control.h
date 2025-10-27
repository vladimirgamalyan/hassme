#pragma once

// Включает/отключает системный звук (mute/unmute)
void muteAudio(bool mute);

// Проверяет, включен ли сейчас mute
bool isAudioMuted();

// Устанавливает громкость (0.0f – 1.0f)
void setVolume(float level);

// Получает текущую громкость (0.0f – 1.0f)
float getVolume();
