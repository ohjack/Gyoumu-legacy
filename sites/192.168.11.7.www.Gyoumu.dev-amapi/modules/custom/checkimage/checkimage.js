$(document).ready(function(){
  $('table.checkimage-result span.show_thumbnails').click(function(){

    $(this).parents('tr:eq(0)').find('a').each(function(){
      var a = $(this);

      var newSrc = '<img width="250px" src="' + a.attr('href') + '"/>';
      a.html(newSrc);
    });
  });

  $('span.show_all_thumbnails').click(function(){
    $('table.checkimage-result span.show_thumbnails').click();
  });
});