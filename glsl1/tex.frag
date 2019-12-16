/* ********
   �e�N�X�`���}�b�s���O�̃v���O�����i�t���O�����g�V�F�[�_�j
   ******** */

// OpenGL�A�v���P�[�V�����̒l���󂯎��uniform�ϐ�
uniform sampler2D smptex;   // �e�N�X�`���T���v��
uniform int imgx;           // �摜��x�����̃s�N�Z����
uniform int imgy;           // �摜��y�����̃s�N�Z����

uniform float TurnRate; //��]��

const float pi = 3.14159265;


void main ( void )
{
  vec2 tex_st;
  vec2 position;

  // �e�N�X�`�����W�l�̌v�Z
  tex_st.s = gl_FragCoord.x / float(imgx);   // 0.0 <= tex_st.s <= 1.0
  tex_st.t = gl_FragCoord.y / float(imgy);    // 0.0 <= tex_st.t <= 1.0


  vec2 center;
  center.x = 0.2;
  center.y = 0.2;


  vec2 p = tex_st.st - center;

  float distance = length(p);
  float theta = TurnRate *(1.0 - distance/0.5) * pi;

  mat2 rotate_matrix = mat2(cos(theta),sin(theta),-sin(theta),cos(theta));
  position = rotate_matrix * p + center;


  vec2 tex = vec2(position.x , position.y );

  // �e�N�X�`���̐F�̃T���v�����O
  vec3 col = texture2D( smptex, tex ).rgb;

  // �t���O�����g�̐F
  gl_FragColor = vec4(col,1.0);
}

/* ******** EOF */
