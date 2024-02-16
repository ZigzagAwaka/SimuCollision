#version 330 core

in vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
in vec3 vNormal_vs; // Normale du sommet transformé dans l'espace View
in vec2 vTexCoords; // Coordonnées de texture du sommet

out vec3 fFragColor;

uniform sampler2D uTexture0;
uniform float uTimeSt; // ellapsed time


// using https://www.shadertoy.com/view/tdj3Ww with minor modifications
// the resulting effect is quite hard to see, but can be observed by zooming on the surface of the sun
// it's definitly not the best "sun surface light" effect-like but it's still nice, i think


float gamma = 4.0;
float scale = 0.4;
float exposure = 10.2;

vec3 sunColor = vec3(0.9,0.9,0.7);

vec2 darklimbUlVl(float wavelength) {
    float ll = wavelength; //wavelength in Angstroms (lambda)
    float au = -8.9829751;
    float bu = 0.0069093916;
    float cu = -1.8144591e-6;
    float du = 2.2540875e-10;
    float eu = -1.3389747e-14;
    float fu = 3.0453572e-19;
    float av = 9.2891180;
    float bv = -0.0062212632;
    float cv = 1.5788029e-6;
    float dv = -1.9359644e-10;
    float ev = 1.1444469e-14;
    float fv = -2.599494e-19;
    float ul = au+(bu*ll)+(cu*pow(ll,2.0))+(du*pow(ll,3.0))+(eu*pow(ll,4.0))+(fu*pow(ll,5.0));
    float vl = av+(bv*ll)+(cv*pow(ll,2.0))+(dv*pow(ll,3.0))+(ev*pow(ll,4.0))+(fv*pow(ll,5.0));
    return vec2(ul,vl);
}


float limbDarkening(in float cosTheta) {
	float ul = 0.85;
	float vl = 0.65;
	float sundisk = cosTheta;
	float limbfilt = 1.0 - ul - vl + ul*cos(asin(sundisk)) + vl*pow(cos(asin(sundisk)),2.0);
    return limbfilt;
}


vec3 limbDarkening3(in float cosTheta) {
	float sundisk = cosTheta;
    vec2 ulvlR = darklimbUlVl(6700.0); //red wavelength in Angstroms
	vec2 ulvlG = darklimbUlVl(5200.0); //green wavelength in Angstroms
    vec2 ulvlB = darklimbUlVl(4500.0); //blue wavelength in Angstroms    
	float limbR = 1.0 - ulvlR.x - ulvlR.y + ulvlR.x*cos(asin(sundisk)) + ulvlR.y*pow(cos(asin(sundisk)),2.0);
    float limbG = 1.0 - ulvlG.x - ulvlG.y + ulvlG.x*cos(asin(sundisk)) + ulvlG.y*pow(cos(asin(sundisk)),2.0);
    float limbB = 1.0 - ulvlB.x - ulvlB.y + ulvlB.x*cos(asin(sundisk)) + ulvlB.y*pow(cos(asin(sundisk)),2.0);
    return vec3(limbR,limbG,limbB);
}


float noise3D(vec3 p) {
	return fract(sin(dot(p ,vec3(12.9898,78.233,128.852))) * 43758.5453)*2.0-1.0;
}


