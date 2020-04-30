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

    db = firebase.database();
       
    var refTanque1 = db.ref("Dados tanque 1 (Caixa d'agua)");
    

   
    refTanque1.on('child_added', function(childSnapshot, prevChildKey) {
      let key = childSnapshot.val();
      //var keys = Object.values(key);
      //valornivelT1 = key['2- Nivel do tanque'];
      valorvolT1 = key['3- Volume do tanque'];
      dataArr = [];
      dataArr.push(valorvolT1);
      
      console.log(dataArr);
    });

    
    
    

    var refTanque2 = db.ref("Dados tanque 2 (Poço)");
    refTanque2.on('child_added', function(childSnapshot, prevChildKey) {
      let item = (childSnapshot.val());
      valornivelT2 = item['2- Nivel do tanque'];
      valorvolT2 = item['3- Volume do tanque'];
    });

    
    

    //Enviando para página HTML os dados de on/off, volume, nível dos tanques e temperatura da bomba
    const volT1ref = db.ref("Volume do tanque 1");
    volT1ref.on('value', snap => volT1.innerText = snap.val() + "m3");

    const volT2ref = db.ref("Volume do tanque 2");
    volT2ref.on('value', snap => volT2.innerText = snap.val() + "m3");

    const nivelT1ref = db.ref("Nivel do tanque 1");
    nivelT1ref.on('value', snap => nivelT1.innerText = snap.val());

    const nivelT2ref = db.ref("Nível do tanque 2");
    nivelT2ref.on('value', snap => nivelT2.innerText = snap.val());

    const tempBombaref = db.ref("temp");
    tempBombaref.on('value', snap => tempBomba.innerText = snap.val() + "ºC");
    
    const sistRef = db.ref("Status do Sistema");
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
