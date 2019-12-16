/* ********
   テクスチャマッピングのプログラム（フラグメントシェーダ）
   ******** */

// OpenGLアプリケーションの値を受け取るuniform変数
uniform sampler2D smptex;   // テクスチャサンプラ
uniform int imgx;           // 画像のx方向のピクセル数
uniform int imgy;           // 画像のy方向のピクセル数

uniform float TurnRate; //回転率

const float pi = 3.14159265;


void main ( void )
{
  vec2 tex_st;
  vec2 position;

  // テクスチャ座標値の計算
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

  // テクスチャの色のサンプリング
  vec3 col = texture2D( smptex, tex ).rgb;

  // フラグメントの色
  gl_FragColor = vec4(col,1.0);
}

/* ******** EOF */
