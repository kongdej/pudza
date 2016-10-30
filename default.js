
    function customValue(val) {
        return val;        
    }

    var g_ec = new JustGage({
        id: "gaugeEC",
        value: 0,
        value:0,
        min: 0,
        max: 5,
        relativeGaugeSize: true,
        gaugeWidthScale: 1,
        decimals:2,
        title: "Fertilizer EC",
        label:"mS/cm",
        titlePosition: "below",
        titleFontSize: "5px",
        titleFontFamily: "Arial"
      });

    var g_flow = new JustGage({
        id: "gaugeFlow",
        value: 0,
        min: 0,
        max: 15,
        counter: true,
        relativeGaugeSize: true,
        gaugeWidthScale: 1,
        decimals:true,
        title: "Water Flow",
        label:"L/min",
        titlePosition: "below"
      });
    var g_light = new JustGage({
        id: "gaugeLight",
        value: 0,
        value:0,
        min: 0,
        max: 65000,
        relativeGaugeSize: true,
        counter: true,
        formatNumber:true,
        gaugeWidthScale: 1,
        title: "Ambient Light",
        label:"Lux",
        titlePosition: "below",
        titleFontSize: "5px",
        titleFontFamily: "Arial"

      });

    var g_atemp = new JustGage({
        id: "gaugeATemp",
        value: 0,
        value:0,
        min: 0,
        max: 100,
        relativeGaugeSize: true,
        decimals:true,
        gaugeWidthScale: 1,
        title: "Ambient Temperature",
        label:"DegC",
        titlePosition: "below"
      });

    var g_gtemp = new JustGage({
        id: "gaugeGTemp",
        value: 0,
        value:0,
        min: 0,
        max: 100,
        relativeGaugeSize: true,
        gaugeWidthScale: 1,
        decimals:true,
        title: "Gutter Temperaure",
        label:"DegC",
        titlePosition: "below"

      });

    var g_abtemp = new JustGage({
        id: "gaugeABTemp",
        value: 0,
        value:0,
        min: 0,
        max: 100,
        relativeGaugeSize: true,
        decimals:true,
        gaugeWidthScale: 1,
        title: "Fertilizer Temperature",
        label:"DegC",
        titlePosition: "below"        
      });

    var g_hum = new JustGage({
        id: "gaugeHum",
        value: 0,
        value:0,
        min: 0,
        max: 100,
        relativeGaugeSize: true,
        donut:true,
        gaugeWidthScale: 1,
        decimals:true,
        title: "Ambient Humidity",
        label:"%",
        titlePosition: "below"        
      });

    var g_rain = new JustGage({
        id: "gaugeRain",
        value: 0,
        value:0,
        min: 0,
        max: 100,
        relativeGaugeSize: true,
        donut:true,
        decimals:true,
        gaugeWidthScale: 1,
        title: "Rain",
        label:"%",
        titlePosition: "below"        
      });

    var g_soil = new JustGage({
        id: "gaugeSoil",
        value: 0,
        value:0,
        min: 0,
        max: 100,
        relativeGaugeSize: true,
        donut:true,
        gaugeWidthScale: 1,
        decimals:true,
        title: "Soil Humidity",
        label:"%",
        titlePosition: "below"        
      });

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
            g_atemp.refresh(msg);
        }
        else if (topic == "/PUDZAHydro/uno/amphum") {
            g_hum.refresh(msg);
        }
        else if (topic == "/PUDZAHydro/uno/wtrtemp") {
            g_abtemp.refresh(msg);
        }
        else if (topic == "/PUDZAHydro/uno/soilhum") {
            g_soil.refresh(msg);
        }
        else if (topic == "/PUDZAHydro/uno/rain") {
            g_rain.refresh(msg);
        }
        else if (topic == "/PUDZAHydro/uno/flow") {
            g_flow.refresh(msg);
        }
        else if (topic == "/PUDZAHydro/uno/ec") {
            g_ec.refresh(msg);
        }
        else if (topic == "/PUDZAHydro/nodemcu") {
            var vals = msg.split(",");
            g_light.refresh(vals[0]);
            g_gtemp.refresh(vals[1]);
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
        microgear.subscribe("/eccal");
        microgear.subscribe("/uno/amptemp");
        microgear.subscribe("/uno/amphum");
        microgear.subscribe("/uno/wtrtemp");
        microgear.subscribe("/uno/flow");
        microgear.subscribe("/uno/soilhum");
        microgear.subscribe("/uno/rain");
        microgear.subscribe("/uno/ec");
        microgear.subscribe("/nodemcu");
        microgear.subscribe("/mist");
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
        console.log(event);
    });

    microgear.resettoken(function(err) {
        microgear.connect(APPID);
    });

    $("#cb").click(function () {
        console.log($('#ec_cal').val());
        microgear.publish ("/eccal", $('#ec_cal').val());
    });

    $("#miston").click(function () {
        console.log("on");
        microgear.publish ("/mist", "ON");
    });

    $("#mistoff").click(function () {
        console.log("off");
        microgear.publish ("/mist", "OFF");
    });
