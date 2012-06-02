
jQuery(document).ready( function() {

    var left = 0, img = null, canvas = null, ctx = null, skip = 5, size = 40;

    if( jQuery.browser.msie ) {
        jQuery("#html5canvas").css( "display", "none" );
    }

    function getPosition( obj ){
        var topValue= 0, leftValue= 0;
        while( obj ) {
            leftValue += obj.offsetLeft;
            topValue += obj.offsetTop;
            obj = obj.offsetParent;
        }
        return( { x : leftValue, y : topValue } );
    }

    var pos = getPosition( document.getElementById( "region-branding" ));
    var top = pos.y + ( size / 2 );
    var stop = pos.x + 640;

    function draw() {
        ctx.clearRect( left - skip, top, size, size );
        ctx.drawImage( img, left, top, size, size );
        if( left >= stop ) clearInterval( id );
        left += skip;
    }

    var id = setInterval( function() {
        if( !canvas ) {
            canvas = document.getElementById("html5canvas");
            canvas.width = window.innerWidth;
            canvas.height = window.innerHeight;
            ctx = canvas.getContext( "2d" );
        }
        if( !img ) {
            img = new Image();
            img.src = "sites/default/themes/msdc/images/html5logo.png";
            img.onload = draw;
        }
        else {
            draw();
        }
    }, 10 );
});

