import { wasi } from './wasi';
import { mw, MInstance, RunResult } from './mw';
import { initZoom, Zoom } from './zoom';
import { get_app, signin, signout } from './app';
import { CodeJar } from 'codejar';
import {withLineNumbers} from 'codejar/linenumbers';
import { draw_surface } from './webgpu/main';

var instance:MInstance;
var runResult:RunResult;
var zoom: Zoom | null = null;
var jar:CodeJar | null = null;

function set_image_data(data:any, width:number, height:number)
{
    let canvasDom = document.getElementById(instance.canvas_id) as HTMLCanvasElement;
    if(!canvasDom) return;
    let ctx = canvasDom.getContext('2d');
    if(!ctx) return;
    canvasDom.width = width;
    canvasDom.height = height;
    let imageData = ctx.createImageData(width, height);
    imageData.data.set(data);
    ctx.putImageData(imageData, 0, 0);
}

mw(wasi(), '/mw.wasm', print, true, set_image_data).then(
    result=>{
        instance = result;
        print_version("version", instance.version);
        init_code();
    }
);

function init_code()
{
    const highlight = (editor:HTMLElement)=>{
        //console.log("input: ", editor.textContent?.length);
        //console.log(JSON.stringify(editor.textContent));
        let highlighted_code = instance.highlight_code(editor.textContent);
        //console.log("output: ", highlighted_code.length);
        //console.log(JSON.stringify(highlighted_code));
        editor.innerHTML = highlighted_code;
    };
    let code_editor = document.getElementById('code-editor');
    if(code_editor){
        jar = CodeJar(code_editor, withLineNumbers(highlight));
        // Update code
        var code_text = 
        `print "plot a mandelbrot set"
    
let plot_mandelbrot_set x0:f64 y0:f64 x1:f64 y1:f64 =
    let width = 400, height = 300
    var a:u8[height][width * 4]
    let scalex = (x1-x0)/width, scaley = (y1-y0)/height, max_iter = 510
    var v = 0.0, r = 0.0, g = 0.0, b = 0.0
    for x in 0..width
        for y in 0..height
            let cx = x0 + scalex*x
            let cy = y0 + scaley*y
            var zx = 0.0, zy = 0.0
            var zx2 = 0.0, zy2 = 0.0
            var n = 0
            while n<max_iter && (zx2 + zy2) < 4.0
                zy = 2.0 * zx * zy + cy
                zx = zx2  - zy2 + cx
                zx2 = zx * zx
                zy2 = zy * zy
                n++
            if n < max_iter then
                v = (log(n+1.5-(log2((log(zx2+zy2))/2.0))))/3.4
                if v < 1.0 then 
                    r = v ** 4;g = v ** 2.5;b = v
                else
                    v = v < 2.0 ? 2.0 - v : 0.0
                    r = v;g = v ** 1.5;b = v ** 3.0
            a[y][4*x] = n == max_iter ? 0 : (u8)(r * 255)
            a[y][4*x+1] = n == max_iter ? 0 : (u8)(g * 255)
            a[y][4*x+2] = n == max_iter ? 0 : (u8)(b * 255)
            a[y][4*x+3] = 255

    setImageData a width height

plot_mandelbrot_set (-2.0) (-1.2) 1.0 1.2
    `;
        instance.text_id = code_editor.dataset.text || '';
        jar.updateCode(code_text);
    } else {
        code_editor = document.getElementById('webgpu-code-editor');
        if(code_editor){
            jar = CodeJar(code_editor, withLineNumbers(highlight));

        }
    }
}

function run(code_id:string)
{
    let result_text_id = code_id + "_result";
    let result_graph_id = code_id + "_graph_result";
    clear(result_text_id, result_graph_id);
    if(runResult != undefined && runResult.code_bytes != undefined){
        const free = instance.mw_instance.exports.free as CallableFunction;
        free(runResult.code_bytes);
    }
    var code_string:any;
    if(jar){
        code_string = jar.toString();
    }else{
        code_string = (document.getElementById(code_id) as HTMLInputElement).value;
    }
    instance.canvas_id = result_graph_id;   
    instance.text_id = result_text_id;     
    runResult = instance.run_code(code_string, false);
    print(runResult.start_result);
    if(!zoom) return;
    zoom.init();
    update_back_forward();
}

