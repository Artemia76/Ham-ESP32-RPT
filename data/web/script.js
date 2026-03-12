'use strict';

var RepControl = document.getElementById('RepControl');
var SqlValue = document.getElementById('SqlValue');
var SqlControl = document.getElementById('SqlControl');
var MagValue = document.getElementById('MagValue');
var MagControl = document.getElementById('MagControl');
var TOTValue = document.getElementById('TOTValue');
var TOTControl = document.getElementById('TOTControl');
var StartValue = document.getElementById('StartValue');
var StartVolume = document.getElementById('StartVolume');
var RBValue = document.getElementById('RBValue');
var RBVolume = document.getElementById('RBVolume');
var EndValue = document.getElementById('EndValue');
var EndVolume = document.getElementById('EndVolume');
var TestBtn = document.getElementById('TestBtn');
var RstBtn = document.getElementById('RstBtn');
window.onload = (event) => {
	  SqlControl.addEventListener('input', function()
    {
      document.getElementById("SqlValue").innerHTML = SqlControl.value;
    });
    
    MagControl.addEventListener('input', function()
    {
      document.getElementById('MagValue').innerHTML = MagControl.value;
    });

    TOTControl.addEventListener('input', function()
    {
      document.getElementById('TOTValue').innerHTML = TOTControl.value;
    });

    StartVolume.addEventListener('input', function()
    {
      document.getElementById('StartValue').innerHTML = StartVolume.value;
    });

    RBVolume.addEventListener('input', function()
    {
      document.getElementById('RBValue').innerHTML = RBVolume.value;
    });

    EndVolume.addEventListener('input', function()
    {
      document.getElementById('EndValue').innerHTML = EndVolume.value;
    });

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
      SqlValue.innerHTML = json.squelch;
      MagControl.value = json.mag;
      MagValue.innerHTML = json.mag;
      TOTControl.value = json.tot;
      TOTValue.innerHTML = json.tot;
      StartVolume.value = json.StartVol;
      StartValue.innerHTML = json.StartVol;
      RBVolume.Value = json.RBVol;
      RBValue.innerHTML = json.RBVol;
      EndVolume.value = json.EndVol;
      EndValue.innerHTML = json.EndVol;

      RepControl.addEventListener('change', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("Rep=" + String(this.checked));
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
      MagControl.addEventListener('change', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("Mag=" + String(this.value));
				}
      );
      TOTControl.addEventListener('change', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("TOT=" + String(this.value));
				}
      );
      StartVolume.addEventListener('change', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("StartVol=" + String(this.value));
				}
      );
      RBVolume.addEventListener('change', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("RBVol=" + String(this.value));
				}
      );
      EndVolume.addEventListener('change', function()
        {
					var xhttp = new XMLHttpRequest();
					xhttp.open("POST", "set", true);
					xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
					xhttp.send("EndVol=" + String(this.value));
				}
      );
      TestBtn.addEventListener('click', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("Test=true");
        }
      );
      RstBtn.addEventListener('click', function()
        {
          var xhttp = new XMLHttpRequest();
          xhttp.open("POST", "set", true);
          xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhttp.send("Restart=true");
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
              var S = Number(xhttp.responseText);
              if (S <= 9)
              {
                document.getElementById("RSSIValue").innerHTML = "S " + xhttp.responseText;
              }
              else
              {
                var plus = (S - 9) *10;
                document.getElementById("RSSIValue").innerHTML = "S 9+" + plus.toString();
              }
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


