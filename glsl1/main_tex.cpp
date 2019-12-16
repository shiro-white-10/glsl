/* ********
   �e�N�X�`���}�b�s���O�̃v���O����
   ********
   ���L�y�[�W�̃v���O�������Q�l�ɂ��č쐬
   ���䌤�����F��P�� �V�F�[�_�v���O�����̓ǂݍ���
   http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20051006
   ���䌤�����F��R�� �e�N�X�`���̎Q��
   http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20051008
   ******** */

/*
** �w�b�_�t�@�C��
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "glut.h"
#include "glext.h"

/*
** �V�F�[�_
*/
#include "glsl.h"
static GLuint vertShader;
static GLuint fragShader;
static GLuint gl2Program;
static char vertname[] = "tex.vert";
static char fragname[] = "tex.frag";
float TurnRate = 0.0;

/*
** �摜
*/
#define IMGMAXX 1600                    // �摜��x�����̍ő�s�N�Z����
#define IMGMAXY 1600                    // �摜��y�����̍ő�s�N�Z����
static GLubyte img[IMGMAXX*IMGMAXY][3]; // �摜��RGB�l
static char imgname[] = "a.ppm";   // �摜��PPM�t�@�C��
static int imgx;                        // �摜��x�����̃s�N�Z����
static int imgy;                        // �摜��y�����̃s�N�Z����

/*
** �֐��̐錾
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
   ���C���v���O����
   **** */

/*
** ���C���֐�
*/
int main( int argc, char *argv[] )
{
  /* �摜�̓ǂݍ��� */
  if( getPPM( imgname, &imgx, &imgy, img ) != 0 ) {
    fprintf( stderr, "error: main() -> getPPM() -> %s\n", imgname );
    return( 1 );
  }

  /* OpenGL�̏����� */
  glutInitWindowSize( imgx, imgy );
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
  glutCreateWindow( argv[0] );

  /* �R�[���o�b�N�֐��̓o�^ */
  glutReshapeFunc( resize );
  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutIdleFunc(idle);

  /* �S�ʓI�ȏ����ݒ� */
  init();
  /* �e�N�X�`���̏����ݒ� */
  init_tex();
  /* GLSL�̏����ݒ� */
  init_glsl();

  /* �C�x���g���[�v */
  glutMainLoop();

  /* GLSL�̏I���ݒ� */
  fin_glsl();
  /* �e�N�X�`���̏I���ݒ� */
  fin_tex();

  /* �I�� */
  return( 0 );
}

/* ****
   �����ݒ�ƏI���ݒ�
   **** */

/*
** �S�ʓI�ȏ����ݒ�
*/
static void init( void )
{
  /* �w�i�F */
  glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

/*
** �e�N�X�`���̏����ݒ�
*/
static void init_tex( void )
{
  /* �e�N�X�`���摜�̓o�C�g�P�ʂɋl�ߍ��܂�Ă��� */
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  
  /* �e�N�X�`�����g��E�k��������@�̎w�� */
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  
  /* �e�N�X�`���̌J��Ԃ����@�̎w�� */
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  
  /* �e�N�X�`���̊��蓖�� */
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imgx, imgy, 0, GL_RGB, GL_UNSIGNED_BYTE, img );

  /* �e�N�X�`���}�b�s���O�̗L���� */
  glEnable( GL_TEXTURE_2D );
}

