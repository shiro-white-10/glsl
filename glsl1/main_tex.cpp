/* ********
   テクスチャマッピングのプログラム
   ********
   下記ページのプログラムを参考にして作成
   床井研究室：第１回 シェーダプログラムの読み込み
   http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20051006
   床井研究室：第３回 テクスチャの参照
   http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20051008
   ******** */

/*
** ヘッダファイル
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glut.h"
#include "glext.h"

/*
** シェーダ
*/
#include "glsl.h"
static GLuint vertShader;
static GLuint fragShader;
static GLuint gl2Program;
static char vertname[] = "tex.vert";
static char fragname[] = "tex.frag";
float TurnRate = 0.0;

/*
** 画像
*/
#define IMGMAXX 1600                    // 画像のx方向の最大ピクセル数
#define IMGMAXY 1600                    // 画像のy方向の最大ピクセル数
static GLubyte img[IMGMAXX*IMGMAXY][3]; // 画像のRGB値
static char imgname[] = "a.ppm";   // 画像のPPMファイル
static int imgx;                        // 画像のx方向のピクセル数
static int imgy;                        // 画像のy方向のピクセル数

/*
** 関数の宣言
*/
static void init( void );
static void init_tex( void );
static void init_glsl( void );
static void fin_glsl( void );
static void fin_tex( void );
static void resize( int w, int h );
static void display( void );
static void keyboard( unsigned char key, int x, int y );
static int getPPM( char nam[], int *xsizp, int *ysizp, unsigned char rgb[][3] );
static void idle(void);

/* ****
   メインプログラム
   **** */

/*
** メイン関数
*/
int main( int argc, char *argv[] )
{
  /* 画像の読み込み */
  if( getPPM( imgname, &imgx, &imgy, img ) != 0 ) {
    fprintf( stderr, "error: main() -> getPPM() -> %s\n", imgname );
    return( 1 );
  }

  /* OpenGLの初期化 */
  glutInitWindowSize( imgx, imgy );
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
  glutCreateWindow( argv[0] );

  /* コールバック関数の登録 */
  glutReshapeFunc( resize );
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutIdleFunc(idle);

  /* 全般的な初期設定 */
  init();
  /* テクスチャの初期設定 */
  init_tex();
  /* GLSLの初期設定 */
  init_glsl();

  /* イベントループ */
  glutMainLoop();

  /* GLSLの終了設定 */
  fin_glsl();
  /* テクスチャの終了設定 */
  fin_tex();

  /* 終了 */
  return( 0 );
}

/* ****
   初期設定と終了設定
   **** */

/*
** 全般的な初期設定
*/
static void init( void )
{
  /* 背景色 */
  glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

/*
** テクスチャの初期設定
*/
static void init_tex( void )
{
  /* テクスチャ画像はバイト単位に詰め込まれている */
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  
  /* テクスチャを拡大・縮小する方法の指定 */
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  
  /* テクスチャの繰り返し方法の指定 */
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  
  /* テクスチャの割り当て */
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imgx, imgy, 0, GL_RGB, GL_UNSIGNED_BYTE, img );

  /* テクスチャマッピングの有効化 */
  glEnable( GL_TEXTURE_2D );
}

/*
** GLSLの初期設定
*/
static void init_glsl( void )
{
  /* シェーダプログラムのコンパイル／リンク結果を得る変数 */
  GLint compiled, linked;

  /* GLSLの初期化 */
  if( glslInit() ) exit( 1 );

  /* シェーダオブジェクトの作成 */
  vertShader = glCreateShader( GL_VERTEX_SHADER );
  fragShader = glCreateShader( GL_FRAGMENT_SHADER );

  /* シェーダのソースプログラムの読み込み */
  if( readShaderSource( vertShader, vertname ) ) exit( 1 );
  if( readShaderSource( fragShader, fragname ) ) exit( 1 );

  /* バーテックスシェーダのソースプログラムのコンパイル */
  glCompileShader( vertShader );
  glGetShaderiv( vertShader, GL_COMPILE_STATUS, &compiled );
  printShaderInfoLog( vertShader );
  if( compiled == GL_FALSE ) {
    fprintf( stderr, "Compile error in vertex shader.\n" );
    exit( 1 );
  }

  /* フラグメントシェーダのソースプログラムのコンパイル */
  glCompileShader( fragShader );
  glGetShaderiv( fragShader, GL_COMPILE_STATUS, &compiled );
  printShaderInfoLog( fragShader );
  if( compiled == GL_FALSE ) {
    fprintf( stderr, "Compile error in fragment shader.\n" );
    exit( 1 );
  }

  /* プログラムオブジェクトの作成 */
  gl2Program = glCreateProgram();

  /* シェーダオブジェクトのシェーダプログラムへの登録 */
  glAttachShader( gl2Program, vertShader );
  glAttachShader( gl2Program, fragShader );

  /* シェーダオブジェクトの削除 */
  glDeleteShader( vertShader );
  glDeleteShader( fragShader );

  /* シェーダプログラムのリンク */
  glLinkProgram( gl2Program );
  glGetProgramiv( gl2Program, GL_LINK_STATUS, &linked );
  printProgramInfoLog( gl2Program );
  if( linked == GL_FALSE ) {
    fprintf( stderr, "Link error.\n" );
    exit( 1 );
  }

  /* シェーダプログラムの有効化 */
  glUseProgram( gl2Program );

  /* フラグメントシェーダに値を渡す */
  GLint smptexLoc = glGetUniformLocation( gl2Program, "smptex" );   // テクスチャサンプラ
  glUniform1i( smptexLoc, 0 );                                      // テクスチャユニット GL_TEXTURE0 を利用
  GLint imgxLoc = glGetUniformLocation( gl2Program, "imgx" );       // 画像のx方向のピクセル数
  glUniform1i( imgxLoc, imgx );
  GLint imgyLoc = glGetUniformLocation( gl2Program, "imgy" );       // 画像のy方向のピクセル数
  glUniform1i( imgyLoc, imgy );
 

}

