'use strict';

var RepControl = document.getElementById('RepControl');
var SqlControl = document.getElementById('SqlControl');

window.onload = (event) => {
	SqlControl.addEventListener('input', function()
    {
      document.getElementById("SqlValue").innerHTML = SqlControl.value;
    }
  );
	SqlControl.addEventListener('change', function()
    {
      var xhttp = new XMLHttpRequest();
			xhttp.open("POST", "set", true);
			xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
			xhttp.send("Sql=" + String(this.value));
    }
  );
  var xhttp = new XMLHttpRequest();
  xhttp.open("POST","get", true);
  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
  xhttp.onreadystatechange = function()
  {
    if(this.readyState == 4 && this.status == 200)
		{
      const json = JSON.parse(this.responseText);
      RepControl.checked = json.state;
      SqlControl.value = json.squelch;
      RepControl.addEventListener('change', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("Rep=" + String(this.checked));
        }
      );
      setInterval(function getData()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST","get", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.onreadystatechange = function()
          {
            if(xhttp.readyState == 4 && xhttp.status == 200)
            {
              document.getElementById("RSSIValue").innerHTML = xhttp.responseText + " dBm";
							document.getElementById("RSSIJauge").value = xhttp.responseText;
            }
          };
          xhttp.send("RSSI=Signal");
        }, 2000
      );
    }
  };
  xhttp.send('Config=All');
};

/*
function input_squelch() {
	document.getElementById("sql_value").innerHTML = SqlControl.value;
}

function change_squelch() {
	var xhttp = new XMLHttpRequest();
  xhttp.open("POST", "set", true);
  xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
  xhttp.send("Sql=" + String(this.value));
}
<input type="range" min="1" max="13" value="9" class="slider2" id="SqlControl" oninput="input_squelch()" onchange="change_squelch()">
*/
