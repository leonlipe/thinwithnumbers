

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function isEmpty(str) {
    return (!str || 0 === str.length);
}

function locationSuccess(pos) {
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
  pos.coords.latitude + '&lon=' + pos.coords.longitude;
  // Send request to OpenWeatherMap
  xhrRequest(url, 'GET', 
    function(responseText) {

      try {
        console.log("Entra a datos...")
          // responseText contains a JSON object with weather info

          // Temperature in Kelvin requires adjustment
          //console.log("Temp:"+json.main.temp);
          //console.log('Temperature is ' + temperature);

          // Conditions
          //console.log('Conditions are ' + conditions);
          //console.log('Code are ' + conditions_code);
          //console.log('Icon are ' + conditions_icon);
          //console.log('Humidity ' +  json.main.humidity);
          // console.log('Wind ' +  (json.wind.speed / 0.62137));

        var json = JSON.parse(responseText);
        var temperature = Math.round(json.main.temp - 273.15);
        var temperatureF = Math.round( ((json.main.temp - 273.15) * 1.8) +32 );
        var conditions = json.weather[0].main;     
        var conditions_code = json.weather[0].id;   
        var conditions_icon = json.weather[0].icon;   
        var wind_speed = Math.floor((json.wind.speed / 0.62137) * 100) / 100  ;
        var sunrise = new Date(json.sys.sunrise*1000);
        var sunset = new Date(json.sys.sunset*1000);
        var now = new Date();
        var sunrise12h = (sunrise.getHours()>12?sunrise.getHours()-12:sunrise.getHours() )+":"+sunrise.getMinutes()+ (sunrise.getHours()>12?"pm":"am");
        var sunrise24h = sunrise.getHours()+":"+sunrise.getMinutes();
        var sunset12h = (sunset.getHours()>12? sunset.getHours()-12:sunset.getHours() )+":"+sunset.getMinutes() +(sunset.getHours()>12?"pm":"am");
        var sunset24h = sunset.getHours()+":"+sunset.getMinutes();
        var humidity = json.main.humidity;
        var wind_speed = json.wind.speed.toString();
         //console.log("Now:"+(now.toString()));
         //console.log("Sunrise:"+(sunrise.toString()));
         //console.log("Sunset:"+(sunset.toString()));
         //console.log("Date res:"+(now.getTime() > sunset.getTime()));

          // Assemble dictionary using our keys


      		var dictionary = {
      		  'KEY_TEMPERATURE': temperature,
            'KEY_TEMPERATUREF': temperatureF,
      		  'KEY_CONDITIONS': conditions,
            'KEY_HUMIDITY': humidity,
            'KEY_WIND': wind_speed,
            'KEY_SUNRISE': sunrise12h+"|"+sunrise24h,
            'KEY_SUNSET': sunset12h+"|"+sunset24h,
            'KEY_CONDITIONS_ID': conditions_code,
            'KEY_CONDITIONS_ICON': conditions_icon
      		};

          console.log("Datos del server de clima");
          if (isNaN(temperature) || isNaN(temperatureF) || isEmpty(conditions)
            || isEmpty(humidity) || isEmpty(wind_speed) || isEmpty(sunrise)
            || isEmpty(sunrise24h) || isEmpty(sunset) || isEmpty(sunset24h)
            || isEmpty(conditions_code) || isEmpty(conditions_icon)){
            throw "Error en los datos del servicio.";
          }
          console.log('Data to be sent: ' +  JSON.stringify(dictionary));
          


    		// Send to Pebble if everithing is ok
    		Pebble.sendAppMessage(dictionary,
    		  function(e) {
    		    console.log('Weather info sent to Pebble successfully!');
    		  },
    		  function(e) {
    		    console.log('Error sending weather info to Pebble!');
    		  }
    		);
    }
    catch(err) {
      console.log("Error when getting weather:"+err.message);
    }
    }      
  );
}

function locationError(err) {
  console.log('Error requesting location!');
}

function putParam(parameter, value, actualLen){
  var respuesta ="";
  if (value){
    respuesta=respuesta+(actualLen>0?"&":"")+parameter+"="+value;
  }
  return respuesta;

}



