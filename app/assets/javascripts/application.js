// This is a manifest file that'll be compiled into application.js, which will include all the files
// listed below.
//
// Any JavaScript/Coffee file within this directory, lib/assets/javascripts, vendor/assets/javascripts,
// or vendor/assets/javascripts of plugins, if any, can be referenced here using a relative path.
//
// It's not advisable to add code directly here, but if you do, it'll appear at the bottom of the
// compiled file.
//
// Read Sprockets README (https://github.com/sstephenson/sprockets#sprockets-directives) for details
// about supported directives.
//
<<<<<<< HEAD
// = require jquery
// = require jquery_ujs
// = require bootstrap
// = require turbolinks
// = require_tree .
=======
//= require jquery
//= require jquery_ujs
//= require turbolinks
//= require_tree .
var set_stars = function(club_id, stars) {
	for(i=1; i <= 5; i++){
		if(i <= stars){
			$(‘#’ + club_id + ‘_’ + i).addClass(“on”);
			  } else {
			  $(‘#’ + club_id + ‘_’ + i).removeClass(“on”);
			  }
			  }
}

$(function() {
$(‘.rating_star’).click(function() {
var star = $(this);
var form_id = star.attr(“data-club-id”);
var stars = star.attr(“data-stars”);

$(‘#’ + club_id + ‘_stars’).val(stars);

$.ajax({
type: “post”,
url: $(‘#’ + club_id).attr(‘action’),
data: $(‘#’ + club_id).serialize()
})
});

$(‘.star_rating_form’).each(function() {
var form_id = $(this).attr(‘id’);
set_stars(form_id, $(‘#’ + club_id + ‘_stars’).val());
});
});
>>>>>>> fd38fe84021f700daea5366078dbf2e04929191e
