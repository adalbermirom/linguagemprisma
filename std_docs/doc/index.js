
function copyText(pre) {
	 function htmlDecode(input) {
         var doc = new DOMParser().parseFromString(input, "text/html");
         return doc.documentElement.textContent;
     }
	 let cod = document.getElementById(pre);
	 let txt = cod.innerHTML;
	 let result = htmlDecode(txt);
	 if ( result === "" ){
		 result = txt;
	 }
	 alert("Copiado para área de transferência.");
     
            /* Copy text into clipboard */
            navigator.clipboard.writeText(result);
                 
}
   

function show_menu_flutuante(bt, divm, navm){
	       let navmenu = document.getElementById(navm);
		   let divmenu = document.getElementById(divm);
		   if( divmenu.hidden ){
			   //bt.innerHTML = '&gt;';
			   bt.title="Ocultar painel lateral";
			   divmenu.hidden = false;
			   navmenu.style.height_old = navmenu.style.height;
			   navmenu.style.height = '97%';
			   
			   //alert('ola');
		   }else{
			  //bt.innerHTML = '&#x2630';
			  bt.title="Mostrar painel lateral";
		      divmenu.hidden = true;
		      navmenu.style.height = navmenu.style.height_old;
		   }
}


function def_valor(key,val){
	if (typeof(Storage) !== "undefined") {
	    localStorage.setItem( key, val);
    } else {
       console.log("Sorry! No Web Storage support..");
    }	
}

function obt_valor(key){
	if (typeof(Storage) !== "undefined") {
	    return localStorage.getItem( key);
    } else {
       console.log("Sorry! No Web Storage support..");
    }	
    return null;
}

function mude_tema(select){
	let tema_css = document.getElementById('tema_css');
	tema_css.href = select.value;
	def_valor('tema_atual', select.value);
}

function set_tema_onload(id){
	let tema_atual = obt_valor('tema_atual'); //local storage;
	if (tema_atual===null) tema_atual = 'tema_padrao.css';
	let tema_css = document.getElementById('tema_css');
	let select = document.getElementById('combo_tema');
	select.value = tema_atual;
	tema_css.href = select.value;
}

function select_insira(select, txt, val){
	let option = document.createElement("option");
    option.text = txt;
    option.value = val;
    select.appendChild(option);
	
}

function select_temas_inclua_itens(){
    let select = document.getElementById('combo_tema');
    let itens = tema.itens; //está em temas_itens.js podendo adicionar mais temas.
    for ( i=0; i<itens.length;i++){
		select_insira(select, itens[i].txt, itens[i].val);	
	}	
}

document.addEventListener("DOMContentLoaded", function(event) {
	select_temas_inclua_itens(); //preenche o select de temas com mais opções além do padrão.
	set_tema_onload();
});


