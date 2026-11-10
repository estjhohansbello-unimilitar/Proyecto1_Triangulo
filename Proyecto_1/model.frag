#version 330 core
 
out vec4 FragColor;
 
in vec3 vNormal;
in vec2 vTex;
 
void main()
{
    // Color base (por ahora fijo; luego vendrá de la textura del material)
    vec3 baseColor = vec3(0.85, 0.65, 0.35);
 
    // Luz direccional fija en el mundo
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.7));
 
    vec3 n = normalize(vNormal);
 
    float ambient = 0.25;
    float diffuse = max(dot(n, lightDir), 0.0);
 
    vec3 color = baseColor * (ambient + diffuse);
 
    FragColor = vec4(color, 1.0);
}