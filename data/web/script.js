'use strict';

var RepControl = document.getElementById('RepControl');
var SqlControl = document.getElementById('SqlControl');
var MagControl = document.getElementById('MagControl');

window.onload = (event) => {
	  SqlControl.addEventListener('input', function()
    {
      document.getElementById("SqlValue").innerHTML = SqlControl.value;
    });
    
    MagControl.addEventListener('input', function()
    {
      document.getElementById('MagValue').innerHTML = MagControl.value;
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
      MagConreol.value = json.mag;
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


