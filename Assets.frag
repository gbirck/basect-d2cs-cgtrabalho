#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D uDiffuseMap;
uniform sampler2D uDetailMap;
uniform float detailScale;
uniform bool useDetail;
uniform sampler2D uNormalMap;
uniform bool useNormalMap;
uniform vec3  fogColor;
uniform float fogDensity;

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

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 diffuseTex)
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
    return (ambient + diffuse + specular) * att;
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

    vec3 result = vec3(0.0);
    for (int i = 0; i < numPointLights; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, baseColor);

    result = max(result, baseColor * 0.08);
     
    float fragDist = length(viewPos - FragPos);
    float fogFactor = exp(-fogDensity * fogDensity * fragDist * fragDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 finalColor = mix(fogColor, result, fogFactor);

    FragColor = vec4(finalColor, texColor.a);
}