/*
** GLSL�̏����ݒ�
*/
static void init_glsl( void )
{
  /* �V�F�[�_�v���O�����̃R���p�C���^�����N���ʂ𓾂�ϐ� */
  GLint compiled, linked;

  /* GLSL�̏����� */
  if( glslInit() ) exit( 1 );

  /* �V�F�[�_�I�u�W�F�N�g�̍쐬 */
  vertShader = glCreateShader( GL_VERTEX_SHADER );
  fragShader = glCreateShader( GL_FRAGMENT_SHADER );

  /* �V�F�[�_�̃\�[�X�v���O�����̓ǂݍ��� */
  if( readShaderSource( vertShader, vertname ) ) exit( 1 );
  if( readShaderSource( fragShader, fragname ) ) exit( 1 );

  /* �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̃R���p�C�� */
  glCompileShader( vertShader );
  glGetShaderiv( vertShader, GL_COMPILE_STATUS, &compiled );
  printShaderInfoLog( vertShader );
  if( compiled == GL_FALSE ) {
    fprintf( stderr, "Compile error in vertex shader.\n" );
    exit( 1 );
  }

  /* �t���O�����g�V�F�[�_�̃\�[�X�v���O�����̃R���p�C�� */
  glCompileShader( fragShader );
  glGetShaderiv( fragShader, GL_COMPILE_STATUS, &compiled );
  printShaderInfoLog( fragShader );
  if( compiled == GL_FALSE ) {
    fprintf( stderr, "Compile error in fragment shader.\n" );
    exit( 1 );
  }

  /* �v���O�����I�u�W�F�N�g�̍쐬 */
  gl2Program = glCreateProgram();

  /* �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^ */
  glAttachShader( gl2Program, vertShader );
  glAttachShader( gl2Program, fragShader );

  /* �V�F�[�_�I�u�W�F�N�g�̍폜 */
  glDeleteShader( vertShader );
  glDeleteShader( fragShader );

  /* �V�F�[�_�v���O�����̃����N */
  glLinkProgram( gl2Program );
  glGetProgramiv( gl2Program, GL_LINK_STATUS, &linked );
  printProgramInfoLog( gl2Program );
  if( linked == GL_FALSE ) {
    fprintf( stderr, "Link error.\n" );
    exit( 1 );
  }

  /* �V�F�[�_�v���O�����̗L���� */
  glUseProgram( gl2Program );

  /* �t���O�����g�V�F�[�_�ɒl��n�� */
  GLint smptexLoc = glGetUniformLocation( gl2Program, "smptex" );   // �e�N�X�`���T���v��
  glUniform1i( smptexLoc, 0 );                                      // �e�N�X�`�����j�b�g GL_TEXTURE0 �𗘗p
  GLint imgxLoc = glGetUniformLocation( gl2Program, "imgx" );       // �摜��x�����̃s�N�Z����
  glUniform1i( imgxLoc, imgx );
  GLint imgyLoc = glGetUniformLocation( gl2Program, "imgy" );       // �摜��y�����̃s�N�Z����
  glUniform1i( imgyLoc, imgy );
 

}

/*
** GLSL�̏I���ݒ�
*/
static void fin_glsl( void )
{
  /* �V�F�[�_�v���O�����̖����� */
  glUseProgram( 0 );

  /* �v���O�����I�u�W�F�N�g�̍폜 */
  glDeleteProgram( gl2Program );
}

/*
** �e�N�X�`���̏I���ݒ�
*/
static void fin_tex( void )
{
  /* �e�N�X�`���}�b�s���O�̖����� */
  glDisable( GL_TEXTURE_2D );
}

/* ****
   �R�[���o�b�N�֐�
   **** */

/*
** �E�B���h�E�̃��T�C�Y
*/
static void resize( int w, int h )
{
  /* �r���[�|�[�g�ϊ� */
  glViewport( 0, 0, w, h );

  /* �ϊ��s��̐ݒ胂�[�h�@���@���e�ϊ� */
  glMatrixMode( GL_PROJECTION );

  /* ���e�ϊ��s��̏������i�P�ʍs��ɂ���j */
  glLoadIdentity();

  /* ���e�ϊ��i���s���e�j�̐ݒ� */
  glOrtho( 0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0 );

  /* �ϊ��s��̐ݒ胂�[�h�@���@���f���r���[�ϊ� */
  glMatrixMode( GL_MODELVIEW );
}

/*
** �E�B���h�E�̕`��
*/
static void display( void )
{
  /* ��ʂ̃N���A */
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  
  glUseProgram(gl2Program);
  GLint TurnLoc = glGetUniformLocation(gl2Program, "TurnRate");
  glUniform1f(TurnLoc, TurnRate);
  

  /* ���f���r���[�ϊ��s��̏������i�P�ʍs��ɂ���j */
  glLoadIdentity();

  /* ����ϊ��i�r���[�C���O�ϊ��j�̐ݒ� */
  gluLookAt( 0.0, 0.0, 1.0,    /* ���_�ʒu */
             0.0, 0.0, 0.0,    /* ���ڈʒu */
             0.0, 1.0, 0.0 );  /* ������x�N�g�� */

  /* �S�p�`�|���S���̕`�� */
  /* �S�̒��_�Ƀe�N�X�`�����W�����蓖�Ă� */
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
  
  /* �_�u���o�b�t�@�����O */
  glutSwapBuffers();
}

/*
** �L�[�{�[�h����̓���
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
    /* ESC �� q �� Q ���^�C�v������I�� */
    exit( 0 );
  default:
    break;
  }
}

/* ****
   �摜�̓ǂݍ���
   **** */

