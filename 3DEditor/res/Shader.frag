#version 130

in vec3 EyespaceNormal;
in vec3 Diffuse;
in float IsLightingEnabled;

uniform vec3 LightPosition = vec3(0.25, 0.25, 1);
uniform vec3 AmbientMaterial;
uniform vec3 SpecularMaterial;
uniform float Transparency;
uniform float Shininess = 50;
uniform vec3 AmbientLightWeighting = vec3(0.4, 0.4, 0.4);
uniform vec3 DiffuseLightWeighting = vec3(0.95, 0.95, 0.95);
uniform vec3 SpecularLightWeighting = vec3(0.15, 0.15, 0.15);

out vec4 FragColor;

void main()
{
    if (IsLightingEnabled > 0.5)
    {
        vec3 N = normalize(EyespaceNormal);
        vec3 L = normalize(LightPosition);
        vec3 E = vec3(0, 0, 1);
        vec3 H = normalize(L + E);
    
        float df = max(0.0, dot(N, L));
        float sf = max(0.0, dot(N, H));
        sf = pow(sf, Shininess);

        vec3 color = 
            (AmbientMaterial * AmbientLightWeighting) + 
            (df * Diffuse * DiffuseLightWeighting) + 
            (sf * SpecularMaterial * SpecularLightWeighting);
        FragColor = vec4(color, Transparency);
    }
    else
    {
        FragColor = vec4(AmbientMaterial, 1);
    }    
}