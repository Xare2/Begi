#version 330

#define M_PI 3.1415926535897932384626433832795

const float DUAL_LOBE_WEIGHT = 0.7;
// const vec3 EXTINCTION_MULT = vec3(0.8, 0.8, 1.0);
const float EXTINCTION_MULT = .8;
const float UV_SCALE = .75;

const int STRATUS = 0;
const int STRATOCOMULUS = 1;
const int CUMULUS = 2;

struct ray_t {
	vec3 origin;
	vec3 dir;
};

struct cloudValue_t {
	vec3 color;
	float transmitance;
};

// https://en.wikipedia.org/wiki/Perlin_noise#Permutation
// https://py3.codeskulptor.org/#user309_O3PX0Buc9D_2.py
const vec3 noise_array[85] = vec3[](vec3(0.592156862745098, 0.6274509803921569, 0.5372549019607843), vec3(0.3568627450980392, 0.3529411764705883, 0.05882352941176471), vec3(0.5137254901960784, 0.05098039215686274, 0.788235294117647), vec3(0.3725490196078431, 0.3764705882352941, 0.207843137254902), vec3(0.7607843137254902, 0.9137254901960784, 0.02745098039215686), vec3(0.8823529411764706, 0.5490196078431373, 0.1411764705882353), vec3(0.403921568627451, 0.1176470588235294, 0.2705882352941176), vec3(0.5568627450980392, 0.03137254901960784, 0.3882352941176471), vec3(0.1450980392156863, 0.9411764705882353, 0.08235294117647059), vec3(0.0392156862745098, 0.09019607843137255, 0.7450980392156863), vec3(0.02352941176470588, 0.5803921568627451, 0.9686274509803922), vec3(0.4705882352941176, 0.9176470588235294, 0.2941176470588235), vec3(0.0, 0.1019607843137255, 0.7725490196078432), vec3(0.2431372549019608, 0.3686274509803922, 0.9882352941176471), vec3(0.8588235294117647, 0.796078431372549, 0.4588235294117647), vec3(0.1372549019607843, 0.04313725490196078, 0.1254901960784314), vec3(0.2235294117647059, 0.6941176470588235, 0.1294117647058824), vec3(0.3450980392156863, 0.9294117647058824, 0.5843137254901961), vec3(0.2196078431372549, 0.3411764705882353, 0.6823529411764706), vec3(0.07843137254901961, 0.4901960784313725, 0.5333333333333333), vec3(0.6705882352941176, 0.6588235294117647, 0.2666666666666667), vec3(0.6862745098039216, 0.2901960784313726, 0.6470588235294118), vec3(0.2784313725490196, 0.5254901960784314, 0.5450980392156862), vec3(0.1882352941176471, 0.1058823529411765, 0.6509803921568628), vec3(0.3019607843137255, 0.5725490196078431, 0.6196078431372549), vec3(0.9058823529411765, 0.3254901960784314, 0.4352941176470588), vec3(0.8980392156862745, 0.4784313725490196, 0.2352941176470588), vec3(0.8274509803921568, 0.5215686274509804, 0.9019607843137255), vec3(0.8627450980392157, 0.4117647058823529, 0.3607843137254902), vec3(0.1607843137254902, 0.2156862745098039, 0.1803921568627451), vec3(0.9607843137254902, 0.1568627450980392, 0.9568627450980393), vec3(0.4, 0.5607843137254902, 0.2117647058823529), vec3(0.2549019607843137, 0.09803921568627451, 0.2470588235294118), vec3(0.6313725490196078, 0.00392156862745098, 0.8470588235294118), vec3(0.3137254901960784, 0.2862745098039216, 0.8196078431372549), vec3(0.2980392156862745, 0.5176470588235295, 0.7333333333333333), vec3(0.8156862745098039, 0.3490196078431372, 0.07058823529411765), vec3(0.6627450980392157, 0.7843137254901961, 0.7686274509803922), vec3(0.5294117647058824, 0.5098039215686274, 0.4549019607843137), vec3(0.7372549019607844, 0.6235294117647059, 0.3372549019607843), vec3(0.6431372549019608, 0.392156862745098, 0.4274509803921568), vec3(0.7764705882352941, 0.6784313725490196, 0.7294117647058823), vec3(0.01176470588235294, 0.2509803921568627, 0.203921568627451), vec3(0.8509803921568627, 0.8862745098039215, 0.9803921568627451), vec3(0.4862745098039216, 0.4823529411764706, 0.0196078431372549), vec3(0.792156862745098, 0.1490196078431373, 0.5764705882352941), vec3(0.4627450980392157, 0.4941176470588236, 1.0), vec3(0.3215686274509804, 0.3333333333333333, 0.8313725490196079), vec3(0.8117647058823529, 0.807843137254902, 0.2313725490196079), vec3(0.8901960784313725, 0.1843137254901961, 0.06274509803921569), vec3(0.2274509803921569, 0.06666666666666667, 0.7137254901960784), vec3(0.7411764705882353, 0.1098039215686274, 0.1647058823529412), vec3(0.8745098039215686, 0.7176470588235294, 0.6666666666666666), vec3(0.8352941176470589, 0.4666666666666667, 0.9725490196078431), vec3(0.596078431372549, 0.007843137254901961, 0.1725490196078431), vec3(0.6039215686274509, 0.6392156862745098, 0.2745098039215687), vec3(0.8666666666666667, 0.6, 0.396078431372549), vec3(0.6078431372549019, 0.6549019607843137, 0.1686274509803922), vec3(0.6745098039215687, 0.03529411764705882, 0.5058823529411764), vec3(0.08627450980392157, 0.1529411764705882, 0.9921568627450981), vec3(0.07450980392156863, 0.3843137254901961, 0.4235294117647059), vec3(0.4313725490196079, 0.3098039215686275, 0.4431372549019608), vec3(0.8784313725490196, 0.9098039215686274, 0.6980392156862745), vec3(0.7254901960784313, 0.4392156862745098, 0.407843137254902), vec3(0.8549019607843137, 0.9647058823529412, 0.3803921568627451), vec3(0.8941176470588236, 0.984313725490196, 0.1333333333333333), vec3(0.9490196078431372, 0.7568627450980392, 0.9333333333333333), vec3(0.8235294117647058, 0.5647058823529412, 0.04705882352941176), vec3(0.7490196078431373, 0.7019607843137254, 0.6352941176470588), vec3(0.9450980392156862, 0.3176470588235294, 0.2), vec3(0.5686274509803921, 0.9215686274509803, 0.9764705882352941), vec3(0.05490196078431372, 0.9372549019607843, 0.4196078431372549), vec3(0.1921568627450981, 0.7529411764705882, 0.8392156862745098), vec3(0.1215686274509804, 0.7098039215686275, 0.7803921568627451), vec3(0.4156862745098039, 0.615686274509804, 0.7215686274509804), vec3(0.3294117647058823, 0.8, 0.6901960784313725), vec3(0.4509803921568628, 0.4745098039215686, 0.196078431372549), vec3(0.1764705882352941, 0.4980392156862745, 0.01568627450980392), vec3(0.5882352941176471, 0.996078431372549, 0.5411764705882353), vec3(0.9254901960784314, 0.803921568627451, 0.3647058823529412), vec3(0.8705882352941177, 0.4470588235294118, 0.2627450980392157), vec3(0.1137254901960784, 0.09411764705882353, 0.2823529411764706), vec3(0.9529411764705882, 0.5529411764705883, 0.5019607843137255), vec3(0.7647058823529411, 0.3058823529411765, 0.2588235294117647), vec3(0.8431372549019608, 0.2392156862745098, 0.611764705882353));

