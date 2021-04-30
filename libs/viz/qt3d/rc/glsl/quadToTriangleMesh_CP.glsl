#version 430

// Defines a workGroup size, it is the number of shader invocations for this group
layout(local_size_x = 6) in;

layout(binding=0) coherent buffer c_in_index
{
    int buff[];
};

void main() {

    if(gl_LocalInvocationID.x == 3)
    {
        buff[gl_GlobalInvocationID.x] = buff[gl_GlobalInvocationID.x - 1];
    }

    if(gl_LocalInvocationID.x == 5)
    {
        buff[gl_GlobalInvocationID.x] = buff[gl_GlobalInvocationID.x - 5];
    }
}
