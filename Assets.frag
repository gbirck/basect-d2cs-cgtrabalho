#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;
uniform sampler2D uDetailMap;
uniform float detailScale;
uniform bool useDetail;
uniform sampler2D uNormalMap;
uniform bool useNormalMap;
uniform vec3  fogColor;
uniform float fogDensity;
uniform samplerCube uEnvMap;
uniform bool useEnvMap;
uniform float reflectivity;
uniform sampler2D uShadowMap;
uniform bool useShadow;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 8
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int        numPointLights;
uniform vec3       viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.005 * (1.0 - dot(norm, lightDir)), 0.0015);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, float shadowFactor)
{
    vec3  lightDir   = normalize(light.position - fragPos);
    float diff       = max(dot(norm, lightDir), 0.0);
    vec3  halfwayDir = normalize(lightDir + viewDir);
    float spec       = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    float dist       = length(light.position - fragPos);
    float att        = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

    vec3 ambient  = light.ambient  * diffuseTex;
    vec3 diffuse  = light.diffuse  * diff * diffuseTex;
    vec3 specular = light.specular * spec;

    return (ambient + (1.0 - shadowFactor) * (diffuse + specular)) * att;
}

void main()
{

    vec4 texColor = texture(uDiffuseMap, TexCoord);
    if (texColor.a < 0.1) discard;

    vec3 baseColor = (texColor.rgb == vec3(0.0)) ? vec3(0.75) : texColor.rgb;

    if (useDetail)
    {
        vec4 detail = texture(uDetailMap, TexCoord * detailScale);
        baseColor *= detail.rgb * 2.0;
    }

    vec3 norm;
    if (useNormalMap)
    {
        vec4 ns = texture(uNormalMap, TexCoord);
        if (ns.rgb != vec3(0.0))
            norm = normalize(TBN * normalize(ns.rgb * 2.0 - 1.0));
        else
            norm = normalize(Normal);
    }
    else
        norm = normalize(Normal);

    vec3 viewDir = normalize(viewPos - FragPos);

    float shadowFactor = 0.0;
    if (useShadow && numPointLights > 0)
    {
        vec3 lightDir0 = normalize(pointLights[0].position - FragPos);
        shadowFactor = ShadowCalculation(FragPosLightSpace, norm, lightDir0);
    }

    vec3 result = vec3(0.0);
    for (int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, baseColor, (i == 0) ? shadowFactor : 0.0);

    vec3 envColor = vec3(0.0);
    if (useEnvMap)
    {
        vec3 I = -viewDir;
        vec3 R = reflect(I, norm);
        envColor = texture(uEnvMap, R).rgb;
    }

    if (useEnvMap)
        result = mix(result, envColor, reflectivity);

    float fragDist = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * fogDensity * fragDist * fragDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 finalColor = mix(fogColor, result, fogFactor);

    FragColor = vec4(finalColor, texColor.a);
}