// uniform sampler2D textureColor;
// uniform sampler2D textureDepth;

uniform sampler2D weatherMap;
uniform sampler2D curlNoise;
uniform sampler3D cloudNoise;
uniform sampler3D detailNoise;
uniform samplerCube cubetextureColor;

// uniform float camRadius;
uniform vec3 camPos;
uniform vec3 boxPos;
uniform vec3 boxSize;
uniform vec3 sunPos;
uniform vec3 sunDir;
uniform vec3 sunColor;
uniform vec3 ambientColor;
// uniform mat4 camRotation;
// uniform mat4 V;
// uniform mat4 P;

uniform int cloudType = STRATOCOMULUS;
uniform float time;
uniform float brightness = 1.;
uniform float silver_intensity = 1.;
uniform float silver_spread = .5;
uniform float eccentrity = .6;
uniform float anvil_bias = .5;
uniform float curlNoiseScale = 200;
uniform float uvScaleBase = 1;
uniform float uvScaleDetail = 1;
uniform float lightSampleScale = 2;
uniform int lightSampleCount = 6;

in vec2 fCoordTex;
in vec3 fPos;
in vec3 fTexCube;

uniform int nSamples = 128;

float avg(vec3 v) {
	return (v.x + v.y + v.z) / 3;
}

float saturate(float t) {
	return clamp(t, 0, 1);
}

