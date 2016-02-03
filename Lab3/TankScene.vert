
attribute vec4 position; 
attribute vec4 color;
attribute vec4 normal; 

varying vec4 pcolor;

uniform mat4 local2clip;
uniform mat4 local2eye;
uniform mat4 normal_matrix;
uniform mat4 world2eye; 

uniform vec4 light_ambient;
uniform vec4 light_diffuse;
uniform vec4 light_specular;
uniform vec4 light_pos;

uniform vec4 mat_ambient;
uniform vec4 mat_diffuse;
uniform vec4 mat_specular;
uniform float mat_shine; 


void main(){

       gl_Position = local2clip * position;
       
       vec4 ambient = light_ambient * mat_ambient; 
       vec3 N =     normalize(vec3(normal_matrix * normal)); 
       vec4 Lpos =  world2eye * light_pos; 
       vec4 Vpos =  local2eye * position; 
       vec3 L = normalize(vec3(Lpos - Vpos)); 

       float NdotL; 
       if (dot(N,L) <0.0) NdotL = 0.0; 
       else NdotL = dot(N, L); 

       vec4 diffuse = light_diffuse * mat_diffuse * NdotL; 

       vec3 R = normalize(reflect(-L, N)); 
       vec3 V = normalize(vec3(-Vpos)); 


       float RdotV; 
       RdotV = dot(R, V); 

       if (NdotL == 0.0) RdotV = 0.0; 
       if (RdotV <0.0) RdotV = 0.0; 

       vec4 specular = light_specular * mat_specular * pow(RdotV,mat_shine); 

	   pcolor = ambient +diffuse+specular;
    
}