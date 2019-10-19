varying vec2 v_p;

#ifdef VERTEX_SHADER

void main()
{
	gl_Position=gl_Vertex;
	v_p=gl_Vertex.xy*0.5+0.5;
}

#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D tex;
uniform vec2 mp;
uniform float radius;

float gauss(float x)
{
	return 1.0/exp(x*x);
}

void main()
{
	vec2 tc=v_p;
	if(tc.x<0.5)
	{
		gl_FragColor=texture2D(tex, tc.xy);
		return;
	}
	vec4 c=vec4(0);
	float w=0.0;
	float radius=2.0;
	int count=int(radius*5.0);
	for(int i=-count;i<=count;i++)
	{
		float g=gauss(float(i)/float(count)*2.0);
		c+=texture2D(tex, tc.xy+vec2(float(i)*2.0/1024.0,0.0))*g;
		w+=g;
	}
	gl_FragColor=c/w;
}

#endif