function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    // Get the initial weather
  //  getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
	getWeather();

  }                     
);

Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  var params = "";
  
  //console.log("Show config"+JSON.stringify(e));
  params = params + putParam('KEY_DATE',window.localStorage.getItem('KEY_DATE'),params.length);
  params = params + putParam('KEY_DAY',window.localStorage.getItem('KEY_DAY'),params.length);
  params = params + putParam('KEY_BT',window.localStorage.getItem('KEY_BT'),params.length,params.length);
  params = params + putParam('KEY_BATTERY',window.localStorage.getItem('KEY_BATTERY'),params.length);
  params = params + putParam('KEY_SECOND_HAND',window.localStorage.getItem('KEY_SECOND_HAND'),params.length);
  params = params + putParam('TEMPERATURE',window.localStorage.getItem('TEMPERATURE'),params.length);
  params = params + putParam('CONDITIONS',window.localStorage.getItem('CONDITIONS'),params.length);
  params = params + putParam('HUMIDITY',window.localStorage.getItem('HUMIDITY'),params.length);
  params = params + putParam('SUNRISE',window.localStorage.getItem('SUNRISE'),params.length);
  params = params + putParam('SUNSET',window.localStorage.getItem('SUNSET'),params.length);
  params = params + putParam('INVERTED',window.localStorage.getItem('INVERTED'),params.length);
  params = params + putParam('CELCIUS',window.localStorage.getItem('CELCIUS'),params.length);
  params = params + putParam('POLLTIME',window.localStorage.getItem('POLLTIME'),params.length);
  //console.log(params);
  //console.log("Val from local storage:"+putParam('KEY_DATE',window.localStorage.getItem('KEY_DATE')));
  Pebble.openURL('http://leonlipe.github.io/pblanalogy/analogy_config.html?'+params);
});

Pebble.addEventListener('webviewclosed',
  function(e) {
     var configuration = JSON.parse(decodeURIComponent(e.response));
    //console.log('Configuration window returned:' ,JSON.stringify(configuration));

     
    var configs = {
      'KEY_DATE': parseInt(configuration.date),
      'KEY_DAY': parseInt(configuration.dow),
      'KEY_BT': parseInt(configuration.bt),
      'KEY_BATTERY': parseInt(configuration.bat),
      'KEY_SECOND_HAND': parseInt(configuration.showSec),
      'BACKTYPE': 0,
      'MARGIN': 6,
      'HAND_LENGTH_SEC': 65,
      'HAND_LENGTH_SEC_INVERSE': 15,
      'HAND_LENGTH_MIN': 65,
      'HAND_LENGTH_HOUR': 45,
      'HAND_TYPE': 2,
      'TEMPERATURE': parseInt(configuration.temperature),
      'CONDITIONS': parseInt(configuration.temperature),
      'HUMIDITY': parseInt(configuration.humidity),
      'WIND': 0,
      'SUNRISE': parseInt(configuration.sunrise),
      'SUNSET': parseInt(configuration.sunset),
      'DIGITALTIME': 0,
      'INVERTED': parseInt(configuration.inverted),
      'NUMBERS': 1,
      'CELCIUS': parseInt(configuration.celcius),
      'POLLTIME': parseInt(configuration.time_poll)

      
    };
      window.localStorage.setItem('KEY_DATE', parseInt(configuration.date));
      window.localStorage.setItem('KEY_DAY', parseInt(configuration.dow));
      window.localStorage.setItem('KEY_BT', parseInt(configuration.bt));
      window.localStorage.setItem('KEY_BATTERY', parseInt(configuration.bat));
      window.localStorage.setItem('KEY_SECOND_HAND', parseInt(configuration.showSec));
      window.localStorage.setItem('TEMPERATURE', parseInt(configuration.temperature));
      window.localStorage.setItem('CONDITIONS', parseInt(configuration.temperature));
      window.localStorage.setItem('HUMIDITY', parseInt(configuration.humidity));
      window.localStorage.setItem('SUNRISE', parseInt(configuration.sunrise));
      window.localStorage.setItem('SUNSET', parseInt(configuration.sunset));
      window.localStorage.setItem('INVERTED', parseInt(configuration.inverted));
      window.localStorage.setItem('CELCIUS', parseInt(configuration.celcius));
      window.localStorage.setItem('POLLTIME', parseInt(configuration.time_poll));

      //window.localStorage.setItem('NUMBERS', 1);
      //window.localStorage.setItem('DIGITALTIME', 0);
      //window.localStorage.setItem('WIND', 0);
      //window.localStorage.setItem('BACKTYPE', 0);
      //window.localStorage.setItem('MARGIN', 6);
      //window.localStorage.setItem('HAND_LENGTH_SEC', 65);
      //window.localStorage.setItem('HAND_LENGTH_SEC_INVERSE', 15);
      //window.localStorage.setItem('HAND_LENGTH_MIN', 65);
      //window.localStorage.setItem('HAND_LENGTH_HOUR', 45);
      //window.localStorage.setItem('HAND_TYPE', 2));

    console.log("Saved on local storage");
        //console.log('I sent:' ,JSON.stringify(configs));



    Pebble.sendAppMessage(
      configs,
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!"+JSON.stringify(e));
      }
    );
  }
);
