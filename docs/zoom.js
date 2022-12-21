function initZoom(canvas, onZoom, x0, y0, x1, y1, width, height) {
    function setMousePosition(e) {  
        mouse.x = e.clientX;
        mouse.y = e.clientY;
    };

    var mouse = {
        x: 0,
        y: 0,
        startX: 0,
        startY: 0
    };
    var element = null;
    var _width = width;
    var _height = height;
    var _x0 = x0;
    var _y0 = y0;
    var _x1 = x1;
    var _y1 = y1;
    var _is_init = false;
    var zoom = {
        history: [],
        cur_pos: -1,
        init: init,
        backward: backward,
        forward: forward
    };

    canvas.onmousemove = onZoomMove;
    // canvas.ontouchmove = onZoomMove;
    // canvas.ontouchstart = onZoomStart;
    // canvas.ontouchend = onZoomEnd;

    function init()
    {
        _is_init = true;
        zoom.history = [];
        zoom.cur_pos = -1;
        save_current();
    }

    function save_current(){
        items_to_remove = zoom.history.length - 1 - zoom.cur_pos;
        for (let i = 0; i < items_to_remove; i++) {
            zoom.history.pop();
        }
        zoom.history.push({x0: _x0, y0: _y0, x1: _x1, y1: _y1});
        zoom.cur_pos += 1;
    }

    function backward(){
        if(zoom.cur_pos > 0){
            zoom.cur_pos -= 1;
            refresh();
        }
    }

    function forward(){
        if(zoom.cur_pos < zoom.history.length - 1){
            zoom.cur_pos += 1;
            refresh();
        }
    }

    function refresh()
    {
        let cur_zoom = zoom.history[zoom.cur_pos];
        _x0 = cur_zoom.x0;
        _y0 = cur_zoom.y0;
        _x1 = cur_zoom.x1;
        _y1 = cur_zoom.y1;
        onZoom(_x0, _y0, _x1, _y1);
    }

    function onZoomStart(e)
    {
        mouse.startX = mouse.x;
        mouse.startY = mouse.y;
        element = document.createElement('div');
        element.className = 'rectangle'
        element.style.left = mouse.x + 'px';
        element.style.top = mouse.y + 'px';
        canvas.appendChild(element)
        canvas.style.cursor = "crosshair";
    }

    function onZoomMove(e){
        setMousePosition(e);
        if (element !== null) {
            element.style.width = Math.abs(mouse.x - mouse.startX) + 'px';
            element.style.height = Math.abs(mouse.y - mouse.startY) + 'px';
            let left = mouse.x < mouse.startX ? mouse.x : mouse.startX;
            let top = mouse.y < mouse.startY ? mouse.y : mouse.startY;
            left += window.pageXOffset;
            top += window.pageYOffset;
            element.style.left = left + 'px';
            element.style.top = top + 'px';
        }
    }

    function onZoomCancel(e)
    {
        canvas.removeChild(element);
        element = null;
    }

    function onZoomEnd(e)
    {
        canvas.removeChild(element);
        element = null;
        canvas.style.cursor = "default";
        var rect = canvas.getBoundingClientRect();
        var x0 = Math.min(mouse.startX, mouse.x) - rect.left;
        var y0 = Math.min(mouse.startY, mouse.y) - rect.top;
        var x1 = Math.max(mouse.startX, mouse.x) - rect.left;
        var y1 = Math.max(mouse.startY, mouse.y) - rect.top;
        var scalex = (_x1 - _x0) / _width; 
        var scaley = (_y1 - _y0) / _height; 
        let _cx0 = _x0;
        let _cy0 = _y0;
        _x0 = _cx0 + scalex * x0;
        _y0 = _cy0 + scaley * y0;
        _x1 = _cx0 + scalex * x1;
        _y1 = _cy0 + scaley * y1; 
        save_current();
        onZoom(_x0, _y0, _x1, _y1); 
    }

    canvas.onclick = function (e) {
        if(!_is_init) return;
        if(e.which == 3){
            onZoomCancel(e);
            return;
        }
        if (element !== null) {
            onZoomEnd(e);
        } else {
            onZoomStart(e);
        }
    }
    return zoom;
}