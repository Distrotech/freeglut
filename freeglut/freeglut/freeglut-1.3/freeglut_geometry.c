/*
 * freeglut_geometry.c
 *
 * Freeglut geometry rendering methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define  G_LOG_DOMAIN  "freeglut-geometry"

#include "../include/GL/freeglut.h"
#include "../include/GL/freeglut_internal.h"

/*
 * TODO BEFORE THE STABLE RELEASE:
 *
 * Following functions have been contributed by Andreas Umbach. I modified
 * them a bit to make them use GLib (for memory allocation).
 *
 *      glutWireCube()          -- could add normal vectors so that lighting works
 *      glutSolidCube()         -- OK
 *      glutWireSphere()        -- OK
 *      glutSolidSphere()       -- OK
 *
 * Following functions have been implemented by me:
 *
 *      glutWireCone()          -- looks OK
 *      glutSolidCone()         -- normals are missing, there are holes in the thing
 *
 * Those functions need to be implemented, as nothing has been done yet.
 * For now all of them draw a wire or solid cube, just to mark their presence.
 *
 *      glutWireTorus()         --
 *      glutSolidTorus()        --
 *      glutWireDodecahedron()  --
 *      glutSolidDodecahedron() --
 *      glutWireOctahedron()    --
 *      glutSolidOctahedron()   --
 *      glutWireTetrahedron()   --
 *      glutSolidTetrahedron()  --
 *      glutWireIcosahedron()   --
 *      glutSolidIcosahedron()  --
 */


/* -- INTERFACE FUNCTIONS -------------------------------------------------- */

/*
 * Draws a wireframed cube. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutWireCube( GLdouble dSize )
{
    float size = (float) dSize * 0.5f;

#   define V(a,b,c) glVertex3f( a size, b size, c size );

    /*
     * PWO: I dared to convert the code to use macros...
     */
    glBegin( GL_LINE_LOOP ); V(-,-,+); V(+,-,+); V(+,+,+); V(-,+,+); glEnd();
    glBegin( GL_LINE_LOOP ); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-); glEnd();
    glBegin( GL_LINES );
        V(-,-,+); V(-,-,-); V(-,+,+); V(-,+,-);
        V(+,-,+); V(+,-,-); V(+,+,+); V(+,+,-);
    glEnd();

#   undef V
}

/*
 * Draws a solid cube. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutSolidCube( GLdouble dSize )
{
    float size = (float) dSize * 0.5f;

#   define V(a,b,c) glVertex3f( a size, b size, c size );
#   define N(a,b,c) glNormal3f( a, b, c );

    /*
     * PWO: Again, I dared to convert the code to use macros...
     */
    glBegin( GL_QUADS );
        N( 1, 0, 0); V(+,-,+); V(+,-,-); V(+,+,-); V(+,+,+);
        N( 0, 1, 0); V(+,+,+); V(+,+,-); V(-,+,-); V(-,+,+);
        N( 0, 0, 1); V(+,+,+); V(-,+,+); V(-,-,+); V(+,-,+);
        N(-1, 0, 0); V(-,-,+); V(-,+,+); V(-,+,-); V(-,-,-);
        N( 0,-1, 0); V(-,-,+); V(-,-,-); V(+,-,-); V(+,-,+);
        N( 0, 0,-1); V(-,-,-); V(-,+,-); V(+,+,-); V(+,-,-);
    glEnd();

#   undef V
#   undef N
}