float simplex3D(vec3 p) {
	float f3 = 1.0/3.0;
	float s = (p.x+p.y+p.z)*f3;
	int i = int(floor(p.x+s));
	int j = int(floor(p.y+s));
	int k = int(floor(p.z+s));
	
	float g3 = 1.0/6.0;
	float t = float((i+j+k))*g3;
	float x0 = float(i)-t;
	float y0 = float(j)-t;
	float z0 = float(k)-t;
	x0 = p.x-x0;
	y0 = p.y-y0;
	z0 = p.z-z0;
	
	int i1,j1,k1;
	int i2,j2,k2;
	
	if(x0>=y0) {
		if(y0>=z0){ i1=1; j1=0; k1=0; i2=1; j2=1; k2=0; } // X Y Z order
		else if(x0>=z0){ i1=1; j1=0; k1=0; i2=1; j2=0; k2=1; } // X Z Y order
		else { i1=0; j1=0; k1=1; i2=1; j2=0; k2=1; }  // Z X Z order
	}
	else {
		if(y0<z0) { i1=0; j1=0; k1=1; i2=0; j2=1; k2=1; } // Z Y X order
		else if(x0<z0) { i1=0; j1=1; k1=0; i2=0; j2=1; k2=1; } // Y Z X order
		else { i1=0; j1=1; k1=0; i2=1; j2=1; k2=0; } // Y X Z order
	}
	
	float x1 = x0 - float(i1) + g3; 
	float y1 = y0 - float(j1) + g3;
	float z1 = z0 - float(k1) + g3;
	float x2 = x0 - float(i2) + 2.0*g3; 
	float y2 = y0 - float(j2) + 2.0*g3;
	float z2 = z0 - float(k2) + 2.0*g3;
	float x3 = x0 - 1.0 + 3.0*g3; 
	float y3 = y0 - 1.0 + 3.0*g3;
	float z3 = z0 - 1.0 + 3.0*g3;	
				 
	vec3 ijk0 = vec3(i,j,k);
	vec3 ijk1 = vec3(i+i1,j+j1,k+k1);	
	vec3 ijk2 = vec3(i+i2,j+j2,k+k2);
	vec3 ijk3 = vec3(i+1,j+1,k+1);	
            
	vec3 gr0 = normalize(vec3(noise3D(ijk0),noise3D(ijk0*2.01),noise3D(ijk0*2.02)));
	vec3 gr1 = normalize(vec3(noise3D(ijk1),noise3D(ijk1*2.01),noise3D(ijk1*2.02)));
	vec3 gr2 = normalize(vec3(noise3D(ijk2),noise3D(ijk2*2.01),noise3D(ijk2*2.02)));
	vec3 gr3 = normalize(vec3(noise3D(ijk3),noise3D(ijk3*2.01),noise3D(ijk3*2.02)));
	
	float n0 = 0.0;
	float n1 = 0.0;
	float n2 = 0.0;
	float n3 = 0.0;

	float t0 = 0.5 - x0*x0 - y0*y0 - z0*z0;
	if(t0>=0.0) {
		t0*=t0;
		n0 = t0 * t0 * dot(gr0, vec3(x0, y0, z0));
	}
	float t1 = 0.5 - x1*x1 - y1*y1 - z1*z1;
	if(t1>=0.0) {
		t1*=t1;
		n1 = t1 * t1 * dot(gr1, vec3(x1, y1, z1));
	}
	float t2 = 0.5 - x2*x2 - y2*y2 - z2*z2;
	if(t2>=0.0) {
		t2 *= t2;
		n2 = t2 * t2 * dot(gr2, vec3(x2, y2, z2));
	}
	float t3 = 0.5 - x3*x3 - y3*y3 - z3*z3;
	if(t3>=0.0) {
		t3 *= t3;
		n3 = t3 * t3 * dot(gr3, vec3(x3, y3, z3));
	}
	return 96.0*(n0+n1+n2+n3);
}


float fbm(vec3 p, vec3 n) {
	float f;
    f  = 0.60000*pow((simplex3D( p*0.9 )),3.0)*0.4+0.6; p = p*2.01;
    f += 0.35000*pow(abs(simplex3D( p + pow(vec3(f*n.x,f*n.y,f*n.z),vec3(3.0))*3.0)),0.8 ); p = p*2.02; //from iq
    f += 0.12500*pow(simplex3D( p*0.9 + pow(vec3(f*n.x,f*n.y,f*n.z),vec3(2.0))*3.0),2.0 ); p = p*2.03;
    f += 0.1250*(simplex3D( p*0.6 + pow(vec3(f*n.x,f*n.y,f*n.z),vec3(2.0))*2.0) ); p = p*2.04;
    f += 0.03125*(simplex3D( p + pow(vec3(f*n.x,f*n.y,f*n.z),vec3(3.0))*3.0) );
	return f;
}


#define ONE vec2(1.0, 0.0)
#define EPS vec2(1e-3, 0.0)

const float pi = 3.1415926;


float N(vec2 p) {
   p = mod(p, 4.0);
   return fract(sin(p.x * 41784.0) + sin(p.y * 32424.0));
}


float smN2(vec2 p) {
	vec2 fp = floor(p);
	vec2 pf = smoothstep(0.0, 1.0, fract(p));
	return mix( mix(N(fp), N(fp + ONE), pf.x), 
			   mix(N(fp + ONE.yx), N(fp + ONE.xx), pf.x), pf.y);
}


