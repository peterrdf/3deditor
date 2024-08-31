#version 130

in vec4 Position;
in vec3 Normal;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform vec3 DiffuseMaterial;
uniform float EnableLighting = 1;

out vec3 EyespaceNormal;
out vec3 Diffuse;
out float IsLightingEnabled;

void main()
{
    if (EnableLighting > 0.5)
    {
        EyespaceNormal = NormalMatrix * Normal;    
        Diffuse = DiffuseMaterial;
    }

    IsLightingEnabled = EnableLighting;

    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;    
}