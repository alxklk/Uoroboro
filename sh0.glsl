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

// Fork of "Isosurface Heart" by klk. https://shadertoy.com/view/XtVSRh
// 2019-10-17 20:19:15

#define PI 3.1415926535897932384626433832795

#define float3 vec3
#define float2 vec2
#define float4 vec4
#define float3x3 mat3

const float MAX_RAY_LENGTH=10000.0;

void RP(float3 tp0, float3 dp1, float3 dp2, float3 rp0, float3 rd, out float t, out float3 uv, out float3 n)
{
	float3 dp0=rp0-tp0;

	float3 dett =cross(dp1,dp2);
	float3 detuv=cross(dp0,rd);

	float det=(-1.0)/dot(dett,rd);

	float u=(dot(detuv,dp2))*det;
	float v=(dot(detuv,dp1))*det;
	t=(dot(dett ,dp0))*det;
    if(t<0.0)
    {
        t=MAX_RAY_LENGTH;
        return;
    }

	uv=float3(u,v,0.0);
    n=normalize(dett);
}

float arws0(float2 p, float t)
{
    float2 p1=float2(p.x+p.y,p.x-p.y);
    float2 f1xy=abs(fract(p1/sqrt(8.0))-0.5)-0.25;
    f1xy=clamp(f1xy*t+0.5,0.0,1.0);
    float f1=mix(f1xy.x,1.0-f1xy.x,f1xy.y);

    float2 fxy=float2(p.x-sqrt(0.125),p.y);
  	fxy=abs(fract((fxy*sqrt(2.0)+0.5)/2.0)-0.5)-0.25;
    fxy=clamp(fxy*t/sqrt(2.0)+0.5,0.0,1.0);
    float f=mix(fxy.x,1.0-f1,fxy.y);

    return f;
}

float chkr(float2 p, float t)
{
    float2 fxy=float2(p.x,p.y);
  	fxy=abs(fract((fxy+0.5)/2.0)-0.5)-0.25;
    fxy=clamp(fxy*t+0.5,0.0,1.0);
    float f=mix(fxy.x,1.0-fxy.x,fxy.y);

    return f;
}

float plnt(float2 p, float t)
{
    return chkr(p,t);
}

void trace(float3 rp0, float3 rd, out float t, out float3 pos, out float3 n)
{
    float t1=MAX_RAY_LENGTH;
    float3 col1;
    float3 n1;
   	RP(float3(0.0,-10.0,0.0),float3(-1.0,0.0,0.0),float3(0.0,0,1.0),rp0, rd, t1, col1, n1);
    pos=rp0+rd*t1;
    t=t1;
}

// From IQ
float smin(float a, float b, float k)
{
    float h=clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);
    return mix(b, a, h)-k*h*(1.0-h);
}

float smax( float a, float b, float k)
{
	return -smin(-a,-b,k);
}

float smin( float a, float b)
{
    return smin(a,b,0.1);
}

float smax( float a, float b)
{
    return smax(a,b,0.1);
}


float sq(float x){return x*x;}

float torus(float x, float y, float z, float R, float r)
{
    return sqrt(sq(sqrt(sq(x)+sq(z))-R)+sq(y))-r;
}


float lid(float x, float y, float z)
{
	float v=sqrt(sq(x)+sq(y-0.55)+sq(z))-1.4;
//    v=smin(v,sqrt(sq(x)+sq(y-2.25)*2.+sq(z))-.75,.2);
    v=smin(v,torus(y-2.,x,z,.2,.08),.1);

    v=smax(v,-sqrt(sq(x)+sq(y-0.55)+sq(z))+1.3);
    v=smax(v,sqrt(sq(x)+sq(y-2.5)+sq(z))-1.3);

    v=smax(v,-sqrt(sq(x-.25)+sq(z-.35))+0.05,.05);
    v=smin(v,torus(x,(y-1.45)*.75,z,.72,.065),.2);
    return v;
}

float nose(float x, float y, float z)
{
	z-=sin((y+0.8)*3.6)*.15;

	float v=sqrt(sq(x)+sq(z));

    v=abs(v-.3+sin(y*1.6+.5)*0.18)-.05;
    v=smax(v,-y-1.);
    v=smax(v,y-0.85,.075);

    return v;
}