float fbm2(vec2 p) {
	float f = 0.0, x;
	for(int i = 1; i <= 9; ++i) {
		x = exp2(float(i));
		f += smN2(p * x) / x;
	}
	return f;
}

// Scalar field for the surface undulations.
float spots(vec2 p) {
	p *= 2.5;
	return smN2(p + EPS.xy * 2.0);
}

float field(vec2 p) {
	p *= 1.5;
	return mix(smN2(p * 3.0), smN2(p * 4.0), 0.5 + 0.5 * cos(uTimeSt * 0.02 + p.x*3.8 + 2.531)+ sin(uTimeSt * 0.01 + p.y*4.2 + 1.536));
}

float field2(vec2 p) {
	p *= 6.5;
	return mix(smN2(p * 2.0), smN2(p * 5.0), 0.5 + 0.5 * sin(uTimeSt * 0.6 + p.x*2.0 + 83.123)+ cos(uTimeSt * 0.5 + p.y*4.0 + 2.323));
}


// Vector field extracted from the scalar field.
vec2 flow(vec2 p) {
    vec2 flowout = vec2(0.0,0.0);
	float f0 = field(p);
	float f1 = field(p + EPS.xy);
	float f2 = field(p + EPS.yx);
    flowout += (vec2(f1 - f0, f2 - f0)).yx * vec2(-1, 1) * 0.13;
    float f20 = field2(p);
	float f21 = field2(p + EPS.xy);
	float f22 = field2(p + EPS.yx);
    flowout += (vec2(f21 - f20, f22 - f20)).yx * vec2(-1, 1) * 0.005;
	return flowout;
}


void mainImage( vec2 U ) {
    scale *= (1.0-(900. / 1000.))+0.05;
    exposure *= ((900. / 1000.))*0.5+0.01;
    
    vec2 R = vec2(1000.,1000.), 
         M = vec2(900.,900.)/R;
         U = (( U + U - R ) / R.y)*scale; 

    float l = length(U),
      dotNL = sqrt(1.-l*l), a = uTimeSt;

    vec3 N = vec3( U.x, U.y, sqrt(1.-l*l) );
    
	float sky = max(0.0,dot(U, U));
    
    float dotVL = dot(vec3(U,dotNL), vec3(sin(a),0,cos(a)) );    
    vec3 sunpos = vec3(U,dotNL);

	vec2 uv = vec2(atan(sunpos.z, sunpos.x) / pi * 2.0, acos(sunpos.y) / pi * 2.0 - 1.0);
    
	const int count = 32;
    float csum = 0.0;
    float wsum = 0.0;
    
	for(int i = 0; i < count; ++i) {
		float w = 1.0;
        uv += flow(uv);
		csum += fbm(vec3(uv + vec2(csum*N.x*(1.0-N.z),csum*N.y*(1.0-N.z))*0.01,dotNL+uTimeSt*0.002)*40.0, N) * 0.5 * (0.5 + 0.5 * cos(float(i) / float(count) * 3.1415926 * 4.0 + uTimeSt * 4.0)) * w;
		wsum += w;
	}

    float sunspot = pow(smoothstep(0.00,0.002,spots(uv)),3.0)*0.9+0.1;
    float sunspot1 = pow(smoothstep(0.005,0.01,spots(uv)),3.0)*0.5+0.5;
    
 	vec4 O = vec4(1.0);

    O.rgb = vec3(pow((csum/wsum)*1.0,1.2)*1.6);
    O.rgb *= sunspot1;
    O.rgb *= sunspot;
    O.rgb /= pow(fbm(vec3(uv,dotNL+uTimeSt*0.001)*80.0, N)*1.0,1.6)*0.9+0.1;

    O.rgb *= limbDarkening3(sky)*sunColor*exposure;  

    O *= smoothstep(0.,3./R.y,1.-l); 
    O.rgb = pow(O.rgb,vec3(1.0/gamma));


    vec4 planetaryTex = texture(uTexture0, vTexCoords);
    fFragColor = O.xyz * vec3(planetaryTex.x, planetaryTex.y, planetaryTex.z);;
}



void main() {
    // vec4 planetaryTex = texture(uTexture0, vTexCoords);
    // fFragColor = vec3(planetaryTex.x, planetaryTex.y, planetaryTex.z);
    mainImage(vPosition_vs.xy);
}
