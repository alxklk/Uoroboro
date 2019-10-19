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

#ifdef WIN32

#define GL_EXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#define WGL_WGLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/wglext.h>

using GLContext=HGLRC;

template <typename T>void wgpa(T& p, const char* name){p=(T)wglGetProcAddress(name);};

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
	WGPA(glUniform4f);

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
	int vsh;
	int psh;
	int prg;
public:
	bool Create(const char* text, int len)
	{
		int newvsh=FH glCreateShader(GL_VERTEX_SHADER);
		int newpsh=FH glCreateShader(GL_FRAGMENT_SHADER);
		
		const char* vshcode[2]={"#define VERTEX_SHADER\n",text};
		const char* fshcode[2]={
			"#define FRAGMENT_SHADER\n"
			"uniform vec4	iMouse;\n"
			"uniform vec3	iResolution;\n"
			"uniform float	iTime;\n"
/*			"float	iTimeDelta;\n"
			"int	iFrame;\n"
			"float	iFrameRate;\n"
			"float	iChannelTime[4];\n"
			"vec3	iChannelResolution[4];\n"
			"sampler2D	iChannel0;\n"
			"sampler2D	iChannel1;\n"
			"sampler2D	iChannel2;\n"
			"sampler2D	iChannel3;\n"
			"vec4	iDate;\n"*/
			,text};
		int codelens[2]={-1,len};
		
		FH glShaderSource(newvsh, 2, vshcode, codelens);
		FH glShaderSource(newpsh, 2, fshcode, codelens);

		GLint ok;
		char errorbuf[1024];
		int errorlen=0;
			
		FH glCompileShader(newvsh);
		FH glGetShaderiv(newvsh, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			FH glGetShaderInfoLog(newvsh, 1024, &errorlen, errorbuf);
			fprintf(stderr,"Veertex: \n%s\n",errorbuf);
			return false;
		}	

		FH glCompileShader(newpsh);
		FH glGetShaderiv(newpsh, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			FH glGetShaderInfoLog(newpsh, 1024, &errorlen, errorbuf);
			fprintf(stderr,"Fragment: \n%s\n",errorbuf);
			return false;
		}
		int newprg=FH glCreateProgram();
		FH glAttachShader(newprg,newvsh);
		FH glAttachShader(newprg,newpsh);
		FH glLinkProgram(newprg);
		
		vsh=newvsh;
		psh=newpsh;
		prg=newprg;
		return true;
	}
	bool CreateFromFile(const char* filename)
	{
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
			fprintf(stderr,"Shader created OK, %s\n", filename);

		return result;
	}

	void SetUniform1f(const char* name, const float& value)
	{
		FH glUniform1f(FH glGetUniformLocation(prg,name),value);
	}
	void SetUniform2f(const char* name, const float& value0, const float& value1)
	{
		FH glUniform2f(FH glGetUniformLocation(prg,name),value0,value1);
	}
	void SetUniform3f(const char* name, const float& value0, const float& value1, const float& value2)
	{
		FH glUniform3f(FH glGetUniformLocation(prg,name),value0,value1,value2);
	}
	void SetUniform4f(const char* name, const float& value0, const float& value1, const float& value2, const float& value3)
	{
		FH glUniform4f(FH glGetUniformLocation(prg,name),value0,value1,value2,value3);
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

	GLShader sh0;
	GLShader shGaussVert;
	GLShader shGaussHorz;
	GLShader shBlit;

	float blurRadius;
	float mx;
	float my;
public:
	void SetMousePos(float x, float y)
	{
		mx=x;
		my=y;
	}

	void IncreaseBlur(){blurRadius+=0.1;};
	void DecreaseBlur()
	{
		blurRadius-=0.1;
		if(blurRadius<0)
			blurRadius=0;
	};

	int Init(const SYS* sys, WINDOW& window)
	{
		blurRadius=1.0;
#ifdef __linux__
		dsp=sys->dsp;
		win=window;
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


		sh0.CreateFromFile("sh0.glsl");
		shGaussVert.CreateFromFile("shgaussvert.glsl");
		shGaussHorz.CreateFromFile("shgausshorz.glsl");
		shBlit.CreateFromFile("shblit.glsl");
		return 1;
	}

	int ReloadShaders()
	{
		sh0.CreateFromFile("sh0.glsl");
		shGaussVert.CreateFromFile("shgaussvert.glsl");
		shGaussHorz.CreateFromFile("shgausshorz.glsl");
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
		sh0.SetUniform1f("iTime", 1.);
		sh0.SetUniform4f("iMouse", mx,my,0,0);
		FSQuad();

		glBindTexture(GL_TEXTURE_2D, textureA);
		FH glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		shGaussVert.SetUniform1f("radius",blurRadius);
		shGaussVert.Use();
		FSQuad();

		glBindTexture(GL_TEXTURE_2D, textureB);
		FH glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		shGaussHorz.SetUniform1f("radius",blurRadius);
		shGaussHorz.Use();
		FSQuad();


		FH glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, textureA);
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

};
