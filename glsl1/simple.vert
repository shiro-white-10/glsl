#version 120

// simple.vert

void main(void)
{
  // ���_�ʒu
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
