$(document).ready(function() {
        
    $("#search").keyup(function() {
	
	$("#sub_btn").attr('href', '#'+$("#search").val())
	
    })

});


