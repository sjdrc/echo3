#version 330 core
out vec4 FragColor;
in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 Colour;

in vec3 TangentViewPos;
in vec3 TangentFragPos;
in mat3 TangentBitangentNormal;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D heightMap;

uniform int numberOfLights;
uniform vec3 lightPosition[32];
uniform vec3 lightColour[32];
uniform float lightPower[32];

uniform vec3 camPos;

//uniform samplerCube skybox;

uniform float heightScale;

const float PI = 3.14159265359;

vec3 getNormalFromMapFast(vec2 texCoords)
{
    vec3 tangentNormal = texture(normalMap, texCoords).xyz * 2.0 - 1.0;
    return normalize(TangentBitangentNormal * tangentNormal);
}

vec3 getNormalFromMap(vec2 texCoords)
{
    vec3 tangentNormal = texture(normalMap, texCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(texCoords);
    vec2 st2 = dFdy(texCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec2 ParallaxMappingSteep(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    //const float numLayers = 10;

	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy * heightScale;
    vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2  currentTexCoords     = texCoords;
	float currentHeightMapValue = 1.0 - texture(heightMap, currentTexCoords).r;
	
	while(currentLayerDepth < currentHeightMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get heightMap value at current texture coordinates
		currentHeightMapValue = 1.0 - texture(heightMap, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentHeightMapValue - currentLayerDepth;
	float beforeDepth = (1.0 - texture(heightMap, prevTexCoords).r) - currentLayerDepth + layerDepth;
	
	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;  	
}	

void main()
{
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir   = normalize(TangentViewPos - TangentFragPos);
	vec2 texCoords = ParallaxMappingSteep(TexCoords,  viewDir);

	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    {
		discard;
	}
	
    vec3 albedo     = pow(texture(albedoMap, texCoords).rgb, vec3(3.2));
    float metallic  = texture(metallicMap, texCoords).r;
    float roughness = texture(roughnessMap, texCoords).r;
    float ao        = texture(aoMap, texCoords).r;
    
	// Fast version not used until tangent calculation is fixed
	//vec3 N = getNormalFromMapFast(texCoords);
	vec3 N = getNormalFromMap(texCoords);
    vec3 V = normalize(camPos - WorldPos);
    
	vec3 Lo = vec3(0.0);
	
	vec3 F0 = vec3(0.04); 
	F0      = mix(F0, albedo, metallic);

	for(int i = 0; i < numberOfLights; ++i) 
	{
		vec3 L = normalize(lightPosition[i] - WorldPos);
		vec3 H = normalize(V + L);
	
		float distance    = length(lightPosition[i] - WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance     = lightPower[i] * lightColour[i] * attenuation; 
		vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 nominator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular     = nominator / max(denominator, 0.001);
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		
		kD *= 1.0 - metallic;
	
		float NdotL = max(dot(N, L), 0.0);        
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	//vec3 I = normalize(WorldPos - camPos);
	//vec3 R = reflect(I, normalize(Normal));
	//vec3 reflection = texture(skybox, R).rgb;
	
	vec3 ambient = vec3(0.03) * albedo * ao;// + (reflection * vec3(0.05));
	vec3 color   = (ambient + Lo);

	// Gamma correction. Try switching this on and off
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));
	
	FragColor = vec4(color, 1.0);
	//FragColor = Colour;
	//FragColor = vec4(albedo, 1.0);
	
	//FragColor = vec4(1,1,1,1.0);
	//FragColor = vec4(Normal,1.0);
}
