var CHECKTIME_INTERVAL = 750;

$(document).ready(function(){
  $('a.check-online').each(function(){
    var $this = $(this);
    $this.hide();
    $this.after('<span class="notice checking">Checking...</span>');
  });

  online_check();
});

function online_check()
{
  var chk = $('span.checking:eq(0)');

  if(chk.length){
    var href = chk.prev('a').attr('href');
    $.post('?q=listing/rakuten/check/js', { href: href },
        function(data){
          chk.hide()
          data = eval('(' + data + ')');

          if(data.success){
            chk.prev('a').show();
          }
          else{
            chk.after(data.msg);
          }

          chk.remove();
          return setTimeout('online_check()', CHECKTIME_INTERVAL);
        }
    )
  }
  else{
    return clearTimeout();
  }
}