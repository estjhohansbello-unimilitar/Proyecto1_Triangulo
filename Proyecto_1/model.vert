#version 330 core
 
// Posición del vértice
layout (location = 0) in vec3 aPos;
 
// Normal del vértice
layout (location = 1) in vec3 aNormal;
 
// Coordenadas de textura
layout (location = 2) in vec2 aTex;
 
// Matriz de transformación completa (proyección * vista * modelo)
uniform mat4 uMVP;
 
// Matriz de modelo sola, para llevar las normales al espacio mundo
uniform mat4 uModel;
 
out vec3 vNormal;
out vec2 vTex;
 
void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
 
    // Matriz normal: evita deformar las normales con escalas no uniformes
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
 
    vTex = aTex;
}