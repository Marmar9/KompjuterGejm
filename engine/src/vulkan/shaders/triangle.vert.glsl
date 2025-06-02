#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 camera;
    mat4 perspective;
} ubo;

layout(location = 0) in vec3 position;

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

layout(location = 0) out vec3 fragColor;
// * ubo.view * ubo.model 
void main() {
    gl_Position = ubo.perspective * ubo.camera * ubo.model * vec4(position, 1.0);
    fragColor = colors[gl_VertexIndex % 3];
}
