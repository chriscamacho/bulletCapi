#version 330 core

in		vec3	vertex_attrib;
in		vec2	uv_attrib;
in		vec3	norm_attrib;

uniform	mat4	mvp_uniform;
uniform mat4	mv_uniform;

out		vec2	v_frag_uv;
out		vec3	v_Position;
out		vec3	v_Normal;

void main(void) {


	v_frag_uv = uv_attrib;

    v_Position = vec3(mv_uniform * vec4(vertex_attrib,0));
    v_Normal = vec3(mv_uniform * vec4(norm_attrib, 0.0));
    gl_Position = mvp_uniform * vec4(vertex_attrib,1);

}