/*
 * Draws a wire sphere. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutWireSphere( GLdouble dRadius, GLint slices, GLint stacks )
{
    float  radius = (float) dRadius, phi, psi, dpsi, dphi;
    float* vertex;
    int    i, j;

    /*
     * Allocate the vertices array
     */
    vertex = calloc( sizeof(float), 3 * slices * (stacks - 1) );

    glPushMatrix();
    glScalef( radius, radius, radius );

    dpsi = M_PI / (stacks + 1);
    dphi = 2 * M_PI / slices;
    psi  = dpsi;

    for( j=0; j<stacks-1; j++ )
    {
        phi = 0;

        for( i=0; i<slices; i++ )
        {
            *(vertex + 3 * j * slices + 3 * i + 0) = sin( phi ) * sin( psi );
            *(vertex + 3 * j * slices + 3 * i + 1) = cos( phi ) * sin( psi );
            *(vertex + 3 * j * slices + 3 * i + 2) = cos( psi );
            phi += dphi;
            glVertex3fv( vertex + 3 * j * slices + 3 * i );
        }

        psi += dpsi;
    }

    for( i=0; i<slices; i++ )
    {
        glBegin( GL_LINE_STRIP );
        glNormal3f( 0, 0, 1 );
        glVertex3f( 0, 0, 1 );

        for( j=0; j<stacks - 1; j++ )
        {
            glNormal3fv( vertex + 3 * j * slices + 3 * i );
            glVertex3fv( vertex + 3 * j * slices + 3 * i );
        }

        glNormal3f(0, 0, -1);
        glVertex3f(0, 0, -1);
        glEnd();
    }

    for( j=0; j<stacks-1; j++ )
    {
        glBegin(GL_LINE_LOOP);

        for( i=0; i<slices; i++ )
        {
            glNormal3fv( vertex + 3 * j * slices + 3 * i );
            glVertex3fv( vertex + 3 * j * slices + 3 * i );
        }

        glEnd();
    }

    free( vertex );
    glPopMatrix();
}

/*
 * Draws a solid sphere. Code contributed by Andreas Umbach <marvin@dataway.ch>
 */
void FGAPIENTRY glutSolidSphere( GLdouble dRadius, GLint slices, GLint stacks )
{
    float  radius = (float) dRadius, phi, psi, dpsi, dphi;
    float *next, *tmp, *row;
    int    i, j;

    glPushMatrix();
    //glScalef( radius, radius, radius );

    row  = calloc( sizeof(float), slices * 3 );
    next = calloc( sizeof(float), slices * 3 );

    dpsi = M_PI / (stacks + 1);
    dphi = 2 * M_PI / slices;
    psi  = dpsi;
    phi  = 0;

    /* init first line + do polar cap */
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0, 0, 1 );
    glVertex3f( 0, 0, radius );

    for( i=0; i<slices; i++ )
    {
        row[ i * 3 + 0 ] = sin( phi ) * sin( psi );
        row[ i * 3 + 1 ] = cos( phi ) * sin( psi );
        row[ i * 3 + 2 ] = cos( psi );

        glNormal3fv( row + 3 * i );
        glVertex3f(
            radius * *(row + 3 * i + 0),
            radius * *(row + 3 * i + 1),
	        radius * *(row + 3 * i + 2)
	    );
	
        phi += dphi;
    }

    glNormal3fv( row );
    glVertex3f( radius * *(row + 0), radius * *(row + 1), radius * *(row + 2) );
    glEnd();

    for( j=0; j<stacks-1; j++ )
    {
        phi = 0;
        psi += dpsi;

        /* get coords */
        glBegin( GL_QUAD_STRIP );

        /* glBegin(GL_LINE_LOOP); */
        for( i=0; i<slices; i++ )
        {
            next[ i * 3 + 0 ] = sin( phi ) * sin( psi );
            next[ i * 3 + 1 ] = cos( phi ) * sin( psi );
            next[ i * 3 + 2 ] = cos( psi );

            glNormal3fv( row + i * 3 );
            glVertex3f(
                radius * *(row + 3 * i + 0),
                radius * *(row + 3 * i + 1),
		        radius * *(row + 3 * i + 2)
		    );

            glNormal3fv( next + i * 3 );
            glVertex3f(
                radius * *(next + 3 * i + 0),
                radius * *(next + 3 * i + 1),
                radius * *(next + 3 * i + 2)
            );

            phi += dphi;
        }

        glNormal3fv( row );
        glVertex3f( radius * *(row + 0), radius * *(row + 1), radius * *(row + 2) );
        glNormal3fv( next );
        glVertex3f( radius * *(next + 0), radius * *(next + 1), radius * *(next + 2) );
        glEnd();

        tmp = row;
        row = next;
        next = tmp;
    }

    /* south pole */
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0, 0, -1 );
    glVertex3f( 0, 0, -radius );
    glNormal3fv( row );
    glVertex3f( radius * *(row + 0), radius * *(row + 1), radius * *(row + 2) );

    for( i=slices-1; i>=0; i-- )
    {
        glNormal3fv(row + 3 * i);
        glVertex3f(
            radius * *(row + 3 * i + 0),
            radius * *(row + 3 * i + 1),
	        radius * *(row + 3 * i + 2)
	   );
    }

    glEnd();

    free(row);
    free(next);
    glPopMatrix();
}

