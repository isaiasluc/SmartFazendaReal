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

const auth=firebase.auth();


    function signIn() {
        var email = document.getElementById("email");
        var password = document.getElementById("password");

        const promise = auth.signInWithEmailAndPassword(email.value, password.value);
        promise.catch(e => alert(e.message));
    }

    auth.onAuthStateChanged(function(user){
        if (user) {
            var email = user.email;
            alert("Usuário ativo " + email);
            window.location.replace('dashboard.html');
            //is signed in
        } else {
            alert("Não há usuário ativo");
            //no user signed in
        }
    });