/*
** GLSLの終了設定
*/
static void fin_glsl( void )
{
  /* シェーダプログラムの無効化 */
  glUseProgram( 0 );

  /* プログラムオブジェクトの削除 */
  glDeleteProgram( gl2Program );
}

/*
** テクスチャの終了設定
*/
static void fin_tex( void )
{
  /* テクスチャマッピングの無効化 */
  glDisable( GL_TEXTURE_2D );
}

/* ****
   コールバック関数
   **** */

/*
** ウィンドウのリサイズ
*/
static void resize( int w, int h )
{
  /* ビューポート変換 */
  glViewport( 0, 0, w, h );

  /* 変換行列の設定モード　→　投影変換 */
  glMatrixMode( GL_PROJECTION );

  /* 投影変換行列の初期化（単位行列にする） */
  glLoadIdentity();

  /* 投影変換（平行投影）の設定 */
  glOrtho( 0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0 );

  /* 変換行列の設定モード　→　モデルビュー変換 */
  glMatrixMode( GL_MODELVIEW );
}

/*
** ウィンドウの描画
*/
static void display( void )
{
  /* 画面のクリア */
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  glUseProgram(gl2Program);
  GLint TurnLoc = glGetUniformLocation(gl2Program, "TurnRate");
  glUniform1f(TurnLoc, TurnRate);
  

  /* モデルビュー変換行列の初期化（単位行列にする） */
  glLoadIdentity();

  /* 視野変換（ビューイング変換）の設定 */
  gluLookAt( 0.0, 0.0, 1.0,    /* 視点位置 */
             0.0, 0.0, 0.0,    /* 注目位置 */
             0.0, 1.0, 0.0 );  /* 上向きベクトル */

  /* ４角形ポリゴンの描画 */
  /* ４個の頂点にテクスチャ座標を割り当てる */
  glBegin( GL_QUADS );
  glTexCoord2d( 0.0, 0.0 );
  glVertex3d( 0.0, 0.0, 0.0 );
  glTexCoord2d( 1.0, 0.0 );
  glVertex3d( (GLdouble)imgx, 0.0, 0.0 );
  glTexCoord2d( 1.0, 1.0 );
  glVertex3d( (GLdouble)imgx, (GLdouble)imgy, 0.0 );
  glTexCoord2d( 0.0, 1.0 );
  glVertex3d( 0.0, (GLdouble)imgy, 0.0 );
  glEnd();
  
  /* ダブルバッファリング */
  glutSwapBuffers();
}

/*
** キーボードからの入力
*/
static void keyboard( unsigned char key, int x, int y )
{
  switch( key ) {
  case 'A':
  case 'a':
	  if (TurnRate < 1.0) {
		  TurnRate = TurnRate + 0.01;
		  printf(" + 0.01 TurnRate = %f\n", TurnRate);
	  }

	  break;
  case'D':
  case 'd':
	  if (TurnRate > -1.0) {
		  TurnRate = TurnRate - 0.01;
		  printf(" - 0.01  TurnRate = %f\n", TurnRate);
	  }

	  break;
  
  case 'q':
  case 'Q':
  case '\033':
    /* ESC か q か Q をタイプしたら終了 */
    exit( 0 );
  default:
    break;
  }
}

/* ****
   画像の読み込み
   **** */

