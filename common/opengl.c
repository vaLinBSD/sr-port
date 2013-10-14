#include <stdio.h>
#include <math.h>
#include <GLES2/gl2.h>
#include <GL/glu.h>
#include "opengl.h"

Matrix identity = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};


void check_error(char *t)
{
	int e = glGetError();
	if (e != GL_NO_ERROR) {
		fprintf(stderr, "[%s] %s\n", t, gluErrorString(e));
	}
}

void draw_triangle_strip(struct program *p, float *obj, int num)
{
	glEnableVertexAttribArray(p->aPosition_location);
	glVertexAttribPointer(p->aPosition_location, 3, GL_FLOAT,
					0, 3 * sizeof(float), obj);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, num);
	glDisableVertexAttribArray(p->aPosition_location);
	//check_error("DRAW");
}

void applyOrtho(float left, float right, float bottom, float top, struct program *p)
{
	float far = 1000.0f, near = -1000.0f;
	float a = 2.0f / (right - left);
	float b = 2.0f / (top - bottom);
	float c = -2.0f / (far - near);

	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);

	float ortho[16] = {
		a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, c, 0,
		tx, ty, tz, 1
	};

	glUniformMatrix4fv(p->pMatrix_location, 1, 0, ortho);
}

int create_program(struct program *p, GLuint v, GLuint f)
{
	char msg[512];

	p->program = glCreateProgram();
	glAttachShader(p->program, v);
	glAttachShader(p->program, f);
	glBindAttribLocation(p->program, 0, "position");
	check_error("ATTACH");

	glLinkProgram(p->program);
	glGetProgramInfoLog(p->program, sizeof msg, NULL, msg);
	printf("program info: %s\n", msg);

	p->pMatrix_location = glGetUniformLocation(p->program, "pMatrix");
	p->uMatrix_location = glGetUniformLocation(p->program, "uMatrix");
	p->aPosition_location = glGetAttribLocation(p->program, "aPosition");
	p->uColor_location = glGetUniformLocation(p->program, "uColor");

	return 0;
}

GLuint compile_vertex_shader(const char *p)
{
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	char msg[512];

	glShaderSource(v, 1, &p, NULL);
	glCompileShader(v);
	glGetShaderInfoLog(v, sizeof msg, NULL, msg);
	printf("vertex shader info: %s\n", msg);

	return v;
}

GLuint compile_fragment_shader(const char *p)
{
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
	char msg[512];

	glShaderSource(f, 1, &p, NULL);
	glCompileShader(f);
	glGetShaderInfoLog(f, sizeof msg, NULL, msg);
	printf("fragment shader info: %s\n", msg);

	return f;
}