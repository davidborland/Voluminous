varying vec3 objectPosition, normal, lightDir, eyeVec;

void main() {
	normal = gl_NormalMatrix * gl_Normal;
	
	vec3 position = gl_ModelViewMatrix * gl_Vertex;
	
	lightDir = gl_LightSource[0].position.xyz - position;
	eyeVec = -position;
	
	objectPosition = gl_Vertex;
	
	gl_Position = ftransform();
}