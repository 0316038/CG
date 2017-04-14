#version 150 compatibility

layout(triangles) in;
layout(line_strip, max_vertices=200) out;
uniform float length;
uniform int number;
uniform float gravity1;
in Vertex{
    vec3 normal;
}vertex[];

out float part;

void main(){
	vec4 temp;	
	vec3 gravity=vec3(0.0f,gravity1,0.0f);
	vec3 newnormal;
    for(int i = 0; i < gl_in.length(); i++){
		newnormal=normalize(vertex[i].normal+gravity);
		temp=gl_in[i].gl_Position;
        for(int j=0;j<number;j++){
			//start point
			part=j;
			gl_Position = gl_ProjectionMatrix * temp;
			EmitVertex();

			//end point
			gl_Position = gl_ProjectionMatrix * (temp + vec4(newnormal, 0.0f) * length);
			temp=temp + (vec4(newnormal, 0.0f) * length);
			newnormal=normalize(newnormal+gravity);
			EmitVertex();

			EndPrimitive();
		}
    }
}