vec3 saturate3(vec3 x) {
	return clamp(x, vec3(0.0), vec3(1.0));
}

float remap(float value, float original_min, float original_max, float new_min, float new_max) {
	return new_min + (((value - original_min) / (original_max - original_min)) * (new_max - new_min));
}

// 0 -> stratus
// 1 -> stratocumulus
// 2 -> cumulus
float heightGradientFactor(float h, int cloudType) {
	if(cloudType == CUMULUS)
		return remap(h, 0.0, 0.1, 0.0, 1.0) * remap(h, 0.6, 0.8, 1.0, 0.0);

	if(cloudType == STRATUS)
		return remap(h, 0.0, 0.05, 0.0, 1.0) * remap(h, 0.1, 0.15, 1.0, 0.0);

	return remap(h, 0.0, 0.1, 0.0, 1.0) * remap(h, 0.2, 0.3, 1.0, 0.0);
}

float henyeyGreenstein(float g, float mu) {
	float gg = g * g;
	return ((1.0 - gg) / pow(1.0 + gg - 2.0 * g * mu, 1.5)) / (4.0 * M_PI);
}

float dualHenyeyGreenstein(float g, float costh) {
	return mix(henyeyGreenstein(-g, costh), henyeyGreenstein(g, costh), DUAL_LOBE_WEIGHT);
}

float phaseFunction(float g, float costh) {
	// float silver_intensity = 1;
	// float silver_spread = .5;

	float p = max(henyeyGreenstein(g, costh), silver_intensity * henyeyGreenstein(.99 - silver_spread, costh));

	// p = dualHenyeyGreenstein(g, costh) + p * .0000001;

	return p;
}

uniform int starting_noise = 0;
int noise_idx = starting_noise;
vec3 getNoiseValue() {
	noise_idx++;
	noise_idx = noise_idx % 85;
	return noise_array[noise_idx] * 2 - vec3(1);
}

vec2 intersectAABB(vec3 rayOrigin, vec3 rayDir, vec3 boxMin, vec3 boxMax) {
	vec3 tMin = (boxMin - rayOrigin) / rayDir;
	vec3 tMax = (boxMax - rayOrigin) / rayDir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	return vec2(tNear, tFar);
}

vec2 intersectSphere(vec3 rayOrigin, vec3 rayDir, vec3 center, float radius) {
	vec3 oc = rayOrigin - center;
	float a = dot(rayDir, rayDir);
	float b = 2.0 * dot(oc, rayDir);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	if(discriminant < 0) {
		return vec2(-1.0);
	}

	float t1 = (-b - sqrt(discriminant)) / (2.0 * a);
	float t2 = (-b + sqrt(discriminant)) / (2.0 * a);

	return vec2(t1, t2);
}

float invlerp(float a, float b, float v) {
	return saturate((v - a) / (b - a));
}

vec3 getUVW(vec3 p, vec3 minBound, vec3 maxBound, float tilingFactor) {
	float u = invlerp(minBound.x, maxBound.x, p.x);
	float v = invlerp(minBound.y, maxBound.y, p.y);
	float w = invlerp(minBound.z, maxBound.z, p.z);

	return vec3(u * tilingFactor, w * tilingFactor, v);
}

