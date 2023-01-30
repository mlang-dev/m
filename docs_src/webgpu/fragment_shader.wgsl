struct Uniforms {
    lightPosition : vec4<f32>,
    eyePosition : vec4<f32>,
};
@binding(1) @group(0) var<uniform> uniforms : Uniforms;

struct Input {
    @location(0) vPosition : vec4<f32>,
    @location(1) vNormal : vec4<f32>,
    @location(2) vColor : vec3<f32>,
};
@fragment
fn main(input: Input) -> @location(0) vec4<f32> {
    let N:vec3<f32> = normalize(input.vNormal.xyz);                
    let L:vec3<f32> = normalize(uniforms.lightPosition.xyz - input.vPosition.xyz);     
    let V:vec3<f32> = normalize(uniforms.eyePosition.xyz - input.vPosition.xyz);          
    let H:vec3<f32> = normalize(L + V);
    let twoSide:i32 = 1;
    let diffuseIntensity:f32 = 0.4;
    var diffuse:f32 = diffuseIntensity * max(dot(N, L), 0.0);
    if(twoSide == 1){
        diffuse = diffuse + diffuseIntensity * max(dot(-N, L), 0.0);
    } 
    var specular:f32;
    let isPhong:i32 = 0;
    let specularIntensity:f32 = 0.4;
    let ambientIntensity:f32 = 0.2;
    let shininess:f32 = 30.0;
    let specularColor:vec3<f32> = vec3<f32>(1.0, 1.0, 1.0);
    if(isPhong == 1){                   
        specular = specularIntensity * pow(max(dot(V, reflect(-L, N)),0.0), shininess);
        if(twoSide == 1) {
            specular = specular + specularIntensity * pow(max(dot(V, reflect(-L, -N)),0.0), shininess);
        }
    } else {
        specular = specularIntensity * pow(max(dot(N, H),0.0), shininess);
        if(twoSide == 1){                     
            specular = specular + specularIntensity * pow(max(dot(-N, H),0.0), shininess);
        }
    }               
    let ambient:f32 = ambientIntensity;
    let finalColor:vec3<f32> = input.vColor * (ambient + diffuse) + specularColor*specular; 
    return vec4<f32>(finalColor, 1.0);
}
