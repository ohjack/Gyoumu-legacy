Drupal.behaviors.oerp_gallery_mapping = function(){
  $('div.pool-slot img.seed').each(function(){
    var $img = $(this);
    $img.draggable({
      revert: true,
      revertDuration: 200
    });
  });

  $('div.mapping-sel').each(function(){
    var $sel = $(this);
    var $slot = $sel.parents('div.mapping-slot:eq(0)');

    if($slot.hasClass('queued')){
      return;
    }

    var accept = '.sku-' + $sel.parents('div.mapping-slot:eq(0)').attr('sku');

    $sel.droppable({
      accept: accept,
      hoverClass: 'hover',
      drop: function(ev, ui){
        var $dropped_sel = $(this);

        var src = ui.draggable.attr('src');

        var $img = $dropped_sel.find('img');
        if(!$img.attr('src_bak')){
          $img.attr('src_bak', $img.attr('src'));
        }
        $img.attr('src', src);

        $dropped_sel.addClass('dropped');
        $dropped_sel
            .parents('div.mapping-slot:eq(0)')
            .addClass('mapped')
            .find('input').val(src);
      }
    })
  });

  $('div.mapping-sel').click(function(){
    var $slot = $(this).parents('div.mapping-slot:eq(0)');

    if(!$slot.hasClass('mapped')){
      return;
    }

    var $img = $slot.find('img');
    $img.attr('src', $img.attr('src_bak'));

    $slot.removeClass('mapped');
    $slot.find('input').val('');
  });

  $('div.mapping-slot.occupied h2').click(function(){
    var $slot = $(this).parents('div.mapping-slot:eq(0)');
    var $chk = $slot.find('input[name^="del-"]');

    if($slot.hasClass('toDelete')){
      $slot.removeClass('toDelete');
      $chk.removeAttr('checked');
    }
    else{
      $slot.addClass('toDelete');
      $chk.attr('checked', 'checked');
    }
  });
};