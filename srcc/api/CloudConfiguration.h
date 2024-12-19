#pragma once
class CloudConfiguration
{
public:
	inline static float cloudType = 2;
	inline static int nSamples = 128;
	inline static float lightSampleScale = 5.5f;
	inline static int lightSampleCount = 6;
	inline static float brightness = 3.5f;
	inline static float silver_intensity = .5f;
	inline static float silver_spread = 1.f;
	inline static float eccentrity = .35f;
	inline static float anvil_bias = .15f;
	inline static float curlNoiseScale = 20000;
	inline static float uvScaleBase = 5;
	inline static float uvScaleDetail = 60;
	inline static float uvScaleCoverage = 2.5f;
	inline static float debugZ = 1;
	inline static float worldRadius = 6371;
	inline static float cloudStartRange = 3000;
	inline static float cloudRange = 1000;
	inline static float fogDensity = .005f;
	inline static float skyFarMultiplier = 30.f;
	inline static float cloudMoveSpeed = 0.f;
};

