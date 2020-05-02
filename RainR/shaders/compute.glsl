#version 450 core
#extension GL_ARB_compute_shader			   : enable
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_compute_variable_group_size  : enable
layout(std140, binding = 0) buffer offs{
  vec4 offsets[];
};
layout(local_size_variable) in;

vec4 velocity = vec4(0.001, -0.01, 0, 0);
void main(){
  uint gid = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_NumWorkGroups.x;
  vec4 off = offsets[ gid ];
  for(int i = 0; i < 100; i++){
    velocity.x += fract(tan(velocity.x));
    velocity.y += atan(fract(cos(velocity.x)));
  }
  off = off + velocity;
  off.x = mod(off.x + 1, 2)-1;
  off.y = mod(off.y + 1, 2)-1;
  offsets[ gid ] = off;
}
