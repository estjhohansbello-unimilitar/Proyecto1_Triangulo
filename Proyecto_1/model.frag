#version 330 core

out vec4 FragColor;

in vec3 vNormal;
in vec2 vTex;

// Material (los envía Model::Draw)
uniform sampler2D uTex;
uniform vec4 uBaseColor;
uniform int uHasTexture;
uniform float uAlphaCutoff;

void main()
{
    // Color del material, multiplicado por la textura si la tiene
    vec4 base = uBaseColor;

    if (uHasTexture != 0)
        base *= texture(uTex, vTex);

    // Descarta fragmentos transparentes (alphaMode MASK / BLEND)
    if (base.a < uAlphaCutoff)
        discard;

    // Normal; si se ve la cara trasera del triángulo, se invierte
    vec3 n = normalize(vNormal);
    if (!gl_FrontFacing)
        n = -n;

    // Luz direccional fija en el mundo
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.7));

    float ambient = 0.5;
    float diffuse = max(dot(n, lightDir), 0.0) * 0.5;

    FragColor = vec4(base.rgb * (ambient + diffuse), 1.0);
}