float value0(float3 p)
{
    p.y-=1.;
    p*=0.4;
    float x=p.x;
    float y=p.y;
    float z=p.z;

    float v=0.0;
    v=sqrt(x*x+z*z)-1.2-sin(y*1.5+2.0)*.4;
    v=smax(v,abs(y)-1.,0.3);



    float v1=sqrt(x*x*4.+sq(y+z*.1)*1.6+sq(z+1.2))-1.0;
    v1=smax(v1,-sqrt(sq(z+1.2)+sq(y+z*.12+.015)*1.8)+.8,.3);
//    v=smin(v,v1);

    v=smin(v,torus(y*1.2+.2+z*.3,x*.75,z+1.25+y*.2,.8,.1),.25);
    v=smin(v,sqrt(sq(x)+sq(y-1.1)+sq(z+1.8))-.05,.32);

    float v3=nose(x,(y+z)*sqrt(.5)-1.6,(z-y)*sqrt(.5)-1.1);
    //return v3;
    v=smin(v,v3,0.2);

    v=smax(v,smin(sin(y*1.4+2.0)*0.5+.95-sqrt(x*x+z*z),y+.8, .2));
    v=smax(v,-sqrt(sq(x)+sq(y+.15)+sq(z-1.5))+.12);

    v=smin(v,torus(x,y-0.95,z,0.9,.075));
    v=smin(v,torus(x,y+1.05,z,1.15,.05),0.15);


    float v2=lid(x,y+.5,z);
    v=min(v,v2);

    return v;
}

float value(float3 p)
{
	float v=value0(p);
    return v;
    return smax(v,p.x*.3,0.01);
}

bool raymarch(float3 start, float3 d, float startT, float stp, const int N, out float t, out float v)
{
    float t0=startT;
    t=t0;

    int i=0;
    for(int j=0;j<1;j+=0)
    {
	    float3 p=start+d*t;
        float v1=value(p);
        if(v1<0.)
        {
            t=t0+(t-t0)*v/(v-v1);
            p=start+d*t;
	        v=value(p);
            return true;
        }
		v=v1;
        t0=t;
        i++;
        if(i>N)
            break;
        t+=max(v,stp);
    }
    return false;
}

float3 calcN(float3 p, float n0)
{
    float3 n;
    float d=0.0001;
    n.x=value(p+float3(d,0.0,0.0))-n0;
    n.y=value(p+float3(0.0,d,0.0))-n0;
    n.z=value(p+float3(0.0,0.0,d))-n0;

    n=normalize(n);
    return n;
}

