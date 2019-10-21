#pragma once

#define GL_VERSION_4_5

#ifdef __linux__

#define GL_EXT_PROTOTYPES
#define GLX_EXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

using GLContext=GLXContext;

#define FH

#endif

#ifdef _WIN32

#define GL_EXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

using GLContext=HGLRC;

template <typename T>void wgpa(T& p, const char* name)
{
	p=(T)wglGetProcAddress(name);
	if(p==0)
	{
		printf("Function %s not found\n", name);
		char nameExt[128];
		strcpy(nameExt,name);
		strcat(nameExt,"EXT");
		p=(T)wglGetProcAddress(name);
		printf("Function %s not found\n", nameExt);
	}
};

#define STR(S) #S
#define WGPA(S) wgpa(S, STR(S))

class FnHelper
{
public:
	PFNGLCREATESHADERPROC         glCreateShader;
	PFNGLSHADERSOURCEPROC         glShaderSource;
	PFNGLCOMPILESHADERPROC        glCompileShader;
	PFNGLGETSHADERIVPROC          glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC     glGetShaderInfoLog;
	PFNGLCREATEPROGRAMPROC        glCreateProgram;
	PFNGLATTACHSHADERPROC         glAttachShader;
	PFNGLACTIVETEXTUREPROC        glActiveTexture;
	PFNGLLINKPROGRAMPROC          glLinkProgram;
	PFNGLUSEPROGRAMPROC           glUseProgram;
	PFNGLGETUNIFORMLOCATIONPROC   glGetUniformLocation;
	PFNGLUNIFORM1FPROC            glUniform1f;
	PFNGLUNIFORM2FPROC            glUniform2f;
	PFNGLUNIFORM3FPROC            glUniform3f;
	PFNGLUNIFORM4FPROC            glUniform4f;
	PFNGLUNIFORM1IPROC            glUniform1i;

	PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
	PFNGLGENRENDERBUFFERSPROC        glGenRenderbuffers;
	PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
	PFNGLBINDRENDERBUFFERPROC        glBindRenderbuffer;
	PFNGLRENDERBUFFERSTORAGEPROC     glRenderbufferStorage;
	PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
	PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;


	void Init()
	{

	WGPA(glCreateShader);
	WGPA(glShaderSource);
	WGPA(glCompileShader);
	WGPA(glGetShaderiv);
	WGPA(glGetShaderInfoLog);
	WGPA(glCreateProgram);
	WGPA(glAttachShader);
	WGPA(glActiveTexture);
	WGPA(glLinkProgram);
	WGPA(glUseProgram);
	WGPA(glGetUniformLocation);
	WGPA(glUniform1f);
	WGPA(glUniform2f);
	WGPA(glUniform3f);
	WGPA(glUniform4f);
	WGPA(glUniform1i);

	WGPA(glGenFramebuffers);
	WGPA(glGenRenderbuffers);
	WGPA(glBindFramebuffer);
	WGPA(glBindRenderbuffer);
	WGPA(glRenderbufferStorage);
	WGPA(glFramebufferRenderbuffer);
	WGPA(glFramebufferTexture2D);

	}

	static FnHelper* Instance()
	{
		static FnHelper* fh=0;
		if(!fh)
		{
			fh=new FnHelper;
			fh->Init();
		}
		return fh;
	}
};

#define FH FnHelper::Instance()->

#endif