/* 
** PPMファイルの読み込み
**   返り値
**     0 : 正常終了
**     1 : 異常終了
*/
static int getPPM(
    char            nam[],      /* (I) ファイル名 XXX.ppm */
    int             *xsizp,     /* (O) 画像のx方向の解像度 x = 0, ..., xsiz-1 */
    int             *ysizp,     /* (O) 画像のy方向の解像度 y = 0, ..., ysiz-1 */
    unsigned char   rgb[][3]    /* (O) 色 rgb[xsiz*y+x][c], c = 0, 1, 2 : R, G, B */
)
{
    int             n, ab, pixmax, i, j, i_x, i_y, a;
    char            str[256];
    char            com[256];       // コメントを読み飛ばすための
    int             commax = 256;   // 文字配列 com のサイズを 256 とする
    unsigned char   *rgb_tmp;
    FILE            *fp;

    /* ========
       入力ファイルのオープン */

    // fprintf( stderr, "getPPM() gets %s ... ", nam );

    fp = fopen( nam, "rb" );
    if( fp == NULL ) {
        fprintf( stderr, "error: getPPM() -> fopen() -> %s\n", nam );
        return( 1 );	
    }

    /* ========
       画像の入力 */

    /* --------
       ヘッダーの入力 */
    
    n = 0;
    while( n < 4 ) {
        fscanf( fp, " %s", str );
        /* ==== コメント */
        if( str[0] == '#' ) {
            fgets( com, commax, fp );           /* 改行まで読み飛ばす */
            if( strlen( com ) >= commax-1 ) {   /* 改行まで読み飛ばせなかった */
                fprintf( stderr, "error: getPPM() -> # ... is longer than commax=%d.\n", commax );
                return( 1 );
            }
        }
        /* ==== コメントでない */
        else {
            /* ---- マジックナンバー */
            if( n == 0 ) {
                if( strcmp( str, "P3" ) == 0 ) {
                    ab = 0;     /* 色情報はアスキー（テキスト）コード ascii */
                }
                else if( strcmp( str, "P6" ) == 0 ) {
                    ab = 1;     /* 色情報はバイナリーコード raw */
                }
                else {
                    fprintf( stderr, "error: getPPM() -> %s is not PPM.\n", nam );
                    return( 1 );
                }
            }
            /* ---- 画像解像度 */
            else if( n == 1 ) {
                *xsizp = atoi( str );
            }
            else if( n == 2 ) {
                *ysizp = atoi( str );
            }
            /* ---- 画素の最大値 */
            else {
                pixmax = atoi( str );
                if( pixmax > 255 ) {
                    fprintf( stderr, "error: getPPM() -> pixmax > 255\n" );
                    return( 1 );
                }
            }
            n++;
        }
    }

    /* --------
       データの入力 */

    /* ==== 色情報はアスキー（テキスト）コード ascii */
    if( ab == 0 ) {
        for( i_y = (*ysizp)-1; i_y >= 0; i_y-- ) {
            for( i_x = 0; i_x < (*xsizp); i_x++ ) {
                for( i = 0; i < 3; i++ ) {
                    fscanf( fp, " %d", &a );
                    rgb[(*xsizp)*i_y+i_x][i] = (unsigned char)a;
                }
            }
        }
    }
    /* ==== 色情報はバイナリーコード raw */
    else {
        fgets( com, commax, fp );   /* 改行まで読み飛ばす．*/
        /* http://www.linux.or.jp/JM/html/LDP_man-pages/man3/gets.3.html
           char *fgets( char *s, int size, FILE *stream );
           fgets() は stream から最大で size - 1 個の文字を読み込み、
           s が指すバッファに格納する。
           読み込みは EOF または改行文字を読み込んだ後で停止する。
           読み込まれた改行文字はバッファに格納される。
           '\0' 文字が一つバッファの中の最後の文字の後に書き込まれる。*/
        /* ** メモリ領域の確保 */
        if( ( rgb_tmp = ( unsigned char * ) malloc( sizeof( unsigned char ) * (*xsizp) * 3 ) ) == NULL ) {
            fprintf( stderr, "error: getPPM() -> malloc() -> rgb_tmp\n" );
            return( 1 );
        }
        /* ** 色の読み込み */
        for( i_y = (*ysizp)-1; i_y >= 0; i_y-- ) {
            if( fread( rgb_tmp, sizeof(unsigned char), (size_t)((*xsizp)*3), fp ) != (size_t)((*xsizp)*3) ) {
                fprintf( stderr, "error: getPPM() -> fread() -> i_y=%d\n", i_y );
                return( 1 );
            }
            j = 0;
            for( i_x = 0; i_x < (*xsizp); i_x++ ) {
                for( i = 0; i < 3; i++ ) {
                    rgb[(*xsizp)*i_y+i_x][i] = rgb_tmp[j];
                    j++;
                }
            }
        }
        /* ** メモリ領域の解放 */
        free( rgb_tmp );
    }

    /* ========
       入力ファイルのクローズ */

    fclose( fp );

    /* ========
       終了処理 */

    // fprintf( stderr, "done.\n" );

    return( 0 );

}

void idle(void)
{
	glutPostRedisplay();
}

/* ******** EOF */
