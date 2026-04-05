#ifndef __HELPER__
#define __HELPER__

#include <Arduino.h>

const char* ssid = "Cracker";
const char* password = "totu7751055";
const int H = 28; // En
const int W = 28; // Boy


String html = R"rawliteral(
<!DOCTYPE html>
<html lang="az">

<head>

<meta charset="UTF-8">

<title>ESP32 AI Reqem Tanima</title>

<style>

body{
font-family:Arial;
text-align:center;
background:#f0f0f0;
margin:0;
padding:20px;
}

h1{
margin:20px;
}

#grid{
display:grid;
grid-template-columns: repeat(28,20px);
grid-gap:1px;
margin:20px auto;
width:560px;
user-select:none;
}

.cell{
width:20px;
height:20px;
background:white;
border:1px solid #ccc;
cursor:crosshair;
}

.cell.black{
background:black;
}

button{
font-size:20px;
padding:12px 40px;
margin:10px;
cursor:pointer;
background:#4CAF50;
color:white;
border:none;
border-radius:5px;
}

button:hover{
background:#45a049;
}

#result{
font-size:120px;
font-weight:bold;
color:red;
margin-top:20px;
}

</style>

</head>

<body>

<h1>ESP32 AI Reqem Tanima</h1>

<p>Sol klik = cız | Sag klik = sil</p>

<div id="grid"></div>

<br>

<button onclick="sendMatrix()">Təxmin Et</button>

<button onclick="clearGrid()">Təmizlə</button>

<h2>Təxmin edilən rəqəm:</h2>

<div id="result">?</div>

<script>

const SIZE = 28;

let matrix = Array(SIZE).fill().map(()=>Array(SIZE).fill(0));

let isDrawing = false;
let eraseMode = false;

const grid = document.getElementById("grid");

for(let y=0;y<SIZE;y++)
{
for(let x=0;x<SIZE;x++)
{

const cell = document.createElement("div");

cell.classList.add("cell");

cell.dataset.x = x;
cell.dataset.y = y;

cell.addEventListener("mousedown",(e)=>{

e.preventDefault();

isDrawing = true;

eraseMode = (e.button === 2);

paintCell(cell);

});

cell.addEventListener("mouseover",()=>{

if(isDrawing)
{
paintCell(cell);
}

});

grid.appendChild(cell);

}
}

document.addEventListener("mouseup",()=>{
isDrawing=false;
});

grid.addEventListener("contextmenu",e=>e.preventDefault());

function paintCell(cell)
{

const x=parseInt(cell.dataset.x);
const y=parseInt(cell.dataset.y);

if(eraseMode)
{
matrix[y][x]=0;
cell.classList.remove("black");
}
else
{
matrix[y][x]=1;
cell.classList.add("black");
}

}

function clearGrid()
{

for(let y=0;y<SIZE;y++)
{
for(let x=0;x<SIZE;x++)
{
matrix[y][x]=0;
}
}

const cells=document.querySelectorAll(".cell");

cells.forEach(cell=>{
cell.classList.remove("black");
});

document.getElementById("result").innerText="?";

}

function sendMatrix()
{

fetch('/send',
{
method:'POST',
headers:{'Content-Type':'application/json'},
body:JSON.stringify({matrix:matrix})
})

.then(response=>response.text())
.then(data=>{

console.log("Təxmin: ",data);

document.getElementById("result").innerText=data;

})

.catch(err=>console.error(err));

}

</script>

</body>
</html>
)rawliteral";

#endif