class GLShader
{
	int vsh=-1;
	int psh=-1;
	int prg=-1;
	const char* shadername="shader";
public:
	bool Create(const char* text, int len)
	{
		if(vsh==-1)vsh=FH glCreateShader(GL_VERTEX_SHADER);
		if(psh==-1)psh=FH glCreateShader(GL_FRAGMENT_SHADER);
		
		const char* vshcode[2]={
			"#define VERTEX_SHADER\n"\
			"uniform vec4	iMouse;\n"\
			"uniform vec3	iResolution;\n"\
			"uniform float	iTime;\n"\
			,text};
		const char* fshcode[2]={
			"#define FRAGMENT_SHADER\n"\
			"uniform vec4	iMouse;\n"\
			"uniform vec3	iResolution;\n"\
			"uniform float	iTime;\n"
			,text};
		int codelens[2]={-1,len};
		
		FH glShaderSource(vsh, 2, vshcode, codelens);
		FH glShaderSource(psh, 2, fshcode, codelens);

		GLint ok;
		char errorbuf[1024];
		int errorlen=0;
			
		FH glCompileShader(vsh);
		FH glGetShaderiv(vsh, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			FH glGetShaderInfoLog(vsh, 1024, &errorlen, errorbuf);
			fprintf(stderr,"Vertex: \n%s\n",errorbuf);
			return false;
		}	

		FH glCompileShader(psh);
		FH glGetShaderiv(psh, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			FH glGetShaderInfoLog(psh, 1024, &errorlen, errorbuf);
			fprintf(stderr,"Fragment: \n%s\n",errorbuf);
			return false;
		}
		if(prg==-1)prg=FH glCreateProgram();
		FH glAttachShader(prg,vsh);
		FH glAttachShader(prg,psh);
		FH glLinkProgram(prg);
		
		return true;
	}
	bool CreateFromFile(const char* filename)
	{
		shadername=filename;
		FILE* f=fopen(filename,"rb");
		if(!f)
		{
			fprintf(stderr,"Shader file missing, %s\n", filename);
			return false;
		}
		fseek(f,0,SEEK_END);
		int len=ftell(f);
		fseek(f,0,SEEK_SET);
		char* text=new char[len];
		fread(text, len, 1, f);
		bool result=Create(text,len);
		delete[] text;
		fclose(f);

		if(result)
		{
			fprintf(stderr,"Shader created OK, %s\n", filename);
			fflush(stderr);
		}

		return result;
	}

	void SetUniform1i(const char* name, const int& value)
	{
		int location=FH glGetUniformLocation(prg,name);
		if(location==-1)
			fprintf(stderr,"Uniform %s not found in shader %s\n",name, shadername);
		FH glUniform1i(location,value);
	}

	void SetUniform1f(const char* name, const float& value)
	{
		int location=FH glGetUniformLocation(prg,name);
		if(location==-1)
			fprintf(stderr,"Uniform %s not found in shader %s\n",name, shadername);
		FH glUniform1f(location,value);
	}
	void SetUniform2f(const char* name, const float& value0, const float& value1)
	{
		int location=FH glGetUniformLocation(prg,name);
		if(location==-1)
			fprintf(stderr,"Uniform %s not found in shader %s\n",name, shadername);
		FH glUniform2f(location,value0,value1);
	}
	void SetUniform3f(const char* name, const float& value0, const float& value1, const float& value2)
	{
		int location=FH glGetUniformLocation(prg,name);
		if(location==-1)
			fprintf(stderr,"Uniform %s not found in shader %s\n",name, shadername);
		FH glUniform3f(location,value0,value1,value2);
	}
	void SetUniform4f(const char* name, const float& value0, const float& value1, const float& value2, const float& value3)
	{
		int location=FH glGetUniformLocation(prg,name);
		if(location==-1)
			fprintf(stderr,"Uniform %s not found in shader %s\n",name, shadername);
		FH glUniform4f(location,value0,value1,value2,value3);
	}

	bool Use()
	{
		FH glUseProgram(prg);
		return true;
	}
};

class CGL
{
#ifdef __linux__
	Display* dsp;
#endif

#ifdef WIN32

	HDC dc;

#endif

	WINDOW win;

	bool doubleBuf;
	GLContext glc;

	GLuint textureA;
	GLuint fboA;
	GLuint textureB;
	GLuint fboB;
	GLuint textureC;
	GLuint fboC;

	GLShader sh0;
	GLShader shGauss;
	GLShader shBlit;

	int blurMode=0;
	float mx;
	float my;
public:
	void SetMousePos(float x, float y)
	{
		mx=x;
		my=y;
	}

	void NextMode()
	{
		blurMode++;
		if(blurMode>5)
			blurMode=0;
		printf("Gaussian mode %i\n", blurMode);
	};

