#version 150 compatibility

in float part;
uniform int number;

void main(){
	float a=part/number; 
	vec4 first=vec4(0.0f,0.0f,0.0f,1.0f);
	vec4 final=vec4(0.3f,0.3f,0.7f,0.0f);
	vec4 interpolation=mix(first,final,a);
    gl_FragColor = interpolation;
}