// https://www.gamedev.net/forums/topic/680832-horizonzero-dawn-cloud-system/?page=1
float sampleDensity(vec3 uvw, int doCheaply, float mipLevel) {
	float height_fraction = uvw.z;

	vec3 wind_direction = vec3(1.0, 0.0, 0.0);

	float cloud_speed = 10.0 / boxSize.x; 

    // cloud_top offset - push the tops of the clouds along this wind direction by this many units.  
	float cloud_top_offset = 500.0 / boxSize.x;  

    // // skew in wind direction  
	uvw += height_fraction * wind_direction * cloud_top_offset;  

    //animate clouds in wind direction and add a small upward bias to the wind direction  
	uvw += (wind_direction + vec3(0.0, 0.1, 0.0)) * time * cloud_speed;

	vec4 low_frequency_noises = textureLod(cloudNoise, uvw, mipLevel).rgba;

    // build an fBm out of  the low frequency Worley noises that can be used to add detail to the Low frequency Perlin-Worley noise  
	float low_freq_fBm = (low_frequency_noises.g * 0.625) + (low_frequency_noises.b * 0.25) + (low_frequency_noises.a * 0.125);  

    // define the base cloud shape by dilating it with the low frequency fBm made of Worley noise.  
	float base_cloud = remap(low_frequency_noises.r, -(1.0 - low_freq_fBm), 1.0, 0.0, 1.0);

    // Get the density-height gradient using the density-height function (not included)  
	float density_height_gradient = heightGradientFactor(height_fraction, cloudType);

    // apply the height function to the base cloud shape  
	base_cloud *= density_height_gradient * (1-height_fraction);

	// esto no tiene mas resolucions >:(
	float cloud_coverage = texture(weatherMap, uvw.xy / uvScaleBase).g;
	// float anvil_bias = .5;

	// cloud_coverage = pow(cloud_coverage, remap(height_fraction, 0.7, 0.8, 1.0, mix(1.0, 0.5, anvil_bias)));
	// cloud_coverage = pow(cloud_coverage, remap(height_fraction, 0.7, 0.8, mix(1.0, 0.5, anvil_bias), 1.));  

    //Use remapper to apply cloud coverage attribute  
	float base_cloud_with_coverage = remap(base_cloud, cloud_coverage, 1.0, 0.0, 1.0);   

    //Multiply result by cloud coverage so that smaller clouds are lighter and more aesthetically pleasing.  
	base_cloud_with_coverage *= cloud_coverage;
	// base_cloud_with_coverage = base_cloud * cloud_coverage;

	float final_cloud = base_cloud_with_coverage;

	if(doCheaply != 1) {
 		// add some turbulence to bottoms of clouds using curl noise.  Ramp the effect down over height and scale it by some value (200 in this example)  
		vec2 curl_noise = texture2D(curlNoise, uvw.xy).rg;

		// pass from [0, 1] range to [-1, 1]
		// curl_noise = curl_noise * 2 - vec2(1);

		// float curlNoiseScale = 200 / boxSize.x;
		float normalizedNoiseScale = curlNoiseScale / boxSize.x;
		uvw.xz += curl_noise.rg * (1.0 - height_fraction) * normalizedNoiseScale;

        // sample high-frequency noises
		vec3 high_frequency_noises = textureLod(detailNoise, uvw * (uvScaleDetail / uvScaleBase), mipLevel).rgb;  

        // build High frequency Worley noise fBm  
		float high_freq_fBm = (high_frequency_noises.r * 0.625) + (high_frequency_noises.g * 0.25) + (high_frequency_noises.b * 0.125);  

        // get the height_fraction for use with blending noise types over height  
		height_fraction = uvw.z;

        // transition from wispy shapes to billowy shapes over height  
		float high_freq_noise_modifier = mix(high_freq_fBm, 1.0 - high_freq_fBm, saturate(height_fraction * 10.0));  

        // erode the base cloud shape with the distorted high frequency Worley noises.  
		final_cloud = remap(base_cloud_with_coverage, high_freq_noise_modifier * 0.2, 1.0, 0.0, 1.0);
		// final_cloud = remap(base_cloud_with_coverage, high_freq_noise_modifier * .3, 1.0, 0.0, 1.0);
	}

	return final_cloud;
}

float getLightEnergy(float height_fraction, float dl, float ds_loded, float eccentrity, float cos_angle, float step_size, float brightness) {
	// height_fraction = heightGradientFactor(height_fraction);

	float primary_attenuation = exp(-dl);

	float secondary_attenuation = exp(-dl * 0.25) * 0.7;

	// beer-lambert
	float attenuation_probability = max(remap(cos_angle, 0.25, 1.0, secondary_attenuation, secondary_attenuation * 0.25), primary_attenuation);

	float phase_probability = phaseFunction(eccentrity, cos_angle);

    // in-scattering – one difference from presentation slides – we also reduce this effect once light has attenuated to make it directional. 
	float depth_probability = mix(0.05 + pow(ds_loded, remap(height_fraction, 0.3, 0.85, 0.5, 2.0)), 1.0, saturate(dl / step_size));

	// valroes menores a .063 petan con la funcion vertical_probability
	float clampledHeightFraction = max(.065, height_fraction);

	float vertical_probability = pow(remap(clampledHeightFraction, 0.07, 0.14, 0.1, 1.0), 0.8);

	// return vertical_probability * depth_probability;
	float in_scatter_probability = depth_probability * vertical_probability;

	float powder = 1.0 - exp(-dl * 2.0 * EXTINCTION_MULT);

	float light_energy = attenuation_probability * phase_probability * in_scatter_probability * brightness;
	// light_energy = attenuation_probability * phase_probability * powder * brightness;

	return light_energy;
}

