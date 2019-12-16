/* ********
   テクスチャマッピングのプログラム（バーテックスシェーダ）
   ******** */

void main( void )
{
  // 頂点のクリッピング座標
  // gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  // 上と下は同じ働きをする
  gl_Position = ftransform();
}

/* ******** EOF */
