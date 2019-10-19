#define GL_VERSION_4_5

#define GL_EXT_PROTOTYPES 
#define GLX_EXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#if 0
class GLH
{
	bool started;
public:
	PFNGLGENFRAMEBUFFERSPROC      glGenFramebuffers     ;
	PFNGLBINDFRAMEBUFFERPROC      glBindFramebuffer     ;
	PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
	PFNGLCREATESHADERPROC         glCreateShader        ;
	PFNGLSHADERSOURCEPROC         glShaderSource        ;
	PFNGLCOMPILESHADERPROC        glCompileShader       ;
	PFNGLGETSHADERIVPROC          glGetShaderiv         ;
	PFNGLGETSHADERINFOLOGPROC     glGetShaderInfoLog    ;
	PFNGLCREATEPROGRAMPROC        glCreateProgram       ;
	PFNGLATTACHSHADERPROC         glAttachShader        ;
	PFNGLLINKPROGRAMPROC          glLinkProgram         ;
	PFNGLUSEPROGRAMPROC           glUseProgram          ;
	PFNGLPROGRAMUNIFORM1FPROC     glProgramUniform1f    ;
	PFNGLPROGRAMUNIFORM2FPROC     glProgramUniform2f    ;
	PFNGLPROGRAMUNIFORM3FPROC     glProgramUniform3f    ;
	PFNGLPROGRAMUNIFORM4FPROC     glProgramUniform4f    ;

	GLH():started(false){};
	GLH& I()
	{
		static GLH glh;
		if(!glh.started)
			Init();
		return glh;
	}
	
#define gl_GetProcAddress glXGetProcAddress
	
	void Init()
	{
		/*PFNGLGENFRAMEBUFFERSPROC      */glGenFramebuffers     =(PFNGLGENFRAMEBUFFERSPROC)      gl_GetProcAddress((const GLubyte*)"glGenFramebuffers"     );
		/*PFNGLBINDFRAMEBUFFERPROC      */glBindFramebuffer     =(PFNGLBINDFRAMEBUFFERPROC)      gl_GetProcAddress((const GLubyte*)"glBindFramebuffer"     );
		/*PFNGLFRAMEBUFFERTEXTURE2DPROC */glFramebufferTexture2D=(PFNGLFRAMEBUFFERTEXTURE2DPROC) gl_GetProcAddress((const GLubyte*)"glFramebufferTexture2D");
		/*PFNGLCREATESHADERPROC         */glCreateShader        =(PFNGLCREATESHADERPROC)         gl_GetProcAddress((const GLubyte*)"glCreateShader"        );
		/*PFNGLSHADERSOURCEPROC         */glShaderSource        =(PFNGLSHADERSOURCEPROC)         gl_GetProcAddress((const GLubyte*)"glShaderSource"        );
		/*PFNGLCOMPILESHADERPROC        */glCompileShader       =(PFNGLCOMPILESHADERPROC)        gl_GetProcAddress((const GLubyte*)"glCompileShader"       );
		/*PFNGLGETSHADERIVPROC          */glGetShaderiv         =(PFNGLGETSHADERIVPROC)          gl_GetProcAddress((const GLubyte*)"glGetShaderiv"         );
		/*PFNGLGETSHADERINFOLOGPROC     */glGetShaderInfoLog    =(PFNGLGETSHADERINFOLOGPROC)     gl_GetProcAddress((const GLubyte*)"glGetShaderInfoLog"    );
		/*PFNGLCREATEPROGRAMPROC        */glCreateProgram       =(PFNGLCREATEPROGRAMPROC)        gl_GetProcAddress((const GLubyte*)"glCreateProgram"       );
		/*PFNGLATTACHSHADERPROC         */glAttachShader        =(PFNGLATTACHSHADERPROC)         gl_GetProcAddress((const GLubyte*)"glAttachShader"        );
		/*PFNGLLINKPROGRAMPROC          */glLinkProgram         =(PFNGLLINKPROGRAMPROC)          gl_GetProcAddress((const GLubyte*)"glLinkProgram"         );
		/*PFNGLUSEPROGRAMPROC           */glUseProgram          =(PFNGLUSEPROGRAMPROC)           gl_GetProcAddress((const GLubyte*)"glUseProgram"          );
		/*PFNGLPROGRAMUNIFORM1FPROC     */glProgramUniform1f    =(PFNGLPROGRAMUNIFORM1FPROC)     gl_GetProcAddress((const GLubyte*)"glProgramUniform1f"    );
		/*PFNGLPROGRAMUNIFORM2FPROC     */glProgramUniform2f    =(PFNGLPROGRAMUNIFORM2FPROC)     gl_GetProcAddress((const GLubyte*)"glProgramUniform2f"    );
		/*PFNGLPROGRAMUNIFORM3FPROC     */glProgramUniform3f    =(PFNGLPROGRAMUNIFORM3FPROC)     gl_GetProcAddress((const GLubyte*)"glProgramUniform3f"    );
		/*PFNGLPROGRAMUNIFORM4FPROC     */glProgramUniform4f    =(PFNGLPROGRAMUNIFORM4FPROC)     gl_GetProcAddress((const GLubyte*)"glProgramUniform4f"    );
	}
};

