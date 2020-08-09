(function (){
    // Inicia o Firebase
    var config = {
      apiKey: "AIzaSyCKfz5qjYidlClPlXCf_X73YXKsRwJkUZo",
      authDomain: "smart-fazenda-real.firebaseapp.com",
      databaseURL: "https://smart-fazenda-real.firebaseio.com",
      projectId: "smart-fazenda-real",
      storageBucket: "smart-fazenda-real.appspot.com",
      messagingSenderId: "813001947604",
      appId: "1:813001947604:web:9a039b36884b9bc0bdbe41",
      measurementId: "G-FCCWN7LQZB"
    };
    firebase.initializeApp(config);

    var db = firebase.database();

    var dadosGlobaisT1 = [];
    var dadosGlobaisT2 = [];
    var timeGlobal = [];
    var distT1 = [];
    var distT2 = [];
    

    var refTanque1 = db.ref("Dados tanque 1 (Caixa d'agua)");
    var refTanque2 = db.ref("Dados tanque 2 (Poço)");
      
    function dataFormatada (time) {
      var date = new Date(time);
      var localeSpecificTime = date.toTimeString();
      return localeSpecificTime.replace(/:\d+ /, ' ');
    }
  
    refTanque1.on('child_added', function(childSnapshot, prevChildKey) {
      let key = childSnapshot.val();
      caixaAlturaAgua = key['caixaAlturaAgua'];
      caixaVol = key['caixaVol'];
      time = key['timestamp'];
      caixaLevel = key['caixaLevel'];
      system_power = key['system_power'];
      volT1.innerText = caixaVol + "m3";
      
      if (caixaLevel == 0) {
        nivelT1.innerText = 'Low';
      } else if (caixaLevel == 1) {
        nivelT1.innerText = 'Ok';
      } else if (caixaLevel == 2) {
        nivelT1.innerText = 'High';
      }

      cxaAlturaAgua.innerText = caixaAlturaAgua + 'cm';

      statusSist.innerText = system_power;


      dateFormatted = dataFormatada(time);
      
      if (dadosGlobaisT1.length < 60) {
      dadosGlobaisT1.push(caixaAlturaAgua);
      timeGlobal.push(dateFormatted);
      } else {
        dadosGlobaisT1.shift();
        timeGlobal.shift();
      }
      
    });
    
    
    var alturaT1 = db.ref("caixaAlturaAgua");
    alturaT1.on('value', function(snapshot) {
      distT1.push(snapshot.val());
    });
    
    var alturaT2 = db.ref("pocoAlturaAgua");
    alturaT2.on('value', function(snapshot) {
      distT2.push(snapshot.val());
    });
    
    
    
    refTanque2.on('child_added', function(childSnapshot, prevChildKey) {
      let key = childSnapshot.val();
      pocoAlturaAgua = key['pocoAlturaAgua'];
      pocoVol = key['pocoVol'];
      time = key['timestamp'];
      pocoLevel = key['pocoLevel'];
      system_power = key['system_power'];
      temp_bomba = key['temp_bomba'];
      pumpStatus = key['pumpStatus']
      volT2.innerText = pocoVol + "m3";

      if (pocoLevel == 0) {
        nivelT2.innerText = 'Low';
      } else if (pocoLevel == 1) {
        nivelT2.innerText = 'Ok';
      } else if (pocoLevel == 2) {
        nivelT2.innerText = 'High';
      }
      
      pcoAlturaAgua.innerText = pocoAlturaAgua + 'cm';

      //tempBomba.innerText = temp_bomba;
      statusSist.innerText = system_power;
      if (pumpStatus == 1) {
        bombStatus.innerText = 'On';
      } else if (pumpStatus == 0) {
        bombStatus.innerText = 'Off';
      }

      dateFormatted = dataFormatada(time);
      
      if (dadosGlobaisT2.length < 60) {
      dadosGlobaisT2.push(pocoAlturaAgua);
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
            label: 'Altura do tanque 1 [mm]',
            backgroundColor: 'blue',
            borderColor: 'rgb(255,99,132)',
            data: distT1
        },
          {
            label: 'Altura do tanque 2 [mm]',
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