varying vec2 v_p;

#ifdef VERTEX_SHADER

void main()
{
	gl_Position=gl_Vertex;
	v_p=gl_Vertex.xy*0.5+0.5;
	v_p.y=1.0-v_p.y;
}

#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D tex;

void main()
{
	vec2 tc=v_p;
	gl_FragColor=texture2D(tex, tc.xy);
}

#endif

