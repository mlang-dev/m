import { vec3 } from 'gl-matrix';

export const Seashell = (u:number, v:number, center:vec3=[0,0,0]) => {
    let x = 2*(-1+Math.exp(u/(6*Math.PI)))*Math.sin(u)*Math.pow(Math.cos(v/2),2);
    let y = 1 - Math.exp(u/(3*Math.PI))-Math.sin(v) + Math.exp(u/(6*Math.PI))*Math.sin(v);
    let z = 2*(1-Math.exp(u/(6*Math.PI)))*Math.cos(u)*Math.pow(Math.cos(v/2),2);
    return vec3.fromValues(x+center[0], y+center[1], z+center[2]);
};

export const Wellenkugel = (u:number, v:number, center:vec3=[0,0,0]) => {
    let x = u*Math.cos(Math.cos(u))*Math.sin(v);        
    let y = u*Math.sin(Math.cos(u));
    let z = u*Math.cos(Math.cos(u))*Math.cos(v);       
    return vec3.fromValues(x+center[0], y+center[1], z+center[2]);
};

export const KleinBottle = (u:number, v:number, center:vec3=[0,0,0]) => {
    let x = 2/15*(3+5*Math.cos(u)*Math.sin(u))*Math.sin(v);    

    let y = -1/15*Math.sin(u)*(3*Math.cos(v)-3*Math.pow(Math.cos(u),2)*Math.cos(v)-
            48*Math.pow(Math.cos(u),4)*Math.cos(v)+48*Math.pow(Math.cos(u),6)*Math.cos(v)-
            60*Math.sin(u)+5*Math.cos(u)*Math.cos(v)*Math.sin(u)- 
            5*Math.pow(Math.cos(u),3)*Math.cos(v)*Math.sin(u)-
            80*Math.pow(Math.cos(u),5)*Math.cos(v)*Math.sin(u)+
            80*Math.pow(Math.cos(u),7)*Math.cos(v)*Math.sin(u));

    let z = -2/15*Math.cos(u)*(3*Math.cos(v)-30*Math.sin(u)+
            90*Math.pow(Math.cos(u),4)*Math.sin(u)-60*Math.pow(Math.cos(u),6)*Math.sin(u)+
            5*Math.cos(u)*Math.cos(v)*Math.sin(u));
   
    return vec3.fromValues(x+center[0], y+center[1], z+center[2]);
};

export const Peaks = (x:number, z:number, center:vec3 = [0,0,0]) => {
    let y = 3*(1-z)*(1-z)*Math.exp(-(z*z)-(x+1)*(x+1))-10*(z/5-z*z*z-x*x*x*x*x)*Math.exp(-z*z-x*x)-1/3*Math.exp(-(z+1)*(z+1)-x*x);
    return vec3.fromValues(x+center[0], y+center[1], z+center[2]);
};

export const Sinc = (x:number, z:number, center:vec3 = [0,0,0]) => {
    let r = Math.sqrt(x*x + z*z) + 0.00001;
    let y = Math.sin(r)/r;
    return vec3.fromValues(x+center[0], y+center[1], z+center[2]);
};

export const TorusPosition = (R:number, r:number, u:number, v:number, center:vec3 = [0,0,0]) => {
    let snu = Math.sin(u*Math.PI/180);
    let cnu = Math.cos(u*Math.PI/180);
    let snv = Math.sin(v*Math.PI/180);
    let cnv = Math.cos(v*Math.PI/180);
    return vec3.fromValues((R+r*cnv)*cnu + center[0], r*snv + center[1], -(R+r*cnv)*snu + center[2]);        
};

export const ConePosition = (radius:number, theta:number, y:number, center:vec3 = [0,0,0]) => {
    let sn = Math.sin(theta*Math.PI/180);
    let cn = Math.cos(theta*Math.PI/180);
    return vec3.fromValues(radius*cn + center[0], y + center[1], -radius*sn + center[2]);        
};

export const CylinderPosition = (radius:number, theta:number, y:number, center:vec3 = [0,0,0]) => {
    let sn = Math.sin(theta*Math.PI/180);
    let cn = Math.cos(theta*Math.PI/180);
    return vec3.fromValues(radius*cn + center[0], y+center[1], -radius*sn + center[2]);        
};

export const SpherePosition = (radius:number, theta:number, phi:number, center:vec3 = [0,0,0]) => {
    let snt = Math.sin(theta*Math.PI/180);
    let cnt = Math.cos(theta*Math.PI/180);
    let snp = Math.sin(phi*Math.PI/180);
    let cnp = Math.cos(phi*Math.PI/180);
    return vec3.fromValues(radius*snt*cnp + center[0], radius*cnt + center[1], -radius*snt*snp + center[2]);     
};