// Import the functions you need from the SDKs you need
import { FirebaseApp, initializeApp } from "firebase/app";
import { getAnalytics, Analytics } from "firebase/analytics";
import { getAuth, GithubAuthProvider, signInWithPopup, Auth, signOut } from 'firebase/auth';
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries
import firebase from 'firebase/compat/app';
import * as firebaseui from 'firebaseui';
import 'firebaseui/dist/firebaseui.css';


export type App = {
  app:FirebaseApp,
  auth: Auth,
  analytics: Analytics,
};

export function get_app(onUserChange: CallableFunction) : App {
  // Your web app's Firebase configuration
  // For Firebase JS SDK v7.20.0 and later, measurementId is optional
  const firebaseConfig = {
    apiKey: "AIzaSyBfejITCASniQLBGFdo__tAUA7r-KYgE6Y",
    authDomain: "mlang-d4add.firebaseapp.com",
    projectId: "mlang-d4add",
    storageBucket: "mlang-d4add.appspot.com",
    messagingSenderId: "287568186330",
    appId: "1:287568186330:web:fb649ef8f46a526d408ee3",
    measurementId: "G-E82C66NLBS"
  };

  // Initialize Firebase
  const app = initializeApp(firebaseConfig);
  const analytics = getAnalytics(app);
  const auth = getAuth(app);
  auth.onAuthStateChanged((user)=>{
    onUserChange(user?.displayName, user?.photoURL, user?.email);
  });
  return {app: app, auth: auth, analytics: analytics}
}

export function showSigninUI(app:App)
{
      // FirebaseUI config.
      var uiConfig = {
        signInSuccessUrl: '/user/profile.html',
        signInOptions: [
          // Leave the lines as is for the providers you want to offer your users.
          firebase.auth.GoogleAuthProvider.PROVIDER_ID,
          // firebase.auth.FacebookAuthProvider.PROVIDER_ID,
          // firebase.auth.TwitterAuthProvider.PROVIDER_ID,
          firebase.auth.GithubAuthProvider.PROVIDER_ID,
          /*
          {
            provider: firebase.auth.EmailAuthProvider.PROVIDER_ID,
            signInMethod: firebase.auth.EmailAuthProvider.EMAIL_LINK_SIGN_IN_METHOD,
            requireDisplayName: true            
          },
          */
          // firebase.auth.PhoneAuthProvider.PROVIDER_ID,
          // firebaseui.auth.AnonymousAuthProvider.PROVIDER_ID
        ],
        // tosUrl and privacyPolicyUrl accept either url string or a callback
        // function.
        // Terms of service url/callback.
        tosUrl: '/terms.html',
        // Privacy policy url/callback.
        privacyPolicyUrl: function() {
          window.location.assign('/privacy.html');
        },

        callbacks: {
          signInSuccessWithAuthResult: function(authResult:any, redirectUrl:any){
            console.log('signed in: ', authResult.user.displayName);
            if(!authResult.user.displayName){
              authResult.user.createProfileChangeRequest().displayName = authResult.user.email.match(/^([^@]*)@/)[1]; 
            }
            return true;
          }
        }
      };

      // Initialize the FirebaseUI Widget using Firebase.
      var ui = new firebaseui.auth.AuthUI(app.auth);//firebase.auth());
      // The start method will wait until the DOM is loaded.
      // Is there an email link sign-in?
      ui.start('#firebaseui-auth-container', uiConfig);  
}

function github_signin(app:App)
{
  let auth = getAuth(app.app);
  const provider = new GithubAuthProvider();
  signInWithPopup(auth, provider).then((result)=>{
  });
}
export function signin(app:App) : void{
  window.location.href = "/signin.html";
}

export function signout(app:App) : void{
  app.auth.signOut();
  window.location.href = '/';
}