function update_back_forward()
{
    if(!zoom) return;
    (document.getElementById('backward') as HTMLButtonElement).disabled = zoom.cur_pos <= 0;
    (document.getElementById('forward') as HTMLButtonElement).disabled = zoom.cur_pos >= zoom.history.length - 1;
}
        
function print(text:string){
    let textDom = document.getElementById(instance.text_id);
    if(!textDom){
        console.log("missing textDom: ", instance.text_id);
        return;
    }
    if(text!=undefined && text != null)
        textDom.textContent += text;
}

function clear(result_text_id:string, result_canvas_id:string){
    let textDom = document.getElementById(result_text_id);
    let canvasDom = document.getElementById(result_canvas_id) as HTMLCanvasElement;
    if(textDom)
        textDom.textContent = '';
    if(!canvasDom) return;
    const ctx = canvasDom.getContext('2d');
    if(!ctx) return;
    ctx.clearRect(0, 0, canvasDom.width, canvasDom.height);
}

function print_version(version_id:string, version:string) {
    let versionDom = document.getElementById(version_id);
    if(!versionDom) return;
    versionDom.textContent = version;
}

window.onload = function() {
    var app = get_app(onUserChange);
    var user_link = document.getElementById("user");
    if (!user_link) return;
    user_link.onclick = () => {
        if(app.auth.currentUser){
            window.location.href = '/user/profile.html';
            //signout(app);
        }else{
            signin(app);
        }
    };

    draw_surface();
    

    var runs = document.querySelectorAll("button[data-run]");
    runs.forEach((runBtn)=>{
        let runDom = runBtn as HTMLElement;
        runDom.addEventListener("click", 
        event => {
            run(runDom.dataset.run || '');
        });
    });
    

    var backward = document.getElementById("backward") as HTMLButtonElement;
    if(backward){
        backward.addEventListener('click', 
            function ()
            {
                if(!zoom) return;
                zoom.backward();
            });
    }
    var forward = document.getElementById("forward") as HTMLButtonElement;
    if(forward){
        forward.addEventListener('click', 
            function ()
            {
                if(!zoom) return;
                zoom.forward();
            }
        );
    }
    function onZoom(x0:number, y0:number, x1:number, y1:number)
    {
        const plot_mandelbrot_set = runResult.code_instance.exports.plot_mandelbrot_set as CallableFunction;
        plot_mandelbrot_set(x0, y0, x1, y1);
        update_back_forward();
    }

    let canvas_container = document.getElementById('canvas_container');
    if(canvas_container)
        zoom = initZoom(canvas_container, onZoom, -2.0, -1.2, 1.0, 1.2, 400, 300);
    if(backward && forward)
        update_back_forward();

    //profile page
    function onUserChange(userDisplayName:string|null, userPhotoURL:string|null){
        if(!user_link) return;
        let signout_link = document.getElementById('signout');
        if(userDisplayName){
            if(userPhotoURL){
                user_link.innerHTML = `<img src="${userPhotoURL}" style="vertical-align: middle;width:35px;height:35px;border-radius: 50%;">`;
            }else{
                user_link.innerHTML = userDisplayName;
            }
            user_link.style.borderStyle = "none";
            let avatar = document.getElementById('avatar');
            if(avatar && app.auth.currentUser && app.auth.currentUser.photoURL){
                (avatar as HTMLImageElement).src = app.auth.currentUser.photoURL;
            }
            let displayname = document.getElementById('displayname');
            if(displayname){
                displayname.textContent = userDisplayName;
            }
            if(signout_link){
                signout_link.addEventListener('click', ()=>signout(app));
            }
        
        }else{
            user_link.innerHTML = "Sign in with Github";
            user_link.style.borderStyle = "solid";
            user_link.style.borderRadius = "5px";
            if(signout_link){
                window.location.href = '/';
            }
        }
    }
};
