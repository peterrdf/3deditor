#version 130

in vec4 Position;
in vec3 Normal;
in vec2 UV;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform vec3 DiffuseMaterial;
uniform float EnableLighting = 1;
uniform float EnableTexture = 0;

out vec3 EyespaceNormal;
out vec3 Diffuse;
out float _EnableLighting;
out float _EnableTexture;
out vec2 _UV;

void main()
{
    if (EnableLighting > 0.5)
    {
        EyespaceNormal = NormalMatrix * Normal;    
        Diffuse = DiffuseMaterial;
    }

    _EnableLighting = EnableLighting;
    _EnableTexture = EnableTexture;
    _UV = UV;

    gl_Position = ProjectionMatrix * ModelViewMatrix * Position;
    gl_PointSize = 5;
}