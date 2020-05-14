var app_firebase = {};
(function(){
  // Your web app's Firebase configuration
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

  app_firebase = firebase;
})()