/*
PFNGLGENFRAMEBUFFERSPROC      glGenFramebuffers     ;
PFNGLBINDFRAMEBUFFERPROC      glBindFramebuffer     ;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLCREATESHADERPROC         glCreateShader        ;
PFNGLSHADERSOURCEPROC         glShaderSource        ;
PFNGLCOMPILESHADERPROC        glCompileShader       ;
PFNGLGETSHADERIVPROC          glGetShaderiv         ;
PFNGLGETSHADERINFOLOGPROC     glGetShaderInfoLog    ;
PFNGLCREATEPROGRAMPROC        glCreateProgram       ;
PFNGLATTACHSHADERPROC         glAttachShader        ;
PFNGLLINKPROGRAMPROC          glLinkProgram         ;
PFNGLUSEPROGRAMPROC           glUseProgram          ;
*/

#endif 

class GLShader
{
	int vsh;
	int psh;
	int prg;
public:
	bool Create(const char* text, int len)
	{
		int newvsh=glCreateShader(GL_VERTEX_SHADER);
		int newpsh=glCreateShader(GL_FRAGMENT_SHADER);
		
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
		
		glShaderSource(newvsh, 2, vshcode, codelens);
		glShaderSource(newpsh, 2, fshcode, codelens);

		GLint ok;
		char errorbuf[1024];
		int errorlen=0;
			
		glCompileShader(newvsh);
		glGetShaderiv(newvsh, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			glGetShaderInfoLog(newvsh, 1024, &errorlen, errorbuf);
			fprintf(stderr,"Veertex: \n%s\n",errorbuf);
			return false;
		}	

		glCompileShader(newpsh);
		glGetShaderiv(newpsh, GL_COMPILE_STATUS, &ok);
		if(!ok)
		{
			glGetShaderInfoLog(newpsh, 1024, &errorlen, errorbuf);
			fprintf(stderr,"Fragment: \n%s\n",errorbuf);
			return false;
		}
		int newprg=glCreateProgram();
		glAttachShader(newprg,newvsh);
		glAttachShader(newprg,newpsh);
		glLinkProgram(newprg);
		
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
		glUniform1f(glGetUniformLocation(prg,name),value);
	}
	void SetUniform2f(const char* name, const float& value0, const float& value1)
	{
		glUniform2f(glGetUniformLocation(prg,name),value0,value1);
	}
	void SetUniform3f(const char* name, const float& value0, const float& value1, const float& value2)
	{
		glUniform3f(glGetUniformLocation(prg,name),value0,value1,value2);
	}
	void SetUniform4f(const char* name, const float& value0, const float& value1, const float& value2, const float& value3)
	{
		glUniform4f(glGetUniformLocation(prg,name),value0,value1,value2,value3);
	}

	bool Use()
	{
		glUseProgram(prg);
		return true;
	}
};

class CGL
{
	bool doubleBuf;
	Display* display;
	Window window;
	GLXContext glxContext;

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

	int Init(Display* dsp, Window& win)
	{
		blurRadius=1.0;
		display=dsp;
		window=win;
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

		glxContext=glXCreateContext(dsp,visualInfo, nullptr, GL_TRUE);
		if(glxContext==nullptr)
		{
			fprintf(stderr, "Could not create rendering context\n");
			return 1;
		}

		glXMakeCurrent(dsp, win, glxContext);


	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureA);
	glBindTexture(GL_TEXTURE_2D, textureA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA,  GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fboA);
	glBindFramebuffer(GL_FRAMEBUFFER, fboA);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureA, 0);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textureB);
	glBindTexture(GL_TEXTURE_2D, textureB);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 768, 0, GL_RGBA,  GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fboB);
	glBindFramebuffer(GL_FRAMEBUFFER, fboB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureB, 0);


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
		glXMakeCurrent(display, window, glxContext);

		glEnable(GL_TEXTURE_2D);

		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		sh0.Use();
		sh0.SetUniform3f("iResolution", 1024,768,1);
		sh0.SetUniform1f("iTime", 1.);
		sh0.SetUniform4f("iMouse", mx,my,0,0);
		FSQuad();

		glBindTexture(GL_TEXTURE_2D, textureA);
		glBindFramebuffer(GL_FRAMEBUFFER, fboB);
		shGaussVert.SetUniform1f("radius",blurRadius);
		shGaussVert.Use();
		FSQuad();

		glBindTexture(GL_TEXTURE_2D, textureB);
		glBindFramebuffer(GL_FRAMEBUFFER, fboA);
		shGaussHorz.SetUniform1f("radius",blurRadius);
		shGaussHorz.Use();
		FSQuad();


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, textureA);
		shBlit.Use();
		FSQuad();

		if(doubleBuf)
				glXSwapBuffers(display, window);
		else
				glFlush();
		return 1;
	}

};