/* 
** PPM�t�@�C���̓ǂݍ���
**   �Ԃ�l
**     0 : ����I��
**     1 : �ُ�I��
*/
static int getPPM(
    char            nam[],      /* (I) �t�@�C���� XXX.ppm */
    int             *xsizp,     /* (O) �摜��x�����̉𑜓x x = 0, ..., xsiz-1 */
    int             *ysizp,     /* (O) �摜��y�����̉𑜓x y = 0, ..., ysiz-1 */
    unsigned char   rgb[][3]    /* (O) �F rgb[xsiz*y+x][c], c = 0, 1, 2 : R, G, B */
)
{
    int             n, ab, pixmax, i, j, i_x, i_y, a;
    char            str[256];
    char            com[256];       // �R�����g��ǂݔ�΂����߂�
    int             commax = 256;   // �����z�� com �̃T�C�Y�� 256 �Ƃ���
    unsigned char   *rgb_tmp;
    FILE            *fp;

    /* ========
       ���̓t�@�C���̃I�[�v�� */

    // fprintf( stderr, "getPPM() gets %s ... ", nam );

    fp = fopen( nam, "rb" );
    if( fp == NULL ) {
        fprintf( stderr, "error: getPPM() -> fopen() -> %s\n", nam );
        return( 1 );	
    }

    /* ========
       �摜�̓��� */

    /* --------
       �w�b�_�[�̓��� */
    
    n = 0;
    while( n < 4 ) {
        fscanf( fp, " %s", str );
        /* ==== �R�����g */
        if( str[0] == '#' ) {
            fgets( com, commax, fp );           /* ���s�܂œǂݔ�΂� */
            if( strlen( com ) >= commax-1 ) {   /* ���s�܂œǂݔ�΂��Ȃ����� */
                fprintf( stderr, "error: getPPM() -> # ... is longer than commax=%d.\n", commax );
                return( 1 );
            }
        }
        /* ==== �R�����g�łȂ� */
        else {
            /* ---- �}�W�b�N�i���o�[ */
            if( n == 0 ) {
                if( strcmp( str, "P3" ) == 0 ) {
                    ab = 0;     /* �F���̓A�X�L�[�i�e�L�X�g�j�R�[�h ascii */
                }
                else if( strcmp( str, "P6" ) == 0 ) {
                    ab = 1;     /* �F���̓o�C�i���[�R�[�h raw */
                }
                else {
                    fprintf( stderr, "error: getPPM() -> %s is not PPM.\n", nam );
                    return( 1 );
                }
            }
            /* ---- �摜�𑜓x */
            else if( n == 1 ) {
                *xsizp = atoi( str );
            }
            else if( n == 2 ) {
                *ysizp = atoi( str );
            }
            /* ---- ��f�̍ő�l */
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
       �f�[�^�̓��� */

    /* ==== �F���̓A�X�L�[�i�e�L�X�g�j�R�[�h ascii */
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
    /* ==== �F���̓o�C�i���[�R�[�h raw */
    else {
        fgets( com, commax, fp );   /* ���s�܂œǂݔ�΂��D*/
        /* http://www.linux.or.jp/JM/html/LDP_man-pages/man3/gets.3.html
           char *fgets( char *s, int size, FILE *stream );
           fgets() �� stream ����ő�� size - 1 �̕�����ǂݍ��݁A
           s ���w���o�b�t�@�Ɋi�[����B
           �ǂݍ��݂� EOF �܂��͉��s������ǂݍ��񂾌�Œ�~����B
           �ǂݍ��܂ꂽ���s�����̓o�b�t�@�Ɋi�[�����B
           '\0' ��������o�b�t�@�̒��̍Ō�̕����̌�ɏ������܂��B*/
        /* ** �������̈�̊m�� */
        if( ( rgb_tmp = ( unsigned char * ) malloc( sizeof( unsigned char ) * (*xsizp) * 3 ) ) == NULL ) {
            fprintf( stderr, "error: getPPM() -> malloc() -> rgb_tmp\n" );
            return( 1 );
        }
        /* ** �F�̓ǂݍ��� */
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
        /* ** �������̈�̉�� */
        free( rgb_tmp );
    }

    /* ========
       ���̓t�@�C���̃N���[�Y */

    fclose( fp );

    /* ========
       �I������ */

    // fprintf( stderr, "done.\n" );

    return( 0 );

}

void idle(void)
{
	glutPostRedisplay();
}

/* ******** EOF */