/*
 * Draws a wire cone
 */
void FGAPIENTRY glutWireCone( GLdouble base, GLdouble height, GLint slices, GLint stacks )
{
    float  alt   = (float) height / (float) (stacks + 1);
    float  angle = (float) M_PI / (float) slices * 2.0f;
    float  slope = (float) tan( height / base );
    float* vertices = NULL;
    int    i, j;

    /*
     * We need 'slices' points on a circle
     */
    vertices = calloc( sizeof(float), 2 * (slices + 1) );

    for( i=0; i<slices+1; i++ )
    {
        vertices[ i*2 + 0 ] = cos( angle * i );
        vertices[ i*2 + 1 ] = sin( angle * i );
    }

    /*
     * First the cone's bottom...
     */
    for( i=0; i<slices; i++ )
    {
        float scl = height / slope;

        glBegin( GL_LINE_LOOP );
            glNormal3f( 0, 0, -1 );
            glVertex3f( vertices[ (i+0)*2+0 ] * scl, vertices[ (i+0)*2+1 ] * scl, 0 );
            glVertex3f( vertices[ (i+1)*2+0 ] * scl, vertices[ (i+1)*2+1 ] * scl, 0 );
            glVertex3f( 0, 0, 0 );
        glEnd();
    }

    /*
     * Then all the stacks between the bottom and the top
     *
     * ekhm jak wektor normalny z trojkata?
     */
    for( i=0; i<stacks; i++ )
    {
        float alt_a = i * alt, alt_b = (i + 1) * alt;
        float scl_a = (height - alt_a) / slope;
        float scl_b = (height - alt_b) / slope;

        for( j=0; j<slices; j++ )
        {
            glBegin( GL_LINE_LOOP );
                glVertex3f( vertices[(j+0)*2+0] * scl_a, vertices[(j+0)*2+1] * scl_a, alt_a );
                glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
                glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
            glEnd();

            glBegin( GL_LINE_LOOP );
                glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
                glVertex3f( vertices[(j+1)*2+0] * scl_b, vertices[(j+1)*2+1] * scl_b, alt_b );
                glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
            glEnd();
        }
    }

    /*
     * Finally have the top part drawn...
     */
    for( i=0; i<slices; i++ )
    {
        float scl = alt / slope;

        glBegin( GL_LINE_LOOP );
            glVertex3f( vertices[ (i+0)*2+0 ] * scl, vertices[ (i+0)*2+1 ] * scl, height - alt );
            glVertex3f( vertices[ (i+1)*2+0 ] * scl, vertices[ (i+1)*2+1 ] * scl, height - alt );
            glVertex3f( 0, 0, height );
        glEnd();
    }
}

/*
 * Draws a solid cone
 */
