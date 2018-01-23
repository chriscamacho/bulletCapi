#version 330 core 

uniform		sampler2D	u_texture;

uniform		vec3		u_lightDir;
uniform		vec3		u_viewDir;

in			vec3		v_Position;
in			vec3		v_Normal;
in			vec2		v_frag_uv;

out			vec4		frag_color;

void main() {
	
	vec4 baseColour = texture2D(u_texture, v_frag_uv);
	float nDOTl = clamp(dot(v_Normal, u_lightDir),0,1);

	vec3 reflect = (2.0 * v_Normal * nDOTl) - u_lightDir;
	float rDOTv = max(0.1, dot(reflect,u_viewDir));
	
// these three vec4's could be material uniforms.	
	vec4 ambi = vec4(.4,.4,.4,0) * baseColour;	
	vec4 diffu = vec4(.5,.5,.5,0) * nDOTl * baseColour;;
		
// spec strength (could be a material uniform)	
    vec4 specu = vec4(.4,.4,.4,0) * pow(rDOTv, 8.0);

	//gl_FragColor = (ambi + diffu + specu);
	frag_color = ambi + diffu + specu;
}