float sampleCloudDensityAlongCone(ray_t lightRay, float sampleLength, vec3 minBound, vec3 maxBound) {
	// float density_along_cone = 0;

	// // sampleLength = distance(maxBound.y, lightRay.origin.y) / 6;

	// float cone_spread_multplier = sampleLength;

	// for(int i = 1; i < lightSampleCount; i++) {
	// 	lightRay.origin = lightRay.origin + lightRay.dir + (getNoiseValue() * cone_spread_multplier * float(i));
	// 	// lightRay.origin = lightRay.origin + lightRay.dir * cone_spread_multplier + getNoiseValue() * i * cone_spread_multplier; // * float(i);

	// 	vec3 uvw = getUVW(lightRay.origin, minBound, maxBound);

	// 	if(uvw.x > 1 || uvw.y > 1 || uvw.z > 1) {
	// 		break;
	// 	}

	// 	int mip_offset = int(i * 0.5);

	// 	density_along_cone += evalDensity(uvw, 0, mip_offset);
	// }

	// return density_along_cone;

	float density_along_cone = 0.0;

	float cone_spread_multplier = length(sampleLength);
	vec3 uvw;
    // lighting ray march loop  
	for(int i = 0; i <= lightSampleCount; i++) {  

        // add the current step offset to the sample position  
		// lightRay.origin = lightRay.origin + lightRay.dir + (cone_spread_multplier * getNoiseValue() * float(i));  

		lightRay.origin = lightRay.origin + lightRay.dir * cone_spread_multplier + getNoiseValue() * float(i);

        // sample cloud density the expensive way  
		float mip_offset = int(i * 0.5);

		uvw = getUVW(lightRay.origin, minBound, maxBound, uvScaleBase);

		// uvw = saturate3(uvw);

		density_along_cone += sampleDensity(uvw, 0, mip_offset);
	}

	// return uvw.z;

	// return density_along_cone;

	return density_along_cone / lightSampleCount;
}

float MultipleOctaveScattering(float density, float mu) {
	float attenuation = 0.2;
	float contribution = 0.2;
	float phaseAttenuation = 0.5;

	float a = 1.0;
	float b = 1.0;
	float c = eccentrity;
	float g = 0.85;
	const float scatteringOctaves = 4.0;

	float luminance = (0.0);

	for(float i = 0.0; i < scatteringOctaves; i++) {
		float phaseFunction = phaseFunction(c, mu);
		float beers = (exp(-density * EXTINCTION_MULT * a));

		// luminance += b * phaseFunction * beers;
		luminance += b * phaseFunction * beers;

		a *= attenuation;
		b *= contribution;
		c *= (1.0 - phaseAttenuation);
	}
	return luminance;
}