void FGAPIENTRY glutSolidCone( GLdouble base, GLdouble height, GLint slices, GLint stacks )
{
    float  alt   = (float) height / (float) (stacks + 1);
    float  angle = (float) M_PI / (float) slices * 2.0f;
    float  slope = (float) tan( height / base );
    float* vertices = NULL;
    int    i, j;

    /*
     * We need 'slices' points on a circle
     */
    vertices = calloc( sizeof(float), 2 * (slices + 1) );

    for( i=0; i<slices+1; i++ )
    {
        vertices[ i*2 + 0 ] = cos( angle * i );
        vertices[ i*2 + 1 ] = sin( angle * i );
    }

    /*
     * First the cone's bottom...
     */
    for( i=0; i<slices; i++ )
    {
        float scl = height / slope;

        glBegin( GL_TRIANGLES );
            glNormal3f( 0, 0, -1 );
            glVertex3f( vertices[ (i+0)*2+0 ] * scl, vertices[ (i+0)*2+1 ] * scl, 0 );
            glVertex3f( vertices[ (i+1)*2+0 ] * scl, vertices[ (i+1)*2+1 ] * scl, 0 );
            glVertex3f( 0, 0, 0 );
        glEnd();
    }

    /*
     * Then all the stacks between the bottom and the top
     *
     * ekhm jak wektor normalny z trojkata?
     */
    for( i=0; i<stacks; i++ )
    {
        float alt_a = i * alt, alt_b = (i + 1) * alt;
        float scl_a = (height - alt_a) / slope;
        float scl_b = (height - alt_b) / slope;

        for( j=0; j<slices; j++ )
        {
            glBegin( GL_TRIANGLES );
                glVertex3f( vertices[(j+0)*2+0] * scl_a, vertices[(j+0)*2+1] * scl_a, alt_a );
                glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
                glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
            glEnd();

            glBegin( GL_TRIANGLES );
                glVertex3f( vertices[(j+1)*2+0] * scl_a, vertices[(j+1)*2+1] * scl_a, alt_a );
                glVertex3f( vertices[(j+1)*2+0] * scl_b, vertices[(j+1)*2+1] * scl_b, alt_b );
                glVertex3f( vertices[(j+0)*2+0] * scl_b, vertices[(j+0)*2+1] * scl_b, alt_b );
            glEnd();
        }
    }

    /*
     * Finally have the top part drawn...
     */
    for( i=0; i<slices; i++ )
    {
        float scl = alt / slope;

        glBegin( GL_TRIANGLES );
            glVertex3f( vertices[ (i+0)*2+0 ] * scl, vertices[ (i+0)*2+1 ] * scl, height - alt );
            glVertex3f( vertices[ (i+1)*2+0 ] * scl, vertices[ (i+1)*2+1 ] * scl, height - alt );
            glVertex3f( 0, 0, height );
        glEnd();
    }
}

/*
 *
 */
void FGAPIENTRY glutWireTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings )
{
    glutWireSphere( dOuterRadius, 5, 5 );
}

/*
 *
 */
void FGAPIENTRY glutSolidTorus( GLdouble dInnerRadius, GLdouble dOuterRadius, GLint nSides, GLint nRings )
{
    glutSolidSphere( dOuterRadius, 5, 5 );
}

/*
 *
 */
