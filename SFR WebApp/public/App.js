(function(){
    
    // Inicia o firebase Firebase
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
  
    // Cria os listeners dos dados no firebase

    var sistRef = db.ref("Status do Sistema");

    const statusSist = document.getElementById("statusSist");
    const statusT1 = document.getElementById("statusT1");
    const statusT2 = document.getElementById("statusT2");
    const volT1 = document.getElementById("volT1");
    const volT2 = document.getElementById("volT2");

    const dbRef2 = firebase.database().ref().child("Nivel do tanque 1");
    dbRef2.on('value', snap => statusT1.innerText = snap.val());

    const volT1ref = firebase.database().ref().child("Volume do tanque 1");
    const volT2ref = firebase.database().ref().child("Volume do tanque 2");
    volT1ref.on('value', snap => volT1.innerText = snap.val() + "m3");
    volT2ref.on('value', snap => volT2.innerText = snap.val() + "m3");

    const dbRef = firebase.database().ref().child("Status do Sistema");
    dbRef.on('value', snap => statusSist.innerText = snap.val());

    const dbRef3 = firebase.database().ref().child("Nível do tanque 2");
    dbRef3.on('value', snap => statusT2.innerText = snap.val());
 
  
  

  
    // Registrar função de click no botão de lampada
    var btnOn = document.getElementById('btn-On');
    btnOn.addEventListener('click', function(evt){
      sistRef.set("Ligado");
    });
    var btnOff = document.getElementById('btn-Off');
    btnOff.addEventListener('click', function(evt){
      sistRef.set("Desligado");
    });
  
})();
  
  
  // Retorna uma função que de acordo com as mudanças dos dados
  // Atualiza o valor atual do elemento, com a metrica passada (currentValueEl e metric)
  // e monta o gráfico com os dados e descrição do tipo de dados (chartEl, label)
  function onNewData(currentValueEl, chartEl, label, metric){
    return function(snapshot){
      var readings = snapshot.val();
      if(readings){
          var currentValue;
          var data = [];
          for(var key in readings){
            currentValue = readings[key]
            data.push(currentValue);
          }
  
          document.getElementById(currentValueEl).innerText = currentValue + ' ' + metric;
          buildLineChart(chartEl, label, data);
      }
    }
  }
  
  // Constroi um gráfico de linha no elemento (el) com a descrição (label) e os
  // dados passados (data)
  function buildLineChart(el, label, data){
    var elNode = document.getElementById(el);
    new Chart(elNode, {
      type: 'line',
      data: {
          labels: new Array(data.length).fill(""),
          datasets: [{
              label: label,
              data: data,
              borderWidth: 1,
              fill: false,
              spanGaps: false,
              lineTension: 0.1,
              backgroundColor: "#F9A825",
              borderColor: "#F9A825"
          }]
      }
    });
  }