float nrand( vec2 n )
{
	return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float3 campos=float3(-20.0,3.0,0.0);
    float3 look_at=float3(0.0,1.0,0.0);
    float3 up=float3(0,1,0);
    float3 forward;
    float3 right;

    float3 light=float3(0,10,10);

    float T=iTime*0.45;

    light.x=cos(T)*10.0;
    light.z=sin(T)*10.0;
    light.y=5.0;

    float mposx=iMouse.x;
    float mposy=iMouse.y;
    if(iMouse.z<0.0)mposx=-iMouse.z;
    if(iMouse.w<0.0)mposy=-iMouse.w;

    float a1=(0.6+(mposy/iResolution.y-0.5)*0.7)*PI;
    float a2=mposx/iResolution.x*PI*2.0-PI/3.0;

    if(iMouse.y<10.0)
    {
        a1=PI*0.55;
        a2=PI+0.3;
    }

    campos.y=cos(a1)*campos.x;
    float camx=sin(a1)*campos.x;
    campos.x=cos(a2)*camx;
    campos.z=sin(a2)*camx;
    light=campos*2.0;
    light.x=10.0;
    light.z=15.0;
    light.y=10.0;
//    campos+=look_at;

    forward=normalize(look_at-campos);
    right=normalize(cross(up,forward));
    up=normalize(cross(forward,right));


	float2 scr = fragCoord.xy /iResolution.xy;
    scr=2.0*scr-1.0;

    float2 scr2ray=scr;
    scr2ray.x*=(iResolution.x/iResolution.y);
	float2 uv=scr2ray;
    float3 ray=normalize(forward+(up*uv.y+right*uv.x)*0.3);

    float3 col=float3(0.0,0.5,0.0);
    float3 n;
	float t;
//	fragColor.rgb=float3(0.5,0.3,0.1);
    float3 fogcol=mix(float3(0.87,0.8,0.83),float3(0.3,0.6,1.0),clamp(1.0-(1.0-ray.y)*(1.0-ray.y),0.,1.));
   	fragColor.rgb=fogcol;
    float3 tpos;
    trace(campos, ray, t, tpos, n);
    col=mix(float3(0.97,0.95,0.83),float3(0.1,0.15,0.4),
                smoothstep(0.0,1.0,plnt(tpos.xz*0.2,36000.0/t/t)));
    float3 tolight=normalize(light);

    if(1<0)
    {
        float t1=MAX_RAY_LENGTH;
        float3 col1;
        float3 n1;
        RP(float3(0.,0.,0.),float3(0.0,1.0,0.0),float3(0.0,0,1.0),campos, ray, t1, col1, n1);
        float3 pos=campos+ray*t1;
        if(t1<t)
        {
            t=t1;
            float v=value0(pos);
            col.r=v>0.?fract(v):0.5;
            col.b=v<0.?fract(v):0.5;
            col.g=.5-abs(clamp(fract(v*10.),0.,1.)-0.5);
        }
    	fragColor.rgb=col;
        return;

    }

    if(t<MAX_RAY_LENGTH)
	{

        col=mix(fogcol,col,exp(-t*0.005));
    	fragColor.rgb=col;
    }
    {
    	float t1;
	    float3 start=campos;
        float n0;
        if(raymarch(start,ray,10.+nrand(scr),.25,50,t1,n0))
        {
            if(t1<t)
            {
            	float3 p=start+ray*t1;
            	float3 n=calcN(p,n0);
//                float3 tolight=normalize(light-p);
			    float3 halfn=normalize(tolight-ray);

                float lamb=pow(clamp(dot(n,tolight),0.0,1.0),1.0)*0.9+0.1;
                float3 refray=reflect(ray,n);

                float spec1=clamp(dot(halfn,n),0.0,1.0);
                float spec2=clamp(dot(tolight,refray),0.0,1.0);

			    float3 reffog=mix(
                    float3(0.87,0.8,0.83),
                    float3(0.3,0.6,1.0),
                    clamp(1.0-(1.0-refray.y)*(1.0-refray.y),0.,1.));

                float3 n1;
//                col=lamb*(0.5+0.5*float3(sin(p.x),sin(p.z),sin(length(p.xz))));
                col=lamb*float3(0.78,0.79,0.8);
                float3 rpos;
                float3 rcol;
                float tr;
				trace(p, reflect(ray,n), tr, rpos, n1);
                float fresn=clamp(1.0-dot(ray,-n),0.0,1.0);
    			if(t1<MAX_RAY_LENGTH)
                {
				    rcol=mix(float3(0.87,0.85,0.83),float3(0.1,0.12,0.4),
                		smoothstep(0.0,1.0,plnt(rpos.xz*0.2,1000.0/tr/tr)));
			        rcol=mix(reffog,rcol,exp(-tr*0.02));
                }
                else
                {
                    rcol=reffog;
                }
                {
                    col=mix(col,rcol,pow(fresn,3.6)*0.50);
                    col+=rcol*(pow(fresn,2.6)*0.2+0.2);
                    col=mix(col,float3(1,1,1),pow(spec2,40.0)*.4);
//                    col*=float3(1.0,0.8,0.5);
                    col=mix(col,float3(1,1,1),.8*pow(spec2,180.0));
			        //col=mix(fogcol,col,exp(-t1*0.01));
//                    col*=float3(1.1,0.95,0.65);
//                    col=rcol;

//			    	fragColor.rgb=mix(col,fragColor.rgb,pow(dot(ray,n),15.0));
                }
                fragColor.rgb=col;
            }
        }
    }

    fragColor.a=1.0;

}

void main()
{
	vec2 tc=v_p;
	vec4 col;
	mainImage(col, tc*1024.0);

	gl_FragColor=col;
}

#endif

