'use strict';

var RepControl = document.getElementById('RepControl');

RepControl.addEventListener('change', function(){
  var xhttp = new XMLHttpRequest();
  if (this.checked) {
    xhttp.open("GET", "RepOn", true);
  } else {
    xhttp.open("GET", "RepOff", true);
  }
  xhttp.send();
});

setInterval(function getData()
{
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function()
    {
        if(this.readyState == 4 && this.status == 200)
        {
            document.getElementById("RSSI").innerHTML = this.responseText;
        }
    };

    xhttp.open("GET","getRSSI", true);
    xhttp.send();
}, 2000);


    