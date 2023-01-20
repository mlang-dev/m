import { wasi } from './wasi';
import { mw, MInstance, RunResult } from './mw';
import { initZoom, Zoom } from './zoom';

window.onload = function() {
    
    var instance:MInstance;
    var runResult:RunResult;
    var codeDom = document.getElementById("code") as HTMLInputElement;
    var consoleDom = document.getElementById("console") as HTMLInputElement;
    var canvasDom = document.getElementById('canvas') as HTMLCanvasElement;
    var versionDom = document.getElementById("version");
    var runDom = document.getElementById("run") as HTMLButtonElement;

    codeDom.addEventListener('keydown', function(e) {
    if (e.key == 'Tab') {
        e.preventDefault();
        let start = codeDom.selectionStart || 0;
        let end = codeDom.selectionEnd || 0;

        // set textarea value to: text before caret + tab + text after caret
        codeDom.value = codeDom.value.substring(0, start) +
        "\t" + codeDom.value.substring(end);

        // put caret at right position again
        codeDom.selectionStart =
            codeDom.selectionEnd = start + 1;
    }
    });
    var backward = document.getElementById("backward") as HTMLButtonElement;
    var forward = document.getElementById("forward") as HTMLButtonElement;
    var zoom: Zoom | null = null;

    backward.addEventListener('click', 
        function ()
        {
            if(!zoom) return;
            zoom.backward();
        });

    forward.addEventListener('click', 
        function ()
        {
            if(!zoom) return;
            zoom.forward();
        }
    );
    function update_back_forward()
    {
        if(!zoom) return;
        backward.disabled = zoom.cur_pos <= 0;
        forward.disabled = zoom.cur_pos >= zoom.history.length - 1;
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
    update_back_forward();
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
    codeDom.value = code_text;
    function set_image_data(data:any, width:number, height:number)
    {
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
            print_version(instance.version);
        }
    );

    runDom.addEventListener("click", 
        function (event) {
            clear();
            if(runResult != undefined && runResult.code_bytes != undefined){
                const free = instance.mw_instance.exports.free as CallableFunction;
                free(runResult.code_bytes);
            }
            let code = codeDom.value;
            runResult = instance.run_code(code, false);
            print(runResult.start_result);
            if(!zoom) return;
            zoom.init();
            update_back_forward();
        }
    );

    function print(text:string){
        if(text == null || text == undefined){
            return;
        }
        consoleDom.textContent += text;
        // const ctx = document.getElementById('canvas').getContext('2d');
        // ctx.font = '12px serif';
        // ctx.fillStyle = 'orange';
        // ctx.fillText(text, 0, 30);
    }

    function clear(){
        consoleDom.textContent = '';
        const ctx = canvasDom.getContext('2d');
        if(!ctx) return;
        ctx.clearRect(0, 0, canvasDom.width, canvasDom.height);
    }

    function print_version(version:string) {
        if(!versionDom) return;
        versionDom.textContent = version;
    }
};