// Import the functions you need from the SDKs you need
import { FirebaseApp, initializeApp } from "firebase/app";
import { getAnalytics, Analytics } from "firebase/analytics";
import { getAuth, GithubAuthProvider, signInWithPopup, Auth, signOut } from 'firebase/auth';
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

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
    onUserChange(user?.displayName, user?.photoURL);
  });
  return {app: app, auth: auth, analytics: analytics}
}

export function signin(app:App) : void{
  let auth = getAuth(app.app);
  const provider = new GithubAuthProvider();
  signInWithPopup(auth, provider).then((result)=>{
  });
}

export function signout(app:App) : void{
  getAuth(app.app).signOut();
}