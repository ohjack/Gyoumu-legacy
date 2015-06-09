$(document).ready(function() {
  var func_submit = function() {
    var $dialog = $(this);
    var send = {
      'type': $dialog.attr('id'),
      'vals': {}
    };

    var to_end = true;

    $dialog.find('input').each(function() {
      var $ele = $(this);
      send['vals'][$ele.attr('name')] = $ele.val();
      to_end = to_end & ($ele.val() >= $ele.attr('order_qty'));
    });

    $('#edit-send').val(JSON.stringify(send));

    var $submit = $('div.hidden-callee input.form-submit');
    $submit.click();
  };

  var $dialog = $('[id^="dialog-"]');

  $dialog.dialog({
    width: 'auto',
    autoOpen: false,
    draggable: true,
    buttons: {
      'Modify': func_submit
    }
  });

  $('[target-dialog]').click(function() {
    var $this = $(this);
    var target = $this.attr('target-dialog');
    var title = $this.val();
    var $dia = $('#' + target);

    $dia.find('input').each(function(){
      $input = $(this);
      $input.val($input.attr('dval'));
    });

    $dia
        .dialog('option', 'title', title)
        .dialog('open')
    ;
    return false;
  });

  Datepicker('body');

  $('input[order_qty]').keyup(function(){
    var $this = $(this);
    var val = parseFloat($this.val());
    console.log(val);
    var $qty_to = $this
        .parents('div.wrapper-line:eq(0)')
        .find('div.qty_hint span.qty_to');

    val = parseFloat($this.attr('picked')) + val;
    val = isNaN(val) ? '?' : val;
    $qty_to.text(val);
  });

  $('td[gu_name="product_id.name"] a[sku]').click(function(){
    var $a = $(this);
    var sku = $a.attr('sku');
    var $history_view = $('div.move-history.sku-' + sku);

    if ($history_view.hasClass('embbed')) {
      $history_view.slideUp('fast').removeClass('embbed');
    }
    else {
      $history_view
          .insertAfter($a)
          .slideDown('fast')
          .addClass('embbed');
    }

    return false;
  });
});
