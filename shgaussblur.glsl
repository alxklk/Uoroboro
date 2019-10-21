varying vec2 v_p;
uniform int mode;
uniform vec2 delta;

#ifdef VERTEX_SHADER

void main()
{
	gl_Position=gl_Vertex;
	v_p=gl_Vertex.xy*0.5+0.5;
}

#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D tex;

float gauss(float x)
{
	return 1.0/exp(x*x);
}

void main()
{
	vec2 tc=v_p;
	if((mode==0)||((mode==1)&&(tc.x<0.5)))
	{
		gl_FragColor=texture2D(tex, tc.xy);
		return;
	}
	vec4 c=vec4(0);
	float w=0.0;
	float radius=15.;
	if(mode==2)
	{
		radius=abs(tc.y-.5)*30.;
	}
	else if(mode==3)
	{
		radius=length(tc-0.5)*3.;
		radius=pow(radius,4.)*10.;
	}
	else if(mode==4)
	{
		float l=length((tc-iMouse.xy/iResolution.xy)/(iResolution.yy/iResolution.xy));
		if(l<0.15)
			radius=40.*(1.-l/.15);
		else
			radius=1.;
	}

	int count=int(radius)+1;

	vec2 d=delta/iResolution.xy;
	
	for(int i=-count;i<=count;i++)
	{
		float g=gauss(float(i)/float(count));
		vec2 t=tc.xy+d*float(i);
		if(any(lessThan(t,vec2(0))))
			continue;
		if(any(greaterThan(t,vec2(1))))
			continue;
		c+=texture2D(tex,t)*g;
		w+=g;
	}
	gl_FragColor=c/w;
}

#endif

