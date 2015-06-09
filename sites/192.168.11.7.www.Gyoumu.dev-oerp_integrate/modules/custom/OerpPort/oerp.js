$(document).ready(function(){
  $('div.oerp-button').hover(
      function(){
        $(this).addClass('ui-state-hover');
      },
      function(){
        $(this).removeClass('ui-state-hover');
      }
  );

  $('div.oerp_debug label').click(function(){
    var container = $(this).parent('div.oerp_debug');
    
    if(!container.hasClass('hide_src')){
      container.addClass('hide_src');
    }
    else{
      $('div.oerp_debug').addClass('hide_src');
      container.removeClass('hide_src');
    }
  });
});