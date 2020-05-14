(function(){
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
//    var firebase = app_firebase;
    var db = firebase.database();
  
    var dadosGlobais = [];
    var timeGlobal = [];
    var distT1 = [];
    var distT2 = [];
    

    var refTanque1 = db.ref("Dados tanque 1 (Caixa d'agua)");
    var refTanque2 = db.ref("Dados tanque 2 (Poço)");
      
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
      
      if (dadosGlobais.length < 50) {
      dadosGlobais.push(alturamedia_caixa);
      timeGlobal.push(dateFormatted);
      } else {
        dadosGlobais.shift();
        timeGlobal.shift();
      }
      
    });
    
    
    var alturaT1 = db.ref("alturamedia_caixa");
    alturaT1.on('value', function(snapshot) {
      distT1.push(snapshot.val());
    });
    
    var alturaT2 = db.ref("alturamedia_tanque");
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
            data: dadosGlobais
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
            data: dadosGlobais
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
            data:distT2
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