void FGAPIENTRY glutWireDodecahedron( void )
{
  /* Magic Numbers:  It is possible to create a dodecahedron by attaching two pentagons to each face of
   * of a cube.  The coordinates of the points are:
   *   (+-x,0, z); (+-1, 1, 1); (0, z, x )
   * where x = 0.618033989 and z = 1.618033939.
   */
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f,  0.525731f,  0.850651f ) ; glVertex3f (  0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f,  0.525731f, -0.850651f ) ; glVertex3f (  0.000000f,  1.618034f, -0.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f, -0.525731f,  0.850651f ) ; glVertex3f (  0.000000f, -1.618034f,  0.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.000000f, -0.525731f, -0.850651f ) ; glVertex3f (  0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.850651f,  0.000000f,  0.525731f ) ; glVertex3f (  0.618034f,  0.000000f,  1.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.850651f,  0.000000f,  0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f,  1.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.850651f,  0.000000f, -0.525731f ) ; glVertex3f (  0.618034f,  0.000000f, -1.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.850651f,  0.000000f, -0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f, -1.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.525731f,  0.850651f,  0.000000f ) ; glVertex3f (  1.618034f,  0.618034f,  0.000000f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f (  0.525731f, -0.850651f,  0.000000f ) ; glVertex3f (  1.618034f, -0.618034f,  0.000000f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.525731f,  0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f,  0.000000f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_LINE_LOOP ) ;
  glNormal3f ( -0.525731f, -0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f,  0.000000f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
}

/*
 *
 */
void FGAPIENTRY glutSolidDodecahedron( void )
{
  /* Magic Numbers:  It is possible to create a dodecahedron by attaching two pentagons to each face of
   * of a cube.  The coordinates of the points are:
   *   (+-x,0, z); (+-1, 1, 1); (0, z, x )
   * where x = 0.618033989 and z = 1.618033939.
   */
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f,  0.525731f,  0.850651f ) ; glVertex3f (  0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f,  0.525731f, -0.850651f ) ; glVertex3f (  0.000000f,  1.618034f, -0.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f, -0.525731f,  0.850651f ) ; glVertex3f (  0.000000f, -1.618034f,  0.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -0.618034f, 0.000000f,  1.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.000000f, -0.525731f, -0.850651f ) ; glVertex3f (  0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  0.618034f, 0.000000f, -1.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.850651f,  0.000000f,  0.525731f ) ; glVertex3f (  0.618034f,  0.000000f,  1.618034f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.850651f,  0.000000f,  0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f,  1.618034f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.850651f,  0.000000f, -0.525731f ) ; glVertex3f (  0.618034f,  0.000000f, -1.618034f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f (  1.618034f,  0.618034f, 0.000000f ) ; glVertex3f (  1.618034f, -0.618034f, 0.000000f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.850651f,  0.000000f, -0.525731f ) ; glVertex3f ( -0.618034f,  0.000000f, -1.618034f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( -1.618034f, -0.618034f, 0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f, 0.000000f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;

  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.525731f,  0.850651f,  0.000000f ) ; glVertex3f (  1.618034f,  0.618034f,  0.000000f ) ; glVertex3f (  1.0f,  1.0f, -1.0f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f (  1.0f,  1.0f,  1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f (  0.525731f, -0.850651f,  0.000000f ) ; glVertex3f (  1.618034f, -0.618034f,  0.000000f ) ; glVertex3f (  1.0f, -1.0f,  1.0f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f (  1.0f, -1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.525731f,  0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f,  0.618034f,  0.000000f ) ; glVertex3f ( -1.0f,  1.0f,  1.0f ) ; glVertex3f ( 0.000000f,  1.618034f,  0.618034f ) ; glVertex3f ( 0.000000f,  1.618034f, -0.618034f ) ; glVertex3f ( -1.0f,  1.0f, -1.0f ) ;
  glEnd () ;
  glBegin ( GL_POLYGON ) ;
  glNormal3f ( -0.525731f, -0.850651f,  0.000000f ) ; glVertex3f ( -1.618034f, -0.618034f,  0.000000f ) ; glVertex3f ( -1.0f, -1.0f, -1.0f ) ; glVertex3f ( 0.000000f, -1.618034f, -0.618034f ) ; glVertex3f ( 0.000000f, -1.618034f,  0.618034f ) ; glVertex3f ( -1.0f, -1.0f,  1.0f ) ;
  glEnd () ;
}

/*
 *
 */
void FGAPIENTRY glutWireOctahedron( void )
{
#define RADIUS    1.0f
  glBegin( GL_LINE_LOOP );
    glNormal3f( 0.577350f, 0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f, 0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f( 0.577350f,-0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f,-0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f, 0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f, 0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f,-0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f,-0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
  glEnd();
#undef RADIUS
}

/*
 *
 */
void FGAPIENTRY glutSolidOctahedron( void )
{
#define RADIUS    1.0f
  glBegin( GL_TRIANGLES );
    glNormal3f( 0.577350f, 0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f, 0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f( 0.577350f,-0.577350f, 0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f( 0.577350f,-0.577350f,-0.577350f); glVertex3f( RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f, 0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f, 0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f, RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
    glNormal3f(-0.577350f,-0.577350f, 0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f, RADIUS );
    glNormal3f(-0.577350f,-0.577350f,-0.577350f); glVertex3f(-RADIUS, 0.0f, 0.0f ); glVertex3f( 0.0f,-RADIUS, 0.0f ); glVertex3f( 0.0f, 0.0f,-RADIUS );
  glEnd();
#undef RADIUS
}

/*
 *
 */
void FGAPIENTRY glutWireTetrahedron( void )
{
  /* Magic Numbers:  r0 = ( 1, 0, 0 )
   *                 r1 = ( -1/3, 2 sqrt(2) / 3, 0 )
   *                 r2 = ( -1/3, -sqrt(2) / 3, sqrt(6) / 3 )
   *                 r3 = ( -1/3, -sqrt(2) / 3, -sqrt(6) / 3 )
   * |r0| = |r1| = |r2| = |r3| = 1
   * Distance between any two points is 2 sqrt(6) / 3
   *
   * Normals:  The unit normals are simply the negative of the coordinates of the point not on the surface.
   */

  float r0[3] = {       1.0f,       0.0f,       0.0f } ;
  float r1[3] = { -0.333333f,  0.942809f,       0.0f } ;
  float r2[3] = { -0.333333f, -0.471405f,  0.816497f } ;
  float r3[3] = { -0.333333f, -0.471405f, -0.816497f } ;

  glBegin( GL_LINE_LOOP ) ;
    glNormal3f (     -1.0f,       0.0f,       0.0f ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r2 ) ;
    glNormal3f ( 0.333333f, -0.942809f,       0.0f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r2 ) ; glVertex3fv ( r3 ) ;
    glNormal3f ( 0.333333f,  0.471405f, -0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r1 ) ;
    glNormal3f ( 0.333333f,  0.471405f,  0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r2 ) ;
  glEnd() ;
}

/*
 *
 */
void FGAPIENTRY glutSolidTetrahedron( void )
{
  /* Magic Numbers:  r0 = ( 1, 0, 0 )
   *                 r1 = ( -1/3, 2 sqrt(2) / 3, 0 )
   *                 r2 = ( -1/3, -sqrt(2) / 3, sqrt(6) / 3 )
   *                 r3 = ( -1/3, -sqrt(2) / 3, -sqrt(6) / 3 )
   * |r0| = |r1| = |r2| = |r3| = 1
   * Distance between any two points is 2 sqrt(6) / 3
   *
   * Normals:  The unit normals are simply the negative of the coordinates of the point not on the surface.
   */

  float r0[3] = {       1.0f,       0.0f,       0.0f } ;
  float r1[3] = { -0.333333f,  0.942809f,       0.0f } ;
  float r2[3] = { -0.333333f, -0.471405f,  0.816497f } ;
  float r3[3] = { -0.333333f, -0.471405f, -0.816497f } ;

  glBegin( GL_TRIANGLES ) ;
    glNormal3f (     -1.0f,       0.0f,       0.0f ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r2 ) ;
    glNormal3f ( 0.333333f, -0.942809f,       0.0f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r2 ) ; glVertex3fv ( r3 ) ;
    glNormal3f ( 0.333333f,  0.471405f, -0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r3 ) ; glVertex3fv ( r1 ) ;
    glNormal3f ( 0.333333f,  0.471405f,  0.816497f ) ; glVertex3fv ( r0 ) ; glVertex3fv ( r1 ) ; glVertex3fv ( r2 ) ;
  glEnd() ;
}

/*
 *
 */
  float r[12][3] = { { 1.0f, 0.0f, 0.0f },
  {  0.447214f,  0.894427f, 0.0f }, {  0.447214f,  0.276393f, 0.850651f }, {  0.447214f, -0.723607f, 0.525731f }, {  0.447214f, -0.723607f, -0.525731f }, {  0.447214f,  0.276393f, -0.850651f },
  { -0.447214f, -0.894427f, 0.0f }, { -0.447214f, -0.276393f, 0.850651f }, { -0.447214f,  0.723607f, 0.525731f }, { -0.447214f,  0.723607f, -0.525731f }, { -0.447214f, -0.276393f, -0.850651f },
  { -1.0f, 0.0f, 0.0f } } ;
  int v [20][3] = { { 0, 1, 2 }, { 0, 2, 3 }, { 0, 3, 4 }, { 0, 4, 5 }, { 0, 5, 1 },
  { 1, 8, 2 }, { 2, 7, 3 }, { 3, 6, 4 }, { 4, 10, 5 }, { 5, 9, 1 },
  { 1, 9, 8 }, { 2, 8, 7 }, { 3, 7, 6 }, { 4, 6, 10 }, { 5, 10, 9 },
  { 11, 9, 10 }, { 11, 8, 9 }, { 11, 7, 8 }, { 11, 6, 7 }, { 11, 10, 6 } } ;

void FGAPIENTRY glutWireIcosahedron( void )
{
  int i ;
  for ( i = 0; i < 20; i++ )
  {
    float normal[3] ;
    normal[0] = ( r[v[i][1]][1] - r[v[i][0]][1] ) * ( r[v[i][2]][2] - r[v[i][0]][2] ) - ( r[v[i][1]][2] - r[v[i][0]][2] ) * ( r[v[i][2]][1] - r[v[i][0]][1] ) ;
    normal[1] = ( r[v[i][1]][2] - r[v[i][0]][2] ) * ( r[v[i][2]][0] - r[v[i][0]][0] ) - ( r[v[i][1]][0] - r[v[i][0]][0] ) * ( r[v[i][2]][2] - r[v[i][0]][2] ) ;
    normal[2] = ( r[v[i][1]][0] - r[v[i][0]][0] ) * ( r[v[i][2]][1] - r[v[i][0]][1] ) - ( r[v[i][1]][1] - r[v[i][0]][1] ) * ( r[v[i][2]][0] - r[v[i][0]][0] ) ;
    glBegin ( GL_LINE_LOOP ) ;
      glNormal3fv ( normal ) ;
      glVertex3fv ( r[v[i][0]] ) ;
      glVertex3fv ( r[v[i][1]] ) ;
      glVertex3fv ( r[v[i][2]] ) ;
    glEnd () ;
  }
}

/*
 *
 */
void FGAPIENTRY glutSolidIcosahedron( void )
{
  int i ;

  glBegin ( GL_TRIANGLES ) ;
  for ( i = 0; i < 20; i++ )
  {
    float normal[3] ;
    normal[0] = ( r[v[i][1]][1] - r[v[i][0]][1] ) * ( r[v[i][2]][2] - r[v[i][0]][2] ) - ( r[v[i][1]][2] - r[v[i][0]][2] ) * ( r[v[i][2]][1] - r[v[i][0]][1] ) ;
    normal[1] = ( r[v[i][1]][2] - r[v[i][0]][2] ) * ( r[v[i][2]][0] - r[v[i][0]][0] ) - ( r[v[i][1]][0] - r[v[i][0]][0] ) * ( r[v[i][2]][2] - r[v[i][0]][2] ) ;
    normal[2] = ( r[v[i][1]][0] - r[v[i][0]][0] ) * ( r[v[i][2]][1] - r[v[i][0]][1] ) - ( r[v[i][1]][1] - r[v[i][0]][1] ) * ( r[v[i][2]][0] - r[v[i][0]][0] ) ;
      glNormal3fv ( normal ) ;
      glVertex3fv ( r[v[i][0]] ) ;
      glVertex3fv ( r[v[i][1]] ) ;
      glVertex3fv ( r[v[i][2]] ) ;
  }

  glEnd () ;
}

/*** END OF FILE ***/
