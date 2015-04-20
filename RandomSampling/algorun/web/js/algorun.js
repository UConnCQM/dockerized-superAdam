$("#run_form").submit(function(event) {
	event.preventDefault();
	var input_data=$("#input_data").val();
	var jqxhr = $.post( "/do/run", { input: input_data })
	.done(function(data,textStatus,jqXHR) {$("#output_data").val(JSON.stringify(data, null, 4));})
	.fail(function() {$("#output_data").val("An error occurred")});
});
$("#populate_input").click(function() {
	$("#input_data").val($("#code_example_1").text());
});
$("#reset_computation").click(function() {
	$("#input_data").val("");
	$("#output_data").val("");
});
