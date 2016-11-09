
const APPID = "PUDZAHydro";
const KEY = "BwBRekvWcYFkRGJ";
const SECRET = "sDJCEcobv9l66aoB0LkaZl4HE";

const ALIAS = "htmlgear";

var microgear = Microgear.create({
    key: KEY,
    secret: SECRET,
    alias : ALIAS
});

function printMsg(topic,msg) {
    console.log(topic+'-'+msg);
    var now = new Date();
    var d = now.getDay();
    var m = now.getMonth();
    m += 1;  
    var y = now.getFullYear();
    var h = now.getHours();
    var i = now.getMinutes();
    var s = now.getSeconds();
    var datetime = d + "/" + m + "/" + y + " " + h + ":" + i + ":" + s;
    document.getElementById("data").innerHTML = '&nbsp;<i class="fa fa-bell-o"></i> '+ datetime + ' # ' +topic+' <i class="fa fa-ellipsis-h"></i> ' + msg;
}


microgear.on('message',function(topic,msg) {
    printMsg(topic,msg);

    if (topic == "/PUDZAHydro/uno/amptemp") {
        $('#hmi_atemp').text(msg+' C');        
    }
    else if (topic == "/PUDZAHydro/uno/amphum") {
        $('#hmi_ahum').text(msg+' %');        
    }
    else if (topic == "/PUDZAHydro/uno/wtrtemp") {
        $('#hmi_nutemp').text(msg+' C');        
    }
    else if (topic == "/PUDZAHydro/uno/soilhum") {
        $('#hmi_msoil').text(msg+' %');        
    }
    else if (topic == "/PUDZAHydro/uno/rain") {
        if (parseFloat(msg) < 15) {
            $("#hmi_rainimg").attr("src","svg/rain.png");            
            $('#hmi_rain').text(msg+' %');        
        }
        else {
            $("#hmi_rainimg").attr("src","");                        
            $('#hmi_rain').text(' ');        
        }
    }
    else if (topic == "/PUDZAHydro/uno/flow") {
        $('#hmi_flow').text(msg+' l/m');        
    }
    else if (topic == "/PUDZAHydro/uno/ec") {
        $('#hmi_ec').text(msg+' mS/cm');
    }
    else if (topic == "/PUDZAHydro/nodemcu") {
        var vals = msg.split(",");
//        console.log(vals[2]);
        $('#hmi_lux').text(vals[0]+' lux');
        $('#hmi_traytemp').text(vals[1]+" C");
        
        light = parseFloat(vals[0]);
        //light = 56000;
        if (light  < 1) {
            $("#hmi_light").attr("src","svg/light1.png");
        }
        else if (light < 5000) {
            $("#hmi_light").attr("src","svg/light2.png");            
        }
        else if (light < 30000) {
            $("#hmi_light").attr("src","svg/light3.png");            
        }
        else if (light >= 30000) {
            $("#hmi_light").attr("src","svg/light4.png");            
        }


        if (vals[2]=='1') {
            $("#hmi_pump").attr("src","svg/pumpon.svg");
//            console.log("pump on");
        }
        else {
            $("#hmi_pump").attr("src","svg/pumpoff.svg");            
//            console.log("pump off");
        }

    }
    else {
        
//            document.getElementById("data").innerHTML = now.getDay() + ":[" + topic+ "] " + msg;
    }
});

microgear.on('connected', function() {
    printMsg('Init',"Connected to NETPIE...");
    $("#s_htmlgear").removeClass("btn-default");
    $("#s_htmlgear").addClass("btn-warning");
    microgear.setAlias(ALIAS);
    microgear.subscribe("/uno/amptemp");
    microgear.subscribe("/uno/amphum");
    microgear.subscribe("/uno/wtrtemp");
    microgear.subscribe("/uno/flow");
    microgear.subscribe("/uno/soilhum");
    microgear.subscribe("/uno/rain");
    microgear.subscribe("/uno/ec");
    microgear.subscribe("/nodemcu");
    microgear.subscribe("/eccal");
    microgear.subscribe("/eccalmsg");
    microgear.subscribe("/mist");
    microgear.subscribe("/sptemp");
    microgear.subscribe("/reporter");
});

microgear.on('present', function(event) {
    printMsg(event.alias,event.type);
    if (event.alias == "reporter") {
        if (event.type == "aliased") {
            $("#s_reporter").removeClass("btn-default");
            $("#s_reporter").addClass("btn-warning");
        }
    }
    console.log(event);
});

microgear.on('absent', function(event) {
    printMsg(event.alias,event.type);
    if (event.alias == "reporter") {
        if (event.type == "offline") {
            $("#s_reporter").removeClass("btn-warning");
            $("#s_reporter").addClass("btn-default");
        }
    }
    if (event.alias == "raspiPython") {
        if (event.type == "offline") {
            $("#s_arduino").removeClass("btn-warning");
            $("#s_arduino").addClass("btn-default");
        }
    }
    if (event.alias == "nodemcu") {
        if (event.type == "offline") {
            $("#s_nodemcu").removeClass("btn-warning");
            $("#s_nodemcu").addClass("btn-default");
        }
    }

    console.log(event);
});

microgear.resettoken(function(err) {
    microgear.connect(APPID);
});

$(function () {

    $("#hmi_pump").click(function () {
        $(this).w2overlay($('#popup1 [rel=body]').html(), { css: { width: '600px', padding: '10px' } });
        console.log("pump");
    });



});
