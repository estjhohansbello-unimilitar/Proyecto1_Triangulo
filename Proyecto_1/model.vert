#version 330 core

// Posición del vértice
layout (location = 0) in vec3 aPos;

// Normal del vértice
layout (location = 1) in vec3 aNormal;

// Coordenadas de textura
layout (location = 2) in vec2 aTex;

// Matriz de transformación
uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
}
