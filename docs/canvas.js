
function initDraw(canvas) {
    function setMousePosition(e) {
        var rect = canvas.getBoundingClientRect();
        mouse.x = e.clientX - rect.left;
        mouse.y = e.clientY - rect.top;
    };

    var mouse = {
        x: 0,
        y: 0,
        startX: 0,
        startY: 0
    };
    var is_selected = false;
    var ctx = canvas.getContext("2d");

    canvas.onmousemove = function (e) {
        setMousePosition(e);
        if (is_selected) {
            // element.style.width = Math.abs(mouse.x - mouse.startX) + 'px';
            // element.style.height = Math.abs(mouse.y - mouse.startY) + 'px';
            // element.style.left = (mouse.x - mouse.startX < 0) ? mouse.x + 'px' : mouse.startX + 'px';
            // element.style.top = (mouse.y - mouse.startY < 0) ? mouse.y + 'px' : mouse.startY + 'px';
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.beginPath();
            ctx.moveTo(mouse.startX, mouse.startY);
            ctx.strokeStyle = "red";
            ctx.lineWidth = 0.5;
            ctx.lineTo(mouse.x, mouse.y);
            ctx.stroke();
            ctx.closePath();
        }
    }

    canvas.onclick = function (e) {
        if (is_selected) {
            is_selected = false;
            canvas.style.cursor = "default";
            console.log("finsihed.", mouse.x, mouse.y);
        } else {
            console.log("begun.", mouse.x, mouse.y);
            mouse.startX = mouse.x;
            mouse.startY = mouse.y;
            is_selected = true;
            // element.className = 'rectangle'
            // element.style.left = mouse.x + 'px';
            // element.style.top = mouse.y + 'px';
            // canvas.appendChild(element)
            canvas.style.cursor = "crosshair";
        }
    }
}