float calculateLightEnergy(ray_t lightRay, vec3 minBound, vec3 maxBound, float totalLightPathDistance, float stepLength, float mu) {
	float lightRayDensity = 0.0;

	lightRayDensity = sampleCloudDensityAlongCone(lightRay, stepLength, minBound, maxBound);

	float beersLaw = MultipleOctaveScattering(lightRayDensity, mu);
	float powder = (1.0 - exp(-lightRayDensity * 2.0 * EXTINCTION_MULT));

	return beersLaw * mix(2.0 * powder, (1.0), remap(mu, -1.0, 1.0, 0.0, 1.0)) * brightness;
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/volume-rendering-for-developers/intro-volume-rendering.html
cloudValue_t computeCloud() {
	cloudValue_t cloudValue;
	cloudValue.color = vec3(0);
	cloudValue.transmitance = 1;

	// float eccentrity = .6;
	// float brightness = 1.9999;

	vec3 rayOrigin = camPos;

	ray_t ray;

	// AB = B - A
	ray.dir = normalize(fPos - camPos);
	ray.origin = rayOrigin;

	float cos_angle = dot(ray.dir, -sunDir);

	vec3 minBound = boxPos - boxSize;
	vec3 maxBound = boxPos + boxSize;

	// vec2 intersections = intersectSphere(ray.origin, ray.dir, boxPos, boxSize.x);
	vec2 intersections = intersectAABB(ray.origin, ray.dir, minBound, maxBound);

	if(max(intersections.x, intersections.y) < 0 || min(intersections.x, intersections.y) < 0) {
		cloudValue.transmitance = -1;
		return cloudValue;
	}

	vec3 pathMarchOrigin = ray.origin + ray.dir * intersections.x;
	vec3 pathMarchEnd = ray.origin + ray.dir * intersections.y;

	float totalPathDistance = distance(pathMarchEnd, pathMarchOrigin);
	float sampleLength = totalPathDistance / float(nSamples);
	float maxSampleLength = boxSize.y / float(nSamples);
	float minSampleLength = maxSampleLength / 2;

	// sampleLength = clamp(sampleLength, minSampleLength, maxSampleLength);
	sampleLength = maxSampleLength;

	float normalizedSampleLength = sampleLength / boxSize.y;

	float lightSampleLength = sampleLength * lightSampleScale;

	ray.origin = pathMarchOrigin;

	float traveledDistance = 0;

	float density_accumulated = 0;

	float sunFactor = remap(cos_angle, -1, 1, 0.25, 1);

	vec3 sunLight = sunColor * sunFactor;

	while(traveledDistance < totalPathDistance) {

		vec3 uvw = getUVW(ray.origin, minBound, maxBound, uvScaleBase);
		float density = sampleDensity(uvw, 0, 0.);

		if(density > 0.001) {

			float ambientFactor = remap(uvw.z, 0, 1, 1, 7);
			vec3 ambient = ambientColor * ambientFactor;
			vec3 lightColor = ambient + sunLight;

			density_accumulated += density;

			float p = phaseFunction(eccentrity, cos_angle);

			ray_t lightRay;
			lightRay.origin = ray.origin;
			// AB = B - A
			lightRay.dir = normalize(sunPos - lightRay.origin);

			float dl = sampleCloudDensityAlongCone(lightRay, lightSampleLength, minBound, maxBound);

			float lightEnergy = getLightEnergy(uvw.z, dl, density, eccentrity, cos_angle, lightSampleLength, brightness);


			float lightEnergy3 = avg(ambient) +  calculateLightEnergy(lightRay, minBound, maxBound, 0., lightSampleLength, cos_angle);

			float T = exp(-(density * sampleLength * EXTINCTION_MULT));

			float integScatt = density * (lightEnergy - lightEnergy * T) / density;
			float integScatt3 = density * (lightEnergy3 - lightEnergy3 * T) / density;
			float light_intensity = integScatt3 * cloudValue.transmitance;

			cloudValue.color += vec3(light_intensity);
			// cloudValue.color += vec3(integScatt3 * cloudValue.transmitance) * lightColor;
			cloudValue.transmitance *= T;

			if(cloudValue.transmitance < 1e-8) {
				break;
			}
		}

		ray.origin = ray.origin + ray.dir * sampleLength;
		traveledDistance += sampleLength;
	}

	// cloudValue.transmitance = 1;

	cloudValue.color = saturate3(cloudValue.color);
	cloudValue.transmitance = saturate(cloudValue.transmitance);

	// cloudFactor.color = vec3(cos_angle);

	return cloudValue;
}

vec4 getSkyBoxColor() {
	vec4 col = vec4(.1, .1, .8, 1.);
	// return col;
	return textureCube(cubetextureColor, fTexCube);
}

void main() {
	vec4 backgroundColor = getSkyBoxColor();

	cloudValue_t cloud = computeCloud();
	vec3 color = vec3(0);

	float t = cloud.transmitance;

	if(t < 0) {
		t = 0;
		// gl_FragColor = vec4(1, 1, 0, 1);
		// return;
	}

	// color = mix(backgroundColor.rgb, cloud.color, 1 - t);
	color = backgroundColor.rgb * t + cloud.color;

	gl_FragColor = vec4(vec3(color), 1);
}