	int Init(const SYS* sys, WINDOW& window)
	{
		puts("******** Init ... **********\n");
		win=window;
#ifdef __linux__
		dsp=sys->GetDSP();
		int doubleBufferVisual[]=
		{
			GLX_RGBA,           // Needs to support OpenGL
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			GLX_DOUBLEBUFFER,   // Needs to support double-buffering
			None                // end of list
		};

		int singleBufferVisual[]=
		{
			GLX_RGBA,           // Needs to support OpenGL
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			None                // end of list
		};

		doubleBuf=true;
		XVisualInfo* visualInfo=glXChooseVisual(dsp, DefaultScreen(dsp), doubleBufferVisual);
		
		if(!visualInfo)
		{
			visualInfo=glXChooseVisual(dsp, DefaultScreen(dsp), singleBufferVisual);
			if(visualInfo==nullptr)
			{
				fprintf(stderr, "No suitable visual\n");
				return 0;
			}
			doubleBuf=false;
		}

		glc=glXCreateContext(dsp,visualInfo, nullptr, GL_TRUE);
		if(glc==nullptr)
		{
			fprintf(stderr, "Could not create rendering context\n");
			return 1;
		}

		glXMakeCurrent(dsp, win, glc);
#endif

#ifdef WIN32

		dc = GetDC(win);
		PIXELFORMATDESCRIPTOR pfd = { 0, 0, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // wrong way ;)
		//		oldPixelFormat=GetPixelFormat(dc);
		SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
		glc = wglCreateContext(dc);
		wglMakeCurrent(dc, glc);
		doubleBuf=true;

#endif

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &textureA);
		glBindTexture(GL_TEXTURE_2D, textureA);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA,  GL_UNSIGNED_BYTE, NULL);

		FH glGenFramebuffers(1, &fboA);
		FH glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		FH glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureA, 0);

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &textureB);
		glBindTexture(GL_TEXTURE_2D, textureB);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA,  GL_UNSIGNED_BYTE, NULL);

		FH glGenFramebuffers(1, &fboB);
		FH glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		FH glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureB, 0);

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &textureC);
		glBindTexture(GL_TEXTURE_2D, textureC);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA,  GL_UNSIGNED_BYTE, NULL);

		FH glGenFramebuffers(1, &fboC);
		FH glBindFramebuffer(GL_FRAMEBUFFER, fboC);
		FH glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureC, 0);

		ReloadShaders();

		puts("******** Init OK **********\n");
		return 1;
	}

	int ReloadShaders()
	{
		sh0.CreateFromFile("sh0.glsl");
		shGauss.CreateFromFile("shgaussblur.glsl");
		shBlit.CreateFromFile("shblit.glsl");
		return 1;
	}

	void FSQuad()
	{
		// TODO: replace with VBO
			glBegin(GL_TRIANGLES);

			glVertex2f(-1.0,-1.0);
			glVertex2f(-1.0, 1.0);
			glVertex2f( 1.0, 1.0);
			glVertex2f(-1.0,-1.0);
			glVertex2f( 1.0,-1.0);
			glVertex2f( 1.0, 1.0);

			glEnd();
	}

	int Render()
	{
#ifdef __linux__
		glXMakeCurrent(dsp, win, glc);
#endif

#ifdef WIN32
		wglMakeCurrent(dc, glc);
#endif

		glEnable(GL_TEXTURE_2D);

		FH glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		sh0.Use();
		sh0.SetUniform3f("iResolution", 1024,768,1);
		sh0.SetUniform1f("iTime", 4.);
		sh0.SetUniform4f("iMouse", mx,my,0,0);
		FSQuad();

		glBindTexture(GL_TEXTURE_2D, textureA);
		FH glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		shGauss.Use();
		shGauss.SetUniform2f("delta",0.,1.);
		shGauss.SetUniform1i("mode",blurMode);
		shGauss.SetUniform3f("iResolution", 1024,768,1);
		shGauss.SetUniform4f("iMouse", mx,my,0,0);
		FSQuad();

		glBindTexture(GL_TEXTURE_2D, textureB);
		FH glBindFramebuffer(GL_FRAMEBUFFER, fboC);
		shGauss.Use();
		shGauss.SetUniform2f("delta",1.,0.);
		shGauss.SetUniform1i("mode",blurMode);
		shGauss.SetUniform3f("iResolution", 1024,768,1);
		shGauss.SetUniform4f("iMouse", mx,my,0,0);
		FSQuad();

		glBindTexture(GL_TEXTURE_2D, textureC);
		FH glBindFramebuffer(GL_FRAMEBUFFER, 0);
		shBlit.Use();
		FSQuad();

		if(doubleBuf)
#ifdef __linux__
				glXSwapBuffers(dsp, win);
#endif
#ifdef WIN32
				SwapBuffers(dc);
#endif
		else
				glFlush();
		return 1;
	}

	void Done()
	{
	}
};
