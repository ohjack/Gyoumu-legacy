$(document).ready(function() {
  $('span.ylister.preview').click(function(){
    var $this = $(this);
    var rid = $this.attr('rid');
    var iframe = $('#preview-iframe');
    var inner_html = iframe.contents().find('html')

    inner_html.html('');

    $.get('?q=yauc_lister/preview/' + rid + '/js',function(data){
      data = eval(data);

      inner_html.html(data);
      inner_html.click(function(){
        iframe.css('left', '-10000px');
      });

      var inner_width = inner_html.find('#main-wrapper').width();
      var pos_left = ($(document).width() - inner_width) / 2;

      iframe
          .css('top', $(window).scrollTop() + 'px')
          .css('width', inner_width + 20)
          .css('left', pos_left);
    });
  });
});