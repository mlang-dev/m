struct Uniforms {
    viewProjectionMatrix : mat4x4<f32>,
    modelMatrix : mat4x4<f32>,      
    normalMatrix : mat4x4<f32>,            
};
@binding(0) @group(0) var<uniform> uniforms : Uniforms;

struct Input {
    @location(0) position : vec4<f32>,
    @location(1) normal : vec4<f32>,
    @location(2) color : vec3<f32>,
};

struct Output {
    @builtin(position) Position : vec4<f32>,
    @location(0) vPosition : vec4<f32>,
    @location(1) vNormal : vec4<f32>,
    @location(2) vColor : vec3<f32>,
};

@vertex
fn main(input: Input) -> Output {        
    var output: Output;        
    let mPosition:vec4<f32> = uniforms.modelMatrix * input.position; 
    output.vPosition = mPosition;                  
    output.vNormal =  uniforms.normalMatrix*input.normal;
    output.Position = uniforms.viewProjectionMatrix * mPosition;     
    output.vColor = input.color;          
    return output;
}
