$(document).ready(function(){ 
	$("#serialize").click(function(){ 
		var myobj = {Name:$("#Name").val(),Comment:$("#Comment").val()}; 
		jobj = JSON.stringify(myobj); 
		$("#json").text(jobj); 
		var url = "comment"; 
		$.ajax({ 
			url:url, 
			type: "POST", 
			data: jobj, 
			contentType: "application/json; charset=utf-8", 
			success: function(data,textStatus) { 
				$("#done").html(textStatus); 
			} 
		});
	});
	$("#getThem").click(function() { 
		$.getJSON('comment', function(data) { 
			console.log(data); 
			var everything = "<ul>"; 
			if(data[0]==null){
				everything += "<li>No comments</li>";
			}
			else {
				for(var comment in data) { 
					com = data[comment]; 
					everything += "<li>Name: " + com.Name + " -- Comment: " + com.Comment + "</li>"; 
				}
			} 
			everything += "</ul>"; 
			$("#comments").html(everything); 
		}); 
	});
	$("#clear").click(function() { 
		var url = "comment"; 
		$.ajax({ 
			url:url, 
			type: "DELETE", 
			data: "{}", 
			contentType: "application/json; charset=utf-8", 
			success: function(data,textStatus) { 
				$("#done").html(textStatus);
				$("#comments").html(textStatus); 
				$("#json").html("Records Cleared");
			} 
		});
		
	});
	
});


