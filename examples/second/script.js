
/*
 *
 */
function clickAction() {
    console.log("clicked");

    var request = new XMLHttpRequest();
    request.errorOutput  = document.getElementById("error");
    request.timeout      = 10000; 
    request.responseType = "json";
    request.ontimeout    = function() {
        this.errorOutput.innerHTML = "Timeout";
    }
    request.onreadystatechange = function() {
        if (this.readyState == 4) {
            if (this.status == 200) {
                console.log("success");
//                this.errorOutput.innerHTML = this.responseText;
            } else {
                this.errorOutput.innerHTML = "Request error";
            }
        }
    }
    request.open("POST", "formaction");
    request.send(document.getElementById('myFile').files[0]);
//    request.send(new FormData(document.getElementById("testForm")));
}

