#version 330

out int i_fFragID;

void main()
{
    i_fFragID = gl_PrimitiveID;
}
