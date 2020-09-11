

function SaveName(){
    
    var name = document.getElementById("name").value;
    if(name !== ""){
        localStorage.setItem("name",name);
        document.getElementById("name").value  = " ";
    } 
}

 function Pusher(){
    var gif = document.getElementById("opener");
    gif.style.display = "none"; 
    document.getElementsByClassName('MainDiv')[0].style.visibility = "visible";

}



