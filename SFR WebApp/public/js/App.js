(function (){
    // Inicia o Firebase
    
    var firebase = app_firebase;
    var db = firebase.database();
  
    var dadosGlobaisT1 = [];
    var dadosGlobaisT2 = [];
    var timeGlobal = [];
    var distT1 = [];
    var distT2 = [];
    

    var refTanque1 = db.ref("Dados tanque 1 (Caixa d'agua)");
    var refTanque2 = db.ref("Dados tanque 2 (Poco)");
      
    function dataFormatada (time) {
      var date = new Date(time*1000);
      var localeSpecificTime = date.toLocaleTimeString();
      return localeSpecificTime.replace(/:\d+ /, ' ');
    }
  
    refTanque1.on('child_added', function(childSnapshot, prevChildKey) {
      let key = childSnapshot.val();
      alturamedia_caixa = key['alturamedia_caixa'];
      tank1_vol = key['tank1_vol'];
      time = key['time'];
      tank1_level = key['tank1_level'];
      system_power = key['system_power'];
      volT1.innerText = tank1_vol + "m3";
      nivelT1.innerText = tank1_level;
      statusSist.innerText = system_power;


      dateFormatted = dataFormatada(time);
      
      if (dadosGlobaisT1.length < 60) {
      dadosGlobaisT1.push(alturamedia_caixa);
      timeGlobal.push(dateFormatted);
      } else {
        dadosGlobaisT1.shift();
        timeGlobal.shift();
      }
      
    });
    
    
    var alturaT1 = db.ref("alturamedia_caixa");
    alturaT1.on('value', function(snapshot) {
      distT1.push(snapshot.val());
    });
    
    var alturaT2 = db.ref("alturamedia_poco");
    alturaT2.on('value', function(snapshot) {
      distT2.push(snapshot.val());
    });
    
    
    
    refTanque2.on('child_added', function(childSnapshot, prevChildKey) {
      let key = childSnapshot.val();
      alturamedia_poco = key['alturamedia_poco'];
      tank2_vol = key['tank2_vol'];
      time = key['time'];
      tank2_level = key['tank2_level'];
      system_power = key['system_power'];
      temp_bomba = key['temp_bomba'];
      status_bomba = key['status_bomba']
      volT2.innerText = tank2_vol + "m3";
      nivelT2.innerText = tank2_level;
      tempBomba.innerText = temp_bomba;
      statusSist.innerText = system_power;

      dateFormatted = dataFormatada(time);
      
      if (dadosGlobaisT2.length < 60) {
      dadosGlobaisT2.push(alturamedia_poco);
      timeGlobal.push(dateFormatted);
      } else {
        dadosGlobaisT2.shift();
        timeGlobal.shift();
      }
      
    });
    
    
    
    //Construindo gráficos
    // GRÁFICO DE VOLUME DO TANQUE 1
    var ctx1 = document.getElementById('volT1Chart').getContext('2d');
    var chart = new Chart(ctx1, {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        labels: timeGlobal,
        label: 'Hora',
        datasets: [{
            label: 'Volume do tanque 1 [m3]',
            backgroundColor: 'transparent',
            borderColor: 'rgb(255, 99, 132)',
            data: dadosGlobaisT1
        }]
    },

    // Configuration options go here
    options: {}
});
    
    // GRÁFICO DE VOLUME DO TANQUE 2

    var ctx2 = document.getElementById('volT2Chart').getContext('2d');
    var chart = new Chart(ctx2, {
    // The type of chart we want to create
    type: 'line',

    // The data for our dataset
    data: {
        labels: timeGlobal,
        label: 'Hora',
        datasets: [{
            label: 'Volume do tanque 2 [m3]',
            backgroundColor: 'transparent',
            borderColor: 'rgb(255, 99, 132)',
            data: dadosGlobaisT2
        }]
    },

    // Configuration options go here
    options: {}
});


  // GRÁFICO DE BARRAS DOS TANQUES

  var ctx3 = document.getElementById('barVolChart').getContext('2d');
  var chart = new Chart(ctx3, {
    // The type of chart we want to create
    type: 'bar',

    // The data for our dataset
    data: {
        
        labels: ['Nível dos Tanques'],
        datasets: [{
            label: 'Altura do tanque 1 [cm]',
            backgroundColor: 'blue',
            borderColor: 'rgb(255,99,132)',
            data: distT1
        },
          {
            label: 'Altura do tanque 2 [cm]',
            backgroundColor: 'red',
            borderColor: 'rgb(255,99,132)',
            data: distT2
          }]
      },

    // Configuration options go here
    options: {
    }
        
  });

    


    //Enviando para página HTML os dados de on/off, volume, nível dos tanques e temperatura da bomba
    
    
    

    const sistRef = db.ref("system_power");
    sistRef.on('value', snap => statusSist.innerText = snap.val());

      // Registrar função de click no botão de Liga/Desliga
    var btnOn = document.getElementById('btn-On');
    btnOn.addEventListener('click', function(evt){
      sistRef.set('Ligado');
    });
    var btnOff = document.getElementById('btn-Off');
    btnOff.addEventListener('click', function(evt){
      sistRef.set('Desligado');
    });

    
  })();

  //Função para mostrar ou ocultar os gráficos
  function showgraphs() {
    var x = document.getElementById("showgraphs");
    
    if (x.style.display === "none") {
      x.style.display = "block";      
    } else {
      x.style.display = "none";
    }
  }